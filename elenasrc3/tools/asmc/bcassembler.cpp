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

ref_t ByteCodeAssembler :: readReference(ScriptToken& tokenInfo)
{
   _reader.read(tokenInfo);

   if (tokenInfo.state == dfaQuote) {
      return _module->mapReference(*tokenInfo.token);
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

int ByteCodeAssembler :: readN(ScriptToken& tokenInfo, bool skipRead)
{
   if (!skipRead)
      _reader.read(tokenInfo);

   if (tokenInfo.state == dfaInteger) {
      return tokenInfo.token.toInt();
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
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

ByteCodeAssembler::Operand ByteCodeAssembler :: compileArg(ScriptToken& tokenInfo, ReferenceMap& locals)
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
      arg.reference = readReference(tokenInfo) | mskMetaArrayRef;

      return arg;
   }
   else if (locals.exist(*tokenInfo.token)) {
      arg.type = Operand::Type::Variable;
      arg.reference = locals.get(*tokenInfo.token);

      return arg;
   }
   else throw SyntaxError(ASM_INVALID_COMMAND, tokenInfo.lineInfo);
}

void ByteCodeAssembler :: compileArgList(ScriptToken& tokenInfo, List<Operand>& operands, ReferenceMap& locals)
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

      Operand arg = compileArg(tokenInfo, locals);
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

bool ByteCodeAssembler :: compileOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, bool skipRead)
{
   command.arg1 = readN(tokenInfo, skipRead);

   ByteCodeUtil::write(writer, command);

   read(tokenInfo);
   return true;
}

bool ByteCodeAssembler :: compileCloseOpN(ScriptToken& tokenInfo, MemoryWriter& writer, ByteCommand& command, ReferenceMap& dataLocals)
{
   if (tokenInfo.compare("[")) {
      command.arg1 = dataLocals.count() * (_mode64 ? 8 : 4);

      read(tokenInfo, "]", ASM_SBRACKETCLOSE_EXPECTED);
   }
   else command.arg1 = readN(tokenInfo, true);

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

bool ByteCodeAssembler :: compileOpIN(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, bool skipRead)
{
   command.arg1 = readI(tokenInfo, skipRead);

   read(tokenInfo, ",", ASM_COMMA_EXPECTED);

   command.arg2 = readN(tokenInfo);

   ByteCodeUtil::write(writer, command);

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

void ByteCodeAssembler :: readArgList(ScriptToken& tokenInfo, ReferenceMap& locals, int factor)
{
   read(tokenInfo);
   while (true) {
      if (!locals.exist(*tokenInfo.token)) {
         locals.add(*tokenInfo.token, (locals.count() + 1) * factor);
      }
      else throw SyntaxError(ASM_DUPLICATE_ARG, tokenInfo.lineInfo);

      read(tokenInfo);
      if (tokenInfo.compare(",")) {
         read(tokenInfo);
      }
      else if (tokenInfo.compare("]")) {
         break;
      }
      else throw SyntaxError(ASM_COMMA_EXPECTED, tokenInfo.lineInfo);
   }
}

bool ByteCodeAssembler :: compileOpenOp(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& locals, ReferenceMap& dataLocals)
{
   if (tokenInfo.compare("[")) {
      readArgList(tokenInfo, locals, 1);

      read(tokenInfo);
      if (tokenInfo.compare(",")) {
         read(tokenInfo, "[", ASM_INVALID_DESTINATION);

         readArgList(tokenInfo, dataLocals, (_mode64 ? 8 : 4) * -1);

         read(tokenInfo);
      }

      command.arg1 = locals.count();
      command.arg2 = dataLocals.count() * (_mode64 ? 8 : 4);

      ByteCodeUtil::write(writer, command);

      return true;
   }
   else return compileOpIN(tokenInfo, writer, command, true);
}

bool ByteCodeAssembler :: compileCallExt(ScriptToken& tokenInfo, MemoryWriter& writer,
   ByteCommand& command, ReferenceMap& locals)
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
      compileArgList(tokenInfo, args, locals);

      if (args.count() > 0)
         ByteCodeUtil::write(writer, ByteCode::AllocI, args.count());

      int index = 0;
      for(auto it = args.start(); !it.eof(); ++it) {
         writeArg(writer, *it, index++);
      }

      command.arg2 = args.count();

      ByteCodeUtil::write(writer, command);

      if (args.count() > 0)
         ByteCodeUtil::write(writer, ByteCode::FreeI, args.count());
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

bool ByteCodeAssembler :: compileByteCode(ScriptToken& tokenInfo, MemoryWriter& writer,
   ReferenceMap& locals, ReferenceMap& dataLocals)
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
            return compileCallExt(tokenInfo, writer, opCommand, locals);
         case ByteCode::OpenIN:
         case ByteCode::OpenHeaderIN:
            return compileOpenOp(tokenInfo, writer, opCommand, locals, dataLocals);
         case ByteCode::CloseN:
            return compileCloseOpN(tokenInfo, writer, opCommand, dataLocals);
         case ByteCode::MovSIFI:
            return compileOpII(tokenInfo, writer, opCommand, true);
         case ByteCode::StoreFI:
            return compileOpFrameI(tokenInfo, writer, opCommand, locals, true);
         case ByteCode::SaveDDisp:
         case ByteCode::SetDDisp:
            return compileDDisp(tokenInfo, writer, opCommand, dataLocals, true);
         case ByteCode::CallMR:
         case ByteCode::VCallMR:
            return compileMR(tokenInfo, writer, opCommand, true);
         default:
            return false;
      }
   }
   else {
      ByteCodeUtil::write(writer, opCommand);
      return true;
   }
}

void ByteCodeAssembler :: compileProcedure(ScriptToken& tokenInfo, ref_t mask)
{
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
      if (!compileByteCode(tokenInfo, codeWriter, locals, dataLocals)) {
         throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
      }
   }
   // fix place holder
   pos_t endPosition = codeWriter.position();
   pos_t size = endPosition - sizePlaceholder - sizeof(pos_t);

   codeWriter.seek(sizePlaceholder);
   codeWriter.writePos(size);
   codeWriter.seek(endPosition);

}

void ByteCodeAssembler :: compile()
{
   ScriptToken  tokenInfo;
   while (_reader.read(tokenInfo)) {
      if (tokenInfo.compare("symbol")) {
         read(tokenInfo);

         compileProcedure(tokenInfo, mskSymbolRef);
      }
      else if (tokenInfo.compare("procedure")) {
         read(tokenInfo);

         compileProcedure(tokenInfo, mskProcedureRef);
      }
      else throw SyntaxError(ASM_SYNTAXERROR, tokenInfo.lineInfo);
   }
}
