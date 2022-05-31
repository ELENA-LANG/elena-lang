//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA Intel X86 Assembler
//		classes.
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "x86assembler.h"
#include "asmconst.h"

using namespace elena_lang;

// --- X86Assembler ---

X86Operand X86Assembler :: defineDBDisp(X86Operand operand)
{
   operand.type = operand.type | X86OperandType::M32disp8;

   return operand;
}

X86Operand X86Assembler :: defineDDDisp(X86Operand operand)
{
   operand.type = operand.type | X86OperandType::M32disp32;

   return operand;
}

bool X86Assembler :: setOffsetValue(X86Operand& operand, X86Operand disp)
{
   if (disp.reference == 0) {
      operand.offset += disp.offset;
   }
   else if (operand.reference == 0) {
      operand.offset += disp.offset;
      operand.reference = disp.reference;
   }
   else return false;

   return true;
}

bool X86Assembler :: setOffset(X86Operand& operand, X86Operand disp)
{
   if (!setOffsetValue(operand, disp))
      return false;

   if (disp.type == X86OperandType::DB) {
      if (test(operand.type, X86OperandType::M16)) {
         operand.type = operand.type | X86OperandType::M16disp8;
      }
      else if (test(operand.type, X86OperandType::M8)) {
         operand.type = operand.type | X86OperandType::M8disp8;
      }
      else operand = defineDBDisp(operand);

      return true;
   }
   else if (disp.type == X86OperandType::DD) {
      operand = defineDDDisp(operand);

      return true;
   }
   return false;
}

X86Operand X86Assembler :: defineRegister(ustr_t token)
{
   if (token.compare("al")) {
      return X86Operand(X86OperandType::AL);
   }
   else if (token.compare("ah")) {
      return X86Operand(X86OperandType::AH);
   }
   else if (token.compare("ax")) {
      return X86Operand(X86OperandType::AX);
   }
   else if (token.compare("eax")) {
      return X86Operand(X86OperandType::EAX);
   }
   else if (token.compare("ebx")) {
      return X86Operand(X86OperandType::EBX);
   }
   else if (token.compare("cl")) {
      return X86Operand(X86OperandType::CL);
   }
   else if (token.compare("cx")) {
      return X86Operand(X86OperandType::CX);
   }
   else if (token.compare("ecx")) {
      return X86Operand(X86OperandType::ECX);
   }
   else if (token.compare("edx")) {
      return X86Operand(X86OperandType::EDX);
   }
   else if (token.compare("esp")) {
      return X86Operand(X86OperandType::ESP);
   }
   else if (token.compare("ebp")) {
      return X86Operand(X86OperandType::EBP);
   }
   else if (token.compare("esi")) {
      return X86Operand(X86OperandType::ESI);
   }
   else if (token.compare("edi")) {
      return X86Operand(X86OperandType::EDI);
   }
   else return X86Operand(X86OperandType::Unknown);
}

X86Operand X86Assembler :: defineOperand(ScriptToken& tokenInfo, X86OperandType prefix, ustr_t errorMessage)
{
   X86Operand operand = defineRegister(*tokenInfo.token);
   if (operand.type == X86OperandType::Unknown) {
      if (getIntConstant(tokenInfo, operand.offset, operand.reference)) {
         setOffsetSize(operand, prefix);

         // !! HOTFIX: 000000080 constant should be considered as int32 rather then int8
         if (tokenInfo.token.length() == 8 && operand.type == X86OperandType::DB)
            operand.type = X86OperandType::DD;
      }
      else if (tokenInfo.compare("data")) {
         operand = compileDataOperand(tokenInfo, errorMessage, false);
      }
      else if (tokenInfo.compare("rdata")) {
         operand = compileDataOperand(tokenInfo, errorMessage, true);
      }
      else if (tokenInfo.compare("mdata")) {
         operand = compileMDataOperand(tokenInfo, errorMessage);
      }
      else if (!errorMessage.empty()) {
         throw SyntaxError(errorMessage, tokenInfo.lineInfo);
      }
   }

   return operand;
}

X86Operand X86Assembler :: defineDisplacement(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   if (tokenInfo.compare("+")) {
      read(tokenInfo);

      return defineOperand(tokenInfo, X86OperandType::DD, errorMessage);
   }
   else if (tokenInfo.compare("-")) {
      read(tokenInfo);

      X86Operand disp = defineOperand(tokenInfo, X86OperandType::DD, errorMessage);
      if (disp.type == X86OperandType::DD || disp.type == X86OperandType::DB) {
         disp.offset = -disp.offset;

         return disp;
      }
      else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
   }
   else if (tokenInfo.token[0]=='-' && (tokenInfo.state == dfaInteger || tokenInfo.state == dfaHexInteger)) {
      X86Operand disp = defineOperand(tokenInfo, X86OperandType::DD, errorMessage);
      if (disp.type == X86OperandType::DD || disp.type == X86OperandType::DB) {
         return disp;
      }
      else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
   }
   else if (tokenInfo.compare("*")) {
      read(tokenInfo);
      if (tokenInfo.compare("4")) {
         return X86Operand(X86OperandType::Factor4);
      }
      else if (tokenInfo.compare("8")) {
         return X86Operand(X86OperandType::Factor8);
      }
      else if (tokenInfo.compare("2")) {
         return X86Operand(X86OperandType::Factor2);
      }
      else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
   }

   return X86Operand();
}

X86Operand X86Assembler :: defineRDisp(X86Operand operand)
{
   // if it is register disp [r]
   if (operand.ebpReg && operand.type == X86OperandType::Disp32) {
      operand.type = operand.type | X86OperandType::M32disp8;
      operand.offset = 0;
   }
   return operand;
}

X86Operand X86Assembler :: readDispOperand(ScriptToken& tokenInfo, X86Operand operand, X86OperandType prefix,
   ustr_t errorMessage)
{
   if (operand.type == X86OperandType::Disp32 && !operand.ebpReg) {
      read(tokenInfo);
      X86Operand disp = defineDisplacement(tokenInfo, errorMessage);
      if (disp.type == X86OperandType::DD || disp.type == X86OperandType::DB) {
         if (!setOffsetValue(operand, disp))
            throw SyntaxError(errorMessage, tokenInfo.lineInfo);

         read(tokenInfo);
      }
      else if (disp.type != X86OperandType::Unknown)
         throw SyntaxError(errorMessage, tokenInfo.lineInfo);

      // if it is [disp]
      return operand;
   }
   else {
      read(tokenInfo);
      X86Operand disp = defineDisplacement(tokenInfo, errorMessage);

      if (disp.type == X86OperandType::DD || disp.type == X86OperandType::DB) {
         // if it is [r + disp]
         if (!setOffset(operand, disp))
            throw SyntaxError(errorMessage, tokenInfo.lineInfo);

         read(tokenInfo);
      }
      else if (disp.type == X86OperandType::Factor2 || disp.type == X86OperandType::Factor4 || disp.type == X86OperandType::Factor8) {
         read(tokenInfo);

         X86Operand disp2 = defineDisplacement(tokenInfo, errorMessage);
         int sibcode = ((char)X86OperandType::Disp32 + ((char)operand.type << 3)) << 24;
         operand.type = (operand.type & 0xFFFFFFF8) | X86OperandType::SIB | disp.type | (X86OperandType)sibcode;
         if (disp2.type == X86OperandType::DD || disp2.type == X86OperandType::DB) {
            // if it is [r*factor + disp]
            operand.offset = disp2.offset;
            operand.reference = disp2.reference;
            operand.factorReg = true;

            read(tokenInfo);
         }
         else if (disp2.type == X86OperandType::Unknown) {
            // if it is [r*factor]
            operand.offset = 0;
            operand.reference = 0;
            operand.factorReg = true;
         }
         else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
      }
      else if (disp.type != X86OperandType::Unknown) {
         read(tokenInfo);

         X86Operand disp2 = defineDisplacement(tokenInfo, errorMessage);
         if (disp2.type == X86OperandType::DD || disp2.type == X86OperandType::DB) {
            // if it is [r + r + disp]
            int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

            if (!setOffset(operand, disp2))
               throw SyntaxError(errorMessage, tokenInfo.lineInfo);

            operand.type = (operand.type & 0xFFFFFFF8) | X86OperandType::SIB | (X86OperandType)sibcode;

            read(tokenInfo);
         }
         else if (disp2.type == X86OperandType::Factor2 || disp2.type == X86OperandType::Factor4 || disp2.type == X86OperandType::Factor8) {
            read(tokenInfo);

            X86Operand disp3 = defineDisplacement(tokenInfo, errorMessage);
            if (disp3.type == X86OperandType::DD || disp3.type == X86OperandType::DB) {
               // if it is [r + r*factor + disp]
               int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

               if (!setOffset(operand, disp3))
                  throw SyntaxError(errorMessage, tokenInfo.lineInfo);

               operand.type = (operand.type & 0xFFFFFFF8) | X86OperandType::SIB | (X86OperandType)sibcode | disp2.type;

               read(tokenInfo);
            }
            else if (disp3.type == X86OperandType::Unknown) {
               // if it is [r + r*factor]
               int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

               operand = defineDBDisp(operand);

               operand.type = (operand.type & 0xFFFFFFF8) | X86OperandType::SIB | (X86OperandType)sibcode | disp2.type;
            }
            else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
         }
         else if (disp2.type == X86OperandType::Unknown) {
            // if it is [r + r]
            int sibcode = ((char)operand.type + ((char)disp.type << 3)) << 24;

            operand.type = (operand.type & 0xFFFFFFF8) | X86OperandType::SIB | (X86OperandType)sibcode;
         }
         else throw SyntaxError(errorMessage, tokenInfo.lineInfo);
      }
      else return defineRDisp(operand);
   }
   return operand;
}

X86Operand X86Assembler :: readDispOperand(ScriptToken& tokenInfo, X86OperandType prefix, ustr_t errorMessage)
{
   X86Operand operand = defineOperand(tokenInfo, prefix, errorMessage);
   if (operand.type != X86OperandType::Unknown) {
      operand.type = X86Helper::addPrefix(operand.type, prefix);
      return readDispOperand(tokenInfo, operand, prefix, errorMessage);
   }
   else return operand;
}

X86Operand X86Assembler :: readPtrOperand(ScriptToken& tokenInfo, X86OperandType prefix, ustr_t errorMessage)
{
   read(tokenInfo);
   if (tokenInfo.compare("ptr"))
      read(tokenInfo);

   if (tokenInfo.compare("[")) {
      read(tokenInfo);
      X86Operand operand = readDispOperand(tokenInfo, prefix, errorMessage);
      if (!tokenInfo.compare("]")) {
         throw SyntaxError(ASM_SBRACKETCLOSE_EXPECTED, tokenInfo.lineInfo);
      }
      else read(tokenInfo);

      return operand;
   }
   else {
      X86Operand operand = defineOperand(tokenInfo, prefix, errorMessage);
      if (operand.type == X86OperandType::DD && operand.reference == 0 || operand.type == X86OperandType::DB) {
         if (prefix == X86OperandType::M16) {
            operand.type = X86OperandType::DW;
         }
         else if (prefix == X86OperandType::M8) {
            operand.type = X86OperandType::DB;
         }
         else if (prefix == X86OperandType::M32){
            operand.type = X86OperandType::DD;
         }
      }
      else throw SyntaxError(ASM_SBRACKET_EXPECTED, tokenInfo.lineInfo);

      read(tokenInfo);

      return operand;
   }
}

X86Operand X86Assembler :: compileCodeOperand(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   X86Operand operand;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      operand.type = X86OperandType::DD;
      operand.reference = readReference(tokenInfo) | mskCodeRef32;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);

   return operand;
}

X86Operand X86Assembler :: compileDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage, bool rdataMode)
{
   X86Operand operand;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   ref_t mask = rdataMode ? mskRDataRef32 : mskDataRef32;

   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      operand.type = X86OperandType::DD;
      operand.reference = readReference(tokenInfo) | mask;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);

   return operand;
}

X86Operand X86Assembler :: compileMDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   X86Operand operand;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   ref_t mask = mskMDataRef32;

   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      operand.type = X86OperandType::DD;
      operand.reference = readReference(tokenInfo) | mask;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);

   return operand;
}

X86Operand X86Assembler :: compileOperand(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   X86Operand operand = {};

   read(tokenInfo);
   if (tokenInfo.compare("[")) {
      read(tokenInfo);
      operand = readDispOperand(tokenInfo, getDefaultPrefix(), errorMessage);
      if (!tokenInfo.compare("]")) {
         throw SyntaxError(ASM_SBRACKET_EXPECTED, tokenInfo.lineInfo);
      }
      else read(tokenInfo);
   }
   else if (tokenInfo.compare("dword")) {
      read(tokenInfo, "ptr", ASM_PTR_EXPECTED);

      operand = readPtrOperand(tokenInfo, X86OperandType::M32, errorMessage);
   }
   else if (tokenInfo.compare("qword")) {
      read(tokenInfo, "ptr", ASM_PTR_EXPECTED);

      operand = readPtrOperand(tokenInfo, X86OperandType::M64, errorMessage);
   }
   else if (tokenInfo.compare("byte")) {
      read(tokenInfo, "ptr", ASM_PTR_EXPECTED);

      operand = readPtrOperand(tokenInfo, X86OperandType::M8, errorMessage);
   }
   else if (tokenInfo.compare("word")) {
      read(tokenInfo, "ptr", ASM_PTR_EXPECTED);

      operand = readPtrOperand(tokenInfo, X86OperandType::M16, errorMessage);
   }
   else if (tokenInfo.compare("code")) {
      operand = compileCodeOperand(tokenInfo, errorMessage);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("data") || tokenInfo.compare("rdata")) {
      operand = compileDataOperand(tokenInfo, errorMessage, tokenInfo.compare("rdata"));

      read(tokenInfo);
      if (tokenInfo.compare("+")) {
         operand.offset += readInteger(tokenInfo);

         read(tokenInfo);
      }
   }
   else {
      operand = defineOperand(tokenInfo, X86OperandType::DD, errorMessage);
      if (operand.type != X86OperandType::Unknown) {
         read(tokenInfo);

         if (tokenInfo.compare("+")) {
            operand.offset += readInteger(tokenInfo);

            read(tokenInfo);
         }
      }
   }

   return operand;
}

void X86Assembler :: declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (labelScope.checkDeclaredLabel(*tokenInfo.token))
      throw SyntaxError(ASM_LABEL_EXISTS, tokenInfo.lineInfo);

   if (!labelScope.declareLabel(*tokenInfo.token, writer))
      throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
}

void X86Assembler :: compileAdc(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileAdc(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler::compileAdd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileAdd(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileAnd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileAnd(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileCall(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   if (tokenInfo.compare("extern")) {
      compileExternCall(tokenInfo, writer);
   }
   else if (tokenInfo.compare("%")) {
      X86Operand operand(X86OperandType::DD);
      operand.reference = readReference(tokenInfo);

      if (!compileCall(operand, writer))
         throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
   }
   else if (tokenInfo.compare("[")) {
      read(tokenInfo);
      X86Operand operand = readDispOperand(tokenInfo, getDefaultPrefix(), INVALID_CALL_TARGET);
      if (!tokenInfo.compare("]"))
         throw SyntaxError(ASM_SBRACKETCLOSE_EXPECTED, tokenInfo.lineInfo);

      if (operand.isM32() || operand.isM64()) {
         if (!compileCall(operand, writer))
            throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
      }
   }
   else if (tokenInfo.compare(RELPTR32_ARGUMENT1)) {
      X86Operand operand(X86OperandType::DD);
      operand.reference = RELPTR32_1;

      if (!compileCall(operand, writer))
         throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
   }
   else if (tokenInfo.compare(RELPTR32_ARGUMENT2)) {
      X86Operand operand(X86OperandType::DD);
      operand.reference = RELPTR32_2;

      if (!compileCall(operand, writer))
         throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
   }
   else {
      X86Operand operand = defineRegister(*tokenInfo.token);
      if (operand.isR32() || operand.isR64()) {
         if (!compileCall(operand, writer))
            throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
      }
      else throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
   }

   read(tokenInfo);
}

void X86Assembler :: compileCdq(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0x99);

   read(tokenInfo);
}

void X86Assembler :: compileCmp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileCmp(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileCMovcc(ScriptToken& tokenInfo, MemoryWriter& writer, X86JumpType type)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileCMovcc(sour, dest, writer, type))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileDiv(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileDiv(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileIDiv(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileIDiv(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileIMul(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileIMul(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileJcc(ScriptToken& tokenInfo, MemoryWriter& writer, X86JumpType type, LabelScope& labelScope)
{
   read(tokenInfo);

   bool shortJump = false;
   if (tokenInfo.compare("short")) {
      shortJump = true;
      read(tokenInfo);
   }

   // if jump forward
   if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
      labelScope.registerJump(*tokenInfo.token, writer);

      if(!compileJccForward(type, shortJump, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
   // if jump backward
   else {
      int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
      if (shortJump && abs(offset) > 0x7F)
         throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

      if (!compileJccBack(type, shortJump, offset, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }

   read(tokenInfo);
}

void X86Assembler :: compileJmp(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   X86Operand operand = compileOperand(tokenInfo, nullptr);

   if (operand.isR32_M32() || operand.isR64_M64() || operand.isRX64_MX64()) {
      if(!compileJmp(operand, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
   else if (operand.type == X86OperandType::DD && operand.reference == INVALID_REF) {
      if(!compileJmp(operand, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
   else {
      bool shortJump = false;
      if (tokenInfo.compare("short")) {
         shortJump = true;
         read(tokenInfo);
      }

      // if jump forward
      if (!labelScope.checkDeclaredLabel(*tokenInfo.token)) {
         labelScope.registerJump(*tokenInfo.token, writer);

         if (!compileJmpForward(shortJump, writer))
            throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
      }
      // if jump backward
      else {
         int offset = labelScope.resolveLabel(*tokenInfo.token, writer);
         if (shortJump && abs(offset) > 0x7F)
            throw SyntaxError(ASM_JUMP_TOO_LONG, tokenInfo.lineInfo);

         if (!compileJmpBack(shortJump, offset, writer))
            throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
      }

      read(tokenInfo);
   }
}

void X86Assembler :: compileLea(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileLea(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileMov(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if(!compileMov(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileMul(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileMul(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileOr(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileOr(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileNeg(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileNeg(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileNop(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0x90);

   read(tokenInfo);
}

void X86Assembler :: compilePop(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compilePop(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compilePush(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compilePush(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileRcr(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileRcr(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileRep(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0xF3);
   read(tokenInfo);
}

void X86Assembler :: compileRet(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeByte(0xC3);
}

void X86Assembler :: compileSetcc(ScriptToken& tokenInfo, MemoryWriter& writer, X86JumpType type)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileSetcc(sour, type, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileShr(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileShr(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileSbb(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileSbb(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler::compileStosd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0xAB);
   read(tokenInfo);
}

void X86Assembler :: compileSub(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileSub(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileTest(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileTest(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler::compileXor(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileXor(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileDBField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);
   if (sour.type != X86OperandType::Unknown)
      read(tokenInfo);

   if (sour.type == X86OperandType::DB) {
      X86Helper::writeImm(writer, sour);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileDWField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (sour.type == X86OperandType::DB)
      sour.type = X86OperandType::DW;

   if (sour.type == X86OperandType::DW) {
      X86Helper::writeImm(writer, sour);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileDDField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (sour.type == X86OperandType::DB)
      sour.type = X86OperandType::DD;

   if (sour.type == X86OperandType::DD) {
      X86Helper::writeImm(writer, sour);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileDQField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (sour.type == X86OperandType::DB)
      sour.type = X86OperandType::DQ;

   if (sour.type == X86OperandType::DQ) {
      X86Helper::writeImm(writer, sour);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

bool X86Assembler :: compileAdc(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x11);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileAdd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.type == X86OperandType::EAX && target.type == X86OperandType::DD) {
      writer.writeByte(0x05);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 0), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DD) {
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 0), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x01);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x00);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x01);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileAnd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x23);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileCall(X86Operand source, MemoryWriter& writer)
{
   if (source.type == X86OperandType::DD) {
      writer.writeByte(0xE8);
      if (source.reference == RELPTR32_1 || source.reference == RELPTR32_2) {
         writer.writeDReference(source.reference, source.offset);
      }
      else writer.writeDReference(source.reference | mskCodeRelRef32, source.offset);
   }
   else if (source.isR32()) {
      writer.writeByte(0xFF);
      writer.writeByte(0xD0 + (char)source.type);
   }
   else if (source.isM32()) {
      writer.writeByte(0xFF);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 2), source);
      X86Helper::writeImm(writer, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileCmp(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x3B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x39);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16() && target.isR16_M16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x3B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DD) {
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 7), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x39);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8() && target.isR8_M8()) {
      writer.writeByte(0x3A);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x38);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileCMovcc(X86Operand source, X86Operand target, MemoryWriter& writer, X86JumpType type)
{
   if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x0F);
      writer.writeByte(0x40 + (int)type);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileDiv(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 6 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileIDiv(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 7 }, source);
   }
   else if (source.isR8_M8()) {
      writer.writeByte(0xF6);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 7 }, source);
   }
   else if (source.isR16_M16()) {
      writer.writeByte(0x66);
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 7 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileIMul(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()){
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 5 }, source);
   }
   else if (source.isR8_M8()) {
      writer.writeByte(0xF6);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 5 }, source);
   }
   else if (source.isR16_M16()) {
      writer.writeByte(0x66);
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 5 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileJccForward(X86JumpType type, bool shortJump, MemoryWriter& writer)
{
   if (shortJump) {
      writer.writeByte((char)type + 0x70);
      writer.writeByte(0);
   }
   else {
      writer.writeByte(0x0F);
      writer.writeByte((char)type + 0x80);
      writer.writeDWord(0);
   }

   return true;
}

bool X86Assembler :: compileJccBack(X86JumpType type, bool shortJump, int offset, MemoryWriter& writer)
{
   if (shortJump) {
      // to exclude the command itself
      offset -= 2;

      writer.writeByte((char)type + 0x70);
      writer.writeByte(offset);
   }
   else {
      // to exclude the command itself
      offset -= 6;

      writer.writeByte(0x0F);
      writer.writeByte((char)type + 0x80);
      writer.writeDWord(offset);
   }

   return true;
}

bool X86Assembler :: compileJmp(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xFF);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
   }
   else if (source.type == X86OperandType::DD && source.reference == INVALID_REF) {
      writer.writeByte(0xE9);
      writer.writeDReference(source.reference, 0);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileJmpForward(bool shortJump, MemoryWriter& writer)
{
   if (shortJump) {
      writer.writeByte(0xEB);
      writer.writeByte(0);
   }
   else {
      writer.writeByte(0xE9);
      writer.writeDWord(0);
   }

   return true;
}

bool X86Assembler :: compileJmpBack(bool shortJump, int offset, MemoryWriter& writer)
{
   if (shortJump) {
      // to exclude the command itself
      offset -= 2;

      writer.writeByte(0xEB);
      writer.writeByte(offset);
   }
   else {
      // to exclude the command itself
      offset -= 5;

      writer.writeByte(0xE9);
      writer.writeDWord(offset);
   }

   return true;
}

bool X86Assembler :: compileLea(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.isM32()) {
      writer.writeByte(0x8D);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileMov(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if(source.type == X86OperandType::EAX && target.type == X86OperandType::Disp32) {
      writer.writeByte(0xA1);
      if (target.reference != 0) {
         writer.writeDReference(target.reference, target.offset);
      }
      else writer.writeDWord(target.offset);
   }
   else if (source.type == X86OperandType::Disp32 && target.type == X86OperandType::EAX) {
      writer.writeByte(0xA3);
      if (target.reference != 0) {
         writer.writeDReference(target.reference, target.offset);
      }
      else writer.writeDWord(target.offset);
   }
   else if (source.isR32() && target.isDB_DD()) {
      target.type = X86OperandType::DD;
      writer.writeByte(0xB8 + (char)source.type);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isM32() && target.isR32()) {
      writer.writeByte(0x89);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x89);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x88);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

void X86Assembler :: compileMovsb(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0xA4);
   read(tokenInfo);
}

bool X86Assembler :: compileMul(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 4 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler::compileNeg(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 3 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileOr(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x09);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compilePop(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32()) {
      writer.writeByte(0x58 + (char)source.type);
   }
   else return false;

   return true;
}

bool X86Assembler :: compilePush(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32()) {
      writer.writeByte(0x50 + (char)source.type);
   }
   else if (source.isM32()) {
      writer.writeByte(0xFF);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 6 }, source);
   }
   else if (source.type == X86OperandType::DB) {
      writer.writeByte(0x6A);
      writer.writeByte(source.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileRcr(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.type == X86OperandType::DB) {
      writer.writeByte(0xD1);
      X86Helper::writeModRM(writer, {X86OperandType::R32 + 3 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileSbb(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x19);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x1B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 3), source);
      writer.writeByte(target.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileSetcc(X86Operand source, X86JumpType type, MemoryWriter& writer)
{
   if (source.isR8()) {
      writer.writeByte(0x0F);
      writer.writeByte(0x90 + (char)type);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R8 + 0), source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileShr(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.type == X86OperandType::DB) {
      writer.writeByte(0xC1);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
      writer.writeByte(target.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileSub(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32_M32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DD) {
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x2B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x29);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x28);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x29);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileTest(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x85);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileXor(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.isR32()) {
      writer.writeByte(0x33);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

void X86Assembler :: compileExternCall(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeWord(0x15FF);
   read(tokenInfo);
   if (tokenInfo.compare(PTR32_ARGUMENT1)) {
      X86Operand operand(X86OperandType::DD);
      operand.reference = PTR32_1;
      operand.offset = 0;

      X86Helper::writeImm(writer, operand);
   }
   else throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
}

bool X86Assembler::compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("adc")) {
      compileAdc(tokenInfo, writer);
   }
   else if (tokenInfo.compare("add")) {
      compileAdd(tokenInfo, writer);
   }
   else if (tokenInfo.compare("and")) {
      compileAnd(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileBOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   return false;
}

bool X86Assembler :: compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("call")) {
      compileCall(tokenInfo, writer);
   }
   else if (tokenInfo.compare("cdq")) {
      compileCdq(tokenInfo, writer);
   }
   else if (tokenInfo.compare("cmp")) {
      compileCmp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("cmovz")) {
      compileCMovcc(tokenInfo, writer, X86JumpType::JZ);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("div")) {
      compileDiv(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler::compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool X86Assembler :: compileIOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("idiv")) {
      compileIDiv(tokenInfo, writer);
   }
   else if (tokenInfo.compare("imul")) {
      compileIMul(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileJOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   if (tokenInfo.compare("jz") || tokenInfo.compare("je")) {
      compileJcc(tokenInfo, writer, X86JumpType::JZ, labelScope);
   }
   else if (tokenInfo.compare("jnz")) {
      compileJcc(tokenInfo, writer, X86JumpType::JNZ, labelScope);
   }
   else if (tokenInfo.compare("jb")) {
      compileJcc(tokenInfo, writer, X86JumpType::JB, labelScope);
   }
   else if (tokenInfo.compare("jbe")) {
      compileJcc(tokenInfo, writer, X86JumpType::JBE, labelScope);
   }
   else if (tokenInfo.compare("jmp")) {
      compileJmp(tokenInfo, writer, labelScope);
   }
   else if (tokenInfo.compare("jae")) {
      compileJcc(tokenInfo, writer, X86JumpType::JAE, labelScope);
   }
   else if (tokenInfo.compare("ja")) {
      compileJcc(tokenInfo, writer, X86JumpType::JA, labelScope);
   }
   else if (tokenInfo.compare("jge")) {
      compileJcc(tokenInfo, writer, X86JumpType::JGE, labelScope);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("lea")) {
      compileLea(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("mov")) {
      compileMov(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movsb")) {
      compileMovsb(tokenInfo, writer);
   }
   else if (tokenInfo.compare("mul")) {
      compileMul(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileNOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("neg")) {
      compileNeg(tokenInfo, writer);
   }
   else if (tokenInfo.compare("nop")) {
      compileNop(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileOOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("or")) {
      compileOr(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler::compilePOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("pop")) {
      compilePop(tokenInfo, writer);
   }
   else if (tokenInfo.compare("push")) {
      compilePush(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler::compileROpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("rcr")) {
      compileRcr(tokenInfo, writer);
   }
   else if (tokenInfo.compare("rep")) {
      compileRep(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ret")) {
      compileRet(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler::compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("sbb")) {
      compileSbb(tokenInfo, writer);
   }
   else if (tokenInfo.compare("setnc")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JAE);
   }
   else if (tokenInfo.compare("shr")) {
      compileShr(tokenInfo, writer);
   }
   else if (tokenInfo.compare("stos")) {
      compileStos(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sub")) {
      compileSub(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileTOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("test")) {
      compileTest(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("xor")) {
      compileXor(tokenInfo, writer);
   }
   else return false;

   return true;
}

void X86Assembler :: compileProcedure(ScriptToken& tokenInfo)
{
   X86LabelHelper helper;

   AssemblerBase::compileProcedure(tokenInfo, &helper);
}

// --- X86_64Assembler ---

X86Operand X86_64Assembler :: defineRegister(ustr_t token)
{
   if (token.compare("rax")) {
      return X86Operand(X86OperandType::RAX);
   }
   else if (token.compare("rbx")) {
      return X86Operand(X86OperandType::RBX);
   }
   else if (token.compare("rcx")) {
      return X86Operand(X86OperandType::RCX);
   }
   else if (token.compare("rdx")) {
      return X86Operand(X86OperandType::RDX);
   }
   else if (token.compare("rsp")) {
      return X86Operand(X86OperandType::RSP);
   }
   else if (token.compare("rbp")) {
      return X86Operand(X86OperandType::RBP);
   }
   else if (token.compare("rsi")) {
      return X86Operand(X86OperandType::RSI);
   }
   else if (token.compare("rdi")) {
      return X86Operand(X86OperandType::RDI);
   }
   else if (token.compare("esp")) {
      return X86Operand(X86OperandType::Unknown);
   }
   else if (token.compare("ebp")) {
      return X86Operand(X86OperandType::Unknown);
   }
   else if (token.compare("r8")) {
      return X86Operand(X86OperandType::RX8);
   }
   else if (token.compare("r9")) {
      return X86Operand(X86OperandType::RX9);
   }
   else if (token.compare("r10")) {
      return X86Operand(X86OperandType::RX10);
   }
   else if (token.compare("r11")) {
      return X86Operand(X86OperandType::RX11);
   }
   else if (token.compare("r12")) {
      return X86Operand(X86OperandType::RX12);
   }
   else if (token.compare("r13")) {
      return X86Operand(X86OperandType::RX13);
   }
   else if (token.compare("r14")) {
      return X86Operand(X86OperandType::RX14);
   }
   else if (token.compare("r15")) {
      return X86Operand(X86OperandType::RX15);
   }
   else return X86Assembler::defineRegister(token);
}

X86Operand X86_64Assembler :: defineRDisp(X86Operand operand)
{
   // if it is register disp [r]
   if (operand.ebpReg && operand.type == X86OperandType::Disp64) {
      operand.type = operand.type | X86OperandType::M64disp8;
      operand.offset = 0;
   }
   return operand;
}

X86Operand X86_64Assembler :: defineDBDisp(X86Operand operand)
{
   //if (operand.type )
   if (test(operand.type, X86OperandType::MX64)) {
      operand.type = operand.type | X86OperandType::MX64disp8;
   }
   else operand.type = operand.type | X86OperandType::M64disp8;

   return operand;
}

X86Operand X86_64Assembler :: defineDDDisp(X86Operand operand)
{
   operand.type = operand.type | X86OperandType::M64disp32;

   return operand;
}

X86Operand X86_64Assembler :: readDispOperand(ScriptToken& tokenInfo, X86Operand operand, X86OperandType prefix,
                                              ustr_t errorMessage)
{
   if (operand.type == X86OperandType::Disp64 && !operand.ebpReg) {
      read(tokenInfo);
      X86Operand disp = defineDisplacement(tokenInfo, errorMessage);

      if (disp.type == X86OperandType::DD || disp.type == X86OperandType::DB) {
         if (!setOffsetValue(operand, disp))
            throw SyntaxError(errorMessage, tokenInfo.lineInfo);

         read(tokenInfo);
      }
      else if (disp.type != X86OperandType::Unknown)
         throw SyntaxError(errorMessage, tokenInfo.lineInfo);

      return operand;
   }
   else return X86Assembler::readDispOperand(tokenInfo, operand, prefix, errorMessage);
}

X86Operand X86_64Assembler :: compileCodeOperand(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   X86Operand operand;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      operand.type = X86OperandType::DQ;
      operand.reference = readReference(tokenInfo) | mskCodeRef64;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);

   return operand;
}

X86Operand X86_64Assembler :: compileDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage, bool rdataMode)
{
   X86Operand operand;

   ref_t mask = rdataMode ? mskRDataRef64 : mskDataRef64;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      operand.type = X86OperandType::DQ;
      operand.reference = readReference(tokenInfo) | mask;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);

   return operand;
}

X86Operand X86_64Assembler :: compileMDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage)
{
   X86Operand operand;

   ref_t mask = mskMDataRef64;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
   if (tokenInfo.compare("%")) {
      operand.type = X86OperandType::DQ;
      operand.reference = readReference(tokenInfo) | mask;
   }
   else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);

   return operand;
}

void X86_64Assembler :: compileExternCall(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeWord(0x15FF);
   read(tokenInfo);
   if (tokenInfo.compare(RELPTR32_ARGUMENT1)) {
      X86Operand operand(X86OperandType::DD);
      operand.reference = RELPTR32_1;
      operand.offset = 0;

      X86Helper::writeImm(writer, operand);
   }
   else throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
}

bool X86_64Assembler::compileAdd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64_M64() && target.type == X86OperandType::DD) {
      writer.writeByte(0x48);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 0), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR64_M64() && target.type == X86OperandType::DB) {
      writer.writeByte(0x48);
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R64 + 0), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR64_M64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x01);
      X86Helper::writeModRM(writer, target, source);
   }
   else return X86Assembler::compileAdd(source, target, writer);

   return true;
}

bool X86_64Assembler :: compileAnd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64() && target.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x23);
      X86Helper::writeModRM(writer, source, target);
   }
   else return X86Assembler::compileAnd(source, target, writer);

   return true;
}

bool X86_64Assembler :: compileCall(X86Operand source, MemoryWriter& writer)
{
   if (source.type == X86OperandType::DD) {
      writer.writeByte(0xE8);
      if (source.reference == RELPTR32_1 || source.reference == RELPTR32_2) {
         writer.writeDReference(source.reference, source.offset);
      }
      else writer.writeDReference(source.reference | mskCodeRelRef32, source.offset);
   }
   else if (source.isR64()) {
      writer.writeByte(0xFF);
      writer.writeByte(0xD0 + (char)source.type);
   }
   else if (source.isM64()) {
      writer.writeByte(0xFF);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 2), source);
      X86Helper::writeImm(writer, source);
   }
   else return false;

   return true;
}

bool X86_64Assembler :: compileCMovcc(X86Operand source, X86Operand target, MemoryWriter& writer, X86JumpType type)
{
   if (source.isR64() && target.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x0F);
      writer.writeByte(0x40 + (int)type);
      X86Helper::writeModRM(writer, source, target);
   }
   else return X86Assembler::compileCMovcc(source, target, writer, type);

   return true;
}

bool X86_64Assembler :: compileCmp(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64_M64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x39);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR64() && target.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x3B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64() && target.isRX64_MX64()) {
      writer.writeByte(0x4B);
      writer.writeByte(0x3B);
      X86Helper::writeModRM(writer, source, target);
   }
   else return X86Assembler::compileCmp(source, target, writer);

   return true;
}

bool X86_64Assembler :: compileIDiv(X86Operand source, MemoryWriter& writer)
{
   if (source.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 7 }, source);
   }
   else return X86Assembler::compileIDiv(source, writer);

   return true;
}

bool X86_64Assembler :: compileIMul(X86Operand source, MemoryWriter& writer)
{
   if (source.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 5 }, source);
   }
   else return X86Assembler::compileIMul(source, writer);

   return true;
}

bool X86_64Assembler :: compileJmp(X86Operand source, MemoryWriter& writer)
{
   if (source.isRX64_MX64()) {
      writer.writeByte(0x4F);
      writer.writeByte(0xFF);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
   }
   else if (source.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0xFF);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
   }
   //else if (source.type == X86OperandType::DD && source.reference == INVALID_REF) {
   //   writer.writeByte(0xE9);
   //   writer.writeDReference(source.reference, 0);
   //}
   else return X86Assembler::compileJmp(source, writer);

   return true;
}

bool X86_64Assembler :: compileLea(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isRX64() && target.isMX64()) {
      writer.writeByte(0x4F);
      writer.writeByte(0x8D);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isRX64() && target.isM64()) {
      writer.writeByte(0x4C);
      writer.writeByte(0x8D);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64() && target.isMX64()) {
      writer.writeByte(0x4B);
      writer.writeByte(0x8D);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64() && target.isM64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x8D);
      X86Helper::writeModRM(writer, source, target);
   }
   else return X86Assembler::compileLea(source, target, writer);

   return true;
}

bool X86_64Assembler :: compileMov(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isRX64() && target.isDB_DD_DQ()) {
      target.type = X86OperandType::DQ;
      writer.writeByte(0x49);
      writer.writeByte(0xB8 + (char)source.type);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR64() && target.isDB_DD_DQ()) {
      target.type = X86OperandType::DQ;
      writer.writeByte(0x48);
      writer.writeByte(0xB8 + (char)source.type);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isRX64() && target.isRX64_MX64()) {
      writer.writeByte(0x4F);
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isRX64() && target.isR64_M64()) {
      writer.writeByte(0x4C);
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64() && target.isMX64()) {
      writer.writeByte(0x4B);
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64() && target.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64() && target.isRX64()) {
      writer.writeByte(0x49);
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isM64() && target.isRX64()) {
      writer.writeByte(0x4C);
      writer.writeByte(0x89);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isM64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x89);
      X86Helper::writeModRM(writer, target, source);
   }
   else return X86Assembler::compileMov(source, target, writer);

   return true;
}

bool X86_64Assembler::compilePop(X86Operand source, MemoryWriter& writer)
{
   if (source.isR64()) {
      writer.writeByte(0x58 + (char)source.type);
   }
   else if (source.isRX64()) {
      writer.writeByte(0x41);
      writer.writeByte(0x58 + (char)source.type);
   }
   else return false;

   return true;
}

bool X86_64Assembler :: compilePush(X86Operand source, MemoryWriter& writer)
{
   if (source.isR64()) {
      writer.writeByte(0x50 + (char)source.type);
   }
   else if (source.isRX64()) {
      writer.writeByte(0x41);
      writer.writeByte(0x50 + (char)source.type);
   }
   else if (source.type == X86OperandType::DB) {
      writer.writeByte(0x6A);
      writer.writeByte(source.offset);
   }
   else return false;

   return true;
}

bool X86_64Assembler :: compileSub(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64_M64() && target.type == X86OperandType::DB) {
      writer.writeByte(0x48);
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR64_M64() && target.type == X86OperandType::DD) {
      writer.writeByte(0x48);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR64_M64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x29);
      X86Helper::writeModRM(writer, target, source);
   }
   else return X86Assembler::compileSub(source, target, writer);

   return true;
}

bool X86_64Assembler :: compileXor(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x33);
      X86Helper::writeModRM(writer, source, target);
   }
   else return X86Assembler::compileXor(source, target, writer);

   return true;
}

void X86_64Assembler :: compileStosq(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0x48);
   writer.writeByte(0xAB);
   read(tokenInfo);
}

bool X86_64Assembler :: compileTest(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x85);
      X86Helper::writeModRM(writer, target, source);
   }
   else return X86Assembler::compileTest(source, target, writer);

   return true;
}
