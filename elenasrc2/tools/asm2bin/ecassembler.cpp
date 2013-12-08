//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Assembler Compiler
//
//		This file contains the implementation of ELENA x86Compiler
//		classes.
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "ecassembler.h"
#include "module.h"

using namespace _ELENA_;

// --- ECodesAssembler ---

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
   if (command.code >= 0x20) {
      writer.writeDWord(command.argument);
   }
   if (command.code >= 0xE0) {
      writer.writeDWord(command.additional);
   }
}

void ECodesAssembler :: compileICommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int offset = token.readSignedInteger(constants);

   writeCommand(ByteCommand(code, offset), writer);
}

void ECodesAssembler :: compileRCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer, _Module* binary)
{
   const wchar16_t* word = token.read();
   if (token.terminal.state == dfaFullIdentifier) {
      size_t reference = binary->mapReference(token.value) | mskSymbolRelRef;

      writeCommand(ByteCommand(code, reference), writer);
   }
   else if (ConstantIdentifier::compare(word, "const")) {
      token.read(_T(":"), _T("Invalid operand"));
      token.read();
      size_t reference = binary->mapReference(token.value) | mskConstantRef;

      writeCommand(ByteCommand(code, reference), writer);
   }
   else if (ConstantIdentifier::compare(word, "class")) {
      token.read(_T(":"), _T("Invalid operand"));
      token.read();
      size_t reference = binary->mapReference(token.value) | mskVMTRef;

      writeCommand(ByteCommand(code, reference), writer);
   }
   else throw AssemblerException(_T("Invalid operand (%d)\n"), token.terminal.row);
}

void ECodesAssembler :: compileNCommand(ByteCode code, TokenInfo& token, MemoryWriter& writer)
{
	int n = token.readInteger(constants);

   writeCommand(ByteCommand(code, n), writer);
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

   switch (opcode)
   {
      case bcNop:
      case bcBreakpoint:
      case bcPushB:
      case bcPop:
      //case bcWriteAcc:
      case bcPushM:
      case bcMCopyVerb:
      case bcThrow:
      case bcMCopySubj:
      case bcPushA:
      case bcPopA:
      case bcACopyB:
      case bcBCopyA:
      case bcPopM:
      case bcBSRedirect:
      case bcClose:
      case bcPopB:
      case bcMQuit:
      //case bcJumpAcc:
      //case bcQuit:
      case bcGet:
      case bcSet:
      //case bcUnhook:
      //case bcExclude:
      //case bcInclude:
      //case bcMccReverse:
      case bcALoadD:
      case bcDDec:
      case bcGetLen:
      case bcDInc:
         writeCommand(ByteCommand(opcode), writer);
         break;
      //case bcPushR:
      //case bcCallExtR:
      //case bcEvalR:
      case bcCallR:
      //case bcAccLoadR:
      //case bcAccSaveR:
      case bcACopyR:
      //case bcAccFillR:
         compileRCommand(opcode, token, writer, binary);
         break;
      //case bcIncFI:
      case bcACopyF:
      case bcACallVI:
      case bcALoadSI:
      case bcASaveSI:
      case bcPushFI:
      //case bcACopyN:
      case bcALoadAI:
      case bcMLoadAI:
      case bcMLoadSI:
      case bcMLoadFI:
      case bcMAddAI:
      case bcPushAI:
      case bcMSaveParams:
      //case bcReserve:
      //case bcPushSelfI:
      case bcPushSI:
      //case bcPushFPI:
      //case bcXPushFPI:
      //case bcPushSPI:
      //case bcPopSelfI:
      //case bcPopFI:
      //case bcXPopAccI:
      //case bcPopSI:
      //case bcPopAccI:
      //case bcIncSI:
      //case bcAccLoadFI:
      //case bcAccSaveSelfI:
      //case bcAccSaveFI:
      //case bcSwapSI:
      //case bcAccSwapSI:
      //case bcXAccSaveFI:
      //case bcXAccCopyFPI:
      case bcACopyS:
      //case bcAccCopyFPI:
      //case bcRethrow:
      //case bcRestore:
      //case bcAccGetSI:
      //case bcAccLoadSelfI:
      case bcDAddAI:
      case bcDSubAI:
      case bcDAddSI:
      case bcDSubSI:
      case bcDLoadAI:
      case bcDSaveAI:
      case bcDLoadSI:
      case bcDSaveSI:
      case bcDLoadFI:
      case bcDSaveFI:
         compileICommand(opcode, token, writer);
         break;
      case bcOpen:
      case bcMAdd:
      case bcAJumpVI:
      case bcMCopy:
      //case bcXMccCopyM:
      case bcQuitN:
      case bcPushN:
      case bcPopI:
      case bcDCopy:
      //case bcAccAddN:
         compileNCommand(opcode, token, writer);
         break;
      //case bcHook:
      case bcJump:
      //case bcElse:
      //case bcThen:
      //case bcMccElseAcc:
      //case bcMccThenAcc:
      case bcDElse:
      case bcDThen:
      case bcWSTest:
      case bcBSTest:
      case bcTest:
         compileJump(opcode, token, writer, info);
         break;
      ////case bcAccCreate:
      ////   break;
      ////case bcElseR:
      ////case bcThenR:
      ////   break;
      case bcMElse:
      case bcMThen:
         compileMccJump(opcode, token, writer, info);
         break;
      //case bcMccElseAccI:
      case bcTestFlag:
      case bcAElseSI:
      case bcAThenSI:
      case bcElseFlag:
      case bcMElseAI:
      case bcMThenAI:
         compileNJump(opcode, token, writer, info);
         break;
      ////case bcCreate:
      ////   break;
      ////case bcCreateN:
      ////   break;
      ////   break;
      ////case bcIAccFillR:
      ////   break;
      ////case bcAccCreateN:
      ////   break;
      ////case bcAccBoxN:
      ////   break;
      ////case bcCallSI:
      ////   break;
      ////case bcRCallN:
      ////   break;
      ////case bcRCallM:
      ////   break;
   default:
      recognized = false;
      break;
   }

   // check if it is function
   if (!recognized) {
      ByteCode code = bcNone;
      const wchar16_t* func = token.value;
      if (token.value[0]=='n') {
         code = bcNFunc;
         func++;
      }
      else if (token.value[0]=='l') {
         code = bcLFunc;
         func++;
      }
      else if (token.value[0]=='r') {
         code = bcRFunc;
         func++;
      }
      else if (token.value[0]=='w' && token.value[1]=='s') {
         code = bcWSFunc;
         func+=2;
      }
      else if (token.value[0]=='b' && token.value[1]=='s') {
         code = bcBSFunc;
         func+=2;
      }
      if (code != bcNone) {
         FunctionCode function = ByteCodeCompiler::codeFunction(func);
         if (function != fnUnknown) {
            writeCommand(ByteCommand(code, function), writer);
            recognized = true;
         }
      }
   }

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
