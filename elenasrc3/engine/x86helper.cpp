//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains CPU native helpers
//		Supported platforms: x86 / x86-64
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "x86helper.h"

using namespace elena_lang;

// --- X86LabelHelper ---
int X86LabelHelper :: fixNearLabel(pos_t labelPos, MemoryWriter& writer)
{
   int offset = writer.position() - labelPos - 4;

   writer.Memory()->write(labelPos, &offset, 4);

   return offset;
}

int X86LabelHelper :: fixShortLabel(pos_t labelPos, MemoryWriter& writer)
{
   int offset = writer.position() - labelPos - 1;

   // if we are unlucky we must change jump from short to near
   if (_abs(offset) > 0x80) {
      convertShortToNear(labelPos, offset, writer);
   }
   else *(char*)(&(*writer.Memory())[labelPos]) = (char)offset;

   return offset;
}

void X86LabelHelper :: convertShortToNear(pos_t position, int offset, MemoryWriter& writer)
{
   MemoryBase* memory = writer.Memory();

   char opcode = (*memory)[position - 1];

   // if jmp opcode
   if ((unsigned char)opcode == 0xEB) {
      writer.insertByte(position + 1, 0);
      writer.insertByte(position + 2, 0);
      writer.insertByte(position + 3, 0);

      *(unsigned char*)(&(*memory)[position - 1]) = 0xE9;
      (*memory)[position] = offset;

      shiftLabels(position, 0, 3);
      fixJumps(position + 2, 3, writer);
   }
   else {
      *(char*)(&(*memory)[position - 1]) = 0x0F;
      *(char*)(&(*memory)[position]) = (char)opcode + 0x10; // to change from short to near
      writer.insertDWord(position + 1, offset);

      shiftLabels(position, 1, 4);
      fixJumps(position + 1, 4, writer);
   }

}

void X86LabelHelper :: fixJumps(pos_t position, int size, MemoryWriter& writer)
{
   MemoryBase* memory = writer.Memory();

   MemoryMap<pos_t, int, Map_StoreUInt, Map_GetUInt> promotions(0);

   auto it = jumps.start();
   while (!it.eof()) {
      pos_t labelPos = (*it).position;
      int   offset = (*it).offset;

      // skip if the inserted label to prevent infinite loop
      if (position == labelPos) {
      }
      // if we inserted the code into existing jump - should be fixed
      // in case of back jump
      else if (offset < 0 && position < labelPos && position > labelPos + offset) {
         offset -= size;

         if (abs(offset) < 0x80) {
            *(char*)(&(*memory)[labelPos]) = (char)offset;
         }
         else {
            // if we are unlucky we must change jump from short to near
            if (abs((*it).offset) < 0x80) {
               promotions.add(it.key(), offset);
            }
            else (*memory)[labelPos] = offset;
         }
         (*it).offset = offset;
      }
      // in case of forward jump
      else if (offset > 0 && position >= labelPos && position <= labelPos + offset/* + size*/) {
         offset += size;

         if (offset < 0x82) {
            *(char*)(&(*memory)[labelPos]) = (char)offset;
         }
         else {
            int opcode = (*memory)[labelPos - 1];

            // call is always near
            // !! should we check 0xB9 (load) as well?
            if ((unsigned char)opcode == 0xE8) {
               (*memory)[labelPos] = offset;
            }
            // if we are unlucky we must change jump from short to near
            else if ((*it).offset < 0x82) {
               promotions.add(it.key(), offset);
               // include command size into offset because we change from short into near (2 -> 5 or 6)
               if ((unsigned char)opcode == 0xEB) {
                  // for jmp
                  offset += 3;
               }
               else offset += 4;
            }
            else (*memory)[labelPos] = offset;
         }
         (*it).offset = offset;
      }
      it++;
   }

   // convert from short to near
   auto p_it = promotions.start();
   while (!p_it.eof()) {
      auto jump = jumps.get(p_it.key());

      convertShortToNear(jump.position, *p_it, writer);

      ++p_it;
   }
}

bool X86LabelHelper :: fixLabel(pos_t label, MemoryWriter& writer)
{
   auto it = jumps.getIt(label);

   while (!it.eof() && it.key() == label) {
      pos_t labelPos = (*it).position;

      // get jump byte
      int opcode = 0;
      writer.Memory()->read(labelPos - 1, &opcode, sizeof(opcode));
      if (isShortJump((char)opcode)) {
         (*it).offset = fixShortLabel(labelPos, writer);
      }
      else (*it).offset = fixNearLabel(labelPos, writer);

      it = jumps.nextIt(label, it);
   }

   return true;
}
