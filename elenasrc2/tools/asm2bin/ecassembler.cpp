//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "ecassembler.h"
#include "module.h"

using namespace _ELENA_;

// --- ECodesAssembler ---

int ECodesAssembler::mapVerb(ident_t literal)
{
   if (verbs.Count() == 0) {
      ByteCodeCompiler::loadVerbs(verbs);
   }

   return verbs.get(literal);
}

void ECodesAssembler :: fixJump(ident_t label, MemoryWriter& writer, LabelInfo& info)
{
   _Memory* code = writer.Memory();

   Map<ident_t, int>::Iterator it = info.fwdJumps.start();
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), label)) {
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

void ECodesAssembler :: readMessage(TokenInfo& token, int& verbId, IdentifierString& subject, int& paramCount)
{
   verbId = mapVerb(token.value);
   if (verbId == 0) {
      verbId = EVAL_MESSAGE_ID;
   }

   token.read();
   while (token.value[0] == '&') {
      subject.append(token.value);

      token.read();
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
   else token.raiseErr("Invalid operand");

   token.read("]", "Invalid operand");
}

ref_t ECodesAssembler :: compileRMessageArg(TokenInfo& token, _Module* binary)
{
   IdentifierString message;
   IdentifierString subject;

   int paramCount = 0;
   int verbId = 0;
   
   readMessage(token, verbId, subject, paramCount);

   // reserve place for param counter
   message.append('0');

   // if it is not a verb - by default it is EVAL message
   if (verbId == 0) {
      message.append('#');
      message.append(EVAL_MESSAGE_ID + 0x20);
      message.append('&');
      message.append(verbId);
   }
   else {
      message.append('#');
      message.append(verbId + 0x20);
   }

   message.append(subject);

   message[0] = message[0] + paramCount;

   return binary->mapReference(message) | mskMessage;
}

ref_t ECodesAssembler::compileMessageArg(TokenInfo& token, _Module* binary)
{
   IdentifierString subject;
   int paramCount = 0;
   int verbId = 0;

   readMessage(token, verbId, subject, paramCount);

   if (subject.Length() > 0) {
      return encodeMessage(binary->mapSubject(subject + 1, false), verbId, paramCount);
   }
   else return encodeMessage(0, verbId, paramCount);
}

ref_t ECodesAssembler :: compileRArg(TokenInfo& token, _Module* binary)
{
   ident_t word = token.read();

   if (token.terminal.state == dfaFullIdentifier) {
      return binary->mapReference(token.value) | mskSymbolRelRef;
   }
   else if (StringHelper::compare(word, "0")) {
      return 0;
   }
   else if (StringHelper::compare(word, "const")) {
      token.read(":", "Invalid operand");
      token.read();

      if (StringHelper::compare(word, "%")) {
         token.read();

         return compileRMessageArg(token, binary);
      }
      else return binary->mapReference(token.value) | mskConstantRef;
   }
   else if (StringHelper::compare(word, "class")) {
      token.read(":", "Invalid operand");
      token.read();
      return binary->mapReference(token.value) | mskVMTRef;
   }
   else if (StringHelper::compare(word, "api")) {
      token.read(":", "Invalid operand");
      token.read();

      ReferenceNs functionName(NATIVE_MODULE, token.value);
      return binary->mapReference(functionName) | mskNativeCodeRef;
   }
   else if (StringHelper::compare(word, "intern")) {
      token.read(":", "Invalid operand");
      token.read();

      return binary->mapReference(token.value) | mskInternalRef;
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

   token.read("%", "Invalid operand");
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
   ident_t word = token.read();
   if (token.terminal.state == dfaInteger || constants.exist(word)) {
      int m = 0;
      if(token.getInteger(m, constants)) {
         writeCommand(ByteCommand(code, m), writer);
      }
      else token.raiseErr("Invalid number (%d)\n");
   }
   else if (StringHelper::compare(word, "subject")) {
      token.read(":", "Invalid operand");
      token.read();

      int paramCount = 0; // NOTE: paramCount might be not equal to stackCount (the actual stack size) in the case if variables are used for virtual methods
      int stackCount = 0;
      int verbId = mapVerb(token.value);
      if (verbId == 0) {
         verbId = EVAL_MESSAGE_ID;
      }

      IdentifierString subject;
      token.read();
      bool first = true;
      while(token.value[0] == '&') {
         if (first) {
            first = false;
         }
         else subject.append(token.value);

         token.read();
         subject.append(token.value);
         token.read();
      }
      if (token.value[0] == '[') {
         paramCount = token.readInteger(constants);
      }
      else token.raiseErr("Invalid operand");

      token.read("]", "Invalid operand");

      ref_t subj = binary->mapSubject(subject, false);

      writeCommand(ByteCommand(code, encodeMessage(subj, verbId, paramCount)), writer);
   }
   else throw AssemblerException("Invalid operand (%d)\n", token.terminal.row);
}

void ECodesAssembler :: compileNNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int n1 = token.readInteger(constants);
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
   if (StringHelper::compare(word, "extern")) {
      token.read(":", "Invalid operand");
      token.read();
      if (StringHelper::compare(token.value, "'dlls'", 6)) {
         ReferenceNs function(DLL_NAMESPACE, token.value + 6);

	      token.read(".", "dot expected (%d)\n");
	      function.append(".");
	      function.append(token.read());

         size_t reference = binary->mapReference(function) | mskImportRef;

         writeCommand(ByteCommand(code, reference), writer);

         return;
      }
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

void ECodesAssembler :: compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info)
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

   int message = token.readInteger(constants);

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
         case bcCallR:
         case bcACopyR:
         case bcPushR:
            compileRCommand(opcode, token, writer, binary);
            break;
         case bcCallExtR:
            compileExtCommand(opcode, token, writer, binary);
            break;
         case bcACallVI:
         case bcAJumpVI:
         case bcALoadSI:
         case bcBLoadSI:
         case bcBLoadFI:
         case bcACopyS:
         case bcACopyF:
         case bcBCopyS:
         case bcBCopyF:
         case bcALoadAI:
         case bcALoadFI:
         case bcPushAI:
         case bcOpen:
         case bcAddN:
         case bcDLoadFI:
         case bcDLoadSI:
         case bcDSaveFI:
         case bcDSaveSI:
         case bcRestore:
         case bcReserve:
         case bcALoadBI:
         case bcASaveSI:
         case bcNSaveI:
         case bcNLoadI:
         case bcESwapSI:
         case bcBSwapSI:
         case bcAXSaveBI:
            //case bcMessage:
         case bcELoadFI:
         case bcELoadSI:
         case bcESaveSI:
         case bcESaveFI:
         case bcShiftN:
         case bcEAddN:
         case bcDSwapSI:
            compileICommand(opcode, token, writer);
            break;
         case bcQuitN:
         case bcPopI:
         case bcDCopy:
         case bcECopy:
         case bcSetVerb:
         case bcSetSubj:
         case bcAndN:
         case bcOrN:
         case bcPushN:
            compileNCommand(opcode, token, writer);
            break;
         case bcCopyM:
            compileMCommand(opcode, token, writer, binary);
            break;
         case bcIfB:
         case bcElseB:
         case bcIf:
         case bcElse:
         case bcLess:
         case bcNotLess:
         case bcNext:
         case bcJump:
         case bcHook:
         case bcAddress:
            compileJump(opcode, token, writer, info);
            break;
         case bcIfM:
         case bcElseM:
            compileMccJump(opcode, token, writer, info);
            break;
         case bcIfN:
         case bcElseN:
         case bcLessN:
            compileNJump(opcode, token, writer, info);
            break;
         case bcIfR:
         case bcElseR:
            compileRJump(opcode, token, writer, info, binary);
            break;
         case bcNewN:
            compileCreateCommand(opcode, token, writer, binary);
            break;
         case bcSelectR:
            compileRRCommand(opcode, token, writer, binary);
            break;
         case bcXIndexRM:
            compileRMCommand(opcode, token, writer, binary);
            break;
         default:
            writeCommand(ByteCommand(opcode), writer);
            break;
      }
   }
   else recognized = false;

   if (!recognized) {
      info.labels.add(token.value, writer.Position());

      fixJump(token.value, writer, info);

      writeCommand(ByteCommand(bcNop), writer);

      token.read(":", "':' expected (%d)\n");
   }
   token.read();
}

void ECodesAssembler :: compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned)
{
   LabelInfo info;

   token.read();
   ReferenceNs refName(binary->Name(), token.value);

   ref_t reference = binary->mapReference(refName) | mskCodeRef;

	if (binary->mapSection(reference, true)!=NULL) {
		throw AssemblerException("Procedure already exists (%d)\n", token.terminal.row);
	}

   _Memory* code = binary->mapSection(reference, false);
	MemoryWriter writer(code);
   writer.writeDWord(0);

   token.read();

	while (!token.check("end")) {
      compileCommand(token, writer, info, binary);
	}

   (*code)[0] = writer.Position() - 4;

}

void ECodesAssembler :: compile(TextReader* source, path_t outputPath)
{
   FileName moduleName(outputPath);

   ReferenceNs name("system", IdentifierString(moduleName));

   Module       binary(name);
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
         compileProcedure(token, &binary, true, true);

         token.read();
		}
		else token.raiseErr("Invalid statement (%d)\n");

	} while (!token.Eof());

   FileWriter writer(outputPath, feRaw, false);
   binary.save(writer);
}
