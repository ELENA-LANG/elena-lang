//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains PPC64 Assembler implementation
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------------
#include "ppc64assembler.h"
#include "asmconst.h"

using namespace elena_lang;

// --- PPC64Assembler ---

PPCOperand PPC64Assembler :: defineRegister(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   PPCOperandType type = PPCOperandType::None;
   if (tokenInfo.compare("r0")) {
      type = PPCOperandType::GPR0;
   }
   else if (tokenInfo.compare("r1")) {
      type = PPCOperandType::GPR1;
   }
   else if (tokenInfo.compare("r2")) {
      type = PPCOperandType::GPR2;
   }
   else if (tokenInfo.compare("r3")) {
      type = PPCOperandType::GPR3;
   }
   else if (tokenInfo.compare("r4")) {
      type = PPCOperandType::GPR4;
   }
   else if (tokenInfo.compare("r5")) {
      type = PPCOperandType::GPR5;
   }
   else if (tokenInfo.compare("r6")) {
      type = PPCOperandType::GPR6;
   }
   else if (tokenInfo.compare("r7")) {
      type = PPCOperandType::GPR7;
   }
   else if (tokenInfo.compare("r8")) {
      type = PPCOperandType::GPR8;
   }
   else if (tokenInfo.compare("r9")) {
      type = PPCOperandType::GPR9;
   }
   else if (tokenInfo.compare("r10")) {
      type = PPCOperandType::GPR10;
   }
   else if (tokenInfo.compare("r11")) {
      type = PPCOperandType::GPR11;
   }
   else if (tokenInfo.compare("r12")) {
      type = PPCOperandType::GPR12;
   }
   else if (tokenInfo.compare("r13")) {
      type = PPCOperandType::GPR13;
   }
   else if (tokenInfo.compare("r14")) {
      type = PPCOperandType::GPR14;
   }
   else if (tokenInfo.compare("r15")) {
      type = PPCOperandType::GPR15;
   }
   else if (tokenInfo.compare("r16")) {
      type = PPCOperandType::GPR16;
   }
   else if (tokenInfo.compare("r17")) {
      type = PPCOperandType::GPR17;
   }
   else if (tokenInfo.compare("r18")) {
      type = PPCOperandType::GPR18;
   }
   else if (tokenInfo.compare("r19")) {
      type = PPCOperandType::GPR19;
   }
   else if (tokenInfo.compare("r10")) {
      type = PPCOperandType::GPR10;
   }
   else if (tokenInfo.compare("r11")) {
      type = PPCOperandType::GPR11;
   }
   else if (tokenInfo.compare("r12")) {
      type = PPCOperandType::GPR12;
   }
   else if (tokenInfo.compare("r13")) {
      type = PPCOperandType::GPR13;
   }
   else if (tokenInfo.compare("r14")) {
      type = PPCOperandType::GPR14;
   }
   else if (tokenInfo.compare("r15")) {
      type = PPCOperandType::GPR15;
   }
   else if (tokenInfo.compare("r16")) {
      type = PPCOperandType::GPR16;
   }
   else if (tokenInfo.compare("r17")) {
      type = PPCOperandType::GPR17;
   }
   else if (tokenInfo.compare("r18")) {
      type = PPCOperandType::GPR18;
   }
   else if (tokenInfo.compare("r19")) {
      type = PPCOperandType::GPR19;
   }
   else if (tokenInfo.compare("r20")) {
      type = PPCOperandType::GPR20;
   }
   else if (tokenInfo.compare("r21")) {
      type = PPCOperandType::GPR21;
   }
   else if (tokenInfo.compare("r22")) {
      type = PPCOperandType::GPR22;
   }
   else if (tokenInfo.compare("r23")) {
      type = PPCOperandType::GPR23;
   }
   else if (tokenInfo.compare("r24")) {
      type = PPCOperandType::GPR24;
   }
   else if (tokenInfo.compare("r25")) {
      type = PPCOperandType::GPR25;
   }
   else if (tokenInfo.compare("r26")) {
      type = PPCOperandType::GPR26;
   }
   else if (tokenInfo.compare("r27")) {
      type = PPCOperandType::GPR27;
   }
   else if (tokenInfo.compare("r28")) {
      type = PPCOperandType::GPR28;
   }
   else if (tokenInfo.compare("r29")) {
      type = PPCOperandType::GPR29;
   }
   else if (tokenInfo.compare("r30")) {
      type = PPCOperandType::GPR30;
   }
   else if (tokenInfo.compare("r31")) {
      type = PPCOperandType::GPR31;
   }
   else throw SyntaxError(errorMessage, tokenInfo.lineInfo);

   return PPCOperand(type);
}

PPCOperand PPC64Assembler::readRegister(ScriptToken& tokenInfo, ustr_t errorMessage, bool checkRc)
{
   bool rc = false;
   read(tokenInfo);
   if (checkRc && tokenInfo.compare(".")) {
      rc = true;
      read(tokenInfo);
   }

   PPCOperand operand = defineRegister(tokenInfo, errorMessage);
   operand.rc = rc;
   if (operand.type != PPCOperandType::None) {
      read(tokenInfo);
   }
   else throw SyntaxError(errorMessage, tokenInfo.lineInfo);

   return operand;
}

PPCOperand PPC64Assembler :: readDispOperand(ScriptToken& tokenInfo, int& disp, ref_t& reference, ustr_t errorMessage)
{
   readIOperand(tokenInfo, disp, reference, errorMessage);

   if (!tokenInfo.compare("("))
      throw SyntaxError(ASM_BRACKET_EXPECTED, tokenInfo.lineInfo);

   PPCOperand operand = readRegister(tokenInfo, errorMessage);
   if (!tokenInfo.compare(")")) {
      throw SyntaxError(ASM_BRACKETCLOSE_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo);

   return operand;
}

void PPC64Assembler :: readIOperand(ScriptToken& tokenInfo, int& value, ref_t& reference, ustr_t errorMessage)
{
   read(tokenInfo);

   if (tokenInfo.compare("import")) {
      reference = mskImportRef64;
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
      value = readIntArg(tokenInfo);
   }
   else if (tokenInfo.compare("rdata")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskRDataRef64, errorMessage);

      read(tokenInfo);
      if (tokenInfo.compare("+")) {
         value = readIntArg(tokenInfo);
      }
   }
   else if (tokenInfo.compare("mdata")) {
      reference = mskMDataRef64;
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
      value = readIntArg(tokenInfo);
   }
   else if (tokenInfo.compare("data")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskDataRef64, errorMessage);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("code")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskCodeRef64, errorMessage);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("rdata32_hi")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskRDataRef32Hi, errorMessage);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("rdata32_lo")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskRDataRef32Lo, errorMessage);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("rdata_disp32hi")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskRDataDisp32Hi, errorMessage);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("rdata_disp32lo")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      readPtrOperand(tokenInfo, value, reference, mskRDataDisp32Lo, errorMessage);

      read(tokenInfo);
   }
   else {
      if (!getIntConstant(tokenInfo, value, reference))
         throw SyntaxError(errorMessage, tokenInfo.lineInfo);

      read(tokenInfo);
   }   
}

void PPC64Assembler :: readPtrOperand(ScriptToken& tokenInfo, int& value, ref_t& reference, ref_t mask,
   ustr_t errorMessage)
{
   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      read(tokenInfo);

      if (constants.exist(*tokenInfo.token)) {
         reference = constants.get(*tokenInfo.token) | mask;
         value = 0;
      }
      else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
   }
   else if (tokenInfo.compare(PTR64_ARGUMENT2)) {
      reference = PTR64_2 | mask;
      value = 0;
   }
   else if (tokenInfo.compare("0")) {
      reference = mask;
      value = 0;
   }
   else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
}

int PPC64Assembler :: readIntArg(ScriptToken& tokenInfo)
{
   int i = readInteger(tokenInfo);
   read(tokenInfo);

   return i;
}

void PPC64Assembler :: writeDReference(ScriptToken& tokenInfo, ref_t reference, MemoryWriter& writer, ustr_t errorMessage)
{
   switch (reference) {
      case ARG16_1:
      case INV_ARG16_1:
      case NARG16_1:
      case ARG16_2:
      case NARG16_2:
      case INV_NARG16_2:
      case ARG32HI_1:
      case ARG32LO_1:
      case NARG_1:
      case NARGHI_1:
         writer.Memory()->addReference(reference, writer.position() - 4);
         break;
      case DISP32HI_1:
      case DISP32LO_1:
      case DISP32HI_2:
      case DISP32LO_2:
         writer.Memory()->addReference(reference, writer.position() - 4);
         break;
      default:
         switch (reference & mskAnyRef) {
            case mskRDataRef32Hi:
            case mskRDataRef32Lo:
            case mskRDataDisp32Hi:
            case mskRDataDisp32Lo:
            case mskCodeDisp32Hi:
            case mskCodeDisp32Lo:
               writer.Memory()->addReference(reference, writer.position() - 4);
               break;
            default:
               throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
   }
}

void PPC64Assembler :: declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (labelScope.checkDeclaredLabel(*tokenInfo.token))
      throw SyntaxError(ASM_LABEL_EXISTS, tokenInfo.lineInfo);

   if(!labelScope.declareLabel(*tokenInfo.token, writer))
      throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
}

void PPC64Assembler :: compileADD(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rz = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (rx.isGPR() && ry.isGPR() && rz.isGPR()) {
      writer.writeDWord(PPCHelper::makeXOCommand(31, rx.type, ry.type,
         rz.type, 0, 266, 0));
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileADDE(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rz = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (rx.isGPR() && ry.isGPR() && rz.isGPR()) {
      writer.writeDWord(PPCHelper::makeXOCommand(31, rx.type, ry.type,
         rz.type, 0, 138, 0));
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileBCLR(int bo, int bi, int bh, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeXLCommand(19, bo, bi, bh, 16, 0));
}

void PPC64Assembler :: compileBCCTRL(int bo, int bi, int bh, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeXLCommand(19, bo, bi, bh, 528, 1));
}

void PPC64Assembler :: compileBCCTR(int bo, int bi, int bh, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeXLCommand(19, bo, bi, bh, 528, 0));
}

void PPC64Assembler :: compileBxx(int offset, int aa, int lk, MemoryWriter& writer)
{
   PPCLabelHelper::writeBxx(offset, aa, lk, writer);
}

void PPC64Assembler :: compileBCxx(int bo, int bi, int bd, int aa, int lk, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeBCommand(16, bo, bi, bd >> 2, aa, lk));
}

void PPC64Assembler :: compileRLDICL(PPCOperand ra, PPCOperand rs, int sh, int mb, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeMDCommand(30, rs.type, ra.type, sh & 0x1F, mb << 1, 0, 
      sh >> 5, 0));
}

void PPC64Assembler :: compileRLDICR(PPCOperand ra, PPCOperand rs, int sh, int mb, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeMDCommand(30, rs.type, ra.type, sh, mb & 0x1F, 1,
      mb >> 5, 0));
}

void PPC64Assembler :: compileSUBF(PPCOperand rt, PPCOperand ra, PPCOperand rb, MemoryWriter& writer)
{
   writer.writeDWord(PPCHelper::makeXOCommand(31, rt.type, ra.type, 
      rb.type, 0, 40, 0));
}

bool PPC64Assembler :: compileMFSPR(PPCOperand rt, int spr, MemoryWriter& writer)
{
   if (rt.isGPR()) {
      writer.writeDWord(PPCHelper::makeXFXCommand(31, rt.type, spr, 339));
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileMTSPR(PPCOperand rt, int spr, MemoryWriter& writer)
{
   if (rt.isGPR()) {
      writer.writeDWord(PPCHelper::makeXFXCommand(31, rt.type, spr, 467));
   }
   else return false;

   return true;
}

void PPC64Assembler :: compileADDI(ScriptToken& tokenInfo, PPCOperand rt, PPCOperand ra, int i, 
   ref_t reference, MemoryWriter& writer)
{
   if (rt.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(14, rt.type, ra.type, i));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileADDIS(ScriptToken& tokenInfo, PPCOperand rt, PPCOperand ra, int i,
   ref_t reference, MemoryWriter& writer)
{
   if (rt.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(15, rt.type, ra.type, i));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileANDI(ScriptToken& tokenInfo, PPCOperand ra, PPCOperand rs, int i,
   ref_t reference, MemoryWriter& writer)
{
   if (ra.isGPR() && rs.isGPR() && ra.rc) {
      writer.writeDWord(PPCHelper::makeDCommand(28, rs.type, ra.type, i));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileCMP(ScriptToken& tokenInfo, int bf, int l, PPCOperand ra, PPCOperand rb, MemoryWriter& writer)
{
   if (ra.isGPR() && rb.isGPR()) {
      writer.writeDWord(PPCHelper::makeXCommand(31, bf, l, ra.type, rb.type, 0));
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileCMPI(ScriptToken& tokenInfo, int bf, int l, PPCOperand ra, int i, ref_t reference, MemoryWriter& writer)
{
   if (ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(11, bf, l, ra.type, i));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

bool PPC64Assembler :: compileISEL(PPCOperand rt, PPCOperand ra, PPCOperand rb, int bc, MemoryWriter& writer)
{
   if (rt.isGPR() && ra.isGPR() && rb.isGPR()) {
      writer.writeDWord(PPCHelper::makeACommand(31, rt.type, ra.type,
         rb.type, bc, 15));
   }
   else return false;

   return true;

}

bool PPC64Assembler :: compileOR(PPCOperand ra, PPCOperand rs, PPCOperand rb, MemoryWriter& writer)
{
   if (rs.isGPR() && ra.isGPR() && rb.isGPR()) {
      writer.writeDWord(PPCHelper::makeXCommand(31, rs.type, ra.type,
         rb.type, 444, 0));
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileXOR(PPCOperand ra, PPCOperand rs, PPCOperand rb, MemoryWriter& writer)
{
   if (rs.isGPR() && ra.isGPR() && rb.isGPR()) {
      writer.writeDWord(PPCHelper::makeXCommand(31, rs.type, ra.type,
         rb.type, 316, 0));
   }
   else return false;

   return true;
}

void PPC64Assembler :: compileORI(ScriptToken& tokenInfo, PPCOperand ra, PPCOperand rs, int i, ref_t reference, MemoryWriter& writer)
{
   if (ra.isGPR() && rs.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(15, rs.type, ra.type, i));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileADDI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   compileADDI(tokenInfo, rt, ra, d, reference, writer);
}

void PPC64Assembler :: compileADDIS(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   compileADDIS(tokenInfo, rt, ra, d, reference, writer);
}

void PPC64Assembler :: compileANDI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE, true);

   checkComma(tokenInfo);

   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   compileANDI(tokenInfo, rt, ra, d, reference, writer);
}

void PPC64Assembler :: compileBCL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   int bo = readIntArg(tokenInfo);
   checkComma(tokenInfo);
   int bh = readIntArg(tokenInfo);
   checkComma(tokenInfo);
   int offset = readIntArg(tokenInfo);

   compileBCxx(bo, bh, offset, 0, 1, writer);
}

void PPC64Assembler :: compileBCxx(ScriptToken& tokenInfo, int bo, int bi, MemoryWriter& writer, LabelScope& labelScope)
{
   read(tokenInfo);
   if (!tokenInfo.compare(":")) {
      // if jump forward
      if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
         labelScope.registerJump(*tokenInfo.token, writer);

         compileBCxx(bo, bi, 0, 0, 0, writer);
      }
      // if jump backward
      else {
         int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
         if (abs(offset) > 0xFFFF)
            throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

         compileBCxx(bo, bi, offset, 0, 0, writer);
      }
   }
   else {
      int offset = readIntArg(tokenInfo);

      compileBCxx(bo, bi, offset, 0, 0, writer);
   }
}

void PPC64Assembler :: compileB(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   read(tokenInfo);
   if (!tokenInfo.compare(":")) {
      // if jump forward
      if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
         labelScope.registerJump(*tokenInfo.token, writer);

         compileBxx(0, 0, 0, writer);
      }
      // if jump backward
      else {
         int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
         if (abs(offset) > 0xFFFF)
            throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

         compileBxx(offset, 0, 0, writer);
      }
   }
   else {
      int offset = readIntArg(tokenInfo);

      compileBxx(offset, 0, 0, writer);
   }

   read(tokenInfo);
}

void PPC64Assembler::compileBEQ(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   compileBCxx(tokenInfo, 12, 2, writer, labelScope);

   read(tokenInfo);
}

void PPC64Assembler :: compileBNE(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   compileBCxx(tokenInfo, 4, 10, writer, labelScope);

   read(tokenInfo);
}

void PPC64Assembler :: compileBLT(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   compileBCxx(tokenInfo, 12, 0, writer, labelScope);

   read(tokenInfo);
}

void PPC64Assembler :: compileBGE(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   compileBCxx(tokenInfo, 4, 0, writer, labelScope);

   read(tokenInfo);
}

void PPC64Assembler :: compileBLR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   compileBCLR(20, 0, 0, writer);

   read(tokenInfo);
}


void PPC64Assembler::compileBCTR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   compileBCCTR(20, 0, 0, writer);

   read(tokenInfo);
}

void PPC64Assembler :: compileBCTRL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   compileBCCTRL(20, 0, 0, writer);

   read(tokenInfo);
}

void PPC64Assembler::compileEXTRDI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int n = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, n, reference, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   int b = 0;
   readIOperand(tokenInfo, b, reference, ASM_INVALID_TARGET);

   int dn = 64 - n;
   if (rx.isGPR() && ry.isGPR() && !reference) {
      compileRLDICL(rx, ry, b + n, dn, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileCMP(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rb = readRegister(tokenInfo, ASM_INVALID_TARGET);

   compileCMP(tokenInfo, 0, 0, ra, rb, writer);
}

void PPC64Assembler ::compileCMPWI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   compileCMPI(tokenInfo, 0, 0, ra, d, reference, writer);
}

void PPC64Assembler :: compileOR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rz = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if(!compileOR(rx, ry, rz, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileORI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   compileORI(tokenInfo, ra, rs, d, reference, writer);
}

void PPC64Assembler :: compileLWZ(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   PPCOperand ra = readDispOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (rs.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(32, rs.type, ra.type, d));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileLD(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   PPCOperand ra = readDispOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (rs.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDSCommand(58, rs.type, ra.type, d));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileISELEQ(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rz = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileISEL(rx, ry, rz, 1, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

}

void PPC64Assembler :: compileLIS(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   PPCOperand ra(PPCOperandType::GPR0); // NOTE : that r0 is treated as 0
   compileADDIS(tokenInfo, rs, ra, d, reference, writer);
}

void PPC64Assembler :: compileLI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   PPCOperand ra(PPCOperandType::GPR0); // NOTE : that r0 is treated as 0
   compileADDI(tokenInfo, rs, ra, d, reference, writer);
}

void PPC64Assembler :: compileMFLR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if(!compileMFSPR(rt, 8, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileMR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if(!compileOR(rx, ry, ry, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileMTLR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileMTSPR(rt, 8, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileMTCTR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileMTSPR(rt, 9, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler::compileRLDICL(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int n = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, n, reference, ASM_INVALID_TARGET);

   checkComma(tokenInfo);

   int dn = 0;
   readIOperand(tokenInfo, dn, reference, ASM_INVALID_TARGET);

   if (rx.isGPR() && ry.isGPR() && !reference) {
      compileRLDICL(rx, ry, n, dn, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

}

void PPC64Assembler :: compileSRDI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int n = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, n, reference, ASM_INVALID_TARGET);

   int dn = 64 - n;
   if (rx.isGPR() && ry.isGPR() && !reference) {
      compileRLDICL(rx, ry, dn, n, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSLDI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int n = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, n, reference, ASM_INVALID_TARGET);

   int dn = 63 - n;
   if (rx.isGPR() && ry.isGPR() && !reference) {
      compileRLDICR(rx, ry, n, dn, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSTW(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   PPCOperand ra = readDispOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (rs.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(36, rs.type, ra.type, d));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSTD(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   PPCOperand ra = readDispOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (rs.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDSCommand(62, rs.type, ra.type, d));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSTWU(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   PPCOperand ra = readDispOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (rs.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDCommand(37, rs.type, ra.type, d));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSTDU(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rs = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   PPCOperand ra = readDispOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (rs.isGPR() && ra.isGPR()) {
      writer.writeDWord(PPCHelper::makeDSCommandU(62, rs.type, ra.type, d >> 2));

      if (reference)
         writeDReference(tokenInfo, reference, writer, ASM_INVALID_DESTINATION);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSUB(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rz = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (rx.isGPR() && ry.isGPR() && rz.isGPR()) {
      compileSUBF(rx, rz, ry, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

}

void PPC64Assembler :: compileSUBF(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rb = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (rt.isGPR() && ra.isGPR() && rb.isGPR()) {
      compileSUBF(rt, ra, rb, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileSUBI(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rt = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ra = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (reference == ARG16_1) {
      reference = INV_ARG16_1;
   }
   else if (reference == NARG16_2) {
      reference = INV_NARG16_2;
   }
   else if (reference)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);

   if (rt.isGPR() && ra.isGPR()) {
      compileADDI(tokenInfo, rt, ra, -d, reference, writer);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler :: compileXOR(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   PPCOperand rx = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand ry = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   PPCOperand rz = readRegister(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileXOR(rx, ry, rz, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void PPC64Assembler::compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   int d = readIntArg(tokenInfo);
   writer.writeByte(d);
}

void PPC64Assembler::compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   int d = readIntArg(tokenInfo);
   writer.writeWord(d);
}

void PPC64Assembler::compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   int d = readIntArg(tokenInfo);
   writer.writeDWord(d);
}

void PPC64Assembler::compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   int d = 0;
   ref_t reference = 0;
   readIOperand(tokenInfo, d, reference, ASM_INVALID_TARGET);

   if (reference) {
      writer.writeQReference(reference, d);
   }
   else writer.writeQWord(d);
}

bool PPC64Assembler :: compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("addi")) {
      compileADDI(tokenInfo, writer);
   }
   else if (tokenInfo.compare("addis")) {
      compileADDIS(tokenInfo, writer);
   }
   else if (tokenInfo.compare("add")) {
      compileADD(tokenInfo, writer);
   }
   else if (tokenInfo.compare("adde")) {
      compileADDE(tokenInfo, writer);
   }
   else if (tokenInfo.compare("andi")) {
      compileANDI(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (tokenInfo.compare("blr")) {
      compileBLR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("b")) {
      compileB(tokenInfo, writer, labelScope);
   }
   else if (tokenInfo.compare("bctrl")) {
      compileBCTRL(tokenInfo, writer);
   }
   else if (tokenInfo.compare("bcl")) {
      compileBCL(tokenInfo, writer);
   }
   else if (tokenInfo.compare("bctr")) {
      compileBCTR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("beq")) {
      compileBEQ(tokenInfo, writer, labelScope);
   }
   else if (tokenInfo.compare("bne")) {
      compileBNE(tokenInfo, writer, labelScope);
   }
   else if (tokenInfo.compare("blt")) {
      compileBLT(tokenInfo, writer, labelScope);
   }
   else if (tokenInfo.compare("bge")) {
      compileBGE(tokenInfo, writer, labelScope);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("cmp")) {
      compileCMP(tokenInfo, writer);
   }
   else if (tokenInfo.compare("cmpwi")) {
      compileCMPWI(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("extrdi")) {
      compileEXTRDI(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileIOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("iseleq")) {
      compileISELEQ(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileJOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   return false;
}

bool PPC64Assembler :: compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("lwz")) {
      compileLWZ(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ld")) {
      compileLD(tokenInfo, writer);
   }
   else if (tokenInfo.compare("lis")) {
      compileLIS(tokenInfo, writer);
   }
   else if (tokenInfo.compare("li")) {
      compileLI(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("mflr")) {
      compileMFLR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("mr")) {
      compileMR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("mtlr")) {
      compileMTLR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("mtctr")) {
      compileMTCTR(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler::compileNOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool PPC64Assembler::compileOOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("or")) {
      compileOR(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ori")) {
      compileORI(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compilePOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool PPC64Assembler :: compileROpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("rldicl")) {
      compileRLDICL(tokenInfo, writer);
   }
   return false;
}

bool PPC64Assembler :: compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("srdi")) {
      compileSRDI(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sldi")) {
      compileSLDI(tokenInfo, writer);
   }
   else if (tokenInfo.compare("stw")) {
      compileSTW(tokenInfo, writer);
   }
   else if (tokenInfo.compare("std")) {
      compileSTD(tokenInfo, writer);
   }
   else if (tokenInfo.compare("stdu")) {
      compileSTDU(tokenInfo, writer);
   }
   else if (tokenInfo.compare("stwu")) {
      compileSTWU(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sub")) {
      compileSUB(tokenInfo, writer);
   }
   else if (tokenInfo.compare("subf")) {
      compileSUBF(tokenInfo, writer);
   }
   else if (tokenInfo.compare("subi")) {
      compileSUBI(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool PPC64Assembler :: compileTOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool PPC64Assembler :: compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("xor")) {
      compileXOR(tokenInfo, writer);
   }
   else return false;

   return true;
}

void PPC64Assembler :: compileProcedure(ScriptToken& tokenInfo)
{
   PPCLabelHelper helper;

   AssemblerBase::compileProcedure(tokenInfo, &helper);
}
