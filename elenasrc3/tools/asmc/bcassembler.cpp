//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA Byte-code assembler
//		classes.
//                                            (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "bcassembler.h"
#include "asmconst.h"

using namespace elena_lang;

// --- ByteCodeAssembler::ByteCodeLabelHelper ---

bool ByteCodeAssembler::ByteCodeLabelHelper :: fixLabel(pos_t label, MemoryWriter& writer)
{
   return false; // !! temporal
}

void ByteCodeAssembler::ByteCodeLabelHelper :: registerJump(ustr_t labelName, MemoryWriter& writer)
{
   ref_t label = labelNames.get(labelName);
   if (!label) {
      label = labelNames.count() + 1;

      labelNames.add(labelName, label);
   }

   declareJump(label, writer);
}

int ByteCodeAssembler::ByteCodeLabelHelper :: resolveLabel(ustr_t label, MemoryWriter& writer)
{
   int offset = labels.get(getLabel(label)) - writer.position();

   return offset;
}

void ByteCodeAssembler::ByteCodeLabelHelper :: checkAllUsedLabels(ustr_t errorMessage, ustr_t procedureName)
{
   auto it = labelNames.start();
   while (!it.eof()) {
      ustr_t label = it.key();

      // Check if label is declared
      if (!declaredLabels.get(*it)) {}
         throw ProcedureError(errorMessage, procedureName, label);

      it++;
   }
}

// --- ByteCodeAssembler ---

ByteCodeAssembler :: ByteCodeAssembler(int tabSize, UStrReader* reader, Module* module, bool mode64)
   : _reader(tabSize, reader)
{
   _module = module;
   _mode64 = mode64;
}

void ByteCodeAssembler :: read(ScriptToken& tokenInfo)
{
   if (!_reader.read(tokenInfo))
      throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
}

ref_t ByteCodeAssembler :: readReference(ScriptToken& tokenInfo, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   ref_t mask = 0;
   if (tokenInfo.compare("rdata")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskRDataRef;
   }
   else if (tokenInfo.compare("class")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskVMTRef;
   }

   if (tokenInfo.state == dfaQuote) {
      return _module->mapReference(*tokenInfo.token) | mask;
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

int ByteCodeAssembler :: readN(ScriptToken& tokenInfo, ReferenceMap& constants, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   if (tokenInfo.compare("-")) {
      int val = readN(tokenInfo, constants, false);

      return -val;
   }
   else if (constants.exist(*tokenInfo.token)) {
      return constants.get(*tokenInfo.token);
   }
   else {
      if (tokenInfo.state == dfaInteger) {
         return tokenInfo.token.toInt();
      }
      else if (tokenInfo.state == dfaHexInteger) {
         return tokenInfo.token.toInt(16);
      }
      else {
         IdentifierString platformConstant(*tokenInfo.token);
         if (_mode64) {
            platformConstant.append("64");
         }
         else platformConstant.append("32");

         if (constants.exist(*platformConstant)) {
            return constants.get(*platformConstant);
         }
         else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
      }
   }
}

mssg_t ByteCodeAssembler :: readM(ScriptToken& tokenInfo, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   mssg_t message = ByteCodeUtil::resolveMessage(*tokenInfo.token, _module);

   return message;
}

int ByteCodeAssembler :: readI(ScriptToken& tokenInfo, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   if (tokenInfo.state == dfaInteger) {
      return tokenInfo.token.toInt();
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}


int ByteCodeAssembler :: readFrameI(ScriptToken& tokenInfo, ReferenceMap& locals, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   if (tokenInfo.state == dfaInteger) {
      return tokenInfo.token.toInt();
   }
   else if (locals.exist(*tokenInfo.token)) {
      return locals.get(*tokenInfo.token);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

int ByteCodeAssembler :: readDisp(ScriptToken& tokenInfo, ReferenceMap& dataLocals, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   if (tokenInfo.state == dfaInteger) {
      return tokenInfo.token.toInt();
   }
   else if (dataLocals.exist(*tokenInfo.token)) {
      return dataLocals.get(*tokenInfo.token);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

bool ByteCodeAssembler :: writeArg(MemoryWriter& writer, Operand& arg, int index)
{
   switch (arg.type) {
      case Operand::Type::R:
         // store :index, :arg.reference
         ByteCodeUtil::write(writer, ByteCode::XStoreSIR, index, arg.reference);
         break;
      case Operand::Type::DataVariable:
         ByteCodeUtil::write(writer, ByteCode::SetDP, arg.reference);
         if (arg.byVal) {
            // load
            // save sp:index
            ByteCodeUtil::write(writer, ByteCode::Load);
            ByteCodeUtil::write(writer, ByteCode::SaveSI, index);
         }
         else {
            // store sp:index
            ByteCodeUtil::write(writer, ByteCode::StoreSI, index);
         }
         break;
      case Operand::Type::Variable:
         if (arg.byVal) {
            // peek fp:arg.reference
            // load
            // save sp:index
            ByteCodeUtil::write(writer, ByteCode::PeekFI, arg.reference);
            ByteCodeUtil::write(writer, ByteCode::Load);
            ByteCodeUtil::write(writer, ByteCode::SaveSI, index);
         }
         else {
            // mov sp:index, fp:arg.reference,
            ByteCodeUtil::write(writer, ByteCode::MovSIFI, index, arg.reference);
         }
         break;
      default:
         return false;
   }

   return true;
}

ByteCodeAssembler::Operand ByteCodeAssembler :: compileArg(ScriptToken& tokenInfo, ReferenceMap& locals, 
   ReferenceMap& dataLocals, ReferenceMap& constants)
{
   Operand arg;
   if (tokenInfo.compare("*")) {
      arg.byVal = true;
      read(tokenInfo);
   }

   if (tokenInfo.compare("array")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskArrayRef;

      return arg;
   }
   else if (tokenInfo.compare("marray")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskTypeListRef;

      return arg;
   }
   else if (locals.exist(*tokenInfo.token)) {
      arg.type = Operand::Type::Variable;
      arg.reference = locals.get(*tokenInfo.token);

      return arg;
   }
   else if (dataLocals.exist(*tokenInfo.token)) {
      arg.type = Operand::Type::DataVariable;
      arg.reference = dataLocals.get(*tokenInfo.token);

      return arg;
   }
   else if (constants.exist(*tokenInfo.token)) {
      arg.type = Operand::Type::Value;
      arg.reference = locals.get(*tokenInfo.token);

      return arg;
   }
   else {
      IdentifierString platformConstant(*tokenInfo.token);
      if (_mode64) {
         platformConstant.append("64");
      }
      else platformConstant.append("32");

      if (constants.exist(*platformConstant)) {
         arg.type = Operand::Type::Value;
         arg.reference = constants.get(*platformConstant);

         return arg;
      }
      else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
   }   
}

void ByteCodeAssembler :: compileArgList(ScriptToken& tokenInfo, List<Operand>& operands,
   ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants)
{
   int counter = 0;
   while (!tokenInfo.compare(")")) {
      counter++;

      if (counter > 1) {
         if (tokenInfo.compare(",")) {
            read(tokenInfo);
         }
         else throw SyntaxError(ASM_COMMA_EXPECTED, tokenInfo.lineInfo);
      }

      Operand arg = compileArg(tokenInfo, locals, dataLocals, constants);
      operands.add(arg);

      read(tokenInfo);
   }

   read(tokenInfo);
}

bool ByteCodeAssembler :: compileDDisp(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   ReferenceMap& dataLocals, bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readDisp(tokenInfo, dataLocals, false);

   ByteCodeUtil::write(writer, command);
   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
   ReferenceMap& constants, bool skipRead)
{
   command.arg1 = readN(tokenInfo, constants, skipRead);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileCloseOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
   ReferenceMap& dataLocals, ReferenceMap& constants)
{
   if (tokenInfo.compare("[")) {
      command.arg1 = dataLocals.count() * (_mode64 ? 8 : 4);

      read(tokenInfo, "]", ASM_SBRACKETCLOSE_EXPECTED);
   }
   else command.arg1 = readN(tokenInfo, constants, true);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileOpFrameI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   ReferenceMap& locals, bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readFrameI(tokenInfo, locals, false);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileOpStackI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   ReferenceMap& locals, bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readFrameI(tokenInfo, locals, false);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileOpIN(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& constants, bool skipRead)
{
   command.arg1 = readI(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   command.arg2 = readN(tokenInfo, constants);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileOpII(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, bool skipRead)
{
   command.arg1 = readI(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   command.arg2 = readI(tokenInfo);

   ByteCodeUtil::write(writer, command);

   return true;
}

int ByteCodeAssembler :: readArgList(ScriptToken& tokenInfo, ReferenceMap& locals, ReferenceMap& constants, 
   int factor, bool allowSize)
{
   int offset = factor;
   int size = 0;

   read(tokenInfo);
   while (true) {
      IdentifierString name(*tokenInfo.token);

      if (!locals.exist(*name)) {
         locals.add(*name, offset);
      }
      else throw SyntaxError(ASM_DUPLICATE_ARG, tokenInfo.lineInfo);

      read(tokenInfo);
      if (allowSize) {
         if (allowSize && tokenInfo.compare(":")) {
            read(tokenInfo);
            auto arg = compileArg(tokenInfo, locals, locals, constants);
            if (arg.type == Operand::Type::Value) {
               offset -= arg.reference;
               size += arg.reference;

               read(tokenInfo);
            }
            else throw SyntaxError(ASM_INVALID_TARGET, tokenInfo.lineInfo);
         }
         else {
            offset += factor;
            size -= factor;
         }
      }
      else {
         offset += factor;
         size += factor;
      }

      if (tokenInfo.compare(",")) {
         read(tokenInfo);
      }
      else if (tokenInfo.compare("]")) {
         break;
      }
      else throw SyntaxError(ASM_COMMA_EXPECTED, tokenInfo.lineInfo);
   }

   return size;
}

bool ByteCodeAssembler :: compileOpenOp(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants)
{
   if (tokenInfo.compare("[")) {
      readArgList(tokenInfo, locals, constants, 1, false);

      read(tokenInfo);
      int dataSize = 0;
      if (tokenInfo.compare(",")) {
         read(tokenInfo, "[", ASM_INVALID_DESTINATION);

         dataSize = readArgList(tokenInfo, dataLocals, constants, (_mode64 ? 8 : 4) * -1, true);

         read(tokenInfo);
      }

      command.arg1 = locals.count();
      command.arg2 = dataSize;

      ByteCodeUtil::write(writer, command);

      return true;
   }
   else return compileOpIN(tokenInfo, writer, command, constants, true);
}

bool ByteCodeAssembler :: compileCallExt(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants)
{
   IdentifierString functionName;
   if (tokenInfo.compare(":")) {
      read(tokenInfo);
   }
   else return false;

   if (tokenInfo.state == dfaIdentifier) {
      functionName.copy(*tokenInfo.token);
   }
   else return false;

   ReferenceName function(RT_FORWARD);
   function.append(".");
   function.append(*tokenInfo.token);

   command.arg1 = _module->mapReference(*function) | mskExternalRef;

   read(tokenInfo);
   if (tokenInfo.compare("(")) {
      List<Operand> args(Operand::Default());

      read(tokenInfo);
      compileArgList(tokenInfo, args, locals, dataLocals, constants);

      int stackSize = 0;
      if (args.count() > 0) {
         if (_mode64) {
            stackSize = align(args.count(), 2);
         }
         else stackSize = args.count();
      }

      if (stackSize > 0)
         ByteCodeUtil::write(writer, ByteCode::AllocI, stackSize);

      int index = 0;
      for(auto it = args.start(); !it.eof(); ++it) {
         writeArg(writer, *it, index++);
      }

      command.arg2 = args.count();

      ByteCodeUtil::write(writer, command);

      if (stackSize > 0)
         ByteCodeUtil::write(writer, ByteCode::FreeI, stackSize);
   }

   return true;
}

bool ByteCodeAssembler :: compileMR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead)
{
   mssg_t arg = readM(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   ref_t arg2 = readReference(tokenInfo);

   ByteCodeUtil::write(writer, command.code, arg, arg2);

   return true;
}

bool ByteCodeAssembler :: compileRR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead)
{
   mssg_t arg = readReference(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   ref_t arg2 = readReference(tokenInfo);

   ByteCodeUtil::write(writer, command.code, arg, arg2);

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileJcc(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, ByteCodeLabelHelper& lh)
{
   if (!lh.checkDeclaredLabel(*tokenInfo.token)) {
      lh.registerJump(*tokenInfo.token, writer);

      ByteCodeUtil::write(writer, command.code, 0);
   }
   else {
      int offset = lh.resolveLabel(*tokenInfo.token, writer);

      ByteCodeUtil::write(writer, command.code, offset);
   }

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileByteCode(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCodeLabelHelper lh,
   ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants)
{
   IdentifierString command(*tokenInfo.token);
   size_t timePos = command.length();

   read(tokenInfo);

   command.append(' ');
   command.append(*tokenInfo.token);

   ByteCommand opCommand;
   opCommand.code = ByteCodeUtil::code(*command);
   if (opCommand.code == ByteCode::None) {
      command.truncate(timePos);

      opCommand.code = ByteCodeUtil::code(*command);
      if (opCommand.code == ByteCode::None)
         return false;
   }
   else read(tokenInfo);

   if (!ByteCodeUtil::isSingleOp(opCommand.code)) {
      switch (opCommand.code) {
         case ByteCode::CallExtR:
            return compileCallExt(tokenInfo, writer, opCommand, locals, dataLocals, constants);
         case ByteCode::OpenIN:
         case ByteCode::OpenHeaderIN:
            return compileOpenOp(tokenInfo, writer, opCommand, locals, dataLocals, constants);
         case ByteCode::CloseN:
            return compileCloseOpN(tokenInfo, writer, opCommand, dataLocals, constants);
         case ByteCode::MovSIFI:
            return compileOpII(tokenInfo, writer, opCommand, true);
         case ByteCode::StoreFI:
         case ByteCode::CmpFI:
            return compileOpFrameI(tokenInfo, writer, opCommand, locals, true);
         case ByteCode::PeekSI:
         case ByteCode::CmpSI:
            return compileOpStackI(tokenInfo, writer, opCommand, locals, true);
         case ByteCode::SaveDP:
         case ByteCode::SetDP:
            return compileDDisp(tokenInfo, writer, opCommand, dataLocals, true);
         case ByteCode::CallMR:
         case ByteCode::VCallMR:
         case ByteCode::JumpMR:
         case ByteCode::VJumpMR:
            return compileMR(tokenInfo, writer, opCommand, true);
         case ByteCode::SelEqRR:
         case ByteCode::SelLtRR:
            return compileRR(tokenInfo, writer, opCommand, true);
         case ByteCode::AndN:
         case ByteCode::ICmpN:
            return compileOpN(tokenInfo, writer, opCommand, constants, true);
         case ByteCode::Jeq:
            return compileJcc(tokenInfo, writer, opCommand, lh);
         default:
            return false;
      }
   }
   else {
      ByteCodeUtil::write(writer, opCommand);
      return true;
   }
}

void ByteCodeAssembler :: declareLabel(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCodeLabelHelper& labelScope)
{
   if (labelScope.checkDeclaredLabel(*tokenInfo.token))
      throw SyntaxError(ASM_LABEL_EXISTS, tokenInfo.lineInfo);

   if (!labelScope.declareLabel(*tokenInfo.token, writer))
      throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);

   read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   read(tokenInfo);
}

void ByteCodeAssembler :: compileProcedure(ScriptToken& tokenInfo, ref_t mask, ReferenceMap& constants)
{
   ByteCodeLabelHelper lh;

   ReferenceName name;
   name.combine(*tokenInfo.token);

   ref_t reference = _module->mapReference(*name) | mask;

   if (_module->mapSection(reference, true) != nullptr) {
      throw SyntaxError(ASM_PROCEDURE_EXIST, tokenInfo.lineInfo);
   }

   read(tokenInfo);
         
   MemoryBase* code = _module->mapSection(reference, false);
   MemoryWriter codeWriter(code);

   // size place holder
   pos_t sizePlaceholder = codeWriter.position();
   codeWriter.writePos(0);

   ReferenceMap locals(0);
   ReferenceMap dataLocals(0);
   while (!tokenInfo.compare("end")) {
      if (!compileByteCode(tokenInfo, codeWriter, lh, locals, dataLocals, constants)) {
         if (tokenInfo.state != dfaEOF) {
            declareLabel(tokenInfo, codeWriter, lh);
         }
         else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
      }
   }

   // fix labels
   lh.checkAllUsedLabels("Used label ( %s ) not declared in procedure %s\n", *name);

   // fix place holder
   pos_t endPosition = codeWriter.position();
   pos_t size = endPosition - sizePlaceholder - sizeof(pos_t);

   codeWriter.seek(sizePlaceholder);
   codeWriter.writePos(size);
   codeWriter.seek(endPosition);

}

void ByteCodeAssembler :: compileConstant(ScriptToken& tokenInfo, ReferenceMap& constants)
{
   if (!constants.exist(*tokenInfo.token)) {
      IdentifierString name(*tokenInfo.token);

      int value = readN(tokenInfo, constants);
      constants.add(*name, value);
   }
   else throw SyntaxError(ASM_DUPLICATE_ARG, tokenInfo.lineInfo);

   read(tokenInfo, ";", ASM_SEMICOLON_EXPECTED);
}

void ByteCodeAssembler :: compile()
{
   ScriptToken   tokenInfo;
   ReferenceMap  constants(0);
   while (_reader.read(tokenInfo)) {
      if (tokenInfo.compare("symbol")) {
         read(tokenInfo);

         compileProcedure(tokenInfo, mskSymbolRef, constants);
      }
      else if (tokenInfo.compare("procedure")) {
         read(tokenInfo);

         compileProcedure(tokenInfo, mskProcedureRef, constants);
      }
      else if (tokenInfo.compare("define")) {
         read(tokenInfo);

         compileConstant(tokenInfo, constants);
      }
      else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
   }
}
