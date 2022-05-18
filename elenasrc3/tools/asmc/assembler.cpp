//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA Assembler
//		classes.
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "assembler.h"
#include "asmconst.h"

using namespace elena_lang;

// --- AssemblerBase ---

AssemblerBase :: AssemblerBase(int tabSize, UStrReader* reader, ModuleBase* target)
   : _reader(tabSize, reader), constants(0)
{
   _target = target;
}

void AssemblerBase :: checkComma(ScriptToken& tokenInfo)
{
   if (!tokenInfo.compare(","))
      throw SyntaxError(ASM_COMMA_EXPECTED, tokenInfo.lineInfo);
}

void AssemblerBase::read(ScriptToken& tokenInfo)
{
   if (!_reader.read(tokenInfo))
      throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
}

ref_t AssemblerBase :: readReference(ScriptToken& tokenInfo)
{
   read(tokenInfo);

   ref_t reference = 0;
   if (tokenInfo.state == dfaInteger) {
      reference = tokenInfo.token.toRef();
   }
   else if (tokenInfo.state == dfaHexInteger) {
      reference = tokenInfo.token.toRef(16);
   }
   else {
      if (constants.exist(*tokenInfo.token)) {
         reference = constants.get(*tokenInfo.token);
      }
      else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
   }   

   return reference;
}

int AssemblerBase :: readInteger(ScriptToken& tokenInfo)
{
   read(tokenInfo);
   int integer = 0;
   if (tokenInfo.state == dfaInteger) {
      integer = tokenInfo.token.toInt();
   }
   else if (tokenInfo.state == dfaHexInteger) {
      integer = tokenInfo.token.toInt(16);
   }
   else if (tokenInfo.compare("-")) {
      return -readInteger(tokenInfo);
   }
   else
   {
      if (constants.exist(*tokenInfo.token)) {
         return constants.get(*tokenInfo.token);
      }
      else throw SyntaxError(ASM_INVALIDNUMBER, tokenInfo.lineInfo);
   }

   return integer;
}

bool AssemblerBase :: getArgReference(ScriptToken& tokenInfo, int& offset, ref_t& reference)
{
   if (tokenInfo.compare(WORD_ARGUMENT1)) {
      reference = ARG16_1;
      offset = 0;
   }
   else if (tokenInfo.compare(WORD_ARGUMENT2)) {
      reference = ARG16_2;
      offset = 0;
   }
   else if (tokenInfo.compare(IMM9_ARGUMENT1)) {
      reference = ARG9_1;
      offset = 0;
   }
   else if (tokenInfo.compare(IMM12_ARGUMENT1)) {
      reference = ARG12_1;
      offset = 0;
   }
   else if (tokenInfo.compare(IMM12_ARGUMENT2)) {
      reference = ARG12_2;
      offset = 0;
   }
   else if (tokenInfo.compare(DWORD_ARGUMENT1)) {
      reference = ARG32_1;
      offset = 0;
   }
   else if (tokenInfo.compare(DWORDHI_ARGUMENT1)) {
      reference = ARG32HI_1;
      offset = 0;
   }
   else if (tokenInfo.compare(DWORDLO_ARGUMENT1)) {
      reference = ARG32LO_1;
      offset = 0;
   }
   else if (tokenInfo.compare(DWORD_ARGUMENT2)) {
      reference = ARG32_2;
      offset = 0;
   }
   else if (tokenInfo.compare(QWORD_ARGUMENT2)) {
      reference = ARG64_2;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR32_ARGUMENT1)) {
      reference = PTR32_1;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR32_ARGUMENT2)) {
      reference = PTR32_2;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR64_ARGUMENT1)) {
      reference = PTR64_1;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR64_ARGUMENT2)) {
      reference = PTR64_2;
      offset = 0;
   }
   else if (tokenInfo.compare(N_ARGUMENT1)) {
      reference = NARG_1;
      offset = 0;
   }
   else if (tokenInfo.compare(N_ARGUMENT2)) {
      reference = NARG_2;
      offset = 0;
   }
   else if (tokenInfo.compare(N16_ARGUMENT2)) {
      reference = NARG16_2;
      offset = 0;
   }
   else if (tokenInfo.compare(N16_ARGUMENT1)) {
      reference = NARG16_1;
      offset = 0;
   }
   else if (tokenInfo.compare(N16_ARGUMENT2)) {
      reference = NARG16_2;
      offset = 0;
   }
   else if (tokenInfo.compare(N16HI_ARGUMENT1)) {
      reference = NARGHI_1;
      offset = 0;
   }
   else if (tokenInfo.compare(N12_ARGUMENT2)) {
      reference = NARG12_2;
      offset = 0;
   }
   else if (tokenInfo.compare(DISP32HI_ARGUMENT1)) {
      reference = DISP32HI_1;
      offset = 0;
   }
   else if (tokenInfo.compare(DISP32HI_ARGUMENT2)) {
      reference = DISP32HI_2;
      offset = 0;
   }
   else if (tokenInfo.compare(DISP32LO_ARGUMENT1)) {
      reference = DISP32LO_1;
      offset = 0;
   }
   else if (tokenInfo.compare(DISP32LO_ARGUMENT2)) {
      reference = DISP32LO_2;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR32HI_ARGUMENT1)) {
      reference = PTR32HI_1;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR32HI_ARGUMENT2)) {
      reference = PTR32HI_2;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR32LO_ARGUMENT2)) {
      reference = PTR32LO_2;
      offset = 0;
   }
   else if (tokenInfo.compare(PTR32LO_ARGUMENT1)) {
      reference = PTR32LO_1;
      offset = 0;
   }
   else if (tokenInfo.compare(RDATA32_ARGUMENT1)) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
      read(tokenInfo);
      if (tokenInfo.compare("%")) {
         reference = readReference(tokenInfo) | mskRDataRef32;
         offset = 0;
      }
      else return false;
   }
   else if (tokenInfo.compare(RDATA64_ARGUMENT1)) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);
      read(tokenInfo);
      if (tokenInfo.compare("%")) {
         reference = readReference(tokenInfo) | mskRDataRef64;
         offset = 0;
      }
      else return false;
   }
   else return false;

   return true;
}

bool AssemblerBase :: getIntConstant(ScriptToken& tokenInfo, int& offset, ref_t& reference)
{
   if (tokenInfo.compare("-")) {
      read(tokenInfo);

      if (getIntConstant(tokenInfo, offset, reference)) {
         if (!reference) {
            offset = -offset;
         }
         else if (reference == ARG16_1) {
            reference = INV_ARG16_1;
         }
         else if (reference == NARG16_2) {
            reference = INV_NARG16_2;
         }
         else if (reference == ARG12_1) {
            reference = INV_ARG12_1;
         }
         else return false;
      }
      else return false;

      return true;
   }

   if (tokenInfo.state == dfaInteger) {
      reference = 0;
      offset = tokenInfo.token.toInt();
   }
   else if (tokenInfo.state == dfaHexInteger) {
      reference = 0;
      offset = tokenInfo.token.toInt(16);
   }
   else if (!getArgReference(tokenInfo, offset, reference)) {
      if (constants.exist(*tokenInfo.token)) {
         offset = constants.get(*tokenInfo.token);
         reference = 0;
      }
      else return false;
   }

   return true;
}

void AssemblerBase :: declareProcedure(ScriptToken& tokenInfo, ProcedureInfo& procInfo)
{
   if (tokenInfo.compare("%")) {
      procInfo.reference = readReference(tokenInfo);
   }
   else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);

   if (_target->mapSection(procInfo.reference, true) != nullptr) {
      throw SyntaxError(ASM_PROCEDURE_EXIST, tokenInfo.lineInfo);
   }

   read(tokenInfo);
}

bool AssemblerBase::compileOpCode(ScriptToken& tokenInfo, MemoryWriter& writer, LabelScope& labelScope)
{
   switch (tokenInfo.token[0]) {
      case 'a':
         return compileAOpCode(tokenInfo, writer);
      case 'b':
         return compileBOpCode(tokenInfo, writer, labelScope);
      case 'c':
         return compileCOpCode(tokenInfo, writer);
      case 'd':
         return compileDOpCode(tokenInfo, writer);
      case 'e':
         return compileEOpCode(tokenInfo, writer);
      case 'i':
         return compileIOpCode(tokenInfo, writer);
      case 'j':
         return compileJOpCode(tokenInfo, writer, labelScope);
      case 'l':
         return compileLOpCode(tokenInfo, writer);
      case 'm':
         return compileMOpCode(tokenInfo, writer);
      case 'n':
         return compileNOpCode(tokenInfo, writer);
      case 'o':
         return compileOOpCode(tokenInfo, writer);
      case 'p':
         return compilePOpCode(tokenInfo, writer);
      case 'r':
         return compileROpCode(tokenInfo, writer);
      case 's':
         return compileSOpCode(tokenInfo, writer);
      case 't':
         return compileTOpCode(tokenInfo, writer);
      case 'x':
         return compileXOpCode(tokenInfo, writer);
      default:
         return false;
   }
}

void AssemblerBase :: compileProcedure(ScriptToken& tokenInfo, LabelHelper* helper)
{
   LabelScope    labelScope(helper);
   ProcedureInfo procInfo;
   declareProcedure(tokenInfo, procInfo);

   MemoryBase*  code = _target->mapSection(procInfo.reference, false);
   MemoryWriter writer(code);

   while (!tokenInfo.compare("end")) {
      if (!compileOpCode(tokenInfo, writer, labelScope)) {
         if (tokenInfo.state != dfaEOF) {
            declareLabel(tokenInfo, writer, labelScope);
         }
         else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
      }
   }
}

void AssemblerBase :: compileStructure(ScriptToken& tokenInfo)
{
   ProcedureInfo procInfo;
   declareProcedure(tokenInfo, procInfo);

   MemoryBase* code = _target->mapSection(procInfo.reference, false);
   MemoryWriter writer(code);

   while (!tokenInfo.compare("end")) {
      if (tokenInfo.compare("db")) {
         compileDBField(tokenInfo, writer);
      }
      else if (tokenInfo.compare("dw")) {
         compileDWField(tokenInfo, writer);
      }
      else if (tokenInfo.compare("dd")) {
         compileDDField(tokenInfo, writer);
      }
      else if (tokenInfo.compare("dq")) {
         compileDQField(tokenInfo, writer);
      } 
      else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
   }
}

void AssemblerBase :: declareConstant(ScriptToken& tokenInfo)
{
   IdentifierString constName(*tokenInfo.token);

   int value = readInteger(tokenInfo);

   if (!constants.add(*constName, value, true))
      throw SyntaxError(ASM_DUPLICATECONST, tokenInfo.lineInfo);
}

void AssemblerBase :: compile()
{
   ScriptToken  tokenInfo;
   while (_reader.read(tokenInfo)) {
      if (tokenInfo.compare("inline")) {
         read(tokenInfo);

         compileProcedure(tokenInfo);
      }
      else if (tokenInfo.compare("procedure")) {
         read(tokenInfo);

         compileProcedure(tokenInfo);
      }
      else if (tokenInfo.compare("structure")) {
         read(tokenInfo);

         compileStructure(tokenInfo);
      }
      else if (tokenInfo.compare("define")) {
         read(tokenInfo);

         declareConstant(tokenInfo);
      }
      else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
   }
}


