//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains ELENA coder opcode helpers
//		Supported platforms: x32-64
//                             (C)2005-2021, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#ifndef amd64helperH
#define amd64helperH 1

namespace _ELENA_
{

// --- AMD64Helper ---
class AMD64Helper
{
public:
   enum OperandType
   {
      otUnknown   = 0,
      otNone      = 0,

      otDD        = 0x00100005,
      otDB        = 0x00200005,
      otDW        = 0x00400005,
      otDQ        = 0x00008005,

      otPtr32     = 0x00100000,
      otPtr8      = 0x00200000,
      otPtr16     = 0x00400000,
      otPtr64     = 0x00008000,
      otPtrX64    = 0x00004000,

      otRX8       = 0x00004300,
      otRX9       = 0x00004301,
      otRX10      = 0x00004302,
      otRX11      = 0x00004303,
      otRX12      = 0x00004304,
      otRX13      = 0x00004305,
      otRX14      = 0x00004306,
      otRX15      = 0x00004307,

      otRAX       = 0x00008300,
      otRCX       = 0x00008301,
      otRDX       = 0x00008302,
      otRBX       = 0x00008303,
      otRSP       = 0x00008304,
      otRBP       = 0x00008305,
      otRSI       = 0x00008306,
      otRDI       = 0x00008307,

      otEAX       = 0x00100300,
      otECX       = 0x00100301,
      otEDX       = 0x00100302,
      otEBX       = 0x00100303,
      otESI       = 0x00100306,
      otEDI       = 0x00100307,

      otAL        = 0x00200300,
      otCL        = 0x00200301,
      otDL        = 0x00200302,
      otBL        = 0x00200303,
      otAH        = 0x00200304,
      otDH        = 0x00200306,
      otBH        = 0x00200307,

      otAX        = 0x00400300,
      otCX        = 0x00400301,
      otDX        = 0x00400302,
      otBX        = 0x00400303,

      otMM0       = 0x00800300,
      otMM1       = 0x00800301,
      otMM2       = 0x00800302,
      otMM3       = 0x00800303,
      otMM4       = 0x00800304,
      otMM5       = 0x00800305,
      otMM6       = 0x00800306,
      otMM7       = 0x00800307,

      otXMM0      = 0x00001300,
      otXMM1      = 0x00001301,
      otXMM2      = 0x00001302,
      otXMM3      = 0x00001303,
      otXMM4      = 0x00001304,
      otXMM5      = 0x00001305,
      otXMM6      = 0x00001306,
      otXMM7      = 0x00001307,

      otR32       = 0x00100300,
      otM32       = 0x00110000,
      otM32disp8  = 0x00110100,
      otM32disp32 = 0x00110200,

      otR64       = 0x00008300,
      otM64       = 0x00018000,
      otM64disp8  = 0x00018100,
      otM64disp32 = 0x00018200,

      otRX64      = 0x00004300,
      otMX64      = 0x00014000,
      otMX64disp8 = 0x00014100,
      otMX64disp32= 0x00014200,
      
      otX128      = 0x00001300,

      otR8        = 0x00200300,
      otM8        = 0x00210000,
      otM8disp8   = 0x00210100,

      otR16       = 0x00400300,
      otM16       = 0x00410000,
      otM16disp8  = 0x00410100,

      otSIB       = 0x00000004,

      otDisp32    = 0x00110005,
      otDisp64    = 0x00018005,

      otFactor2   = 0x40000000,
      otFactor4   = 0x80000000,
      otFactor8   = 0xC0000000
   };
      
   //enum SegmentPrefix
   //{
   //   spNone = 0,
   //   spFS = 0x64
   //};

   struct Operand
   {
      bool          ebpReg;		// to resolve conflict between [ebp] and disp32
      bool          factorReg;   // to implement [r*factor] SIB
      OperandType   type;
      int           offset;
      ref_t         reference;
      //SegmentPrefix prefix;

      Operand()
      {
         type = otUnknown;
         factorReg = ebpReg = false;
         reference = offset = 0;
         //prefix = spNone;
      }
      Operand(OperandType type)
      {
         this->type = type;
         this->ebpReg = (this->type == otRBP);
         this->reference = this->offset = 0;
         //this->prefix = spNone;
         this->factorReg = false;
      }
      Operand(int number)
      {
         this->type = (OperandType)number;
         this->ebpReg = (this->type == otRBP);
         this->reference = this->offset = 0;
         //this->prefix = spNone;
         this->factorReg = false;
      }
   };

   enum AMD64JumpType
   {
      JUMP_TYPE_JB = 0x02,
      JUMP_TYPE_JAE = 0x03,
      JUMP_TYPE_JZ = 0x04,
      JUMP_TYPE_JE = 0x04,
      JUMP_TYPE_JNZ = 0x05,
      JUMP_TYPE_JBE = 0x06,
      JUMP_TYPE_JA = 0x07,
      JUMP_TYPE_JS = 0x08,
      JUMP_TYPE_JNS = 0x09,
      JUMP_TYPE_JP = 0x0A,
      JUMP_TYPE_JPO = 0x0B,
      JUMP_TYPE_JL = 0x0C,
      JUMP_TYPE_JGE = 0x0D,
      JUMP_TYPE_JLE = 0x0E,
      JUMP_TYPE_JG = 0x0F,

      JUMP_TYPE_JMP = 0x00,
   };

   static OperandType getOperandType(OperandType type, OperandType prefix)
   {
      if (prefix != otNone) {
         if (test(type, otR64)) {
            type = (OperandType)((type & ~otR64) | prefix);
         }
         else if (test(type, otR32)) {
            type = (OperandType)((type & ~otR32) | prefix);
         }
         else if (type == otDB) {
            type = (OperandType)((type & ~otDB) | otDD | prefix);
         }
         else type = (OperandType)(type | prefix);
      }
      return type;
   }

   static OperandType getOperandType(OperandType baseType, OperandType prefix, int offset)
   {
      OperandType type = getOperandType(baseType, prefix);
      if (abs(offset) < 0x80) {
         type = (OperandType)(type | otM64disp8);
      }
      else type = (OperandType)(type | otM64disp32);

      return type;
   }

   static OperandType addPrefix(OperandType type, OperandType prefix)
   {
      if (prefix != otNone) {
         if (test(type, otR64)) {
            type = (OperandType)((type & ~otR64) | prefix);
         }
         else if (test(type, otRX64)) {
            if (prefix == otM64) {
               type = (OperandType)((type & ~otR64) | otMX64);
            }
            else type = (OperandType)((type & ~otR64) | prefix);
         }
         else if (test(type, otR32)) {
            type = (OperandType)((type & ~otR32) | prefix);
         }
         else if (type == otDB) {
            if (prefix == otDW) {
               type = (OperandType)((type & ~otDB) | prefix);
            }
            else if (prefix != otDB) {
               type = (OperandType)((type & ~otDB) | otDD | prefix);
            }
         }
         else if (type == otDD && prefix == otM64) {
            type = (OperandType)((type & ~otDD) | otDQ | prefix);
         }
         else type = (OperandType)(type | prefix);
      }
      return type;
   }

   static OperandType overrideOperand16(OperandType op16)
   {
      if (test(op16, otM16)) {
         op16 = (OperandType)(op16 & ~otM16);

         return (OperandType)(otM32 + op16);
      }
      else {
         op16 = (OperandType)(op16 & ~otR16);

         return (OperandType)(otR32 + op16);
      }
   }

   static void writeModRM(MemoryWriter* code, Operand sour, Operand dest)
   {
      int prefix = (dest.type & 0x300) >> 2;

      int opcode = prefix + ((char)sour.type << 3) + (char)dest.type;

      code->writeByte((unsigned char)opcode);
      if ((char)dest.type == otSIB && dest.type != otRSP && dest.type != otAH) {
         int sib = (char)(dest.type >> 24);
         if (sib == 0) {
            sib = 0x24;
         }
         code->writeByte((unsigned char)sib);
      }
      if (test(dest.type, otM32disp8) || test(dest.type, otM8disp8) || test(dest.type, otM64disp8)) {
         code->writeByte((unsigned char)dest.offset);
      }
      // !! should only otM32 be checked?
      else if (test(dest.type, otM32disp32) || test(dest.type, otM64disp32) || dest.factorReg) {
         if (dest.reference != 0) {
            code->writeRef(dest.reference, dest.offset);
         }
         else code->writeDWord(dest.offset);
      }
      else if (dest.type == otDisp32) {
         if (dest.reference != 0) {
            code->writeRef(dest.reference, dest.offset);
         }
         else code->writeDWord(dest.offset);
      }
      else if (dest.type == otDisp64) {
         if (dest.reference != 0) {
            if ((dest.reference & mskAnyRef) == mskPreloadDataRef) {
               code->writeRef((dest.reference & ~mskAnyRef) | mskPreloadRelDataRef, dest.offset);
            }
            else code->writeRef(dest.reference, dest.offset);
         }
         else code->writeDWord(dest.offset);
      }
   }

   static void leaRM64disp(MemoryWriter* code, OperandType sour, OperandType dest, int destOffs)
   {
      Operand sourOp(sour);
      Operand destOp(getOperandType(dest, otM64, destOffs));

      destOp.offset = destOffs;

      code->writeByte(0x48);
      code->writeByte(0x8D);
      writeModRM(code, sourOp, destOp);
   }

   static void writeImm(MemoryWriter* code, Operand sour)
   {
      if (sour.type == otDD) {
         if (sour.reference != 0) {
            code->writeRef(sour.reference, sour.offset);
         }
         else code->writeDWord(sour.offset);
      }
      else if (sour.type == otDQ) {
         if (sour.reference != 0) {
            code->writeRef(sour.reference, sour.offset);
            code->writeDWord(0);
         }
         else code->writeQWord(sour.offset);
      }
      else if (sour.type == otDB) {
         code->writeByte((unsigned char)sour.offset);
      }
      else if (sour.type == otDW) {
         code->writeWord((unsigned short)sour.offset);
      }
   }

   static void writeImm(MemoryWriter* code, Operand sour, bool override)
   {
      if (override && sour.type == otDD) {
         sour.type = otDW;

         writeImm(code, sour);
      }
      else writeImm(code, sour);
   }
};


class AMD64LabelHelper
{
   struct AMD64JumpInfo
   {
      pos_t position;
      int offset;

      AMD64JumpInfo()
      {
         position = (pos_t)-1;
         offset = 0;
      }
      AMD64JumpInfo(pos_t position)
      {
         this->position = position;
         this->offset = 0;
      }
      AMD64JumpInfo(pos_t position, int offset)
      {
         this->position = position;
         this->offset = offset;
      }
   };

   MemoryWriter* code;
   MemoryMap<int, AMD64JumpInfo, false> jumps;
   MemoryMap<int, pos_t, false>         labels;

public:
   void addFixableJump(pos_t position, int offset)
   {
      jumps.add(0, AMD64JumpInfo(position, offset));
   }

   bool isShortJump(unsigned char opcode);

   bool checkLabel(int label)
   {
      return labels.exist(label);
   }

   void setLabel(int label);

   void writeJxxForward(int label, AMD64Helper::AMD64JumpType prefix);
   void writeShortJxxForward(int label, AMD64Helper::AMD64JumpType prefix);

   void writeJxxBack(AMD64Helper::AMD64JumpType prefix, int label);
   void writeNearJxxBack(AMD64Helper::AMD64JumpType prefix, int label);
   void writeShortJxxBack(AMD64Helper::AMD64JumpType prefix, int label);

   void writeLoadForward(int label);
   void writeJmpForward(int label);
   void writeShortJmpForward(int label);

   void writeLoadBack(int label);
   void writeJmpBack(int label);
   void writeNearJmpBack(int label);
   void writeShortJmpBack(int label);

   //void writeLoopForward(int label);
   //void writeLoopBack(int label);

   //void writeCallForward(int label);
   //void writeCallBack(int label);

   int fixShortLabel(pos_t labelPos);
   int fixNearLabel(pos_t labelPos);
   void fixLabel(int label);

   void shiftLabels(pos_t position, int firstDisplacement, int displacement)
   {
      shift(labels.start(), position + 1, displacement);

      auto it = jumps.start();
      while (!it.Eof()) {
         if ((*it).position == position) {
            (*it).position += firstDisplacement;
         }
         else if ((*it).position > position)
            (*it).position += displacement;

         it++;
      }
   }

   void fixJumps(pos_t position, int size);
   void convertShortToNear(pos_t position, int offset);

   AMD64LabelHelper(MemoryWriter* code)
   {
      this->code = code;
   }
};

} // _ELENA_

#endif // amd64helperH
