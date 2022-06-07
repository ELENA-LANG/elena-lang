//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains CPU native helpers
//		Supported platforms: ARM64
//                                            (C)2021-2022, by Aleksey Rakov
//                                            (C)2016, from The LLVM Compiler
//---------------------------------------------------------------------------

#ifndef ARMHELPER_H
#define ARMHELPER_H
#include "ppchelper.h"

namespace elena_lang
{
   enum class JumpType
   {
      EQ = 0x0,
      NE = 0x1,
      CS = 0x2,
      CC = 0x3,
      MI = 0x4,
      PL = 0x5,
      VS = 0x6,
      VC = 0x7,
      HI = 0x8,
      LS = 0x9,
      GE = 0xA,
      LT = 0xB,
      GT = 0xC,
      LE = 0xD,
      AL = 0xE
   };

   // --- ARMOperandType ---
   enum class ARMOperandType : unsigned int
   {
      None      = 0,

      X0        = 0x0100,
      X1        = 0x0101,
      X2        = 0x0102,
      X3        = 0x0103,
      X4        = 0x0104,
      X5        = 0x0105,
      X6        = 0x0106,
      X7        = 0x0107,
      X8        = 0x0108,
      X9        = 0x0109,
      X10       = 0x010A,
      X11       = 0x010B,
      X12       = 0x010C,
      X13       = 0x010D,
      X14       = 0x010E,
      X15       = 0x010F,
      X16       = 0x0110,
      X17       = 0x0111,
      X18       = 0x0112,
      X19       = 0x0113,
      X20       = 0x0114,
      X21       = 0x0115,
      X22       = 0x0116,
      X23       = 0x0117,
      X24       = 0x0118,
      X25       = 0x0119,
      X26       = 0x011A,
      X27       = 0x011B,
      X28       = 0x011C,
      X29       = 0x011D,
      X30       = 0x011E,
      SP        = 0x011F,
      XZR       = 0x111F,

      W0        = 0x1000,
      W1        = 0x1001,
      W2        = 0x1002,
      W3        = 0x1003,
      W4        = 0x1004,
      W5        = 0x1005,
      W6        = 0x1006,
      W7        = 0x1007,
      W8        = 0x1008,
      W9        = 0x1009,
      W10       = 0x100A,
      W11       = 0x100B,
      W12       = 0x100C,
      W13       = 0x100D,
      W14       = 0x100E,
      W15       = 0x100F,
      W16       = 0x1010,
      W17       = 0x1011,
      W18       = 0x1012,
      W19       = 0x1013,
      W20       = 0x1014,
      W21       = 0x1015,
      W22       = 0x1016,
      W23       = 0x1017,
      W24       = 0x1018,
      W25       = 0x1019,
      W26       = 0x101A,
      W27       = 0x101B,
      W28       = 0x101C,
      W29       = 0x101D,
      W30       = 0x101E,

      XR        = 0x0100,
      Preindex  = 0x0200,
      Postindex = 0x0400,
      Unsigned  = 0x0800,
      WR        = 0x1000,

      Imm       = 0x300,
   };

   inline bool test(ARMOperandType type, ARMOperandType mask)
   {
      return (((unsigned int)type & (unsigned int)mask) == (unsigned int)mask);
   }
   inline ARMOperandType operator | (ARMOperandType arg1, ARMOperandType arg2)
   {
      return (ARMOperandType)(static_cast<unsigned int>(arg1) | static_cast<unsigned int>(arg2));
   }

   // --- ARMOperand ---
   struct ARMOperand
   {
      ARMOperandType type;
      int            imm;
      ref_t          reference;

      bool isXR()
      {
         return test(type, ARMOperandType::XR) && !test(type, ARMOperandType::Preindex);
      }
      bool isWR()
      {
         return test(type, ARMOperandType::WR) && !test(type, ARMOperandType::Preindex);
      }
      bool isPreindex()
      {
         return test(type, ARMOperandType::Preindex);
      }
      bool isPostindex()
      {
         return test(type, ARMOperandType::Postindex);
      }
      bool isUnsigned()
      {
         return test(type, ARMOperandType::Unsigned);
      }

      ARMOperand()
      {
         type = ARMOperandType::None;
         imm = 0;
         reference = 0;
      }
      ARMOperand(ARMOperandType type)
      {
         this->type = type;
         imm = 0;
         reference = 0;
      }
      ARMOperand(ARMOperandType type, int imm)
      {
         this->type = type;
         this->imm = imm;
         this->reference = 0;
      }
   };

   // --- ARMHelper ---
   class ARMHelper
   {
   public:
      static unsigned int makeOpcode(int sf, int op, int op2, int op3, ARMOperandType rm, int op4, int o1,
         ARMOperandType rn, ARMOperandType rd)
      {
         return (sf << 31) | (op << 30) | (op2 << 29) | (op2 << 21) | (((unsigned int)rm & 0x1F) << 16) | (op4 << 11)
            | (o1 << 10) | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeOpcode(int sf, int op1, int op2, int op3, ARMOperandType rm, int op4, ARMOperandType ra,
         ARMOperandType rn, ARMOperandType rd)
      {
         return (sf << 31) | (op1 << 29) | (op2 << 24) | (op3 << 23) | (((unsigned int)rm & 0x1F) << 16)
            | (op4 << 15) | (((unsigned int)ra & 0x1F) << 10) | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeOpcode(int opc, int op1, int op2, int op3, int l, int imm, ARMOperandType rt2, ARMOperandType rn, ARMOperandType rt)
      {
         return (opc << 30) | (op1 << 27) | (op2 << 26) | (op3 << 23) | (l << 22) | ((imm & 0x7F) << 15) | (((unsigned int)rt2 & 0x1F) << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rt & 0x1F);
      }

      static unsigned int makeROpcode(int op, int op1, int imm, ARMOperandType rt)
      {
         return (op << 31) | ((imm & 3) << 29) | (op1 << 24) | (((imm >> 2) & 0x7FFFF) << 5)| ((unsigned int)rt & 0x1F);
      }

      static unsigned int makeImm6ShiftOpcode(int size, int op, int s, int op2, int shift, int imm6, ARMOperandType rm, 
         ARMOperandType rn, ARMOperandType rd)
      {
         return (size << 31) | (op << 30) | (s << 29) | (op2 << 24) | (shift << 22)
            | (((unsigned int)rm & 0x1F) << 16) | (imm6 << 10) | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeImm9Opcode(int size, int op1, int op2, int op3, int opc, int op4, int imm9, int op5, ARMOperandType rn, ARMOperandType rt)
      {
         return (size << 30) | (op1 << 27) | (op2 << 26) | (op3 << 24) | (opc << 22) | (op4 << 21) | ((imm9 & 0x1FF) << 12) | (op5 << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rt & 0x1F);
      }

      static unsigned int makeImm12Opcode(int size, int op1, int op2, int op3, int opc, int imm12, ARMOperandType rn, ARMOperandType rt)
      {
         return (size << 30) | (op1 << 27) | (op2 << 26) | (op3 << 24) | (opc << 22) | ((imm12 & 0xFFF) << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rt & 0x1F);
      }

      static unsigned int makeImm16Opcode(int sf, int opc, int op1, int hw, int imm16, ARMOperandType rd)
      {
         return (sf << 31) | (opc << 29) | (op1 << 23) | (hw << 21) | 
            ((imm16 & 0xFFFF) << 5)  | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeImm12SOpcode(int sf, int op, int s, int op1, int sh, int imm, ARMOperandType rn, ARMOperandType rt)
      {
         return (sf << 31) | (op << 30) | (s << 29) | (op1 << 23) | (sh << 22) | ((imm & 0xFFF) << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rt & 0x1F);
      }

      static bool isMask_64(uint64_t Value) {
         return Value && ((Value + 1) & Value) == 0;
      }

      static bool isShiftedMask_64(uint64_t Value) {
         return Value && isMask_64((Value - 1) | Value);
      }

      static uint64_t countTrailingOnes(uint64_t value)
      {
         uint64_t current = 1;
         uint64_t counter = 0;
         while (test64(value, current)) {
            counter++;
            current <<= 1;
         }

         return counter;
      }

      static uint32_t countLeadingOnes(uint32_t value)
      {
         uint32_t current = 0x80000000;
         uint32_t counter = 0;
         while (test(value, current)) {
            counter++;
            current >>= 1;
         }

         return counter;
      }

      static uint64_t countTrailingZeros(uint64_t value)
      {
         return countTrailingOnes(~value);
      }

      static bool decodeLogicalImmediate(uint64_t imm, int& N, int& imms, int& immr) {
         if (imm == 0ULL || imm == ~0ULL)
            return false;

         // First, determine the element size.
         unsigned Size = 64;

         do {
            Size /= 2;
            uint64_t Mask = (1ULL << Size) - 1;

            if ((imm & Mask) != ((imm >> Size) & Mask)) {
               Size *= 2;
               break;
            }
         } while (Size > 2);

         // Second, determine the rotation to make the element be: 0^m 1^n.
         uint32_t CTO, I;
         uint64_t Mask = ((uint64_t)-1LL) >> (64 - Size);
         imm &= Mask;

         if (isShiftedMask_64(imm)) {
            I = countTrailingZeros(imm);
            CTO = countTrailingOnes(imm >> I);
         }
         else {
            imm |= ~Mask;
            if (!isShiftedMask_64(~imm))
               return false;

            unsigned CLO = countLeadingOnes(imm);
            I = 64 - CLO;
            CTO = CLO + countTrailingOnes(imm) - (64 - Size);
         }

         // Encode in Immr the number of RORs it would take to get *from* 0^m 1^n
         // to our target value, where I is the number of RORs to go the opposite
         // direction.
         immr = (Size - I) & (Size - 1);

         // If size has a 1 in the n'th bit, create a value that has zeroes in
         // bits [0, n] and ones above that.
         imms = ~(Size - 1) << 1;

         // Or the CTO value into the low bits, which must be below the Nth bit
         // bit mentioned above.
         imms |= (CTO - 1);
         imms &= 0x3f;

         // Extract the seventh bit and toggle it to create the N field.
         N = ((imms >> 6) & 1) ^ 1;

         return true;
      }

      static unsigned int makeLogocalImm13Opcode(int sf, int op, int s, int imm, ARMOperandType rn, ARMOperandType rd)
      {
         int n = 0;
         int imms = 0;
         int immr = 0;

         decodeLogicalImmediate(imm, n, imms, immr);

         return (sf << 31) | (op << 29) | (s << 23) | (n << 22) | (immr << 16) | (imms << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeImmRImmSOpcode(int sf, int op, int s, int immr, int imms, ARMOperandType rn, ARMOperandType rd)
      {
         return (sf << 31) | (op << 29) | (s << 23) | (1 << 22) | ((immr & 0x3F) << 16) | ((imms & 0x3F) << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeBOpcode(int op1, int z, int op2, int op, int op3, int op4, int a, int m, ARMOperandType rn, ARMOperandType rm)
      {
         return (op1 << 25) | (z << 24) | (op2 << 23) | (op << 21) | (op3 << 16) | (op4 << 12) | (a << 11) | (m << 10)
            | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rm & 0x1F);
      }

      static unsigned int makeBxxOpcode(int op, int z, int imm19, int op2, int cond)
      {
         return (op << 25) | (z << 24) | ((imm19 & 0x7FFFF) << 5) | (op2 << 4) | cond;
      }

      static unsigned int makeCondOpcode(int sf, int op, int op2, ARMOperandType rm, int cond, int o2, 
         ARMOperandType rn, ARMOperandType rd)
      {
         return (sf << 31) | (op << 30) | (op2 << 21) | (((unsigned int)rm & 0x1F) << 16) | (cond << 12)
            | (o2 << 10) | (((unsigned int)rn & 0x1F) << 5) | ((unsigned int)rd & 0x1F);
      }

      static unsigned int makeBOpcode(int op, int imm26)
      {
         return (op << 26) | ((imm26 & 0xFFFFFFF) >> 2);
      }

      static bool isBxxCommand(void* opcodePtr)
      {
         int opcode = *(int*)opcodePtr;

         opcode >>= 25;

         return opcode == 0x2A;
      }

      static bool isBCommand(void* opcodePtr)
      {
         int opcode = *(int*)opcodePtr;

         opcode >>= 26;

         return opcode == 0x5;
      }

      static void fixBxxCommand(void* opcode, int imm19)
      {
         *(unsigned int*)opcode = *(unsigned int*)opcode | ((imm19 >> 2) << 5);
      }

      static void fixBCommand(void* opcode, int imm26)
      {
         *(unsigned int*)opcode = *(unsigned int*)opcode | ((imm26 & 0xFFFFFFF) >> 2);
      }

      static void fixMovZCommand(void* opcode, int imm16)
      {
         *(unsigned int*)opcode = *(unsigned int*)opcode | ((imm16 & 0xFFFF) << 5);
      }
   };

   // --- ARMLabelHelper ---
   struct ARMLabelHelper : LabelHelper
   {
      static void writeB(int imm, MemoryWriter& writer)
      {
         writer.writeDWord(ARMHelper::makeBOpcode(0x5, imm));
      }

      static void writeBcc(int imm, int cond, MemoryWriter& writer)
      {
         writer.writeDWord(ARMHelper::makeBxxOpcode(0x2A, 0, imm >> 2, 0, cond));
      }

      bool fixLabel(pos_t label, MemoryWriter& writer) override
      {
         for (auto it = jumps.getIt(label); !it.eof(); ++it) {
            if (it.key() == label) {
               ref_t labelPos = (*it).position;
               int offset = writer.position() - labelPos;

               void* opcode = writer.Memory()->get(labelPos);
               if (ARMHelper::isBxxCommand(opcode)) {
                  if (abs(offset) > 0x1FFFF)
                     return false;

                  ARMHelper::fixBxxCommand(opcode, offset);
               }
               else if (ARMHelper::isBCommand(opcode)) {
                  if (abs(offset) > 0x3FFFFFF)
                     return false;

                  ARMHelper::fixBCommand(opcode, offset);
               }
            }
         }
         return true;
      }

      void writeJumpBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0x3FFFF)
            throw InternalError(-1);

         writeB(offset, writer);
      }

      void writeJumpForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeB(0, writer);
      }

      void writeJeqForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBcc(0, (int)JumpType::EQ, writer);
      }

      void writeJeqBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0x3FFFF)
            throw InternalError(-1);

         writeBcc(offset, (int)JumpType::EQ, writer);
      }

      void writeJneForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBcc(0, (int)JumpType::NE, writer);
      }

      void writeJneBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0x3FFFF)
            throw InternalError(-1);

         writeBcc(offset, (int)JumpType::NE, writer);
      }
   };
}

#endif
