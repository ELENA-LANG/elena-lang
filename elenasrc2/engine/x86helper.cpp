//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA ELENA coder opcode helper
//		classes.
//                                              (C)2005-2009, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "x86helper.h"

using namespace _ELENA_;

// --- x86LabelHelper Class ---

bool x86LabelHelper :: isShortJump(unsigned char opcode)
{
   // if it is jump of address load
   if (opcode == 0xE8 || opcode == 0xB9)
      return false;
   else if ((opcode >= 0x80 && opcode < 0x90)  || opcode == 0xE9) {
      return false;
   }
   else return true;
}

void x86LabelHelper :: writeJxxForward(int label, x86Helper::x86JumpType prefix)
{
	code->writeByte(0x0F);
   code->writeByte((unsigned char)(0x80 + prefix));

   jumps.add(label, x86JumpInfo(code->Position()));
   code->writeDWord(0);
}

void x86LabelHelper :: writeShortJxxForward(int label, x86Helper::x86JumpType prefix)
{
   code->writeByte((unsigned char)(0x70 + prefix));

   jumps.add(label, x86JumpInfo(code->Position()));
   code->writeByte(0);
}

void x86LabelHelper :: writeJxxBack(x86Helper::x86JumpType prefix, int label)
{
   int offset = labels.get(label) - code->Position();

   if (abs(offset) < 0x80) {
      writeShortJxxBack(prefix, label);
   }
   else writeNearJxxBack(prefix, label);
}

void x86LabelHelper :: writeNearJxxBack(x86Helper::x86JumpType prefix, int label)
{
   int offset = labels.get(label) - code->Position();

	code->writeByte(0x0F);
   code->writeByte((unsigned char)(0x80 + prefix));

   // to exclude the command itself
   offset -= 6;

   jumps.add(label, x86JumpInfo(code->Position(), offset));

   code->writeDWord(offset);
}

void x86LabelHelper :: writeShortJxxBack(x86Helper::x86JumpType prefix, int label)
{
   int offset = labels.get(label) - code->Position();

   code->writeByte((unsigned char)(0x70 + prefix));

   // to exclude the command itself
   offset -= 2;

   jumps.add(label, x86JumpInfo(code->Position(), offset));

   code->writeByte((unsigned char)offset);
}

void x86LabelHelper :: writeJmpForward(int label)
{
   code->writeByte(0xE9);

   jumps.add(label, x86JumpInfo(code->Position()));

   code->writeDWord(0);
}

void x86LabelHelper :: writeLoadForward(int label)
{
   code->writeByte(0xB9);

   jumps.add(label, x86JumpInfo(code->Position()));

   code->writeDWord(0);
}

void x86LabelHelper :: writeLoadBack(int label)
{
   code->writeByte(0xB9);

   int offset = labels.get(label) - code->Position() - 4;

   code->writeDWord(offset);
}

void x86LabelHelper :: writeShortJmpForward(int label)
{
   code->writeByte(0xEB);

   jumps.add(label, x86JumpInfo(code->Position()));

   code->writeByte(0);
}

void x86LabelHelper :: writeJmpBack(int label)
{
   int offset = labels.get(label) - code->Position();

   if (abs(offset) < 0x7F) {
      writeShortJmpBack(label);
   }
   else writeNearJmpBack(label);
}

void x86LabelHelper :: writeNearJmpBack(int label)
{
   int offset = labels.get(label) - code->Position();

   code->writeByte(0xE9);

   // to exclude the command itself
   offset -= 5;

   jumps.add(label, x86JumpInfo(code->Position(), offset));

   code->writeDWord(offset);
}

void x86LabelHelper :: writeShortJmpBack(int label)
{
   int offset = labels.get(label) - code->Position();

   code->writeByte(0xEB);

   // to exclude the command itself
   offset -= 2;

   jumps.add(label, x86JumpInfo(code->Position(), offset));

   code->writeByte((unsigned char)offset);
}

void x86LabelHelper :: writeLoopForward(int label)
{
   code->writeByte(0xE2);

   jumps.add(label, x86JumpInfo(code->Position()));
   code->writeByte(0);
}

void x86LabelHelper :: writeLoopBack(int label)
{
   int offset = labels.get(label) - code->Position();

	code->writeByte(0xE2);

   // to exclude the command itself
   offset -= 2;

   jumps.add(label, x86JumpInfo(code->Position(), offset));

   code->writeByte((unsigned char)offset);
}

void x86LabelHelper :: writeCallForward(int label)
{
   code->writeByte(0xE8);

   jumps.add(label, x86JumpInfo(code->Position()));
   code->writeDWord(0);
}

void x86LabelHelper :: writeCallBack(int label)
{
   int offset = labels.get(label) - code->Position();

   code->writeByte(0xE8);

   // to exclude the command itself
   offset -= 5;

   jumps.add(label, x86JumpInfo(code->Position(), offset));

   code->writeDWord(offset);
}

void x86LabelHelper :: setLabel(int label)
{   
   labels.add(label, code->Position());

   fixLabel(label);
}

int x86LabelHelper :: fixShortLabel(size_t labelPos)
{
   int offset = code->Position() - labelPos - 1;

   // if we are unlucky we must change jump from short to near
   if (abs(offset) > 0x80) {
      convertShortToNear(labelPos, offset);
   }
   else *(char*)(&(*code->Memory())[labelPos]) = (char)offset;

   return offset;
}

int x86LabelHelper :: fixNearLabel(size_t labelPos)
{
   int offset = code->Position() - labelPos - 4;

   (*code->Memory())[labelPos] = offset;

   return offset;
}

void x86LabelHelper :: fixLabel(const int label)
{
   MemoryMap<int, x86JumpInfo>::Iterator it = jumps.getIt(label);

   while (!it.Eof() && it.key() == label) {
      int labelPos = (*it).position;

      // get jump byte
      int opcode = (*code->Memory())[labelPos - 1];
      if (isShortJump((char)opcode)) {
         (*it).offset = fixShortLabel(labelPos);
      }
      else (*it).offset = fixNearLabel(labelPos);

      it = jumps.getNextIt(label, it);
   }
}

void x86LabelHelper :: fixJumps(int position, int size)
{
   _Memory* memory = code->Memory();

   MemoryMap<int, int> promotions;

   MemoryMap<int, x86JumpInfo>::Iterator it = jumps.start();
   while (!it.Eof()) {
      int labelPos = (*it).position;
      int offset = (*it).offset;

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
   MemoryMap<int, int>::Iterator p_it = promotions.start();
   while (!p_it.Eof()) {
      x86JumpInfo jump = jumps.get(p_it.key());

      convertShortToNear(jump.position, *p_it);

      p_it++;
   }
}

void x86LabelHelper :: convertShortToNear(int position, int offset)
{
   _Memory* memory = code->Memory();

   int opcode = (*memory)[position - 1];

   // if jmp opcode
   if ((unsigned char)opcode == 0xEB) {
      code->insertByte(position + 1, 0);
      code->insertByte(position + 2, 0);
      code->insertByte(position + 3, 0);

      *(unsigned char*)(&(*memory)[position - 1]) = 0xE9;
      (*memory)[position] = offset;

      shiftLabels(position, 0, 3);
      fixJumps(position + 2, 3);
   }
   else {
      *(char*)(&(*memory)[position - 1]) = 0x0F;
      *(char*)(&(*memory)[position]) = (char)opcode + 0x10; // to change from short to near
      code->insertDWord(position + 1, offset);

      shiftLabels(position, 1, 4);
      fixJumps(position + 1, 4);
   }
}
