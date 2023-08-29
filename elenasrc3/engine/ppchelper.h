//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains CPU native helpers
//		Supported platforms: PPC64
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PPCHELPER_H
#define PPCHELPER_H

#include "lbhelper.h"

namespace elena_lang
{
   // --- PPCOperand ---
   enum class PPCOperandType : unsigned int
   {
      None,

      GPR0     = 0x0100,
      GPR1     = 0x0101,
      GPR2     = 0x0102,
      GPR3     = 0x0103,
      GPR4     = 0x0104,
      GPR5     = 0x0105,
      GPR6     = 0x0106,
      GPR7     = 0x0107,
      GPR8     = 0x0108,
      GPR9     = 0x0109,
      GPR10    = 0x010A,
      GPR11    = 0x010B,
      GPR12    = 0x010C,
      GPR13    = 0x010D,
      GPR14    = 0x010E,
      GPR15    = 0x010F,
      GPR16    = 0x0110,
      GPR17    = 0x0111,
      GPR18    = 0x0112,
      GPR19    = 0x0113,
      GPR20    = 0x0114,
      GPR21    = 0x0115,
      GPR22    = 0x0116,
      GPR23    = 0x0117,
      GPR24    = 0x0118,
      GPR25    = 0x0119,
      GPR26    = 0x011A,
      GPR27    = 0x011B,
      GPR28    = 0x011C,
      GPR29    = 0x011D,
      GPR30    = 0x011E,
      GPR31    = 0x011F,

      GPR      = 0x0100,

      FPR0     = 0x0200,
      FPR1     = 0x0201,
      FPR2     = 0x0202,
      FPR3     = 0x0203,
      FPR4     = 0x0204,
      FPR5     = 0x0205,
      FPR6     = 0x0206,
      FPR7     = 0x0207,
      FPR8     = 0x0208,
      FPR9     = 0x0209,
      FPR10    = 0x020A,
      FPR11    = 0x020B,
      FPR12    = 0x020C,
      FPR13    = 0x020D,
      FPR14    = 0x020E,
      FPR15    = 0x020F,
      FPR16    = 0x0210,
      FPR17    = 0x0211,
      FPR18    = 0x0212,
      FPR19    = 0x0213,
      FPR20    = 0x0214,
      FPR21    = 0x0215,
      FPR22    = 0x0216,
      FPR23    = 0x0217,
      FPR24    = 0x0218,
      FPR25    = 0x0219,
      FPR26    = 0x021A,
      FPR27    = 0x021B,
      FPR28    = 0x021C,
      FPR29    = 0x021D,
      FPR30    = 0x021E,
      FPR31    = 0x021F,

      FPR      = 0x0200,
   };

   inline bool test(PPCOperandType type, PPCOperandType mask)
   {
      return (((unsigned int)type & (unsigned int)mask) == (unsigned int)mask);
   }

   // --- PPCOperand ---
   struct PPCOperand
   {
      PPCOperandType type;
      bool           rc;

      bool isGPR()
      {
         return test(type, PPCOperandType::GPR);
      }

      bool isFPR()
      {
         return test(type, PPCOperandType::FPR);
      }

      bool isRX()
      {
         return (type >= PPCOperandType::GPR0 && type <= PPCOperandType::GPR15);
      }

      PPCOperand()
      {
         type = PPCOperandType::None;
         rc = false;
      }

      PPCOperand(PPCOperandType type)
      {
         this->type = type;
         rc = false;
      }
   };

   // --- PPCHelper ---
   class PPCHelper
   {
   public:
      static short getHiAdjusted(disp_t n)
      {
         // HOTFIX : if the DWORD LO is over 0x7FFF - adjust DWORD HI (by adding 1) to be properly combined in the following code:
         //     addis   r16, r16, __xdisp32hi_1
         //     addi    r16, r16, __xdisp32lo_1
         short lo = n & 0xFFFF;
         if (lo < 0)
            n += 0x10000;

         return (short)(n >> 16);
      }

      static unsigned int makeDSCommand(unsigned int opcode, PPCOperandType rs, PPCOperandType ra, int d)
      {
         return (opcode << 26) | (((unsigned int)rs & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16) | ((unsigned int)d & 0xFFFC);
      }
      static unsigned int makeDSCommandU(unsigned int opcode, PPCOperandType rs, PPCOperandType ra, int d)
      {
         return (opcode << 26) | (((unsigned int)rs & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16) 
            | ((d & 0x3FFF) << 2) | 1;
      }

      static unsigned int makeXFXCommand(unsigned int opcode, PPCOperandType rt, int form, int xopcode)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21) | (form << 16) | (xopcode << 1);
      }

      static unsigned int makeACommand(unsigned int opcode, PPCOperandType rt, PPCOperandType ra, PPCOperandType rb,
         int bc, int xo)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16)
            | (((unsigned int)rb & 0x1F) << 11) | (((unsigned int)bc & 0x1F) << 6) | (xo << 1) | 0;
      }

      static unsigned int makeACommand(unsigned int opcode, PPCOperandType rt, PPCOperandType ra, PPCOperandType rb,
         int xo)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16)
            | (((unsigned int)rb & 0x1F) << 6) | (xo << 1) | 0;
      }

      static unsigned int makeXCommand(unsigned int opcode, PPCOperandType rt, PPCOperandType ra, PPCOperandType rb,
         int xo, int rc)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16)
            | (((unsigned int)rb & 0x1F) << 11) | (xo << 1) | rc;
      }
      static unsigned int makeXCommand(unsigned int opcode, PPCOperandType rt, PPCOperandType rb,
         int xo, int rc)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21)
            | (((unsigned int)rb & 0x1F) << 11) | (xo << 1) | rc;
      }
      static unsigned int makeX2Command(unsigned int opcode, PPCOperandType rt, PPCOperandType rb,
         int xo, int rc)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21)
            | (((unsigned int)rb & 0x1F) << 16) | (xo << 1) | rc;
      }

      static unsigned int makeDCommand(unsigned int opcode, PPCOperandType rs, PPCOperandType ra, int d)
      {
         return (opcode << 26) | (((unsigned int)rs & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16)
            | ((unsigned int)d & 0xFFFF);
      }
      static unsigned int makeDCommand(unsigned int opcode, int bf, int l, PPCOperandType ra, int d)
      {
         return (opcode << 26) | (bf << 23) | (l << 21) | (((unsigned int)ra & 0x1F) << 16)
            | ((unsigned int)d & 0xFFFF);
      }

      static unsigned int makeMDCommand(unsigned int opcode, PPCOperandType rs, PPCOperandType ra, int sh, int mb,
         int xo, int sh_hi, int rc)
      {
         return (opcode << 26) | (((unsigned int)rs & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16)
            | ((sh & 0x1F) << 11) | ((mb & 0x3F) << 5) | ((xo & 3) << 2) | ((sh_hi & 1) << 1) | rc;
      }

      static unsigned int makeXLCommand(unsigned int opcode, int bo, int bi, int bh, int xo, int lk)
      {
         return (opcode << 26) | (bo << 21) | (bi << 16) | (bh << 11) | (xo << 1) | lk;
      }
      static unsigned int makeBCommand(unsigned int opcode, int bo, int bi, int bd, int aa, int lk)
      {
         return (opcode << 26) | (bo << 21) | (bi << 16) | ((bd & 0x3FFF) << 2) | (aa << 1) | lk;
      }
      static unsigned int makeXOCommand(unsigned int opcode, PPCOperandType rt, PPCOperandType ra, PPCOperandType rb, 
         int oe, int xo, int rc)
      {
         return (opcode << 26) | (((unsigned int)rt & 0x1F) << 21) | (((unsigned int)ra & 0x1F) << 16)
            | (((unsigned int)rb & 0x1F) << 11) | (oe << 10) | (xo << 1) | rc;
      }
      static unsigned int makeICommand(unsigned int opcode, int li, int aa, int lk)
      {
         return (opcode << 26) | ((li & 0xFFFFFF) << 2) | (aa << 1) | lk;
      }
      static unsigned int makeXCommand(unsigned int opcode, int bc, int l, PPCOperandType ra, PPCOperandType rb,
         int xo)
      {
         return (opcode << 26) | ((bc & 0x7) << 23) | (l << 21) | (((unsigned int)ra & 0x1F) << 16)
            | (((unsigned int)rb & 0x1F) << 11) | (xo << 1) | 0;
      }

      static void fixBCommand(void* opcode, int bd)
      {
         *(unsigned int*)opcode = *(unsigned int*)opcode | bd;
      }
   };

   // --- PPCLabelHelper ---
   struct PPCLabelHelper : LabelHelper
   {
      static short getHiAdjusted(disp_t n)
      {
         // HOTFIX : if the DWORD LO is over 0x7FFF - adjust DWORD HI (by adding 1) to be properly combined in the following code:
         //     addis   r16, r16, __xdisp32hi_1
         //     addi    r16, r16, __xdisp32lo_1
         short lo = n & 0xFFFF;
         if (lo < 0)
            n += 0x10000;

         return (short)(n >> 16);
      }

      static void writeBxx(int offset, int aa, int lk, MemoryWriter& writer)
      {
         writer.writeDWord(PPCHelper::makeICommand(18, offset >> 2, aa, lk));
      }

      static void writeBCxx(int bo, int bi, int bd, int aa, int lk, MemoryWriter& writer)
      {
         writer.writeDWord(PPCHelper::makeBCommand(16, bo, bi, bd >> 2, aa, lk));
      }

      bool fixLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase* rh) override
      {
         for (auto it = jumps.getIt(label); !it.eof(); ++it) {
            if (it.key() == label) {
               ref_t labelPos = (*it).position;
               int offset = writer.position() - labelPos;
               if (abs(offset) > 0xFFFF)
                  return false;

               void* opcode = writer.Memory()->get(labelPos);

               PPCHelper::fixBCommand(opcode, offset);
            }
         }

         for (auto a_it = addresses.getIt(label); !a_it.eof(); a_it = addresses.nextIt(label, a_it)) {
            auto info = *a_it;

            rh->resolveLabel(writer, info.mask, info.position);
         }

         return true;
      }

      void writeJumpBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBxx(offset, 0, 0, writer);
      }

      void writeJumpForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBxx(0, 0, 0, writer);
      }

      void writeJeqForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBCxx(12, 2, 0, 0, 0, writer);
      }

      void writeJeqBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBCxx(12, 2, offset, 0, 0, writer);
      }

      void writeJneForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBCxx(4, 2, 0, 0, 0, writer);
      }

      void writeJneBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBCxx(4, 2, offset, 0, 0, writer);
      }

      void writeJltForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBCxx(12, 0, 0, 0, 0, writer);
      }

      void writeJltBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBCxx(12, 0, offset, 0, 0, writer);
      }

      void writeJgeForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBCxx(4, 0, 0, 0, 0, writer);
      }

      void writeJgeBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBCxx(4, 0, offset, 0, 0, writer);
      }

      void writeJgrForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBCxx(12, 1, 0, 0, 0, writer);
      }

      void writeJgrBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBCxx(12, 1, offset, 0, 0, writer);
      }

      void writeJleForward(pos_t label, MemoryWriter& writer, int byteCodeOffset) override
      {
         jumps.add(label, { writer.position() });

         writeBCxx(4, 1, 0, 0, 0, writer);
      }

      void writeJleBack(pos_t label, MemoryWriter& writer) override
      {
         int offset = labels.get(label) - writer.position();
         if (abs(offset) > 0xFFFF)
            throw InternalError(-1);

         writeBCxx(4, 1, offset, 0, 0, writer);
      }
   };

}

#endif
