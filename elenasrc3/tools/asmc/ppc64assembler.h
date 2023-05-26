//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This header contains PPC64 Assembler declarations
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PPC64ASSEMBLER_H
#define PPC64ASSEMBLER_H

#include "assembler.h"
#include "ppchelper.h"

namespace elena_lang
{
   // --- PPC64Assember ---
   class PPC64Assembler : public AssemblerBase
   {
   protected:
      PPCOperand defineRegister(ScriptToken& tokenInfo, ustr_t errorMessage);

      PPCOperand readRegister(ScriptToken& tokenInfo, ustr_t errorMessage, bool checkRc = false);
      PPCOperand readDispOperand(ScriptToken& tokenInfo, int& disp, ref_t& reference, ustr_t errorMessage);
      void readIOperand(ScriptToken& tokenInfo, int& value, ref_t& reference, ustr_t errorMessage);
      void readPtrOperand(ScriptToken& tokenInfo, int& value, ref_t& reference, ref_t mask, ustr_t errorMessage);

      int readIntArg(ScriptToken& tokenInfo);

      void writeDReference(ScriptToken& tokenInfo, ref_t reference, MemoryWriter& writer, ustr_t errorMessage);

      void declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;

      bool compileMFSPR(PPCOperand rt, int spr, MemoryWriter& writer);
      bool compileMTSPR(PPCOperand rt, int spr, MemoryWriter& writer);

      void compileBCLR(int bo, int bi, int bh, MemoryWriter& writer);      

      void compileBCCTRL(int bo, int bi, int bh, MemoryWriter& writer);
      void compileBCCTR(int bo, int bi, int bh, MemoryWriter& writer);

      void compileADDI(ScriptToken& tokenInfo, PPCOperand rx, PPCOperand ry, int i,
         ref_t reference, MemoryWriter& writer);
      void compileADDIS(ScriptToken& tokenInfo, PPCOperand rx, PPCOperand ry, int i,
         ref_t reference, MemoryWriter& writer);
      bool compileAND(PPCOperand rx, PPCOperand ry, PPCOperand rz, MemoryWriter& writer);
      void compileANDI(ScriptToken& tokenInfo, PPCOperand rx, PPCOperand ry, int i,
         ref_t reference, MemoryWriter& writer);
      void compileBxx(int address, int aa, int lk, MemoryWriter& writer);
      void compileBCxx(int bo, int bi, int bd, int aa, int lk, MemoryWriter& writer);
      void compileBCxx(ScriptToken& tokenInfo, int aa, int lk, MemoryWriter& writer, LabelScope& labelScope);
      void compileCMP(ScriptToken& tokenInfo, int bf, int l, PPCOperand ra, PPCOperand rb,
         MemoryWriter& writer);
      void compileCMPI(ScriptToken& tokenInfo, int bf, int l, PPCOperand ra, int i, ref_t reference,
         MemoryWriter& writer);
      void compileFCMPU(ScriptToken& tokenInfo, int bf, PPCOperand fra, PPCOperand frb,
         MemoryWriter& writer);
      bool compileISEL(PPCOperand rx, PPCOperand ry, PPCOperand rz, int bc, MemoryWriter& writer);
      bool compileNAND(PPCOperand rx, PPCOperand ry, PPCOperand rz, MemoryWriter& writer);
      bool compileOR(PPCOperand rx, PPCOperand ry, PPCOperand rz, MemoryWriter& writer);
      void compileORI(ScriptToken& tokenInfo, PPCOperand ra, PPCOperand rs, int i,
         ref_t reference, MemoryWriter& writer);
      void compileRLDICL(PPCOperand rx, PPCOperand ry, int n1, int n2, MemoryWriter& writer);
      void compileRLDICR(PPCOperand rx, PPCOperand ry, int n1, int n2, MemoryWriter& writer);
      void compileSUBF(PPCOperand rx, PPCOperand ry, PPCOperand rz, MemoryWriter& writer);
      bool compileXOR(PPCOperand rx, PPCOperand ry, PPCOperand rz, MemoryWriter& writer);

      void compileADD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileADDE(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileADDI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileADDIS(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileANDI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileAND(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileBCTR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileBCL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileB(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileBLR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileBCTRL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileBEQ(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileBNE(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileBLT(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileBGE(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope);
      void compileCMP(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCMPWI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileCMPDI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFCTIW(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileDIVD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileDIVW(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileEXTRDI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileEXTSW(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFADD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFSUB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFCFID(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFCMPU(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFDIV(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFMUL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileFRIZ(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileISELEQ(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileISELLT(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileISELSO(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileOR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileORI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLBZ(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLFD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLHZ(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLHA(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLWZ(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLIS(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileLI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMFLR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMTLR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMTCTR(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMULLD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileMULLW(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileNAND(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileNEG(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileRLDICL(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSRDI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSLD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSLDI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSRD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTW(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTH(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTFD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTD(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTDU(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSTWU(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSUB(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSUBF(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileSUBI(ScriptToken& tokenInfo, MemoryWriter& writer);
      void compileXOR(ScriptToken& tokenInfo, MemoryWriter& writer);

      void compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      void compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer) override;

      bool compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope) override;
      bool compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
      bool compileFOpCode(ScriptToken& tokenInfo, MemoryWriter& writer) override;
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

      void compileProcedure(ScriptToken& tokenInfo) override;

   public:
      PPC64Assembler(int tabSize, UStrReader* reader, ModuleBase* target)
         : AssemblerBase(tabSize, reader, target)
      {         
      }
   };

}

#endif
