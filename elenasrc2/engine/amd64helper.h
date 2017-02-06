//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains ELENA coder opcode helpers
//		Supported platforms: amd64
//                             (C)2005-2017, by Alexei Rakov, Alexandre Bencz
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
      otDQ        = 0x02000005,

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

      otXMM0      = 0x01000300,
      otXMM1      = 0x01000301,

      otRAX       = 0x02000300,
      otRCX       = 0x02000301,
      otRDX       = 0x02000302,
      otRBX       = 0x02000303,
      otRSP       = 0x02000304,
      otRBP       = 0x02000305,
      otRSI       = 0x02000306,
      otRDI       = 0x02000307,

      otR32       = 0x00100300,
      otM32       = 0x00110000,
      otM32disp8  = 0x00110100,
      otM32disp32 = 0x00110200,

      otR8        = 0x00200300,
      otM8disp8   = 0x00210100,

      otR16       = 0x00400300,

      otR64       = 0x02000300,
      otM64       = 0x02010000,

      otX128      = 0x01000300,

      otSIB       = 0x00000004,
   };
      
   struct Operand
   {
      OperandType   type;
      int           offset;
      ref_t         reference;

      Operand()
      {
         type = otUnknown;
         reference = offset = 0;
      }
      Operand(OperandType type)
      {
         this->type = type;
         reference = offset = 0;
      }
      Operand(int number)
      {
         this->type = (OperandType)number;
         this->reference = this->offset = 0;
      }
   };

   static void writeModRM(MemoryWriter* code, Operand sour, Operand dest)
   {
      int prefix = (dest.type & 0x300) >> 2;

      int opcode = prefix + ((char)sour.type << 3) + (char)dest.type;

      code->writeByte((unsigned char)opcode);
      if ((char)dest.type == otSIB && dest.type != otRSP && dest.type != otAH) {
         int sib = (char)(dest.type >> 24) & 0xFD; // excluding 64bit mask
         if (sib == 0) {
            sib = 0x24;
         }
         code->writeByte((unsigned char)sib);
      }
      if (test(dest.type, otM32disp8) || test(dest.type, otM8disp8)) {
         code->writeByte((unsigned char)dest.offset);
      }
      // !! should only otM32 be checked?
      else if (test(dest.type, otM32disp32)/* || dest.factorReg*/) {
         if (dest.reference != 0) {
            code->writeRef(dest.reference, dest.offset);
         }
         else code->writeDWord(dest.offset);
      }
      //else if (dest.type == otDisp32) {
      //   if (dest.reference != 0) {
      //      code->writeRef(dest.reference, dest.offset);
      //   }
      //   else code->writeDWord(dest.offset);
      //}
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
         code->writeQWord(sour.offset);
      }
      else if (sour.type == otDB) {
         code->writeByte((unsigned char)sour.offset);
      }
      else if (sour.type == otDW) {
         code->writeWord((unsigned short)sour.offset);
      }
   }

   static OperandType addPrefix(OperandType type, OperandType prefix)
   {
      if (prefix != otNone) {
         if (test(type, otR64)) {
            type = (OperandType)((type & ~otR64) | prefix);
         }

      //   if (test(type, otR32)) {
      //      type = (OperandType)((type & ~otR32) | prefix);
      //   }
      //   else if (type == otDB) {
      //      if (prefix == otDW) {
      //         type = (OperandType)((type & ~otDB) | prefix);
      //      }
      //      else if (prefix != otDB) {
      //         type = (OperandType)((type & ~otDB) | otDD | prefix);
      //      }
      //   }
      //   else type = (OperandType)(type | prefix);
      }
      return type;
   }
};

} // _ELENA_

#endif // amd64helperH
