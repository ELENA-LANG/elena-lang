//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains ELENA coder opcode helpers
//		Supported platforms: PPC64 (Power8)
//                             (C)2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef ppc64helperH
#define ppc64helperH 1

namespace _ELENA_
{

// --- PPC64Helper ---
class PPC64Helper
{
public:
   enum OperandType
   {
      otUnknown   = 0,

      // ******************************
      // *** General Purpose Registers
      // ******************************
      otR0 = 0x00000000,
      otR1 = 0x00000001,
      otR2 = 0x00000002,
      otR3 = 0x00000003,
      otR4 = 0x00000004,
      otR5 = 0x00000005,
      otR6 = 0x00000006,
      otR7 = 0x00000007,
      otR8 = 0x00000008,
      otR9 = 0x00000009,
      otR10 = 0x0000000A,
      otR11 = 0x0000000B,
      otR12 = 0x0000000C,
      otR13 = 0x0000000D,
      otR14 = 0x0000000E,
      otR15 = 0x0000000F,
      otR16 = 0x00000010,
      otR17 = 0x00000011,
      otR18 = 0x00000012,
      otR19 = 0x00000013,
      otR20 = 0x00000014,
      otR21 = 0x00000015,
      otR22 = 0x00000016,
      otR23 = 0x00000017,
      otR24 = 0x00000018,
      otR25 = 0x00000019,
      otR26 = 0x0000001A,
      otR27 = 0x0000001B,
      otR28 = 0x0000001C,
      otR29 = 0x0000001D,
      otR30 = 0x0000001E,
      otR31 = 0x0000001F,
   };
      
   struct Operand
   {
//      bool          ebpReg;		// to resolve conflict between [ebp] and disp32
//      bool          factorReg;   // to implement [r*factor] SIB
      OperandType   type;
//      int           offset;
//      ref_t         reference;

      Operand()
      {
         type = otUnknown;
//         factorReg = ebpReg = false;
//         reference = offset = 0;
//         //prefix = spNone;
      }
      Operand(OperandType type)
      {
         this->type = type;
//         this->ebpReg = (this->type == otRBP);
//         this->reference = this->offset = 0;
//         //this->prefix = spNone;
//         this->factorReg = false;
      }
//      Operand(int number)
//      {
//         this->type = (OperandType)number;
//         this->ebpReg = (this->type == otRBP);
//         this->reference = this->offset = 0;
//         //this->prefix = spNone;
//         this->factorReg = false;
//      }
   };

//   enum AMD64JumpType
//   {
//      JUMP_TYPE_JB = 0x02,
//      JUMP_TYPE_JAE = 0x03,
//      JUMP_TYPE_JZ = 0x04,
//      JUMP_TYPE_JE = 0x04,
//      JUMP_TYPE_JNZ = 0x05,
//      JUMP_TYPE_JBE = 0x06,
//      JUMP_TYPE_JA = 0x07,
//      JUMP_TYPE_JS = 0x08,
//      JUMP_TYPE_JNS = 0x09,
//      JUMP_TYPE_JP = 0x0A,
//      JUMP_TYPE_JPO = 0x0B,
//      JUMP_TYPE_JL = 0x0C,
//      JUMP_TYPE_JGE = 0x0D,
//      JUMP_TYPE_JLE = 0x0E,
//      JUMP_TYPE_JG = 0x0F,
//
//      JUMP_TYPE_JMP = 0x00,
//   };
//
//   static OperandType getOperandType(OperandType type, OperandType prefix)
//   {
//      if (prefix != otNone) {
//         if (test(type, otR64)) {
//            type = (OperandType)((type & ~otR64) | prefix);
//         }
//         else if (test(type, otR32)) {
//            type = (OperandType)((type & ~otR32) | prefix);
//         }
//         else if (type == otDB) {
//            type = (OperandType)((type & ~otDB) | otDD | prefix);
//         }
//         else type = (OperandType)(type | prefix);
//      }
//      return type;
//   }
//
//   static OperandType getOperandType(OperandType baseType, OperandType prefix, int offset)
//   {
//      OperandType type = getOperandType(baseType, prefix);
//      if (abs(offset) < 0x80) {
//         type = (OperandType)(type | otM64disp8);
//      }
//      else type = (OperandType)(type | otM64disp32);
//
//      return type;
//   }
//
//   static OperandType addPrefix(OperandType type, OperandType prefix)
//   {
//      if (prefix != otNone) {
//         if (test(type, otR64)) {
//            type = (OperandType)((type & ~otR64) | prefix);
//         }
//         else if (test(type, otRX64)) {
//            if (prefix == otM64) {
//               type = (OperandType)((type & ~otR64) | otMX64);
//            }
//            else type = (OperandType)((type & ~otR64) | prefix);
//         }
//         else if (test(type, otR32)) {
//            type = (OperandType)((type & ~otR32) | prefix);
//         }
//         else if (type == otDB) {
//            if (prefix == otDW) {
//               type = (OperandType)((type & ~otDB) | prefix);
//            }
//            else if (prefix != otDB) {
//               type = (OperandType)((type & ~otDB) | otDD | prefix);
//            }
//         }
//         else if (type == otDD && prefix == otM64) {
//            type = (OperandType)((type & ~otDD) | otDQ | prefix);
//         }
//         else type = (OperandType)(type | prefix);
//      }
//      return type;
//   }
//
//   static OperandType overrideOperand16(OperandType op16)
//   {
//      if (test(op16, otM16)) {
//         op16 = (OperandType)(op16 & ~otM16);
//
//         return (OperandType)(otM32 + op16);
//      }
//      else {
//         op16 = (OperandType)(op16 & ~otR16);
//
//         return (OperandType)(otR32 + op16);
//      }
//   }
//
//   static void writeModRM(MemoryWriter* code, Operand sour, Operand dest)
//   {
//      int prefix = (dest.type & 0x300) >> 2;
//
//      int opcode = prefix + ((char)sour.type << 3) + (char)dest.type;
//
//      code->writeByte((unsigned char)opcode);
//      if ((char)dest.type == otSIB && dest.type != otRSP && dest.type != otAH) {
//         int sib = (char)(dest.type >> 24);
//         if (sib == 0) {
//            sib = 0x24;
//         }
//         code->writeByte((unsigned char)sib);
//      }
//      if (test(dest.type, otM32disp8) || test(dest.type, otM8disp8) || test(dest.type, otM64disp8)) {
//         code->writeByte((unsigned char)dest.offset);
//      }
//      // !! should only otM32 be checked?
//      else if (test(dest.type, otM32disp32) || test(dest.type, otM64disp32) || dest.factorReg) {
//         if (dest.reference != 0) {
//            code->writeRef(dest.reference, dest.offset);
//         }
//         else code->writeDWord(dest.offset);
//      }
//      else if (dest.type == otDisp32) {
//         if (dest.reference != 0) {
//            code->writeRef(dest.reference, dest.offset);
//         }
//         else code->writeDWord(dest.offset);
//      }
//      else if (dest.type == otDisp64) {
//         if (dest.reference != 0) {
//            if ((dest.reference & mskAnyRef) == mskPreloadDataRef) {
//               code->writeRef((dest.reference & ~mskAnyRef) | mskPreloadRelDataRef, dest.offset);
//            }
//            else code->writeRef(dest.reference, dest.offset);
//         }
//         else code->writeDWord(dest.offset);
//      }
//   }
//
//   static void leaRM64disp(MemoryWriter* code, OperandType sour, OperandType dest, int destOffs)
//   {
//      Operand sourOp(sour);
//      Operand destOp(getOperandType(dest, otM64, destOffs));
//
//      destOp.offset = destOffs;
//
//      code->writeByte(0x48);
//      code->writeByte(0x8D);
//      writeModRM(code, sourOp, destOp);
//   }
//
//   static void writeImm(MemoryWriter* code, Operand sour)
//   {
//      if (sour.type == otDD) {
//         if (sour.reference != 0) {
//            code->writeRef(sour.reference, sour.offset);
//         }
//         else code->writeDWord(sour.offset);
//      }
//      else if (sour.type == otDQ) {
//         if (sour.reference != 0) {
//            code->writeRef(sour.reference, sour.offset);
//            code->writeDWord(0);
//         }
//         else code->writeQWord(sour.offset);
//      }
//      else if (sour.type == otDB) {
//         code->writeByte((unsigned char)sour.offset);
//      }
//      else if (sour.type == otDW) {
//         code->writeWord((unsigned short)sour.offset);
//      }
//   }
//
//   static void writeImm(MemoryWriter* code, Operand sour, bool override)
//   {
//      if (override && sour.type == otDD) {
//         sour.type = otDW;
//
//         writeImm(code, sour);
//      }
//      else writeImm(code, sour);
//   }
};
//
//class AMD64LabelHelper
//{
//   struct AMD64JumpInfo
//   {
//      pos_t position;
//      int offset;
//
//      AMD64JumpInfo()
//      {
//         position = (pos_t)-1;
//         offset = 0;
//      }
//      AMD64JumpInfo(pos_t position)
//      {
//         this->position = position;
//         this->offset = 0;
//      }
//      AMD64JumpInfo(pos_t position, int offset)
//      {
//         this->position = position;
//         this->offset = offset;
//      }
//   };
//
//   MemoryWriter* code;
//   MemoryMap<int, AMD64JumpInfo, false> jumps;
//   MemoryMap<int, pos_t, false>         labels;
//
//public:
//   void addFixableJump(pos_t position, int offset)
//   {
//      jumps.add(0, AMD64JumpInfo(position, offset));
//   }
//
//   bool isShortJump(unsigned char opcode);
//
//   bool checkLabel(int label)
//   {
//      return labels.exist(label);
//   }
//
//   void setLabel(int label);
//
//   void writeJxxForward(int label, AMD64Helper::AMD64JumpType prefix);
//   void writeShortJxxForward(int label, AMD64Helper::AMD64JumpType prefix);
//
//   void writeJxxBack(AMD64Helper::AMD64JumpType prefix, int label);
//   void writeNearJxxBack(AMD64Helper::AMD64JumpType prefix, int label);
//   void writeShortJxxBack(AMD64Helper::AMD64JumpType prefix, int label);
//
//   void writeLoadForward(int label);
//   void writeJmpForward(int label);
//   void writeShortJmpForward(int label);
//
//   void writeLoadBack(int label);
//   void writeJmpBack(int label);
//   void writeNearJmpBack(int label);
//   void writeShortJmpBack(int label);
//
//   //void writeLoopForward(int label);
//   //void writeLoopBack(int label);
//
//   //void writeCallForward(int label);
//   //void writeCallBack(int label);
//
//   int fixShortLabel(pos_t labelPos);
//   int fixNearLabel(pos_t labelPos);
//   void fixLabel(int label);
//
//   void shiftLabels(pos_t position, int firstDisplacement, int displacement)
//   {
//      shift(labels.start(), position + 1, displacement);
//
//      auto it = jumps.start();
//      while (!it.Eof()) {
//         if ((*it).position == position) {
//            (*it).position += firstDisplacement;
//         }
//         else if ((*it).position > position)
//            (*it).position += displacement;
//
//         it++;
//      }
//   }
//
//   void fixJumps(pos_t position, int size);
//   void convertShortToNear(pos_t position, int offset);
//
//   AMD64LabelHelper(MemoryWriter* code)
//   {
//      this->code = code;
//   }
//};

} // _ELENA_

#endif // ppc64helperH
