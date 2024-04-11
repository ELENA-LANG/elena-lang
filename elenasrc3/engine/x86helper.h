//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains CPU native helpers
//		Supported platforms: x86 / x86-64
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef X86HELPER_H
#define X86HELPER_H

#include "elena.h"
#include "lbhelper.h"

namespace elena_lang
{
   // --- X86OperandType ---
   enum class X86OperandType : unsigned int
   {
      Unknown    = 0,
      None       = 0,

      ST         = 0x00000001,

      DD         = 0x00100005,
      DB         = 0x00200005,
      DW         = 0x00400005,
      DQ         = 0x00008005,

      AL         = 0x00200300,
      CL         = 0x00200301,
      DL         = 0x00200302,
      BL         = 0x00200303,
      AH         = 0x00200304,
      CH         = 0x00200305,
      DH         = 0x00200306,
      BH         = 0x00200307,

      AX         = 0x00400300,
      CX         = 0x00400301,
      DX         = 0x00400302,
      BX         = 0x00400303,

      EAX        = 0x00100300,
      ECX        = 0x00100301,
      EDX        = 0x00100302,
      EBX        = 0x00100303,
      ESP        = 0x00100304,
      EBP        = 0x00100305,
      ESI        = 0x00100306,
      EDI        = 0x00100307,

      RAX        = 0x00008300,
      RCX        = 0x00008301,
      RDX        = 0x00008302,
      RBX        = 0x00008303,
      RSP        = 0x00008304,
      RBP        = 0x00008305,
      RSI        = 0x00008306,
      RDI        = 0x00008307,

      RX8        = 0x00004300,
      RX9        = 0x00004301,
      RX10       = 0x00004302,
      RX11       = 0x00004303,
      RX12       = 0x00004304,
      RX13       = 0x00004305,
      RX14       = 0x00004306,
      RX15       = 0x00004307,

      XMM0       = 0x00800300,
      XMM1       = 0x00800301,
      XMM2       = 0x00800302,
      XMM3       = 0x00800303,
      XMM4       = 0x00800304,
      XMM5       = 0x00800305,
      XMM6       = 0x00800306,
      XMM7       = 0x00800307,

      R32        = 0x00100300,
      M32        = 0x00110000,
      M32disp8   = 0x00110100,
      M32disp32  = 0x00110200,

      R64        = 0x00008300,
      M64        = 0x00018000,
      M64disp8   = 0x00018100,
      M64disp32  = 0x00018200,

      RX64       = 0x00004300,
      MX64       = 0x00014000,
      MX64disp8  = 0x00014100,
      MX64disp32 = 0x00014200,

      R8         = 0x00200300,
      M8         = 0x00210000,
      M8disp8    = 0x00210100,

      R16        = 0x00400300,
      M16        = 0x00410000,
      M16disp8   = 0x00410100,

      XMM64      = 0x00800300,

      SIB        = 0x00000004,

      Disp32     = 0x00110005,
      Disp64     = 0x00018005,

      Factor2    = 0x40000000,
      Factor4    = 0x80000000,
      Factor8    = 0xC0000000
   };

   enum class SegmentPrefix : unsigned int
   {
      None  = 0,
      FS    = 0x64
   };

   // --- X86JumpType ---
   enum class X86JumpType : unsigned int
   {
      JMP   = 0x00,

      JB    = 0x02,
      JAE   = 0x03,
      JE    = 0x04,
      JZ    = 0x04,
      JNZ   = 0x05,
      JBE   = 0x06,
      JA    = 0x07,
      JS    = 0x08,
      JNS   = 0x09,
      JP    = 0x0A,
      JL    = 0x0C,
      JGE   = 0x0D,
      JLE   = 0x0E,
      JG    = 0x0F,
   };

   inline bool test(X86OperandType type, X86OperandType mask)
   {
      return (((unsigned int)type & (unsigned int)mask) == (unsigned int)mask);
   }

   inline X86OperandType operator | (X86OperandType arg1, X86OperandType arg2)
   {
      return (X86OperandType)(static_cast<unsigned int>(arg1) | static_cast<unsigned int>(arg2));
   }
   inline X86OperandType operator & (X86OperandType arg1, X86OperandType arg2)
   {
      return (X86OperandType)(static_cast<unsigned int>(arg1) & static_cast<unsigned int>(arg2));
   }
   inline X86OperandType operator & (X86OperandType arg1, unsigned int arg2)
   {
      return (X86OperandType)(static_cast<unsigned int>(arg1) & arg2);
   }
   inline X86OperandType operator ~ (X86OperandType arg1)
   {
      return (X86OperandType)(~static_cast<unsigned int>(arg1));
   }
   inline X86OperandType operator + (X86OperandType arg1, unsigned int arg2)
   {
      return (X86OperandType)(static_cast<unsigned int>(arg1) + arg2);
   }

   // --- X86Operand ---
   struct X86Operand
   {
      bool           ebpReg;     // to resolve conflict between [ebp] and disp32
      bool           factorReg;   // to implement [r*factor] SIB
      bool           accReg;

      X86OperandType type;
      ref_t          reference;
      int            offset;
      SegmentPrefix  prefix;

      bool isR8() const
      {
         return test(type, X86OperandType::R8);
      }
      bool isM8() const
      {
         return test(type, X86OperandType::M8);
      }
      bool isR8_M8() const
      {
         return test(type, X86OperandType::R8) || test(type, X86OperandType::M8);
      }
      bool isM16() const
      {
         return test(type, X86OperandType::M16);
      }
      bool isR16() const
      {
         return test(type, X86OperandType::R16);
      }
      bool isR16_M16() const
      {
         return test(type, X86OperandType::R16) || test(type, X86OperandType::M16);
      }
      bool isR32() const
      {
         return test(type, X86OperandType::R32);
      }
      bool isM32() const
      {
         return test(type, X86OperandType::M32);
      }
      bool isR32_M32() const
      {
         return test(type, X86OperandType::R32) || test(type, X86OperandType::M32);
      }
      bool isR64() const
      {
         return test(type, X86OperandType::R64);
      }
      bool isRX64() const
      {
         return test(type, X86OperandType::RX64);
      }
      bool isR64_M64() const
      {
         return test(type, X86OperandType::R64) || test(type, X86OperandType::M64);
      }
      bool isRX64_MX64() const
      {
         return test(type, X86OperandType::RX64) || test(type, X86OperandType::MX64);
      }
      bool isM64() const
      {
         return test(type, X86OperandType::M64);
      }
      bool isMX64() const
      {
         return test(type, X86OperandType::MX64);
      }
      bool isXMM64() const
      {
         return test(type, X86OperandType::XMM64);
      }

      bool isDB_DD() const
      {
         return type == X86OperandType::DD || type == X86OperandType::DB;
      }
      bool isDB() const
      {
         return type == X86OperandType::DB;
      }
      bool isDB_DD_DQ() const
      {
         return type == X86OperandType::DQ || type == X86OperandType::DD || type == X86OperandType::DB;
      }

      X86Operand()
      {
         this->type = X86OperandType::Unknown;
         this->reference = 0;
         this->offset = 0;
         this->factorReg = this->ebpReg = false;
         this->accReg = false;
         this->prefix = SegmentPrefix::None;
      }
      X86Operand(X86OperandType type)
      {
         this->type = type;
         this->reference = 0;
         this->offset = 0;
         this->factorReg = false;
         this->ebpReg = (type == X86OperandType::EBP || this->type == X86OperandType::RBP);
         this->accReg = (type == X86OperandType::EAX || this->type == X86OperandType::RAX);
         this->prefix = SegmentPrefix::None;
      }
   };

   // --- X86Helper ---
   class X86Helper
   {
   public:
      static bool isFarUnconditionalJump(unsigned char opcode)
      {
         return opcode == 0xE9;
      }

      static void fixShortLabel(MemoryWriter& writer, pos_t labelPos)
      {
         char offset = (char)(writer.position() - labelPos - 1);

         writer.Memory()->write(labelPos, &offset, sizeof(offset));
      }

      static void fixNearLabel(MemoryWriter& writer, pos_t labelPos)
      {
         int offset = (int)(writer.position() - labelPos - 4);

         writer.Memory()->write(labelPos, &offset, sizeof(offset));
      }

      static void writeModRM(MemoryWriter& writer, X86Operand sour, X86Operand dest)
      {
         int prefix = ((unsigned int)dest.type & 0x300) >> 2;
         int opcode = prefix + ((char)sour.type << 3) + (char)dest.type;

         writer.writeByte((unsigned char)opcode);
         if ((char)dest.type == (char)X86OperandType::SIB && dest.type != X86OperandType::ESP 
            && dest.type != X86OperandType::RSP && dest.type != X86OperandType::RX12 && dest.type != X86OperandType::AH) {
            int sib = (char)((unsigned int)dest.type >> 24);
            if (sib == 0)
               sib = 0x24;

            writer.writeByte((unsigned char)sib);
         }
         if (test(dest.type, X86OperandType::M32disp8) || test(dest.type, X86OperandType::M8disp8)
            || test(dest.type, X86OperandType::M64disp8) || test(dest.type, X86OperandType::MX64disp8)) {
            writer.writeByte((unsigned char)dest.offset);
         }
         else if (test(dest.type, X86OperandType::M32disp32) || test(dest.type, X86OperandType::M64disp32) || dest.factorReg) {
            if (dest.reference != 0) {
               writer.writeDReference(dest.reference, dest.offset);
            }
            else writer.writeDWord(dest.offset);
         }
         else if (dest.type == X86OperandType::Disp32) {
            if (dest.reference != 0) {
               writer.writeDReference(dest.reference, dest.offset);
            }
            else writer.writeDWord(dest.offset);
         }
         else if (dest.type == X86OperandType::Disp64) {
            if (dest.reference != 0) {
               switch (dest.reference & mskRefType) {
                  case mskRef64:
                     dest.reference = (dest.reference & ~mskRef64) | mskRelRef32;
                     break;
                  default:
                     break;
               }

               writer.writeDReference(dest.reference, dest.offset);
            }
            else writer.writeDWord(dest.offset);
         }
      }

      static void writeImm(MemoryWriter& writer, X86Operand operand)
      {
         switch (operand.type) {
            case X86OperandType::DQ:
               if (operand.reference != 0) {
                  writer.writeQReference(operand.reference, operand.offset);
               }
               else writer.writeQWord(operand.offset);
               break;
            case X86OperandType::DD:
               if (operand.reference != 0) {
                  writer.writeDReference(operand.reference, operand.offset);
               }
               else writer.writeDWord(operand.offset);
               break;
            case X86OperandType::DB:
               writer.writeByte((unsigned char)operand.offset);
               break;
            case X86OperandType::DW:
               writer.writeWord((unsigned short)operand.offset);
               break;
            default:
               // to make compiler happy
               break;
         }
      }

      static X86OperandType addPrefix(X86OperandType type, X86OperandType prefix)
      {
         if (prefix != X86OperandType::None) {
            if (test(type, X86OperandType::R64)) {
               type = (type & ~X86OperandType::R64) | prefix;
            }
            else if (test(type, X86OperandType::RX64) && prefix == X86OperandType::M64) {
               type = (type & ~X86OperandType::RX64) | X86OperandType::MX64;
            }
            else if (test(type, X86OperandType::R32)) {
               type = (type & ~X86OperandType::R32) | prefix;
            }
            else if (type == X86OperandType::DB) {
               if (prefix == X86OperandType::DW) {
                  type = (type & ~X86OperandType::DB) | prefix;
               }
               else if (prefix != X86OperandType::DB) {
                  type = (type & ~X86OperandType::DB)
                     | X86OperandType::DD
                     | prefix;
               }
            }
            else if (type == X86OperandType::DD && prefix == X86OperandType::M64) {
               type = (type & ~X86OperandType::DD) | X86OperandType::DQ | X86OperandType::MX64;
            }
            //else if (type == otDD && prefix == otM64) {
            //   type = (OperandType)((type & ~otDD) | otDQ | prefix);
            //}
            else type = type | prefix;
         }
         return type;
      }
   };

   // --- X86LabelHelper ---
   struct X86LabelHelper : LabelHelper
   {
      static bool isNearJmp(unsigned char opcode)
      {
         return opcode == 0xE9;
      }

      static bool isShortJump(unsigned char opcode)
      {
         // if it is jump of address load
         if (opcode == 0xE8 || opcode == 0xB9)
            return false;
         else if ((opcode == 0x0F) || opcode == 0xE9) {
            return false;
         }
         else return true;
      }

      int fixShortLabel(pos_t labelPos, MemoryWriter& writer);
      int fixNearJccLabel(pos_t labelPos, MemoryWriter& writer);
      int fixNearJmpLabel(pos_t labelPos, MemoryWriter& writer);

      void convertShortToNear(pos_t position, int offset, MemoryWriter& writer);

      void shiftLabels(pos_t position, int firstDisplacement, int displacement)
      {
         shift(labels.start(), position + 1, displacement);

         auto it = jumps.start();
         while (!it.eof()) {
            if ((*it).position == position) {
               (*it).position += firstDisplacement;
            }
            else if ((*it).position > position)
               (*it).position += displacement;

            ++it;
         }
      }

      void fixJumps(pos_t position, int size, MemoryWriter& writer);
      bool fixLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase* rh) override;

      void writeShortJmpForward(pos_t label, MemoryWriter& writer);
      void writeJmpForward(pos_t label, MemoryWriter& writer);
      void writeShortJmpBack(pos_t label, MemoryWriter& writer);
      void writeNearJmpBack(pos_t label, MemoryWriter& writer);

      void writeShortJccForward(X86JumpType type, pos_t label, MemoryWriter& writer);
      void writeNearJccForward(X86JumpType type, pos_t label, MemoryWriter& writer);
      void writeShortJccBack(X86JumpType type, pos_t label, MemoryWriter& writer);
      void writeNearJccBack(X86JumpType type, pos_t label, MemoryWriter& writer);

      void writeJumpBack(pos_t label, MemoryWriter& writer) override;
      void writeJumpForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override;

      void writeJccBack(X86JumpType type, pos_t label, MemoryWriter& writer);
      void writeJccForward(X86JumpType type, pos_t label, MemoryWriter& writer, int byteCodeOffset);

      void writeJeqForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         writeJccForward(X86JumpType::JZ, label, writer, byteCodeOffset);
      }
      void writeJeqBack(pos_t label, MemoryWriter& writer) override
      {
         writeJccBack(X86JumpType::JZ, label, writer);
      }

      void writeJneForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         writeJccForward(X86JumpType::JNZ, label, writer, byteCodeOffset);
      }

      void writeJneBack(pos_t label, MemoryWriter& writer) override
      {
         writeJccBack(X86JumpType::JNZ, label, writer);
      }

      void writeJltForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         writeJccForward(X86JumpType::JL, label, writer, byteCodeOffset);
      }

      void writeJltBack(pos_t label, MemoryWriter& writer) override
      {
         writeJccBack(X86JumpType::JL, label, writer);
      }

      void writeJgeForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         writeJccForward(X86JumpType::JGE, label, writer, byteCodeOffset);
      }

      void writeJgeBack(pos_t label, MemoryWriter& writer) override
      {
         writeJccBack(X86JumpType::JGE, label, writer);
      }

      void writeJgrForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         writeJccForward(X86JumpType::JG, label, writer, byteCodeOffset);
      }

      void writeJgrBack(pos_t label, MemoryWriter& writer) override
      {
         writeJccBack(X86JumpType::JG, label, writer);
      }

      void writeJleForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         writeJccForward(X86JumpType::JLE, label, writer, byteCodeOffset);
      }

      void writeJleBack(pos_t label, MemoryWriter& writer) override
      {
         writeJccBack(X86JumpType::JLE, label, writer);
      }
   };
}

#endif