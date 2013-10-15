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
   if (token.check(_T("accloadacci"))) {
      compileICommand(bcAccLoadAccI, token, writer);
   }
   else if (token.check(_T("acccopyn"))) {
      compileICommand(bcAccCopyN, token, writer);
   }
   else if (token.check(_T("accgetfi"))) {
      compileICommand(bcAccGetFI, token, writer);
   }
   else if (token.check(_T("pushfi"))) {
      compileICommand(bcPushFI, token, writer);
   }
   else if (token.check(_T("accloadsi"))) {
      compileICommand(bcAccLoadSI, token, writer);
   }
   else if (token.check(_T("accsavesi"))) {
      compileICommand(bcAccSaveSI, token, writer);
   }
   else if (token.check(_T("accsavedstsi"))) {
      compileICommand(bcAccSaveDstSI, token, writer);
   }
   else if (token.check(_T("bsredirect"))) {
      writeCommand(ByteCommand(bcBSRedirect), writer);
   }  
   else if (token.check(_T("callacc"))) {
      compileICommand(bcCallAcc, token, writer);
   }
   else if (token.check(_T("close"))) {
      writeCommand(ByteCommand(bcClose), writer);
   }
   else if (token.check(_T("else"))) {
      compileJump(bcElse, token, writer, info);
   }
   else if (token.check(_T("init"))) {
      writeCommand(ByteCommand(bcInit), writer);
   }
   else if (token.check(_T("incfi"))) {
      compileICommand(bcIncFI, token, writer);
   }
   else if (token.check(_T("jump"))) {
      compileJump(bcJump, token, writer, info);
   }
   else if (token.check(_T("jumpaccn"))) {
      compileNCommand(bcJumpAccN, token, writer);
   }
   else if (token.check(_T("mccaddm"))) {
      compileNCommand(bcMccAddM, token, writer);
   }
   else if (token.check(_T("mcccopyacci"))) {
      compileICommand(bcMccCopyAccI, token, writer);
   }
   else if (token.check(_T("mccaddacci"))) {
      compileICommand(bcMccAddAccI, token, writer);
   }
   else if (token.check(_T("mcccopyfi"))) {
      compileICommand(bcMccCopyFI, token, writer);
   }
   else if (token.check(_T("mcccopym"))) {
      compileNCommand(bcMccCopyM, token, writer);
   }
   else if (token.check(_T("mcccopysubj"))) {
      writeCommand(ByteCommand(bcMccCopySubj), writer);
   }
   else if (token.check(_T("mccelse"))) {
      compileMccJump(bcMccElse, token, writer, info);
   }
   else if (token.check(_T("mccelseacci"))) {
      compileNJump(bcMccElseAccI, token, writer, info);
   }
   else if (token.check(_T("mccelseacc"))) {
      compileJump(bcMccElseAcc, token, writer, info);
   }
   else if (token.check(_T("open"))) {
      compileNCommand(bcOpen, token, writer);
   }
   else if (token.check(_T("popacc"))) {
      writeCommand(ByteCommand(bcPopAcc), writer);
   }
   else if (token.check(_T("popself"))) {
      writeCommand(ByteCommand(bcPopSelf), writer);
   }
   else if (token.check(_T("pushacc"))) {
      writeCommand(ByteCommand(bcPushAcc), writer);
   }
   else if (token.check(_T("pushacci"))) {
      compileICommand(bcPushAccI, token, writer);
   }
   else if (token.check(_T("pushmcc"))) {
      writeCommand(ByteCommand(bcPushMcc), writer);
   }
   else if (token.check(_T("mcccopyprmfi"))) {
      compileICommand(bcMccCopyPrmFI, token, writer);
   }
   else if (token.check(_T("pushi"))) {
      compileNCommand(bcPushI, token, writer);
   }
   else if (token.check(_T("quitmcc"))) {
      writeCommand(ByteCommand(bcQuitMcc), writer);
   }
   else if (token.check(_T("quitn"))) {
      compileNCommand(bcQuitN, token, writer);
   }
   else if (token.check(_T("thenflag"))) {
      compileNJump(bcThenFlag, token, writer, info);
   }
   else if (token.check(_T("throw"))) {
      writeCommand(ByteCommand(bcThrow), writer);
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
