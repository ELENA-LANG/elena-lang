//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "ecassembler.h"
#include "module.h"

using namespace _ELENA_;

// --- ECodesAssembler ---

int ECodesAssembler :: mapVerb(const wchar16_t* literal)
{
   if (verbs.Count() == 0) {
      ByteCodeCompiler::loadVerbs(verbs);
   }

   return verbs.get(literal);
}

void ECodesAssembler :: fixJump(const wchar16_t* label, MemoryWriter& writer, LabelInfo& info)
{
   _Memory* code = writer.Memory();

   Map<const wchar16_t*, int>::Iterator it = info.fwdJumps.start();
   while (!it.Eof()) {
      if (StringHelper::compare(it.key(), label)) {
         (*code)[*it] = writer.Position();
      }
      it++;
   }
}

void ECodesAssembler :: writeCommand(ByteCommand command, MemoryWriter& writer)
{
   writer.writeByte(command.code);
   if (command.code >= MAX_SINGLE_ECODE) {
      writer.writeDWord(command.argument);
   }
   if (command.code >= MAX_DOUBLE_ECODE) {
      writer.writeDWord(command.additional);
   }
}

void ECodesAssembler :: compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int offset = token.readSignedInteger(constants);

   writeCommand(ByteCommand(code, offset), writer);
}

ref_t ECodesAssembler :: compileRArg(TokenInfo& token, _Module* binary)
{
   const wchar16_t* word = token.read();

   if (token.terminal.state == dfaFullIdentifier) {
      return binary->mapReference(token.value) | mskSymbolRelRef;
   }
   else if (ConstantIdentifier::compare(word, "0")) {
      return 0;
   }
   else if (ConstantIdentifier::compare(word, "const")) {
      token.read(_T(":"), _T("Invalid operand"));
      token.read();
      return binary->mapReference(token.value) | mskConstantRef;
   }
   else if (ConstantIdentifier::compare(word, "class")) {
      token.read(_T(":"), _T("Invalid operand"));
      token.read();
      return binary->mapReference(token.value) | mskVMTRef;
   }
   else if (ConstantIdentifier::compare(word, "api")) {
      token.read(_T(":"), _T("Invalid operand"));
      token.read();

      ReferenceNs functionName(PACKAGE_MODULE, token.value);
      return binary->mapReference(functionName) | mskNativeCodeRef;
   }
   else throw AssemblerException(_T("Invalid operand (%d)\n"), token.terminal.row);
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

void ECodesAssembler :: compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int n = token.readInteger(constants);

   writeCommand(ByteCommand(code, n), writer);
}

void ECodesAssembler :: compileMCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{   
	const wchar16_t* word = token.read();
   if (token.terminal.state == dfaInteger || constants.exist(word)) {
      int m = 0;
      if(token.getInteger(m, constants)) {
         writeCommand(ByteCommand(code, m), writer);
      }
      else token.raiseErr(_T("Invalid number (%d)\n"));
   }
   else if (ConstantIdentifier::compare(word, "subject")) {
      token.read(_T(":"), _T("Invalid operand"));
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
      else token.raiseErr(_T("Invalid operand"));

      token.read(_T("]"), _T("Invalid operand"));

      ref_t subj = binary->mapSubject(subject, false);

      writeCommand(ByteCommand(code, encodeMessage(subj, verbId, paramCount)), writer);
   }
   else throw AssemblerException(_T("Invalid operand (%d)\n"), token.terminal.row);
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
   const wchar16_t* word = token.read();
   if (ConstantIdentifier::compare(word, "extern")) {
      token.read(_T(":"), _T("Invalid operand"));
      token.read();
      if (StringHelper::compare(token.value, _T("'dlls'"), 6)) {
         ReferenceNs function(DLL_NAMESPACE, token.value + 6);

	      token.read(_T("."), _T("dot expected (%d)\n"));
	      function.append(_T("."));
	      function.append(token.read());

         size_t reference = binary->mapReference(function) | mskImportRef;

         writeCommand(ByteCommand(code, reference), writer);

         return;
      }
   }
   throw AssemblerException(_T("Invalid operand (%d)\n"), token.terminal.row);
}

void ECodesAssembler :: compileNJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info)
{
   writer.writeByte(code);

   int label = 0;

   token.read();

   if (info.labels.exist(token.value)) {
      label = info.labels.get(token.value);
   }
   else {
      info.fwdJumps.add(token.value, 4 + writer.Position());
   }

   int n = token.readInteger(constants);

   writer.writeDWord(n);
   writer.writeDWord(label);
}

void ECodesAssembler :: compileMccJump(ByteCode code, TokenInfo& token, MemoryWriter& writer, LabelInfo& info)
{
   writer.writeByte(code);

   int label = 0;

   token.read();

   if (info.labels.exist(token.value)) {
      label = info.labels.get(token.value);
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
      label = info.labels.get(token.value);
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
         case bcALoadAI:
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
         case bcMessage:
         case bcELoadSI:
         case bcESaveSI:
            compileICommand(opcode, token, writer);
            break;
         case bcQuitN:
         case bcPopI:
         case bcDCopy:
         case bcECopy:
         case bcSetVerb:
         case bcSetSubj:
         case bcAndN:
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
         case bcNewN:
            compileCreateCommand(opcode, token, writer, binary);
            break;
         case bcSelectR:
            compileRRCommand(opcode, token, writer, binary);
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

      token.read(_T(":"), _T("':' expected (%d)\n"));
   }
   token.read();
}

void ECodesAssembler :: compileProcedure(TokenInfo& token, _Module* binary, bool inlineMode, bool aligned)
{
   LabelInfo info;

   token.read();
   ReferenceNs refName(PACKAGE_MODULE, token.value);

   ref_t reference = binary->mapReference(refName) | mskCodeRef;

	if (binary->mapSection(reference, true)!=NULL) {
		throw AssemblerException(_T("Procedure already exists (%d)\n"), token.terminal.row);
	}

   _Memory* code = binary->mapSection(reference, false);
	MemoryWriter writer(code);

   token.read();
      
	while (!token.check(_T("end"))) {
      compileCommand(token, writer, info, binary);
	}
}

void ECodesAssembler :: compile(TextReader* source, const wchar_t* outputPath)
{
   Module       binary(_T("$binary"));
   SourceReader reader(4, source);

   TokenInfo    token(&reader);

   token.read();
	do {
		if (token.check(_T("define"))) {
         IdentifierString name(token.read());
			size_t value = token.readInteger(constants);

         if (name[0]=='\'')
            constants.erase(name);

			if (!constants.add(name, value, true))
				token.raiseErr(_T("Constant already exists (%d)\n"));

			token.read();
		}
		else if (token.check(_T("procedure"))) {
         compileProcedure(token, &binary, true, true);

         token.read();
		}
		else token.raiseErr(_T("Invalid statement (%d)\n"));

	} while (!token.Eof());

   FileWriter writer(outputPath, feRaw, false);
   binary.save(writer);
}
