//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains AARCH64 Assembler declarations
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ARMASSEMBLER_H
#define ARMASSEMBLER_H

#include "assembler.h"
#include "armhelper.h"

namespace elena_lang
{
   // --- Arm64Assembler ---
   class Arm64Assembler : public AssemblerBase
   {
   protected:
      int readIntArg(ScriptToken& tokenInfo, ref_t& reference);
      int readReferenceArg(ScriptToken& tokenInfo, ref_t& reference, ustr_t errorMessage);
      bool readOperandReference(ScriptToken& tokenInfo, ref_t mask, int& value, ref_t& reference);

      ARMOperand defineRegister(ScriptToken& tokenInfo);

      ARMOperand readOperand(ScriptToken& tokenInfo, ustr_t error);
      ARMOperand readPtrOperand(ScriptToken& tokenInfo, ustr_t error);

      virtual int readLSL(ScriptToken& tokenInfo, ustr_t error);

      JumpType readCond(ScriptToken& tokenInfo);
      JumpType invertCond(JumpType type);

      void writeReference(ScriptToken& tokenInfo, ref_t reference, MemoryWriter& writer, ustr_t errorMessage);

      void declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;

      virtual bool compileADDShifted(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry,
         int shift, int ampount, MemoryWriter& writer);
      virtual bool compileANDShifted(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry,
         int shift, int ampount, MemoryWriter& writer);
      virtual bool compileADDImm(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry, MemoryWriter& writer);
      virtual bool compileANDImm(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry, MemoryWriter& writer);
      virtual bool compileANDSImm(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry, MemoryWriter& writer);
      virtual bool compileANDSShifted(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry,
         int shift, int ampount, MemoryWriter& writer);
      virtual bool compileADRP(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand imm, MemoryWriter& writer);
      virtual bool compileCSEL(ARMOperand rd, ARMOperand rn, ARMOperand rm, JumpType cond, MemoryWriter& writer);
      virtual bool compileCSINC(ARMOperand rd, ARMOperand rn, ARMOperand rm, JumpType cond, MemoryWriter& writer);
      virtual bool compileLDP(ARMOperand rt, ARMOperand rx, ARMOperand n1, ARMOperand n2, MemoryWriter& writer);
      virtual bool compileLDR(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileLDRB(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileLDRSB(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileLDRSW(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileLDRSH(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileMOVZ(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rn, MemoryWriter& writer);
      virtual bool compileMOVK(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rn, int lsl, MemoryWriter& writer);
      virtual bool compileMADD(ARMOperand rd, ARMOperand rn, ARMOperand rm, ARMOperand ra, MemoryWriter& writer);
      virtual bool compileORRShifted(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry,
         int shift, int ampount, MemoryWriter& writer);
      virtual bool compileORRImm(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry, MemoryWriter& writer);
      virtual bool compileRET(ARMOperand r, MemoryWriter& writer);
      virtual bool compileB(int imm, MemoryWriter& writer);
      virtual bool compileBxx(int imm, int cond, MemoryWriter& writer);
      virtual bool compileBLR(ARMOperand r, MemoryWriter& writer);
      virtual bool compileBR(ARMOperand r, MemoryWriter& writer);
      virtual bool compileSDIV(ARMOperand rd, ARMOperand rn, ARMOperand rm, MemoryWriter& writer);
      virtual bool compileSTP(ARMOperand t1, ARMOperand t2, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileSTR(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileSTRB(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileSTRH(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand ptr, MemoryWriter& writer);
      virtual bool compileSUBImm(ScriptToken& tokenInfo, ARMOperand rt, ARMOperand rx, ARMOperand ry, MemoryWriter& writer);
      virtual bool compileSUBS(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rt, MemoryWriter& writer);
      virtual bool compileSUBSShifted(ScriptToken& tokenInfo, ARMOperand rd, ARMOperand rn, ARMOperand rt, 
         int shift, int ampount, MemoryWriter& writer);
      virtual bool compileUBFM(ARMOperand rd, ARMOperand rn, ARMOperand immr, ARMOperand imms, MemoryWriter& writer);

      void compileADD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileADRP(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileAND(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileB(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileBxx(ScriptToken& tokenInfo, JumpType cond, MemoryWriter& writer, LabelScope& labelScope);
      void compileBLR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileBR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCSET(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCSEL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCSINC(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCMP(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLDP(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLDR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLDRB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLDRSB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLDRSW(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLDRSH(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLSL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLSR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMOV(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMOVK(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMOVZ(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMUL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileORR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSDIV(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTP(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTRB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTRH(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileRET(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSUB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileTST(ScriptToken& tokenInfo, MemoryWriter& writer);

      bool compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;
      bool compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileIOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileJOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;
      bool compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileNOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileOOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compilePOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileROpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileTOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;

      void compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer) override;

      void compileProcedure(ScriptToken& tokenInfo) override;

   public:
      Arm64Assembler(int tabSize, UStrReader* reader, ModuleBase* target)
         : AssemblerBase(tabSize, reader, target)
      {

      }
   };
}

#endif