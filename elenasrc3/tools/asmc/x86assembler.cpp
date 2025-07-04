//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA Intel X86 Assembler
//		classes.
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "x86assembler.h"
#include "asmconst.h"

using namespace elena_lang;

constexpr auto LOCK_PREFIX = 0x0001;

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
      return { X86OperandType::AL };
   }
   else if (token.compare("ah")) {
      return { X86OperandType::AH };
   }
   else if (token.compare("ax")) {
      return { X86OperandType::AX };
   }
   else if (token.compare("eax")) {
      return { X86OperandType::EAX };
   }
   else if (token.compare("ebx")) {
      return { X86OperandType::EBX };
   }
   else if (token.compare("cl")) {
      return { X86OperandType::CL };
   }
   else if (token.compare("ch")) {
      return { X86OperandType::CH };
   }
   else if (token.compare("cx")) {
      return { X86OperandType::CX };
   }
   else if (token.compare("ecx")) {
      return { X86OperandType::ECX };
   }
   else if (token.compare("dl")) {
      return { X86OperandType::DL };
   }
   else if (token.compare("dx")) {
      return { X86OperandType::DX };
   }
   else if (token.compare("edx")) {
      return { X86OperandType::EDX };
   }
   else if (token.compare("esp")) {
      return { X86OperandType::ESP };
   }
   else if (token.compare("ebp")) {
      return { X86OperandType::EBP };
   }
   else if (token.compare("esi")) {
      return { X86OperandType::ESI };
   }
   else if (token.compare("edi")) {
      return { X86OperandType::EDI };
   }
   else if (token.compare("xmm0")) {
      return { X86OperandType::XMM0 };
   }
   else if (token.compare("xmm1")) {
      return { X86OperandType::XMM1 };
   }
   else if (token.compare("xmm2")) {
      return { X86OperandType::XMM2 };
   }
   else if (token.compare("xmm3")) {
      return { X86OperandType::XMM3 };
   }
   else if (token.compare("xmm4")) {
      return { X86OperandType::XMM4 };
   }
   else if (token.compare("xmm5")) {
      return { X86OperandType::XMM5 };
   }
   else if (token.compare("xmm6")) {
      return { X86OperandType::XMM6 };
   }
   else if (token.compare("xmm7")) {
      return { X86OperandType::XMM7 };
   }
   else return { X86OperandType::Unknown };
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
         operand = compileDataOperand(tokenInfo, errorMessage, mskDataRef);
      }
      else if (tokenInfo.compare("rdata")) {
         operand = compileDataOperand(tokenInfo, errorMessage, mskRDataRef);
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
   else if (operand.type == X86OperandType::M32 && !operand.accReg) {
      operand.type = X86OperandType::Disp32;
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
      if ((operand.type == X86OperandType::DD && operand.reference == 0) || operand.type == X86OperandType::DB) {
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

int X86Assembler :: readStReg(ScriptToken& tokenInfo)
{
   read(tokenInfo);
   if (tokenInfo.compare("(")) {
      int index = readInteger(tokenInfo);

      read(tokenInfo, ")", ASM_BRACKETCLOSE_EXPECTED);
      read(tokenInfo);

      return index;
   }
   else return 0;
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

X86Operand X86Assembler :: compileDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage, ref_t mask)
{
   X86Operand operand;

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   mask |= mskRef32;

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
   else if (tokenInfo.compare("data") || tokenInfo.compare("rdata") || tokenInfo.compare("stat")) {
      ref_t mask = 0;
      if (tokenInfo.compare("data")) {
         mask = mskDataRef;
      }
      else if (tokenInfo.compare("rdata")) {
         mask = mskRDataRef;
      }
      else if (tokenInfo.compare("stat")) {
         mask = mskStatDataRef;
      }

      operand = compileDataOperand(tokenInfo, errorMessage, mask);

      read(tokenInfo);
      if (tokenInfo.compare("+")) {
         operand.offset += readInteger(tokenInfo);

         read(tokenInfo);
      }
   }
   else if (tokenInfo.compare("st")) {
      operand.type = X86OperandType::ST;
      operand.offset = readStReg(tokenInfo);
   }
   else if (tokenInfo.compare("fs")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
      operand = readPtrOperand(tokenInfo, X86OperandType::M32, errorMessage);
      if (operand.prefix != SegmentPrefix::None) {
         throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
      }
      else operand.prefix = SegmentPrefix::FS;
   }
   else if (tokenInfo.compare("gs")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
      operand = readPtrOperand(tokenInfo, X86OperandType::M64disp32, errorMessage);

      if (operand.prefix != SegmentPrefix::None) {
         throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
      }
      else operand.prefix = SegmentPrefix::GS;
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

void X86Assembler :: compileCWDE(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0x98);

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

void X86Assembler :: compileCmpxchg(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileCmpxchg(sour, dest, writer, prefixScope))
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

void X86Assembler :: compileDec(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileDec(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileDiv(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileDiv(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFadd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   bool valid = false;
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   if (sour.type == X86OperandType::ST) {
      checkComma(tokenInfo);

      X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

      valid = compileFadd(sour, dest, writer);
   }
   else valid = compileFadd(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFcomip(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileFcomip(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFdiv(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFdiv(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFfree(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileFfree(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler::compileFinit(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeByte(0x9B);
   writer.writeWord(0xE3DB);
}

void X86Assembler :: compileFild(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFild(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFld(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFld(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFldcw(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFldcw(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFmul(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFmul(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFstcw(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFstcw(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFstp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFstp(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFstsw(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFstsw(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFistp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFistp(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFisub(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, nullptr);

   bool valid = compileFisub(sour, writer);

   if (!valid)
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileFrndint(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xFCD9);
}

void X86Assembler :: compileFprem(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xF8D9);
}

void X86Assembler :: compileFscale(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xFDD9);
}

void X86Assembler :: compileFsin(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xFED9);
}

void X86Assembler :: compileFcos(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xFFD9);
}

void X86Assembler :: compileFpatan(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xF3D9);
}

void X86Assembler :: compileF2xm1(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xF0D9);
}

void X86Assembler :: compileFld1(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xE8D9);
}

void X86Assembler :: compileFxch(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xC9D9);
}

void X86Assembler :: compileFmulp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xC9DE);
}

void X86Assembler :: compileFdivp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xF9DE);
}

void X86Assembler ::compileFsubp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xE9DE);
}

void X86Assembler :: compileFldl2e(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xEAD9);
}

void X86Assembler :: compileFldln2(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xEDD9);
}

void X86Assembler :: compileFldpi(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xEBD9);
}

void X86Assembler :: compileFyl2x(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xF1D9);
}

void X86Assembler :: compileFaddp(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xC1DE);
}

void X86Assembler :: compileFsqrt(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xFAD9);
}

void X86Assembler :: compileFabs(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   read(tokenInfo);

   writer.writeWord(0xE1D9);
}

void X86Assembler :: compileFsub(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   bool valid = false;

   X86Operand sour = compileOperand(tokenInfo, nullptr);
   if (sour.type == X86OperandType::ST) {
      checkComma(tokenInfo);

      X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

      valid = compileFsub(sour, dest, writer);
   }
   else valid = compileFsub(sour, writer);

   if (!valid)
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

   if (!tokenInfo.compare(",")) {
      if (!compileIMul(sour, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
   else {
      X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

      if (!compileIMul(sour, dest, writer))
         throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
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
   else if (operand.type == X86OperandType::DD && (operand.reference == RELPTR32_1 || operand.reference == RELPTR32_2)) {
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

void X86Assembler :: compileMovq(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileMovq(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileMovd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileMovd(sour, dest, writer))
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
   X86Operand sour = compileOperand(tokenInfo, nullptr);
   if (sour.type != X86OperandType::Unknown) {
      if (sour.type == X86OperandType::M32disp8) {
         writer.writeByte(0x0F);
         writer.writeByte(0x1F);
         writer.writeWord(0x40);
      }
      else if (sour.type == X86OperandType::EaxEaxDisp8) {
         writer.writeByte(0x0F);
         writer.writeByte(0x1F);
         writer.writeWord(0x44);
         writer.writeByte(0);
      }
      else if (sour.type == X86OperandType::EaxEaxDisp32) {
         writer.writeByte(0x0F);
         writer.writeByte(0x1F);
         writer.writeWord(0x84);
         writer.writeDWord(0);
      }      
      else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }
   else writer.writeByte(0x90);
}

void X86Assembler :: compileNot(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   if (!compileNot(sour, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
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

void X86Assembler :: compileSar(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileSar(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
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

void X86Assembler::compileShl(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileShl(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileShld(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   checkComma(tokenInfo);

   X86Operand third = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileShld(sour, dest, third, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void X86Assembler :: compileShrd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   checkComma(tokenInfo);

   X86Operand third = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileShrd(sour, dest, third, writer))
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

void X86Assembler :: compileXadd(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileXadd(sour, dest, writer, prefixScope))
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

void X86Assembler :: compileXorps(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileXorps(sour, dest, writer))
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

void X86Assembler :: compileDoubleField(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

bool X86Assembler :: compileAdc(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.type == X86OperandType::EAX && target.type == X86OperandType::DD) {
      writer.writeByte(0x15);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x11);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 2), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR8_M8() && target.type == X86OperandType::DB) {
      writer.writeByte(0x80);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R8 + 2), source);
      X86Helper::writeImm(writer, target);
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
   else if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x03);
      X86Helper::writeModRM(writer, source, target);
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
   else if (source.isR32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DD) {
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x21);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x20);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x21);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16() && target.type == X86OperandType::DD) {
      writer.writeByte(0x66);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 4 }, source);
      writer.writeWord(target.offset);
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
   else if (source.isR32_M32() && (target.type == X86OperandType::DD || target.type == X86OperandType::DB)) {
      target.type = X86OperandType::DD;

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
   else if (source.isR8_M8() && target.type == X86OperandType::DB) {
      writer.writeByte(0x80);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R8 + 7), source);
      X86Helper::writeImm(writer, target);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileCmpxchg(X86Operand source, X86Operand target, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (test(prefixScope.value, LOCK_PREFIX)) {
      writer.writeByte(0xF0);

      prefixScope.value &= ~LOCK_PREFIX;
   }
   if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x0F);
      writer.writeByte(0xB1);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x0F);
      writer.writeByte(0xB0);
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

bool X86Assembler :: compileDec(X86Operand sour, MemoryWriter& writer)
{
   if (sour.isR32()) {
      writer.writeByte(0x48 + (char)sour.type);
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

bool X86Assembler :: compileFadd(X86Operand source, MemoryWriter& writer)
{
   if (source.isM64()) {
      writer.writeByte(0xDC);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 0 }, source);
   }
   else return false;

   return true;

}

bool X86Assembler::compileFadd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.type == X86OperandType::ST && source.offset == 0 && target.type == X86OperandType::ST) {
      writer.writeByte(0xD8);
      writer.writeByte(0xC0 + target.offset);
   }
   else if (source.type == X86OperandType::ST && target.type == X86OperandType::ST && target.offset == 0) {
      writer.writeByte(0xDC);
      writer.writeByte(0xC0 + target.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFcomip(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.type == X86OperandType::ST && source.offset == 0 && target.type == X86OperandType::ST) {
      writer.writeByte(0xDF);
      writer.writeByte(0xF0 + target.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFdiv(X86Operand source, MemoryWriter& writer)
{
   if (source.isM64()) {
      writer.writeByte(0xDC);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 6 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFfree(X86Operand source, MemoryWriter& writer)
{
   if (source.type == X86OperandType::ST) {
      writer.writeByte(0xDD);
      writer.writeByte(0xC0 + source.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFild(X86Operand source, MemoryWriter& writer)
{
   if (source.isM32()) {
      writer.writeByte(0xDB);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 0 }, source);
   }
   else if (source.isM64()) {
      writer.writeByte(0xDF);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 5 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFistp(X86Operand source, MemoryWriter& writer)
{
   if (source.isM32()) {
      writer.writeByte(0xDB);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 3 }, source);
   }
   else if (source.isM64()) {
      writer.writeByte(0xDF);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 7 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFisub(X86Operand source, MemoryWriter& writer)
{
   if (source.isM32()) {
      writer.writeByte(0xDA);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 4 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFld(X86Operand source, MemoryWriter& writer)
{
   if (source.type == X86OperandType::ST) {
      writer.writeByte(0xD9);
      writer.writeByte(0xC0 + source.offset);
   }
   else if (source.isM64()) {
      writer.writeByte(0xDD);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 0 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFldcw(X86Operand source, MemoryWriter& writer)
{
   if (source.isM16()) {
      writer.writeByte(0xD9);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 5 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFmul(X86Operand source, MemoryWriter& writer)
{
   if (source.isM64()) {
      writer.writeByte(0xDC);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 1 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFstcw(X86Operand source, MemoryWriter& writer)
{
   if (source.isM16()) {
      writer.writeByte(0x9B);
      writer.writeByte(0xD9);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 7 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFstp(X86Operand source, MemoryWriter& writer)
{
   if (source.type == X86OperandType::ST) {
      writer.writeByte(0xDD);
      writer.writeByte(0xD8 + source.offset);
   }
   else if (source.isM64()) {
      writer.writeByte(0xDD);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 3 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFstsw(X86Operand source, MemoryWriter& writer)
{
   if (source.type == X86OperandType::AX) {
      writer.writeByte(0x9B);
      writer.writeWord(0xE0DF);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFsub(X86Operand source, MemoryWriter& writer)
{
   if (source.isM64()) {
      writer.writeByte(0xDC);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 4 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileFsub(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.type == X86OperandType::ST && source.offset == 0 && target.type == X86OperandType::ST) {
      writer.writeByte(0xD8);
      writer.writeByte(0xE0 + target.offset);
   }
   else if (source.type == X86OperandType::ST && target.type == X86OperandType::ST && target.offset == 0) {
      writer.writeByte(0xDC);
      writer.writeByte(0xE8 + source.offset);
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

bool X86Assembler :: compileIMul(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x6B);
      X86Helper::writeModRM(writer, source, source);
      writer.writeByte(target.offset);
   }
   if (source.isR32() && target.type == X86OperandType::DD) {
      writer.writeByte(0x69);
      X86Helper::writeModRM(writer, source, source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32() && target.isR32()) {
      writer.writeByte(0x0F);
      writer.writeByte(0xAF);
      X86Helper::writeModRM(writer, source, target);
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
   else if (source.type == X86OperandType::DD && (source.reference == RELPTR32_1 || source.reference == RELPTR32_2)) {
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
   if (target.prefix == SegmentPrefix::FS) {
      writer.writeByte(0x64);
   }
   else if (target.prefix == SegmentPrefix::GS) {
      writer.writeByte(0x65);
   }

   if (source.isR32() && target.isM32()) {
      writer.writeByte(0x8D);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileMov(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (target.prefix == SegmentPrefix::FS) {
      writer.writeByte(0x64);
   }
   else if (target.prefix == SegmentPrefix::GS) {
      writer.writeByte(0x65);
   }

   if(source.type == X86OperandType::EAX && target.type == X86OperandType::Disp32) {
      writer.writeByte(0xA1);
      if (target.reference != 0) {
         writer.writeDReference(target.reference, target.offset);
      }
      else writer.writeDWord(target.offset);
   }
   else if (source.type == X86OperandType::Disp32 && target.type == X86OperandType::EAX) {
      writer.writeByte(0xA3);
      if (source.reference != 0) {
         writer.writeDReference(source.reference, source.offset);
      }
      else writer.writeDWord(source.offset);
   }
   else if (source.isR32() && target.isDB_DD()) {
      target.type = X86OperandType::DD;
      writer.writeByte(0xB8 + (char)source.type);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR32_M32() && target.isDB_DD()) {
      target.type = X86OperandType::DD;
      writer.writeByte(0xC7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 0 }, source);
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
   else if (source.isR16() && target.isR16_M16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x8B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x88);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8() && target.isR8_M8()) {
      writer.writeByte(0x8A);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isM8() && target.isDB()) {
      writer.writeByte(0xC6);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 0 }, source);
      X86Helper::writeImm(writer, target);
   }
   else return false;

   return true;
}


bool X86Assembler :: compileMovq(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64_M64() && target.isXMM64()) {
      writer.writeByte(0x66);
      writer.writeByte(0x0F);
      writer.writeByte(0xD6);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isXMM64() && target.isR64_M64()) {
      writer.writeByte(0xF3);
      writer.writeByte(0x0F);
      writer.writeByte(0x7E);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileMovd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isXMM64() && target.isR32_M32()) {
      writer.writeByte(0x66);
      writer.writeByte(0x0F);
      writer.writeByte(0x6E);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

void X86Assembler :: compileMovsb(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0xA4);
   read(tokenInfo);
}

void X86Assembler::compileMovsd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0xA5);
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

bool X86Assembler :: compileNeg(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 3 }, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileNot(X86Operand source, MemoryWriter& writer)
{
   if (source.isR32_M32()) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 2 }, source);
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
   else if (source.isR32() && target.isR32_M32()) {
      writer.writeByte(0x0B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DB) {
      writer.writeByte(0x83);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 1 }, source);
      writer.writeByte(target.offset);
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DD) {
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 1 }, source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x08);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x09);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16() && target.type == X86OperandType::DD) {
      writer.writeByte(0x66);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 1 }, source);
      writer.writeWord(target.offset);
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

bool X86Assembler :: compileSar(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.type == X86OperandType::DB) {
      writer.writeByte(0xC1);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 7), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR32() && target.type == X86OperandType::CL) {
      writer.writeByte(0xD3);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 7), source);
   }
   else return false;

   return true;
}

void X86Assembler :: compileSahf(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0x9E);
   read(tokenInfo);
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
   else if (source.isR32() && target.type == X86OperandType::CL) {
      writer.writeByte(0xD3);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
   }
   else if (source.isR8() && (target.type == X86OperandType::DB && target.offset == 1)) {
      writer.writeByte(0xD0);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R8 + 5), source);
   }
   else if (source.isR8() && target.type == X86OperandType::DB) {
      writer.writeByte(0xD3);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileShl(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR32() && target.type == X86OperandType::DB) {
      writer.writeByte(0xC1);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR32() && target.type == X86OperandType::CL) {
      writer.writeByte(0xD3);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileShld(X86Operand source, X86Operand target, X86Operand third, MemoryWriter& writer)
{
   if (source.isR32() && target.isR32() && third.type == X86OperandType::DB) {
      writer.writeByte(0x0F);
      writer.writeByte(0xA4);
      X86Helper::writeImm(writer, third);
   }
   else if (source.isR32() && target.isR32() && third.type == X86OperandType::CL) {
      writer.writeByte(0x0F);
      writer.writeByte(0xA5);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileShrd(X86Operand source, X86Operand target, X86Operand third, MemoryWriter& writer)
{
   if (source.isR32() && target.isR32() && third.type == X86OperandType::DB) {
      writer.writeByte(0x0F);
      writer.writeByte(0xAC);
      X86Helper::writeImm(writer, third);
   }
   else if (source.isR32() && target.isR32() && third.type == X86OperandType::CL) {
      writer.writeByte(0x0F);
      writer.writeByte(0xAD);
      X86Helper::writeModRM(writer, target, source);
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
   else if (source.isR8_M8() && target.type == X86OperandType::DB) {
      writer.writeByte(0x80);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R8 + 5), source);
      X86Helper::writeImm(writer, target);
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
   else if (source.isR32_M32() && target.type == X86OperandType::DD) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 0), source);
      X86Helper::writeImm(writer, target);   
   }
   else if (source.isR32_M32() && target.type == X86OperandType::DB) {
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 0), source);
      writer.writeDWord(target.offset);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileXadd(X86Operand source, X86Operand target, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (test(prefixScope.value, LOCK_PREFIX)) {
      writer.writeByte(0xF0);

      prefixScope.value &= ~LOCK_PREFIX;
   }

   if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x0F);
      writer.writeByte(0xC0);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x0F);
      writer.writeByte(0xC0);
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
   else if (source.isR32_M32() && target.isR32()) {
      writer.writeByte(0x31);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR8_M8() && target.isR8()) {
      writer.writeByte(0x30);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR16_M16() && target.isR16()) {
      writer.writeByte(0x66);
      writer.writeByte(0x31);
      X86Helper::writeModRM(writer, target, source);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileXorps(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isXMM64() && target.isXMM64()) {
      writer.writeByte(0x0F);
      writer.writeByte(0x57);
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
   else {
      if (tokenInfo.state == dfaQuote) {
         X86Operand operand(X86OperandType::DD);
         operand.reference = _target->mapReference(*tokenInfo.token) | mskImportRef32;
         operand.offset = 0;

         X86Helper::writeImm(writer, operand);
      }
      else throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
   }
}

bool X86Assembler::compileAOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope&)
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

bool X86Assembler :: compileCOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
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
   else if (tokenInfo.compare("cmpxchg")) {
      compileCmpxchg(tokenInfo, writer, prefixScope);
   }
   else if (tokenInfo.compare("cmovb")) {
      compileCMovcc(tokenInfo, writer, X86JumpType::JB);
   }
   else if (tokenInfo.compare("cmovg")) {
      compileCMovcc(tokenInfo, writer, X86JumpType::JG);
   }
   else if (tokenInfo.compare("cmovl")) {
      compileCMovcc(tokenInfo, writer, X86JumpType::JL);
   }
   else if (tokenInfo.compare("cmovz")) {
      compileCMovcc(tokenInfo, writer, X86JumpType::JZ);
   }
   else if (tokenInfo.compare("cwde")) {
      compileCWDE(tokenInfo, writer);
   }
   else if (tokenInfo.compare("cmovnz")) {
      compileCMovcc(tokenInfo, writer, X86JumpType::JNZ);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileDOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("dec")) {
      compileDec(tokenInfo, writer);
   }
   else if (tokenInfo.compare("div")) {
      compileDiv(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileEOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool X86Assembler :: compileFOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("f2xm1")) {
      compileF2xm1(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fabs")) {
      compileFabs(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fadd")) {
      compileFadd(tokenInfo, writer);
   }
   else if (tokenInfo.compare("faddp")) {
      compileFaddp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fcomip")) {
      compileFcomip(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fdiv")) {
      compileFdiv(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fdivp")) {
      compileFdivp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("ffree")) {
      compileFfree(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fild")) {
      compileFild(tokenInfo, writer);
   }
   else if (tokenInfo.compare("finit")) {
      compileFinit(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fistp")) {
      compileFistp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fisub")) {
      compileFisub(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fld")) {
      compileFld(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fld1")) {
      compileFld1(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fldl2e")) {
      compileFldl2e(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fldcw")) {
      compileFldcw(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fldln2")) {
      compileFldln2(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fldpi")) {
      compileFldpi(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fmul")) {
      compileFmul(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fmulp")) {
      compileFmulp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fprem")) {
      compileFprem(tokenInfo, writer);
   }
   else if (tokenInfo.compare("frndint")) {
      compileFrndint(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fscale")) {
      compileFscale(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fsin")) {
      compileFsin(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fsubp")) {
      compileFsubp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fcos")) {
      compileFcos(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fpatan")) {
      compileFpatan(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fstcw")) {
      compileFstcw(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fsqrt")) {
      compileFsqrt(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fstp")) {
      compileFstp(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fstsw")) {
      compileFstsw(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fsub")) {
      compileFsub(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fxch")) {
      compileFxch(tokenInfo, writer);
   }
   else if (tokenInfo.compare("fyl2x")) {
      compileFyl2x(tokenInfo, writer);
   }
   else return false;

   return true;
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
   else if (tokenInfo.compare("jc")) {
      compileJcc(tokenInfo, writer, X86JumpType::JB, labelScope);
   }
   else if (tokenInfo.compare("jpe")) {
      compileJcc(tokenInfo, writer, X86JumpType::JP, labelScope);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileLOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (tokenInfo.compare("lea")) {
      compileLea(tokenInfo, writer);
   }
   else if (tokenInfo.compare("lock")) {
      prefixScope.value |= LOCK_PREFIX;

      read(tokenInfo);
   }
   else return false;

   return true;
}

bool X86Assembler :: compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("mov")) {
      compileMov(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movd")) {
      compileMovd(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movsb")) {
      compileMovsb(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movsd")) {
      compileMovsd(tokenInfo, writer);
   }
   else if (tokenInfo.compare("movq")) {
      compileMovq(tokenInfo, writer);
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
   else if (tokenInfo.compare("not")) {
      compileNot(tokenInfo, writer);
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
   else if (tokenInfo.compare("rgw")) {
      writer.writeByte(0x66);

      read(tokenInfo);
   }
   else if (tokenInfo.compare("ret")) {
      compileRet(tokenInfo, writer);
   }
   else return false;

   return true;
}

bool X86Assembler::compileSOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("sar")) {
      compileSar(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sbb")) {
      compileSbb(tokenInfo, writer);
   }
   else if (tokenInfo.compare("seta")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JA);
   }
   else if (tokenInfo.compare("setae")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JAE);
   }
   else if (tokenInfo.compare("setnc")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JAE);
   }
   else if (tokenInfo.compare("sete")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JE);
   }
   else if (tokenInfo.compare("setg")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JG);
   }
   else if (tokenInfo.compare("setl")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JL);
   }
   else if (tokenInfo.compare("setns")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JNS);
   }
   else if (tokenInfo.compare("sets")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JS);
   }
   else if (tokenInfo.compare("setz")) {
      compileSetcc(tokenInfo, writer, X86JumpType::JZ);
   }
   else if (tokenInfo.compare("shr")) {
      compileShr(tokenInfo, writer);
   }
   else if (tokenInfo.compare("sahf")) {
      compileSahf(tokenInfo, writer);
   }
   else if (tokenInfo.compare("shl")) {
      compileShl(tokenInfo, writer);
   }
   else if (tokenInfo.compare("shld")) {
      compileShld(tokenInfo, writer);
   }
   else if (tokenInfo.compare("shrd")) {
      compileShrd(tokenInfo, writer);
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

bool X86Assembler :: compileUOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   return false;
}

bool X86Assembler :: compileXOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (tokenInfo.compare("xadd")) {
      compileXadd(tokenInfo, writer, prefixScope);
   }
   else if (tokenInfo.compare("xor")) {
      compileXor(tokenInfo, writer);
   }
   else if (tokenInfo.compare("xorps")) {
      compileXorps(tokenInfo, writer);
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
   else if (operand.type == X86OperandType::M64 && !operand.accReg) {
      operand.type = X86OperandType::Disp64;
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

X86Operand X86_64Assembler :: compileDataOperand(ScriptToken& tokenInfo, ustr_t errorMessage, ref_t mask)
{
   X86Operand operand;

   mask |= mskRef64;

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
   else {
      if (tokenInfo.state == dfaQuote) {
         X86Operand operand(X86OperandType::DD);
         operand.reference = _target->mapReference(*tokenInfo.token) | mskImportRelRef32;
         operand.offset = 0;

         X86Helper::writeImm(writer, operand);
      }
      else throw SyntaxError(ASM_INVALID_CALL_LABEL, tokenInfo.lineInfo);
   }
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
   else if (source.isRX64() && target.isR64()) {
      writer.writeByte(0x4C);
      writer.writeByte(0x01);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR64() && target.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x03);
      X86Helper::writeModRM(writer, source, target);
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
   else if (source.isR64_M64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x21);
      X86Helper::writeModRM(writer, target, source);
   }
   else if (source.isR64_M64() && target.type == X86OperandType::DD) {
      writer.writeByte(0x48);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
      X86Helper::writeImm(writer, target);
   }
   else if (source.isR64_M64() && target.type == X86OperandType::DB) {
      writer.writeByte(0x48);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);

      target.type = X86OperandType::DD;
      X86Helper::writeImm(writer, target);
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
   else if (source.isR64() && target.isRX64()) {
      writer.writeByte(0x4B);
      writer.writeByte(0x0F);
      writer.writeByte(0x40 + (int)type);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isRX64() && target.isR64()) {
      writer.writeByte(0x4C);
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
   else if (source.isRX64() && target.isR64_M64()) {
      writer.writeByte(0x4C);
      writer.writeByte(0x3B);
      X86Helper::writeModRM(writer, source, target);
   }
   else if (source.isR64_M64() && (target.type == X86OperandType::DD || target.type == X86OperandType::DB)) {
      target.type = X86OperandType::DD;

      writer.writeByte(0x48);
      writer.writeByte(0x81);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 7), source);
      X86Helper::writeImm(writer, target);
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
   if (target.prefix == SegmentPrefix::GS) {
      writer.writeByte(0x65);
   }

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

bool X86_64Assembler :: compileNeg(X86Operand source, MemoryWriter& writer)
{
   if (source.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 3 }, source);
   }
   else return X86Assembler::compileNeg(source, writer);

   return true;
}

bool X86_64Assembler :: compileNot(X86Operand source, MemoryWriter& writer)
{
   if (source.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0xF7);
      X86Helper::writeModRM(writer, { X86OperandType::R32 + 2 }, source);
   }
   else return X86Assembler::compileNeg(source, writer);

   return true;
}

bool X86_64Assembler :: compileOr(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64_M64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x09);
      X86Helper::writeModRM(writer, target, source);
   }
   else return X86Assembler::compileOr(source, target, writer);

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
   else if (source.isM64()) {
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

bool X86_64Assembler :: compileShr(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64() && target.type == X86OperandType::DB) {
      writer.writeByte(0x48);
      writer.writeByte(0xC1);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 5), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR64() && target.type == X86OperandType::CL) {
      writer.writeByte(0x48);
      writer.writeByte(0xD3);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R64 + 5), source);
   }
   else return X86Assembler::compileShr(source, target, writer);

   return true;
}

bool X86_64Assembler :: compileShl(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64() && target.type == X86OperandType::DB) {
      writer.writeByte(0x48);
      writer.writeByte(0xC1);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
      writer.writeByte(target.offset);
   }
   else if (source.isRX64() && target.type == X86OperandType::DB) {
      writer.writeByte(0x4C);
      writer.writeByte(0xC1);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R32 + 4), source);
      writer.writeByte(target.offset);
   }
   else if (source.isR64() && target.type == X86OperandType::CL) {
      writer.writeByte(0x48);
      writer.writeByte(0xD3);
      X86Helper::writeModRM(writer, X86Operand(X86OperandType::R64 + 4), source);
   }
   else return X86Assembler::compileShl(source, target, writer);

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
   else if (source.isR64() && target.isR64_M64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x2B);
      X86Helper::writeModRM(writer, source, target);
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
   else if (source.isR64_M64() && target.isR64()) {
      writer.writeByte(0x48);
      writer.writeByte(0x31);
      X86Helper::writeModRM(writer, target, source);
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

void X86_64Assembler :: compileMovsq(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   writer.writeByte(0x48);
   writer.writeByte(0xA5);
   read(tokenInfo);
}

bool X86_64Assembler :: compileMovsxd(X86Operand source, X86Operand target, MemoryWriter& writer)
{
   if (source.isR64() && target.isR32_M32()) {
      writer.writeByte(0x48);
      writer.writeByte(0x63);
      X86Helper::writeModRM(writer, source, target);
   }
   else return false;

   return true;
}

void X86_64Assembler :: compileMovsxd(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   X86Operand sour = compileOperand(tokenInfo, ASM_INVALID_SOURCE);

   checkComma(tokenInfo);

   X86Operand dest = compileOperand(tokenInfo, ASM_INVALID_DESTINATION);

   if (!compileMovsxd(sour, dest, writer))
      throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

bool X86_64Assembler :: compileMOpCode(ScriptToken& tokenInfo, MemoryWriter& writer)
{
   if (tokenInfo.compare("movsq")) {
      compileMovsq(tokenInfo, writer);

      return true;
   }
   else if (tokenInfo.compare("movsxd")) {
      compileMovsxd(tokenInfo, writer);

      return true;
   }
   else return X86Assembler::compileMOpCode(tokenInfo, writer);
}

bool X86_64Assembler :: compileXadd(X86Operand source, X86Operand target, MemoryWriter& writer, PrefixInfo& prefixScope)
{
   if (test(prefixScope.value, LOCK_PREFIX)) {
      writer.writeByte(0xF0);

      prefixScope.value &= ~LOCK_PREFIX;
   }

   if (source.isM64() && target.isR32()) {
      writer.writeByte(0x0F);
      writer.writeByte(0xC0);
      X86Helper::writeModRM(writer, target, source);
   }
   else return X86Assembler::compileXadd(source, target, writer, prefixScope);

   return true;
}
