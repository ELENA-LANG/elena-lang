//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2019, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "ecassembler.h"
#include "module.h"

using namespace _ELENA_;

// --- ECodesAssembler ---

//int ECodesAssembler::mapVerb(ident_t literal)
//{
//   if (verbs.Count() == 0) {
//      ByteCodeCompiler::loadVerbs(verbs);
//   }
//
//   return verbs.get(literal);
//}

void ECodesAssembler :: fixJump(ident_t label, MemoryWriter& writer, LabelInfo& info)
{
   _Memory* code = writer.Memory();

   Map<ident_t, int>::Iterator it = info.fwdJumps.start();
   while (!it.Eof()) {
      if (label.compare(it.key())) {
         (*code)[*it] = writer.Position() - *it - 4;
      }
      it++;
   }
}

void ECodesAssembler :: writeCommand(ByteCommand command, MemoryWriter& writer)
{
   writer.writeByte(command.code);
   if (command.code > MAX_SINGLE_ECODE) {
      writer.writeDWord(command.argument);
   }
   if (command.code > MAX_DOUBLE_ECODE) {
      writer.writeDWord(command.additional);
   }
}

void ECodesAssembler :: compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int offset = token.readSignedInteger(constants);

   writeCommand(ByteCommand(code, offset), writer);
}

bool ECodesAssembler :: readMessage(ident_t quote, IdentifierString& subject, ref_t& signRef, int& paramCount, _Module* binary)
{
   size_t len = getlength(quote);
   size_t param_index = quote.find('[');
   if (len == 0 || param_index == NOTFOUND_POS || quote[len - 1] != ']')
      return false;

   size_t sign_index = quote.find('<');
   if (sign_index != NOTFOUND_POS) {
      size_t sign_end_index = param_index - 1;
      if (quote[sign_end_index] != '>')
         return false;

      subject.copy(quote, sign_index);

      ref_t signatures[ARG_COUNT];
      size_t sign_len = 0;
      IdentifierString argType;
      size_t startArg = sign_index + 1;
      while (startArg <= sign_end_index) {
         size_t endArg = quote.find(startArg, ',', sign_end_index);

         argType.copy(quote.c_str() + startArg, endArg - startArg);
         signatures[sign_len++] = binary->mapReference(argType.ident(), false);

         startArg = endArg + 1;
      }

      signRef = binary->mapSignature(signatures, sign_len, false);
   }
   else {
      signRef = 0;

      subject.copy(quote, param_index);
   }

   IdentifierString content;
   content.copy(quote + param_index + 1, len - param_index - 2);

   paramCount = content.ident().toInt();

   return true;
}

void ECodesAssembler :: readMessage(TokenInfo& token, IdentifierString& subject, ref_t& signRef, int& paramCount)
{
   while (token.value[0] != '[') {
      subject.append(token.value);

      token.read();
      if (token.value[0] == '$') {
         subject.append(token.value);
         token.read();
      }
   }
   if (token.value[0] == '[') {
      paramCount = token.readInteger(constants);
   }
   else token.raiseErr("Invalid operand (%d)");

   token.read("]", "Invalid operand (%d)");
}

void ECodesAssembler :: compileMessage(TokenInfo& token, IdentifierString& message, ref_t& signRef, _Module* binary)
{   
   IdentifierString action;

   int paramCount = 0;
   if (token.terminal.state == dfaQuote) {
      QuoteTemplate<IdentifierString> quote(token.terminal.line);

      if (!readMessage(quote.ident(), action, signRef, paramCount, binary))
         token.raiseErr("Invalid operand (%d)");
   }
   else readMessage(token, action, signRef, paramCount);

   // reserve place for param counter
   message.append('0');
   message.append(action);

   message[0] = message[0] + paramCount;
}

void ECodesAssembler :: compileMessageName(TokenInfo& token, IdentifierString& messageName)
{
   IdentifierString action;

   ref_t signRef = 0;

   if (token.terminal.state == dfaQuote) {
      QuoteTemplate<IdentifierString> quote(token.terminal.line);

      messageName.copy(quote.ident());
   }
   else token.raiseErr("Invalid operand (%d)");
}

ref_t ECodesAssembler :: compileRMessageArg(TokenInfo& token, _Module* binary)
{
   IdentifierString message;
   ref_t signRef = 0;
   compileMessage(token, message, signRef, binary);
   if (signRef)
      token.raiseErr("Strong-typed message is not supported (%d)\n");

   return binary->mapReference(message) | mskMessage;
}

ref_t ECodesAssembler :: compileMessageArg(TokenInfo& token, _Module* binary)
{
   if (token.terminal.state == dfaInteger || constants.exist(token.value)) {
      int m = 0;
      if (token.getInteger(m, constants)) {
         return m;
      }
      else token.raiseErr("Invalid number (%d)\n");
   }
   else if (token.check("message")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      IdentifierString message;
      ref_t signRef = 0;
      compileMessage(token, message, signRef, binary);

      int paramCount = message[0] - '0';
      int flags = 0;

      //IdentifierString subject(token.value);
      //subject.copy(message.c_str() + 1);

      //if (subject.ident().startsWith("params#")) {
      //   subject.cut(0, 7);
      //   flags |= VARIADIC_MESSAGE;
      //}

      //if (subject.compare(INVOKE_MESSAGE)) {
      //   flags |= SPECIAL_MESSAGE;
      //}

      ref_t subj = /*binary->mapAction(subject, signRef, false)*/0;

      return encodeMessage(subj, paramCount, flags);
   }
   else if (token.check("messagename")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      IdentifierString subject;
      compileMessageName(token, subject);
      ref_t subj = binary->mapAction(subject, 0, false);

      return subj;
   }
   else token.raiseErr("Invalid operand (%d)");

   return 0; // dummy returning value 
}

ref_t ECodesAssembler :: compileRArg(TokenInfo& token, _Module* binary)
{
   ident_t word = token.read();

   if (token.terminal.state == dfaFullIdentifier) {
      return binary->mapReference(token.value) | mskSymbolRelRef;
   }
   else if (word.compare("0")) {
      return 0;
   }
   else if (word.compare("const")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      if (word.compare("%")) {
         token.read();

         return compileRMessageArg(token, binary);
      }
      else if (token.terminal.state == dfaInteger || token.terminal.state == dfaHexInteger) {
         int n = 0;
         token.getInteger(n, constants);

         String<char, 16> tmp;
         tmp.appendHex(n);

         return binary->mapConstant(tmp.c_str()) | mskInt32Ref;
      }
      else return binary->mapReference(token.value) | mskConstantRef;
   }
   else if (word.compare("constarray")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      if (token.terminal.state == dfaQuote) {
         QuoteTemplate<IdentifierString> quote(token.terminal.line);

         return binary->mapReference(quote.ident()) | mskConstArray;
      }
      else return binary->mapReference(token.value) | mskConstArray;
   }
   else if (word.compare("rdata")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      IdentifierString s(token.terminal.line + 1, token.terminal.length - 2);

      return binary->mapReference(s.c_str()) | mskNativeRDataRef;
   }
   else if (word.compare("class")) {
      token.read(":", "Invalid operand (%d)");
      token.read();
      return binary->mapReference(token.value) | mskVMTRef;
   }
   else if (word.compare("intern")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      return binary->mapReference(token.value) | mskInternalRef;
   }
   else if (word.compare("entry")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      IdentifierString s(token.terminal.line + 1, token.terminal.length - 2);

      return binary->mapReference(s.c_str()) | mskEntryRef;
   }
   else if (word.compare("api")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      ReferenceNs functionName(NATIVE_MODULE, CORE_MODULE);
      functionName.combine(token.value);

      return binary->mapReference(functionName) | mskNativeCodeRef;
   }
   else throw AssemblerException("Invalid operand (%d)\n", token.terminal.row);
}

void ECodesAssembler :: compileRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   size_t reference = compileRArg(token, binary);

   writeCommand(ByteCommand(code, reference), writer);
}

void ECodesAssembler :: compileRRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   size_t reference1 = compileRArg(token, binary);
   size_t reference2 = compileRArg(token, binary);

   writeCommand(ByteCommand(code, reference1, reference2), writer);
}

void ECodesAssembler::compileRMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   size_t reference1 = compileRArg(token, binary);

   token.read();
   size_t reference2 = compileMessageArg(token, binary);

   writeCommand(ByteCommand(code, reference1 & ~mskAnyRef, reference2), writer);
}

void ECodesAssembler :: compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int n = token.readInteger(constants);

   writeCommand(ByteCommand(code, n), writer);
}

void ECodesAssembler :: compileMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   token.read();

   int arg = compileMessageArg(token, binary);

   writeCommand(ByteCommand(code, arg), writer);
}

void ECodesAssembler :: compileNNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int n1 = token.readSignedInteger(constants);
	int n2 = token.readInteger(constants);

   writeCommand(ByteCommand(code, n1, n2), writer);
}

void ECodesAssembler :: compileCreateCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   ref_t reference = compileRArg(token, binary);
	int n = token.readInteger(constants);

   writeCommand(ByteCommand(code, reference, n), writer);
}

void ECodesAssembler :: compileExtCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   ident_t word = token.read();
   if (word.compare("extern")) {
      token.read(":", "Invalid operand (%d)");
      token.read();
      if (token.check("'dlls'", 6)) {
         ReferenceNs function(DLL_NAMESPACE, token.value + 6);

	      token.read(".", "dot expected (%d)\n");
	      function.append(".");
	      function.append(token.read());

         size_t reference = binary->mapReference(function) | mskImportRef;

         writeCommand(ByteCommand(code, reference), writer);

         return;
      }
      else {
         ReferenceNs function(DLL_NAMESPACE, RTDLL_FORWARD);
         function.append(".");
         function.append(token.value);

         size_t reference = binary->mapReference(function) | mskImportRef;

         writeCommand(ByteCommand(code, reference), writer);

         return;
      }
   }
   else if (word.compare("api")) {
      token.read(":", "Invalid operand (%d)");
      token.read();

      ReferenceNs functionName(NATIVE_MODULE, CORE_MODULE);
      functionName.combine(token.value);

      size_t reference = binary->mapReference(functionName) | mskNativeCodeRef;

      writeCommand(ByteCommand(code, reference), writer);

      return;
   }
   throw AssemblerException("Invalid operand (%d)\n", token.terminal.row);
}

void ECodesAssembler :: compileNJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info)
{
   writer.writeByte(code);

   int label = 0;

   token.read();

   if (info.labels.exist(token.value)) {
      label = info.labels.get(token.value) - writer.Position() - 8;
   }
   else {
      info.fwdJumps.add(token.value, writer.Position() + 4);
   }

   int n = token.readInteger(constants);

   writer.writeDWord(n);

   writer.writeDWord(label);
}

void ECodesAssembler :: compileRJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary)
{
   writer.writeByte(code);

   int label = 0;

   token.read();

   if (info.labels.exist(token.value)) {
      label = info.labels.get(token.value) - writer.Position() - 8;
   }
   else {
      info.fwdJumps.add(token.value, writer.Position() + 4);
   }
   size_t reference = compileRArg(token, binary);

   writer.writeDWord(reference);
   writer.writeDWord(label);
}

void ECodesAssembler :: compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary)
{
   writer.writeByte(code);

   int label = 0;

   token.read();

   if (info.labels.exist(token.value)) {
      label = info.labels.get(token.value) - writer.Position() - 8;
   }
   else {
      info.fwdJumps.add(token.value, 4 + writer.Position());
   }

   token.read();

   int message = compileMessageArg(token, binary);

   writer.writeDWord(message);
   writer.writeDWord(label);
}

void ECodesAssembler :: compileJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info)
{
   writer.writeByte(code);

   int label = 0;

   token.read();

   if (info.labels.exist(token.value)) {
      label = info.labels.get(token.value) - writer.Position() - 4;
   }
   else {
      info.fwdJumps.add(token.value, writer.Position());
   }

   writer.writeDWord(label);
}

void ECodesAssembler :: compileCommand(TokenInfo& token, MemoryWriter& writer, LabelInfo& info, _Module* binary)
{
   bool recognized = true;
   ByteCode opcode = ByteCodeCompiler::code(token.value);
   if (opcode != bcNone) {
      switch (opcode)
      {
         //case bcCallR:
         case bcACopyR:
         //case bcBCopyR:
         case bcPushR:
            compileRCommand(opcode, token, writer, binary);
            break;
         case bcCallExtR:
            compileExtCommand(opcode, token, writer, binary);
            break;
         //case bcACallVI:
         //case bcAJumpVI:
         //case bcALoadSI:
         //case bcBLoadSI:
         //case bcBLoadFI:
         //case bcACopyS:
         //case bcACopyF:
         //case bcSCopyF:
         //case bcBCopyS:
         //case bcBCopyF:
         //case bcALoadAI:
         //case bcALoadFI:
         //case bcPushAI:
         //case bcOpen:
         //case bcAddN:
         //case bcMulN:
         //case bcDLoadFI:
         //case bcDLoadSI:
         //case bcDSaveFI:
         //case bcDSaveSI:
         //case bcRestore:
         //case bcReserve:
         //case bcALoadBI:
         //case bcASaveSI:
         //case bcASaveFI:
         //case bcNSaveI:
         //case bcNLoadI:
         //case bcESwapSI:
         //case bcBSwapSI:
         //case bcAXSaveBI:
         //case bcELoadFI:
         //case bcELoadSI:
         //case bcESaveSI:
         //case bcESaveFI:
         //case bcShiftLN:
         //case bcShiftRN:
         //case bcEAddN:
         //case bcDSwapSI:
         //case bcAJumpI:
         //case bcACallI:
         //case bcNReadI:
         //case bcNWriteI:
         //case bcPushF:
         //case bcEOrN:
         //   compileICommand(opcode, token, writer);
         //   break;
         //case bcQuitN:
         case bcPopI:
         //case bcDCopy:
         //case bcECopy:
         //case bcAndN:
         //case bcOrN:
         //case bcPushN:
            compileNCommand(opcode, token, writer);
            break;
         //case bcSetVerb:
         //case bcCopyM:
         //   compileMCommand(opcode, token, writer, binary);
         //   break;
         //case bcIfB:
         //case bcElseB:
         //case bcIf:
         //case bcElse:
         //case bcLess:
         //case bcNotLess:
         //case bcNext:
         //case bcJump:
         //case bcHook:
         //case bcAddress:
         //   compileJump(opcode, token, writer, info);
         //   break;
         //case bcIfM:
         //case bcElseM:
         //   compileMccJump(opcode, token, writer, info, binary);
         //   break;
         //case bcIfN:
         //case bcElseN:
         //case bcLessN:
         //case bcNotLessN:
         //case bcGreaterN:
         //case bcNotGreaterN:
         //   compileNJump(opcode, token, writer, info);
         //   break;
         //case bcIfR:
         //case bcElseR:
         //   compileRJump(opcode, token, writer, info, binary);
         //   break;
         //case bcNewN:
         //   compileCreateCommand(opcode, token, writer, binary);
         //   break;
         //case bcSelectR:
         //   compileRRCommand(opcode, token, writer, binary);
         //   break;
         //case bcSaveFI:
         //case bcAddFI:
         //   compileNNCommand(opcode, token, writer);
         //   break;
         //case bcXIndexRM:
         //case bcXCallRM:
         //   compileRMCommand(opcode, token, writer, binary);
         //   break;
         default:
            writeCommand(ByteCommand(opcode), writer);
            break;
      }
   }
   else if (token.value[0] == '#') {
	   token.read();
	   token.read();
	   recognized = true;
   }
   else recognized = false;

   if (!recognized) {
      info.labels.add(token.value, writer.Position());

      fixJump(token.value, writer, info);

      info.declareLabel(token.value);

      writeCommand(ByteCommand(bcNop), writer);

      token.read(":", "':' expected (%d)\n");
   }
   token.read();
}

void ECodesAssembler :: compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned, int mask)
{
   LabelInfo info;
   
   token.read();

   IdentifierString method;
   method.copy(token.value);

   token.read();

   if (token.check(".")) {
      token.read();

      IdentifierString message;
      ref_t signRef = 0;
      compileMessage(token, message, signRef, binary);
      if (signRef)
         token.raiseErr("Explicitly defined a strong-typed message is not supported (%d)\n");

      method.append('.');
      method.append(message);

      token.read();
   }

   IdentifierString refName("'", method);
   ref_t reference = binary->mapReference(refName) | mask;

	if (binary->mapSection(reference, true)!=NULL) {
		throw AssemblerException("Procedure already exists (%d)\n", token.terminal.row);
	}

   _Memory* code = binary->mapSection(reference, false);
	MemoryWriter writer(code);
   writer.writeDWord(0);

	while (!token.check("end")) {
      compileCommand(token, writer, info, binary);
	}

	info.checkAllUsedLabels("Used label ( %s ) not declared in procedure %s\n", method);

   (*code)[0] = writer.Position() - 4;

}

void ECodesAssembler :: compile(TextReader* source, path_t outputPath)
{
   FileName moduleName(outputPath);
   moduleName.appendExtension(outputPath);

   ReferenceNs name;
   name.pathToName(moduleName.c_str());

   Module       binary(name);

   //// load predefine messages
   //if (verbs.Count() == 0) {
   //   ByteCodeCompiler::loadVerbs(verbs);
   //}
   //for (MessageMap::Iterator it = verbs.start(); !it.Eof(); it++) {
   //   binary.mapPredefinedAction(it.key(), *it);
   //}

   SourceReader reader(4, source);

   TokenInfo    token(&reader);

   token.read();
	do {
		if (token.check("define")) {
         IdentifierString name(token.read());
			size_t value = token.readInteger(constants);

         if (name[0]=='\'')
            constants.erase(name);

			if (!constants.add(name, value, true))
				token.raiseErr("Constant already exists (%d)\n");

			token.read();
		}
		else if (token.check("procedure")) {
         compileProcedure(token, &binary, true, true, mskCodeRef);

         token.read();
		}
      else if (token.check("symbol")) {
         compileProcedure(token, &binary, true, true, mskSymbolRef);

         token.read();
      }
      else if (token.value[0] == '#') {
			token.read();
			token.read();
			token.read();
		}
		else token.raiseErr("Invalid statement (%d)\n");

	} while (!token.Eof());

   FileWriter writer(outputPath, feRaw, false);
   binary.save(writer);
}
