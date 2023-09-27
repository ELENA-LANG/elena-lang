//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA Byte-code assembler
//		classes.
//                                            (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "bcassembler.h"
#include "asmconst.h"

using namespace elena_lang;

// --- ByteCodeAssembler::ByteCodeLabelHelper ---

bool ByteCodeAssembler::ByteCodeLabelHelper :: fixLabel(pos_t label, MemoryWriter& writer, ReferenceHelperBase*)
{
   MemoryBase* memory = writer.Memory();

   auto it = jumps.start();
   while (!it.eof()) {
      if (it.key() == label) {
         pos_t labelPos = (*it).position + 1;
         pos_t offset = writer.position() - labelPos - 4;

         MemoryBase::writeDWord(memory, labelPos, offset);
      }

      ++it;
   }

   return true;
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
   int offset = labels.get(getLabel(label)) - writer.position() - 5;

   return offset;
}

void ByteCodeAssembler::ByteCodeLabelHelper :: checkAllUsedLabels(ustr_t errorMessage, ustr_t procedureName)
{
   auto it = labelNames.start();
   while (!it.eof()) {
      ustr_t label = it.key();

      // Check if label is declared
      if (!declaredLabels.get(*it))
         throw ProcedureError(errorMessage, procedureName, label);

      it++;
   }
}

// --- ByteCodeAssembler ---

ByteCodeAssembler :: ByteCodeAssembler(int tabSize, UStrReader* reader, Module* module, 
   bool mode64, int rawDataAlignment)
   : _reader(tabSize, reader)
{
   _module = module;
   _mode64 = mode64;
   _rawDataAlignment = rawDataAlignment;
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

   bool constantMode = false;
   ref_t mask = 0;
   if (tokenInfo.compare("rdata")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskRDataRef;
   }
   else if (tokenInfo.compare("nil")) {
      return 0;
   }
   else if (tokenInfo.compare("terminator")) {
      return -1;
   }
   else if (tokenInfo.compare("class")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskVMTRef;
   }
   else if (tokenInfo.compare("mssgconst")) {
      constantMode = true;
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      ByteCodeUtil::resolveMessage(*tokenInfo.token, _module, false);

      mask = mskMssgLiteralRef;
   }
   else if (tokenInfo.compare("procedure")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskProcedureRef;
   }
   else if (tokenInfo.compare("marray")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskTypeListRef;
   }
   else if (tokenInfo.compare("array")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskConstArray;
   }
   else if (tokenInfo.compare("pstr")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      _reader.read(tokenInfo);

      mask = mskPSTRRef;
   }

   if (tokenInfo.state == dfaQuote) {
      if (constantMode) {
         return _module->mapConstant(*tokenInfo.token) | mask;
      }
      else return _module->mapReference(*tokenInfo.token) | mask;
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
   if (tokenInfo.compare("~")) {
      int val = readN(tokenInfo, constants, false);

      return ~val;
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
         else if (constants.exist(*tokenInfo.token)) {
            return constants.get(*tokenInfo.token);
         }
         else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
      }
   }
}

mssg_t ByteCodeAssembler :: readM(ScriptToken& tokenInfo, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   mssg_t message = ByteCodeUtil::resolveMessage(*tokenInfo.token, _module, false);

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

int ByteCodeAssembler :: readFrameI(ScriptToken& tokenInfo, ReferenceMap& parameters, 
   ReferenceMap& locals, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   if (tokenInfo.compare("-")) {
      int val = readFrameI(tokenInfo, parameters, locals, false);

      return -val;
   }
   else if (tokenInfo.state == dfaInteger) {
      return tokenInfo.token.toInt();
   }
   else if (parameters.exist(*tokenInfo.token)) {
      return parameters.get(*tokenInfo.token);
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
         if (arg.byVal64) {
            // load
            // save sp:index
            ByteCodeUtil::write(writer, ByteCode::LLoad);
            ByteCodeUtil::write(writer, ByteCode::LSaveSI, index);
         }
         else if (arg.byVal) {
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
         if (arg.byVal64) {
            // peek fp:arg.reference
            // load
            // save sp:index
            ByteCodeUtil::write(writer, ByteCode::PeekFI, arg.reference);
            ByteCodeUtil::write(writer, ByteCode::LLoad);
            ByteCodeUtil::write(writer, ByteCode::LSaveSI, index);
         }
         else if (arg.byVal) {
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
      case Operand::Type::Value:
         ByteCodeUtil::write(writer, ByteCode::MovN, arg.reference);
         ByteCodeUtil::write(writer, ByteCode::SaveSI, index);
         break;
      default:
         return false;
   }

   return true;
}

ByteCodeAssembler::Operand ByteCodeAssembler :: compileArg(ScriptToken& tokenInfo, ReferenceMap& parameters, ReferenceMap& locals,
   ReferenceMap& dataLocals, ReferenceMap& constants)
{
   Operand arg;
   if (tokenInfo.compare("*")) {
      read(tokenInfo);

      arg.byVal = true;
   }
   if (tokenInfo.token[0] == '$') {
      tokenInfo.token.cut(0, 1);

      if(arg.byVal) {
         if (_mode64) {
            arg.byVal64 = true;
            arg.byVal = false;
         }
      }
      else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
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
   else if (tokenInfo.compare("constarray")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskConstArray;

      return arg;
   }
   else if (tokenInfo.compare("constdump")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskConstant;

      return arg;
   }
   else if (tokenInfo.compare("pstr")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskPSTRRef;

      return arg;
   }
   else if (tokenInfo.compare("procedure")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskProcedureRef;

      return arg;
   }
   else if (tokenInfo.compare("symbol")) {
      read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

      arg.type = Operand::Type::R;
      arg.reference = readReference(tokenInfo) | mskSymbolRef;

      return arg;
   }
   else if (parameters.exist(*tokenInfo.token)) {
      arg.type = Operand::Type::Variable;
      arg.reference = parameters.get(*tokenInfo.token);

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
      arg.reference = constants.get(*tokenInfo.token);

      return arg;
   }
   else if (tokenInfo.state == dfaInteger) {
      arg.type = Operand::Type::Value;
      arg.reference = tokenInfo.token.toInt();

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
   ReferenceMap& parameters, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants)
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

      Operand arg = compileArg(tokenInfo, parameters, locals, dataLocals, constants);
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

bool ByteCodeAssembler :: compileDDispR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   ReferenceMap& dataLocals, bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readDisp(tokenInfo, dataLocals, false);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   command.arg2 = readReference(tokenInfo);

   ByteCodeUtil::write(writer, command);
   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileDDispN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   ReferenceMap& dataLocals, ReferenceMap& constants, bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readDisp(tokenInfo, dataLocals, false);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   command.arg2 = readN(tokenInfo, constants);

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

bool ByteCodeAssembler :: compileOpI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   bool skipRead)
{
   command.arg1 = readI(tokenInfo, skipRead);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileCloseOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
   int dataSize, ReferenceMap& constants)
{
   if (tokenInfo.compare("[")) {
      command.arg1 = align(dataSize, _rawDataAlignment);

      read(tokenInfo, "]", ASM_SBRACKETCLOSE_EXPECTED);
   }
   else if (tokenInfo.compare(":")) {
      command.arg1 = readN(tokenInfo, constants, false);
   }
   else command.arg1 = readN(tokenInfo, constants, true);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileOpFrameI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   ReferenceMap& parameters, ReferenceMap& locals, bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readFrameI(tokenInfo, parameters, locals, false);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileOpStackI(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command,
   bool skipRead)
{
   if (skipRead) {
      if (!tokenInfo.compare(":"))
         throw SyntaxError(ASM_DOUBLECOLON_EXPECTED, tokenInfo.lineInfo);
   }
   else read(tokenInfo, ":", ASM_DOUBLECOLON_EXPECTED);

   command.arg1 = readI(tokenInfo, false);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileOpIN(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& constants, bool skipRead)
{
   if (tokenInfo.compare(":"))
      read(tokenInfo);

   command.arg1 = readI(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   read(tokenInfo);
   if (tokenInfo.compare(":"))
      read(tokenInfo);

   command.arg2 = readN(tokenInfo, constants, true);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileOpII(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, bool skipRead)
{
   if (tokenInfo.compare(":"))
      read(tokenInfo);

   command.arg1 = readI(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   command.arg2 = readI(tokenInfo);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);

   return true;
}

int ByteCodeAssembler :: readArgList(ScriptToken& tokenInfo, ReferenceMap& locals, ReferenceMap& constants,
   int factor, bool allowSize)
{
   int offset = factor;
   int size = 0;

   read(tokenInfo);
   if (tokenInfo.compare("]")) {
      // if it is an empty list
      return 0;
   }

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
            auto arg = compileArg(tokenInfo, locals, locals, locals, constants);
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

void ByteCodeAssembler :: readParameterList(ScriptToken& tokenInfo, ReferenceMap& parameters)
{
   read(tokenInfo);
   if (tokenInfo.compare(")")) {
      // if it is an empty list
      return;
   }

   int offset = -1;
   while (true) {
      IdentifierString name(*tokenInfo.token);

      if (!parameters.exist(*name)) {
         parameters.add(*name, offset);
      }
      else throw SyntaxError(ASM_DUPLICATE_ARG, tokenInfo.lineInfo);

      read(tokenInfo);
      if (tokenInfo.compare(",")) {
         read(tokenInfo);
      }
      else if (tokenInfo.compare(")")) {
         break;
      }
      else throw SyntaxError(ASM_COMMA_EXPECTED, tokenInfo.lineInfo);

      offset -= 1;
   }
}

bool ByteCodeAssembler :: compileOpenOp(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants, 
   int& dataSize)
{
   int argCount = 0;
   if (tokenInfo.compare("(")) {
      argCount = readN(tokenInfo, constants);
      read(tokenInfo, ")", ASM_INVALID_DESTINATION);
      read(tokenInfo);
      if (tokenInfo.compare(","))
         read(tokenInfo);
   }

   dataSize = 0;
   if (tokenInfo.compare("[")) {
      readArgList(tokenInfo, locals, constants, 1, false);

      read(tokenInfo);
      
      if (tokenInfo.compare(",")) {
         read(tokenInfo, "[", ASM_INVALID_DESTINATION);

         dataSize = readArgList(tokenInfo, dataLocals, constants, (_mode64 ? 8 : 4) * -1, true);

         read(tokenInfo);
      }

      command.arg1 = locals.count() + argCount;
      command.arg2 = align(dataSize, _rawDataAlignment);

      if (_mode64)
         command.arg1 = align(command.arg1, 2);

      ByteCodeUtil::write(writer, command);

      return true;
   }
   else {
      bool retVal = compileOpIN(tokenInfo, writer, command, constants, true);
      if (retVal)
         dataSize = command.arg2;

      return retVal;
   }
}

bool ByteCodeAssembler :: compileCallExt(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& parameters, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants)
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

   functionName.copy(*tokenInfo.token);

   read(tokenInfo);
   if (tokenInfo.compare(".")) {
      read(tokenInfo);

      function.copy(*functionName);
      functionName.copy(*tokenInfo.token);

      read(tokenInfo);
   }

   function.append(".");
   function.append(*functionName);

   command.arg1 = _module->mapReference(*function) | mskExternalRef;
   
   if (tokenInfo.compare("(")) {
      List<Operand> args(Operand::Default());

      read(tokenInfo);
      compileArgList(tokenInfo, args, parameters, locals, dataLocals, constants);

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

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileOpM(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead)
{
   mssg_t arg = readM(tokenInfo, skipRead);

   ByteCodeUtil::write(writer, command.code, arg);

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileNR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, 
   ReferenceMap& constants, bool skipRead)
{
   mssg_t arg = readN(tokenInfo, constants, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   ref_t arg2 = readReference(tokenInfo);

   ByteCodeUtil::write(writer, command.code, arg, arg2);

   read(tokenInfo);

   return true;
}

bool ByteCodeAssembler :: compileR(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead)
{
   ref_t arg = readReference(tokenInfo, skipRead);

   ByteCodeUtil::write(writer, command.code, arg);

   read(tokenInfo);

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

bool ByteCodeAssembler :: compileByteCode(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCodeLabelHelper& lh,
   ReferenceMap& parameters, ReferenceMap& locals, ReferenceMap& dataLocals, ReferenceMap& constants, int& dataSize)
{
   IdentifierString command(*tokenInfo.token);

   if (tokenInfo.token[0] == '$') {
      command.copy(*tokenInfo.token);

      command.cut(0, 1);

      if (_mode64) {
         command.insert("l", 0);
      }
   }

   size_t timePos = command.length();

   read(tokenInfo);

   command.append(' ');
   command.append(*tokenInfo.token);

   ByteCommand opCommand;
   opCommand.code = ByteCodeUtil::code(*command);
   if (opCommand.code == ByteCode::None) {
      command.truncate(timePos);

      opCommand.code = ByteCodeUtil::code(*command);
      if (opCommand.code == ByteCode::None) {
         if (tokenInfo.state != dfaEOF) {
            return declareLabel(*command, tokenInfo, writer, lh);
         }
      }
   }
   else read(tokenInfo);

   if (!ByteCodeUtil::isSingleOp(opCommand.code)) {
      switch (opCommand.code) {
         case ByteCode::CallExtR:
            return compileCallExt(tokenInfo, writer, opCommand, parameters, locals, dataLocals, constants);
         case ByteCode::XOpenIN:
         case ByteCode::OpenIN:
         case ByteCode::OpenHeaderIN:
            return compileOpenOp(tokenInfo, writer, opCommand, locals, dataLocals, constants, dataSize);
         case ByteCode::CloseN:
            return compileCloseOpN(tokenInfo, writer, opCommand, dataSize, constants);
         case ByteCode::MovSIFI:
         case ByteCode::XMovSISI:
            return compileOpII(tokenInfo, writer, opCommand, true);
         case ByteCode::StoreFI:
         case ByteCode::CmpFI:
         case ByteCode::PeekFI:
         case ByteCode::SetFP:
         case ByteCode::XSetFP:
         case ByteCode::SetSP:
            return compileOpFrameI(tokenInfo, writer, opCommand, parameters, locals, true);
         case ByteCode::PeekSI:
         case ByteCode::StoreSI:
         case ByteCode::CmpSI:
         case ByteCode::XFlushSI:
         case ByteCode::XSwapSI:
         case ByteCode::SwapSI:
         case ByteCode::XRefreshSI:
         case ByteCode::XLoadArgSI:
         case ByteCode::SaveSI:
            return compileOpStackI(tokenInfo, writer, opCommand, true);
         case ByteCode::SaveDP:
         case ByteCode::LSaveDP:
         case ByteCode::SetDP:
         case ByteCode::LoadDP:
         case ByteCode::XCmpDP:
         case ByteCode::FTruncDP:
         case ByteCode::FRoundDP:
         case ByteCode::FAbsDP:
         case ByteCode::FSqrtDP:
         case ByteCode::FExpDP:
         case ByteCode::FLnDP:
         case ByteCode::FSinDP:
         case ByteCode::FCosDP:
         case ByteCode::FArctanDP:
         case ByteCode::FPiDP:
         case ByteCode::NConvFDP:
         case ByteCode::LLoadDP:
         case ByteCode::XAddDP:
            return compileDDisp(tokenInfo, writer, opCommand, dataLocals, true);
         case ByteCode::TstM:
         case ByteCode::MovM:
            return compileOpM(tokenInfo, writer, opCommand, false);
         case ByteCode::AssignI:
         case ByteCode::GetI:
         case ByteCode::AllocI:
         case ByteCode::FreeI:
         case ByteCode::XStoreI:
         case ByteCode::XAssignI:
            return compileOpI(tokenInfo, writer, opCommand, false);
         case ByteCode::XHookDPR:
         case ByteCode::XLabelDPR:
            return compileDDispR(tokenInfo, writer, opCommand, dataLocals, true);
         case ByteCode::CallMR:
         case ByteCode::VCallMR:
         case ByteCode::JumpMR:
         case ByteCode::VJumpMR:
            return compileMR(tokenInfo, writer, opCommand, false);
         case ByteCode::SelEqRR:
         case ByteCode::SelLtRR:
         case ByteCode::SelULtRR:
            return compileRR(tokenInfo, writer, opCommand, true);
         case ByteCode::ICmpN:
         case ByteCode::TstN:
         case ByteCode::NLen:
         case ByteCode::ReadN:
         case ByteCode::WriteN:
         case ByteCode::System:
         case ByteCode::DCopy:
            return compileOpN(tokenInfo, writer, opCommand, constants, true);
         case ByteCode::MovN:
         case ByteCode::AndN:
         case ByteCode::OrN:
         case ByteCode::AddN:
         case ByteCode::SubN:
         case ByteCode::CmpN:
         case ByteCode::MulN:
         case ByteCode::TstFlag:
            return compileOpN(tokenInfo, writer, opCommand, constants, false);
         case ByteCode::Copy:
            return compileOpN(tokenInfo, writer, opCommand, constants, true);
         case ByteCode::CallVI:
         case ByteCode::JumpVI:
            return compileOpI(tokenInfo, writer, opCommand, false);
         case ByteCode::Jeq:
         case ByteCode::Jne:
         case ByteCode::Jlt:
         case ByteCode::Jge:
         case ByteCode::Jle:
         case ByteCode::Jump:
         case ByteCode::Jgr:
            return compileJcc(tokenInfo, writer, opCommand, lh);
         case ByteCode::SetR:
         case ByteCode::CmpR:
         case ByteCode::XCreateR:
            return compileR(tokenInfo, writer, opCommand, true);
         case ByteCode::XNewNR:
            return compileNR(tokenInfo, writer, opCommand, constants, true);
         case ByteCode::NSaveDPN:
         case ByteCode::NAddDPN:
         case ByteCode::CopyDPN:
         case ByteCode::IAddDPN:
         case ByteCode::ISubDPN:
         case ByteCode::IMulDPN:
         case ByteCode::IDivDPN:
         case ByteCode::UDivDPN:
            return compileDDispN(tokenInfo, writer, opCommand, dataLocals, constants, true);
         default:
            return false;
      }
   }
   else {
      ByteCodeUtil::write(writer, opCommand);
      return true;
   }
}

bool ByteCodeAssembler :: declareLabel(ustr_t label, ScriptToken& tokenInfo, MemoryWriter& writer, 
   ByteCodeLabelHelper& labelScope)
{
   if (!tokenInfo.compare(":"))
      return false;

   if (labelScope.checkDeclaredLabel(label))
      throw SyntaxError(ASM_LABEL_EXISTS, tokenInfo.lineInfo);

   labelScope.fixLabel(labelScope.getLabel(label), writer, nullptr);

   labelScope.declareLabel(label, writer);

   ByteCodeUtil::write(writer, ByteCode::Nop);

   read(tokenInfo);

   return true;
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
         
   ReferenceMap parameters(0);
   if (tokenInfo.compare("(")) {
      readParameterList(tokenInfo, parameters);

      read(tokenInfo);
   }

   MemoryBase* code = _module->mapSection(reference, false);
   MemoryWriter codeWriter(code);

   // size place holder
   pos_t sizePlaceholder = codeWriter.position();
   codeWriter.writePos(0);

   ReferenceMap locals(0);
   ReferenceMap dataLocals(0);
   int dataSize = 0;
   while (!tokenInfo.compare("end")) {
      if (!compileByteCode(tokenInfo, codeWriter, lh, parameters, locals, dataLocals, constants, dataSize))
         throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
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
