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
int X86LabelHelper :: fixNearJccLabel(pos_t jumpPos, MemoryWriter& writer)
{
   int offset = writer.position() - jumpPos - 6;

   writer.Memory()->write(jumpPos + 2, &offset, 4);

   return offset;
}

int X86LabelHelper::fixNearJmpLabel(pos_t jumpPos, MemoryWriter& writer)
{
   int offset = writer.position() - jumpPos - 5;

   writer.Memory()->write(jumpPos + 1, &offset, 4);

   return offset;
}

int X86LabelHelper :: fixShortLabel(pos_t jumpPos, MemoryWriter& writer)
{
   int offset = writer.position() - jumpPos - 2;

   // if we are unlucky we must change jump from short to near
   if (_abs(offset) > 0x80) {
      convertShortToNear(jumpPos, offset, writer);
   }
   else *(char*)(&(*writer.Memory())[jumpPos + 1]) = (char)offset;

   return offset;
}

void X86LabelHelper :: convertShortToNear(pos_t position, int offset, MemoryWriter& writer)
{
   MemoryBase* memory = writer.Memory();

   char opcode = (*memory)[position];

   // if jmp opcode
   if ((unsigned char)opcode == 0xEB) {
      writer.insertByte(position + 1, 0);
      writer.insertByte(position + 2, 0);
      writer.insertByte(position + 3, 0);

      *(unsigned char*)(&(*memory)[position]) = 0xE9;
      MemoryBase::writeDWord(memory, position + 1, offset);

      shiftLabels(position, 0, 3);
      fixJumps(position + 1, 3, writer);
   }
   else {
      *(char*)(&(*memory)[position]) = 0x0F;
      *(char*)(&(*memory)[position + 1]) = (char)opcode + 0x10; // to change from short to near
      writer.insertDWord(position + 2, offset);

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
      pos_t jumpPos = (*it).position;
      int   offset = (*it).offset;

      // skip if the inserted label to prevent infinite loop
      if (position == jumpPos) {
      }
      // if we inserted the code into existing jump - should be fixed
      // in case of back jump
      else if (offset < 0 && position < jumpPos && position > jumpPos + offset) {
         offset -= size;

         if (abs(offset) < 0x80) {
            *(char*)(&(*memory)[jumpPos + 1]) = (char)offset;
         }
         else {
            char opcode = (*memory)[jumpPos];
            // if jmp opcode
            if ((unsigned char)opcode == 0xEB) {
               if (abs((*it).offset) < 0x80) {
                  // if we are unlucky we must change jump from short to near
                  promotions.add(it.key(), offset);
               }
               else MemoryBase::writeDWord(memory, jumpPos + 1, offset);
            }
            else {
               if (abs((*it).offset) < 0x80) {
                  // if we are unlucky we must change jump from short to near
                  promotions.add(it.key(), offset);
               }
               else MemoryBase::writeDWord(memory, jumpPos + 2, offset);
            }
         }
         (*it).offset = offset;
      }
      // in case of forward jump
      else if (offset > 0 && position >= jumpPos && position <= jumpPos + offset/* + size*/) {
         offset += size;

         if (offset < 0x82) {
            *(char*)(&(*memory)[jumpPos + 1]) = (char)offset;
         }
         else {
            int opcode = (*memory)[jumpPos];

            // call is always near
            // !! should we check 0xB9 (load) as well?
            if ((unsigned char)opcode == 0xE8) {
               MemoryBase::writeDWord(memory, jumpPos + 1, offset);
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
            else MemoryBase::writeDWord(memory, jumpPos + 2, offset);
         }
         (*it).offset = offset;
      }
      ++it;
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
      pos_t jumpPos = (*it).position;

      // get jump byte
      char opcode = 0;
      writer.Memory()->read(jumpPos, &opcode, sizeof(opcode));
      if (isShortJump(opcode)) {
         (*it).offset = fixShortLabel(jumpPos, writer);
      }
      else if (isNearJmp(opcode)) {
         (*it).offset = fixNearJmpLabel(jumpPos, writer);
      }
      else (*it).offset = fixNearJccLabel(jumpPos, writer);

      it = jumps.nextIt(label, it);
   }

   for (auto a_it = addresses.getIt(label); !a_it.eof(); a_it = addresses.nextIt(label, a_it)) {
      auto info = *a_it;
      int addr = writer.position() - info.position - 4;

      switch (info.mask) {
         case mskRef32:
            MemoryBase::writeDWord(writer.Memory(), info.position, writer.position());
            writer.Memory()->addReference(mskCodeRef32, info.position);
            break;
         case mskRef64:
            MemoryBase::writeDWord(writer.Memory(), info.position, writer.position());
            writer.Memory()->addReference(mskCodeRef64, info.position);
            break;
         default:
            break;
      }
   }

   return true;
}

void X86LabelHelper :: writeJmpForward(pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();

   writer.writeByte(0xE9);

   jumps.add(label, { position });

   writer.writeDWord(0);
}

void X86LabelHelper :: writeNearJccForward(X86JumpType type, pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();

   writer.writeByte(0x0F);
   writer.writeByte((unsigned char)0x80 + (unsigned char)type);

   jumps.add(label, { position });

   writer.writeDWord(0);
}

void X86LabelHelper :: writeShortJmpForward(pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();

   writer.writeByte(0xEB);

   jumps.add(label, { position });

   writer.writeByte(0);
}

void X86LabelHelper :: writeShortJccForward(X86JumpType type, pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();

   writer.writeByte((unsigned char)0x70 + (unsigned char)type);

   jumps.add(label, { position });

   writer.writeByte(0);
}

void X86LabelHelper :: writeNearJmpBack(pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();
   int offset = labels.get(label) - position;

   writer.writeByte(0xE9);

   // to exclude the command itself
   offset -= 5;

   jumps.add(label, { position, offset });

   writer.writeDWord(offset);
}

void X86LabelHelper :: writeNearJccBack(X86JumpType type, pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();
   int offset = labels.get(label) - position;

   writer.writeByte(0x0F);
   writer.writeByte((unsigned char)0x80 + (unsigned char)type);

   // to exclude the command itself
   offset -= 6;

   jumps.add(label, { position, offset });

   writer.writeDWord(offset);
}

void X86LabelHelper :: writeShortJmpBack(pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();
   int offset = labels.get(label) - position;

   writer.writeByte(0xEB);

   // to exclude the command itself
   offset -= 2;

   jumps.add(label, { position, offset });

   writer.writeByte((unsigned char)offset);
}

void X86LabelHelper :: writeShortJccBack(X86JumpType type, pos_t label, MemoryWriter& writer)
{
   pos_t position = writer.position();
   int offset = labels.get(label) - position;

   writer.writeByte((unsigned char)0x70 + (unsigned char)type);

   // to exclude the command itself
   offset -= 2;

   jumps.add(label, { position, offset });

   writer.writeByte((unsigned char)offset);
}

void X86LabelHelper :: writeJumpBack(pos_t label, MemoryWriter& writer)
{
   int offset = labels.get(label) - writer.position();

   if (abs(offset) < 0x7F) {
      writeShortJmpBack(label, writer);
   }
   else writeNearJmpBack(label, writer);
}

void X86LabelHelper :: writeJccBack(X86JumpType type, pos_t label, MemoryWriter& writer)
{
   int offset = labels.get(label) - writer.position();

   if (abs(offset) < 0x7F) {
      writeShortJccBack(type, label, writer);
   }
   else writeNearJccBack(type, label, writer);
}

void X86LabelHelper :: writeJumpForward(pos_t label, MemoryWriter& writer, int byteCodeOffset)
{
   if (_abs(byteCodeOffset) < 0x10) {
      writeShortJmpForward(label, writer);
   }
   else writeJmpForward(label, writer);
}

void X86LabelHelper :: writeJccForward(X86JumpType type, pos_t label, MemoryWriter& writer, int byteCodeOffset)
{
   if (_abs(byteCodeOffset) < 0x10) {
      writeShortJccForward(type, label, writer);
   }
   else writeNearJccForward(type, label, writer);
}
