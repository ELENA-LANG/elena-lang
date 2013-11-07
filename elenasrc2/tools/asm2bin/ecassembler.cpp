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

void ECodesAssembler :: compileCommand(TokenInfo& token, MemoryWriter& writer, LabelInfo& info)
{
   bool recognized = true;
   ByteCode opcode = ByteCodeCompiler::code(token.value);

   switch (opcode)
   {
      case bcNop:
      case bcBreakpoint:
      case bcPushSelf:
      case bcPop:
      case bcWriteAcc:
      case bcPushMcc:
      case bcMccCopyVerb:
      case bcThrow:
      case bcMccCopySubj:
      case bcPushAcc:
      case bcPopAcc:
      case bcAccCopySelf:
      case bcPopMcc:
      case bcBSRedirect:
      case bcInit:
      case bcClose:
      case bcPopSelf:
      case bcQuitMcc:
      case bcJumpAcc:
      case bcQuit:
      case bcGet:
      case bcSet:
      case bcUnhook:
      case bcExclude:
      case bcInclude:
         writeCommand(ByteCommand(opcode), writer);
         break;
      //case bcPushR:
      //case bcCallExtR:
      //case bcEvalR:
      //case bcCallR:
      //case bcAccLoadR:
      //case bcAccSaveR:
      //case bcAccCopyR:
      //case bcAccFillR:
      //   break;
      case bcIncFI:
      case bcCopyFPI:
      case bcCallAcc:
      case bcAccLoadSI:
      case bcAccSaveSI:
      case bcPushFI:
      case bcAccCopyN:
      case bcAccLoadAccI:
      case bcAccGetFI:
      case bcMccCopyAccI:
      case bcMccCopySI:
      case bcMccCopyFI:
      case bcMccAddAccI:
      case bcPushAccI:
      case bcMccCopyPrmFI:
      case bcReserve:
      case bcPushSelfI:
      case bcPushSI:
      case bcPushFPI:
      case bcXPushFPI:
      case bcPushSPI:
      case bcPopSelfI:
      case bcPopFI:
      case bcXPopAccI:
      case bcPopSI:
      case bcPopAccI:
      case bcIncSI:
      case bcAccLoadFI:
      case bcAccSaveSelfI:
      case bcAccSaveFI:
      case bcSwapSI:
      case bcAccSwapSI:
      case bcXAccSaveFI:
      case bcXAccCopyFPI:
      case bcAccCopyFPI:
      case bcRethrow:
      case bcRestore:
      case bcAccGetSI:
      case bcAccLoadSelfI:
         compileICommand(opcode, token, writer);
         break;
      case bcOpen:
      case bcMccAddM:
      case bcJumpAccN:
      case bcMccCopyM:
      case bcPushI:
      case bcQuitN:
      case bcPushN:
      case bcPopN:
      case bcAccAddN:
      case bcNWrite:
      case bcGetLen:
         compileNCommand(opcode, token, writer);
         break;
      case bcHook:
      case bcJump:
      case bcElse:
      case bcThen:
      case bcMccElseAcc:
      case bcMccThenAcc:
         compileJump(opcode, token, writer, info);
         break;
      //case bcAccCreate:
      //   break;
      //case bcElseR:
      //case bcThenR:
      //   break;
      case bcMccElse:
      case bcMccThen:
         compileMccJump(opcode, token, writer, info);
         break;
      case bcMccElseAccI:
      case bcThenFlag:
      case bcElseSI:
      case bcThenSI:
      case bcElseFlag:
      case bcMccThenAccI:
         compileNJump(opcode, token, writer, info);
         break;
      //case bcCreate:
      //   break;
      //case bcCreateN:
      //   break;
      //case bcIAccCopyR:
      //   break;
      //case bcIAccFillR:
      //   break;
      //case bcAccCreateN:
      //   break;
      //case bcAccBoxN:
      //   break;
      //case bcCallSI:
      //   break;
      //case bcRCallN:
      //   break;
      //case bcRCallM:
      //   break;
   default:
      recognized = false;
      break;
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
      compileCommand(token, writer, info);
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
