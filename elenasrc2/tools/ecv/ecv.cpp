//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                              (C)2012-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// --------------------------------------------------------------------------
#include "elena.h"
#include "libman.h"
#include "module.h"
#include "config.h"
#include "bytecode.h"

#define PROJECT_SECTION _T("project")
#define ROOTPATH_OPTION _T("libpath")

#define MAX_LINE        256

using namespace _ELENA_;

// === Variables ===
MessageMap         _verbs;
ConstantIdentifier _integer(INT_CLASS);
//ConstantIdentifier _long(LONG_FORWARD);
//ConstantIdentifier _real(REAL_FORWARD);
//ConstantIdentifier _literal(LITERAL_FORWARD);

TextFileWriter* _writer;

// === Helper functions ===

// --- trim ---

inline const wchar16_t* trim(const wchar16_t* s)
{
   while (s[0]==' ')
      s++;

   return s;
}

// --- commands ---

void print(const wchar16_t* line)
{
   wprintf(line);
   if (_writer)
      _writer->writeText(line);
}

void printLine(const wchar16_t* line1, const wchar16_t* line2)
{
   wprintf(line1);
   wprintf(line2);
   wprintf(_T("\n"));

   if (_writer) {
      _writer->writeText(line1);
      _writer->writeText(line2);
      _writer->writeNewLine();
   }
}

void printLoadError(LoadResult result)
{
   switch(result)
   {
   case lrNotFound:
      print(_T("Module not found\n"));
      break;
   case lrWrongStructure:
      print(_T("Invalid module\n"));
      break;
   case lrWrongVersion:
      print(_T("Module out of date\n"));
      break;
   }
}

void printHelp()
{
   wprintf(_T("-q                       - quit\n"));
   wprintf(_T("-h                       - help\n"));
   wprintf(_T("-m<class>.<method name>  - view method byte codes\n"));
   wprintf(_T("-s<symbol>               - view symbol byte codes\n"));
   wprintf(_T("-o<path>                 - save the output\n"));
   wprintf(_T("-c<path>                 - save the output\n"));
   wprintf(_T("-l                       - list all classes\n"));
   wprintf(_T("-lm<class>               - list all class methods\n"));
}

_Memory* findClassMetaData(_Module* module, const wchar16_t* referenceName)
{
   ref_t reference = module->mapReference(referenceName, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskMetaRDataRef, true);
}

_Memory* findClassVMT(_Module* module, const wchar16_t* referenceName)
{
   ref_t reference = module->mapReference(referenceName, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskVMTRef, true);
}

_Memory* findClassCode(_Module* module, const wchar16_t* referenceName)
{
   ref_t reference = module->mapReference(referenceName, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskClassRef, true);
}

_Memory* findSymbolCode(_Module* module, const wchar16_t* referenceName)
{
   ref_t reference = module->mapReference(referenceName, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskSymbolRef, true);
}

bool loadClassInfo(_Module* module, const wchar_t* className, ClassInfo& info)
{
   // find class meta data
   ReferenceNs reference(module->Name(), className);
   _Memory* data = findClassMetaData(module, reference);
   if (data == NULL) {
      wprintf(_T("Class %s not found\n"), (const wchar_t*)reference);

      return false;
   }

   MemoryReader reader(data);
   info.load(&reader);

   return true;
}

ref_t resolveMessage(_Module* module, const wchar16_t* method)
{
   int paramCount = 0;

   int subjIndex = StringHelper::find(method, '&', -1);
   int paramIndex = StringHelper::find(method, '[', -1);

   IdentifierString verbName;
   IdentifierString subjectName;

   if (subjIndex != -1) {
      verbName.copy(method, subjIndex);
      if (paramIndex != -1) {
         subjectName.copy(method + subjIndex + 1, paramIndex - subjIndex - 1);
      }
      else subjectName.copy(method + subjIndex + 1);
   }
   else if (paramIndex != -1) {
      verbName.copy(method, paramIndex);
   }
   else verbName.copy(method);

   if (paramIndex != -1) {
      IdentifierString countStr(method + paramIndex + 1, getlength(method) - paramIndex - 2);
      paramCount = StringHelper::strToInt(countStr);
   }

   ref_t verb = _verbs.get(verbName);
   if (verb == 0) {
      if (StringHelper::compare(verbName, _T("resend"))) {
         verb = SEND_MESSAGE_ID;
      }
      else if (StringHelper::compare(verbName, _T("dispatch"))) {
         verb = DISPATCH_MESSAGE_ID;
      }
      else {
         wprintf(_T("Unknown verb %s\n"), (const wchar16_t*)verbName);

         return 0;
      }
   }

   ref_t subject = emptystr(subjectName) ? 0 : module->mapSubject(subjectName, true);
   if (subject == 0 && !emptystr(subjectName)) {
      wprintf(_T("Unknown subject %s\n"), (const wchar16_t*)subjectName);

      return 0;
   }

   return encodeMessage(subject, verb, paramCount);
}

const wchar16_t* decode(unsigned char code)
{
   switch ((ByteCode)code) {
      case bcNop:
         return _T("nop");
      case bcBreakpoint:
         return _T("breakpoint");
      case bcPushSelf:
         return _T("pushself");
      case bcPop:
         return _T("pop");
      case bcWriteAcc:
         return _T("writeacc");
      case bcPushMcc:
         return _T("pushmcc");
      case bcThrow:
         return _T("throw");
      case bcMccCopySubj:
         return _T("mcccopysubj");
      case bcPushAcc:
         return _T("pushacc");
      case bcPopAcc:
         return _T("popacc");
      case bcAccCopySelf:
         return _T("acccopyself");
      case bcPopMcc:
         return _T("popMcc");
      case bcBSRedirect:
         return _T("bsredirect");
      //case bcMccCopyVerb:
      //   return _T("mcccopyverb");
      //case bcMccCopyAcc:
      //   return _T("mcccopyacc");
      //case bcAccCopyMcc:
      //   return _T("acccopymcc");
      //case bcReSend:
      //   return _T("resend");
      case bcOpen:
         return _T("open");
      case bcInit:
         return _T("init");
      case bcClose:
         return _T("close");
      case bcPopSelf:
         return _T("popself");
      case bcJumpAcc:
         return _T("jumpacc");
      //case bcClose:
      //   return _T("close");
      case bcQuit:
         return _T("quit");
      case bcGet:
         return _T("get");
      case bcSet:
         return _T("set");
      case bcQuitMcc:
         return _T("quitmcc");
      case bcRestore:
         return _T("restore");
      case bcUnhook:
         return _T("unhook");
      case bcExclude:
         return _T("exclude");
      case bcInclude:
         return _T("include");
      //case bcAccAddSelf:
      //   return _T("accaddself");
      //case bcRedirect:
      //   return _T("redirect");
      case bcReserve:
         return _T("reserve");
      case bcPushN:
         return _T("pushn");
      case bcPushR:
         return _T("pushr");
      case bcPushSelfI:
         return _T("pushselfi");
      case bcPushAccI:
         return _T("pushacci");
      case bcPushI:
         return _T("pushi");
      case bcPushFI:
         return _T("pushfi");
      case bcMccCopyPrmFI:
         return _T("mcccopyprmfi");
      case bcPushSI:
         return _T("pushsi");
      case bcPushFPI:
         return _T("pushfpi");
      case bcXPushFPI:
         return _T("x_pushfpi");
      case bcPushSPI:
         return _T("pushspi");
      case bcPopN:
         return _T("popn");
      case bcPopSelfI:
         return _T("popselfi");
      case bcPopFI:
         return _T("popfi");
      case bcXPopAccI:
         return _T("x_popacci");
      case bcPopSI:
         return _T("popsi");
      case bcPopAccI:
         return _T("popacci");
      //case bcAccTryN:
      //   return _T("acctryn");
      //case bcAccTryR:
      //   return _T("acctryr");
      case bcQuitN:
         return _T("quitn");
      case bcCallExtR:
         return _T("callextr");
      case bcEvalR:
         return _T("evalr");
      case bcCallAcc:
         return _T("callacc");
      case bcCallR:
         return _T("callr");
      //case bcSendVMTR:
      //   return _T("sendvmtr");
      case bcMccCopyAccI:
         return _T("mcccopyacci");
      case bcMccCopySI:
         return _T("mcccopysi");
      case bcMccCopyFI:
         return _T("mcccopyfi");
      case bcMccAddAccI:
         return _T("mccaddacci");
      case bcMccCopyM:
         return _T("mcccopym");
      case bcMccAddM:
         return _T("mccaddm");
      case bcIncSI:
         return _T("incsi");
      case bcIncFI:
         return _T("incfi");
      //case bcAccInc:
      //   return _T("accinc");
      case bcAccLoadR:
         return _T("accloadr");
      case bcAccLoadFI:
         return _T("accloadfi");
      case bcAccLoadSI:
         return _T("accloadsi");
      //case bcAccTestFlagN:
      //   return _T("acctestflagn");
      case bcAccSaveSI:
         return _T("accsavesi");
      case bcAccSaveFI:
         return _T("accsavefi");
      case bcAccSaveSelfI:
         return _T("accsaveselfi");
      case bcAccSaveR:
         return _T("accsaver");
      case bcAccSaveDstSI:
         return _T("accsavedstsi");
      case bcSwapSI:
         return _T("swapsi");
      case bcAccSwapSI:
         return _T("swapsi");
      case bcXAccSaveFI:
         return _T("x_accsavefi");
      case bcCopyFPI:
         return _T("copyfpi");
      //case bcAccCopySPI:
      //return _T("acccopyspi");
      case bcAccCopyR:
         return _T("acccopyr");
      case bcAccCopyN:
         return _T("acccopyn");
      case bcAccLoadAccI:
         return _T("accloadacci");
      case bcRethrow:
         return _T("rethrow");
      //case bcAccCopyM:
      //   return _T("acccopym");
      case bcXAccCopyFPI:
         return _T("x_acccopyfpi");
      case bcAccCopyFPI:
         return _T("acccopyfpi");
      case bcAccAddN:
         return _T("accaddn");
      //case bcTryLock:
      //   return _T("trylock");
      //case bcFreeLock:
      //   return _T("freelock");
      //case bcSPTryLock:
      //   return _T("sptrylock");
      //case bcAccFreeLock:
      //   return _T("accfreelock");
      case bcJump:
         return _T("jump");
      case bcJumpAccN:
         return _T("jumpaccn");
      case bcHook:
         return _T("hook");
      //case bcJumpR:
      //   return _T("jumpr");
      case bcElse:
         return _T("else");
      case bcThen:
         return _T("then");
      case bcMccElseAcc:
         return _T("mccelseacc");
      case bcMccThenAcc:
         return _T("mccthenacc");
      case bcNWrite:
         return _T("nwrite");
      case bcGetLen:
         return _T("getlen");
      case bcAccGetSI:
         return _T("accgetsi");
      case bcAccGetFI:
         return _T("accgetfi");
      case bcAccCreate:
         return _T("acccreate");
      //case bcAccMergeR:
      //   return _T("accmerger");
      case bcElseR:
         return _T("elser");
      case bcThenR:
         return _T("thenr");
      case bcMccElse:
         return _T("mccelse");
      case bcMccThen:
         return _T("mccthen");
      //case bcMccElseSI:
      //   return _T("mccelsesi");
      //case bcMccThenSI:
      //   return _T("mccthensi");
      //case bcMccVerbElseSI:
      //   return _T("mccverbelsesi");
      //case bcMccVerbThenSI:
      //   return _T("mccverbthensi");
      //case bcElseN:
      //   return _T("elsen");
      //case bcThenN:
      //   return _T("thenn");
      case bcElseSI:
         return _T("elsesi");
      case bcThenSI:
         return _T("thensi");
      case bcMccElseAccI:
         return _T("mccelseacci");
      case bcMccThenAccI:
         return _T("mccthenacci");
      case bcElseFlag:
         return _T("elseflag");
      case bcThenFlag:
         return _T("thenflag");
      case bcAccLoadSelfI:
         return _T("accloadselfi");
      case bcCreate:
         return _T("create");
      case bcCreateN:
         return _T("createn");
      case bcAccCreateN:
         return _T("acccreaten");
      case bcAccBoxN:
         return _T("accboxn");
      case bcIAccCopyR:
         return _T("iacccopyr");
      case bcIAccFillR:
         return _T("iaccfillr");
      case bcRCallM:
         return _T("rcallm");
      case bcRCallN:
         return _T("rcalln");
      case bcCallSI:
         return _T("callsi");
      //case bcIAccCopyN:
      //   return _T("iacccopyn");
      default:
         return _T("unknown");
   }
}

inline void appendHex32(IdentifierString& command, unsigned int hex)
{
   unsigned int len = hex / 0x10 + 1;
   while (len < 8) {
      command.append(_T('0'));
      len++;
   }

   command.appendHex(hex);
}

int getLabelIndex(int label, List<int>& labels)
{
   int index = 0;
   List<int>::Iterator it = labels.start();
   while (!it.Eof()) {
      if (*it == label)
         return index;

      index++;
      it++;
   }

   return -1;
}

void printLabel(IdentifierString& command, int labelPosition, List<int>& labels)
{
   int index = getLabelIndex(labelPosition, labels);
   if (index == -1) {
      index = labels.Count();

      labels.add(labelPosition);
   }

   command.append(_T("Lab"));
   if (index < 10) {
      command.append('0');
   }
   command.appendInt(index);
}

void printReference(IdentifierString& command, _Module* module, size_t reference)
{
   bool literalConstant = false;
   const wchar16_t* referenceName = NULL;
   int mask = reference & mskAnyRef;
   if (mask == mskInt32Ref) {
      referenceName = _integer;
      literalConstant = true;
   }
   //else if (mask == mskInt64Ref) {
   //   referenceName = _long;
   //   literalConstant = true;
   //}
   //else if (mask == mskLiteralRef) {
   //   referenceName = _literal;
   //   literalConstant = true;
   //}
   //else if (mask == mskRealRef) {
   //   referenceName = _real;
   //   literalConstant = true;
   //}
   else referenceName = module->resolveReference(reference & ~mskAnyRef);

   if (emptystr(referenceName)) {
      command.append(_T("unknown"));
   }
   else {
      command.append(referenceName);
      if (literalConstant) {
         command.append(_T("("));
         command.append(module->resolveConstant(reference & ~mskAnyRef));
         command.append(_T(")"));
      }
   }
}

void printMessage(IdentifierString& command, _Module* module, size_t reference)
{
   size_t signRef = 0;
   int verb = 0;
   int paramCount = 0;
   decodeMessage(reference, signRef, verb, paramCount);

   if (verb == SEND_MESSAGE_ID) {
      command.append(_T("resend"));
   }
   else if (verb == DISPATCH_MESSAGE_ID) {
      command.append(_T("dispatch"));
   }
   else {
      const wchar16_t* verbName = retrieveKey(_verbs.start(), verb, (const wchar16_t*)NULL);
      command.append(verbName);
   }

   if (signRef != 0) {
      const wchar16_t* subjectName = module->resolveSubject(signRef);
      command.append(_T('&'));
      command.append(subjectName);
   }

   if (paramCount > 0) {
      command.append('[');
      command.appendInt(paramCount);
      command.append(']');
   }
}

void printCommand(_Module* module, MemoryReader& codeReader, int indent, List<int>& labels)
{
   // read bytecode + arguments
   int position = codeReader.Position();
   unsigned char code = codeReader.getByte();

   const wchar16_t* opcode = decode(code);

   IdentifierString command;
   while (indent > 0) {
      command.append(_T(" "));

      indent--;
   }
   if (code < 0x10)
      command.append(_T('0'));

   command.appendHex((int)code);
   command.append(_T(' '));

   int argument = 0;
   int argument2 = 0;
   if (code >= 0xE0) {
      argument = codeReader.getDWord();
      argument2 = codeReader.getDWord();

      appendHex32(command, argument);
      command.append(_T(' '));

      appendHex32(command, argument2);
      command.append(_T(' '));
   }
   else if (code >= 0x20) {
      argument = codeReader.getDWord();

      appendHex32(command, argument);
      command.append(_T(' '));
   }

   int tabbing = code == bcNop ? 24 : 31;
   while (getlength(command) < tabbing) {
      command.append(_T(' '));
   }

   switch(code)
   {
      case bcPushFPI:
      case bcXPushFPI:
      case bcCopyFPI:
      case bcXAccCopyFPI:
      case bcAccCopyFPI:
         command.append(opcode);
         command.append(_T(" fp:"));
         command.appendHex(argument);
         break;
      case bcPushSPI:
         command.append(opcode);
         command.append(_T(" sp:"));
         command.appendHex(argument);
         break;
      case bcElse:
      case bcThen:
      case bcMccElseAcc:
      case bcMccThenAcc:
      case bcJump:
      //case bcElseLocal:
         command.append(opcode);
         command.append(_T(' '));
         printLabel(command, position + argument + 5, labels);
         break;
      case bcMccElse:
      case bcMccThen:
         command.append(opcode);
         command.append(_T(' '));
         printMessage(command, module, argument);
         command.append(_T(' '));
         printLabel(command, position + argument2 + 9, labels);
         break;
      case bcElseR:
      case bcThenR:
         command.append(opcode);
         command.append(_T(' '));
         printReference(command, module, argument);
         command.append(_T(' '));
         printLabel(command, position + argument2 + 9, labels);
         break;
      case bcThenFlag:
      case bcElseFlag:
      //case bcElseN:
      //case bcThenN:
         command.append(opcode);
         command.append(_T(' '));
         command.appendHex(argument);
         command.append(_T(' '));
         printLabel(command, position + argument2 + 9, labels);
         break;
      case bcMccElseAccI:
      case bcMccThenAccI:
         command.append(opcode);
         command.append(_T(' acc['));
         command.appendHex(argument);
         command.append(_T('] '));
         printLabel(command, position + argument2 + 9, labels);
         break;
      case bcElseSI:
      case bcThenSI:
      //case bcMccElseSI:
      //case bcMccThenSI:
      //case bcMccVerbElseSI:
      //case bcMccVerbThenSI:
         command.append(opcode);
         command.append(_T(" sp["));
         command.appendInt(argument);
         command.append(_T("] "));
         printLabel(command, position + argument2 + 9, labels);
         break;
      case bcNop:
         printLabel(command, position + argument, labels);
         command.append(_T(':'));
         command.append(_T(' '));
         command.append(opcode);
         break;
      case bcPushR:
      case bcAccLoadR:
      case bcCallExtR:
      case bcEvalR:
      case bcCallR:
      //case bcSendVMTR:
      case bcAccSaveR:
      case bcAccCopyR:
      //case bcAccTryR:
      //case bcAccMergeR:
      //case bcJumpR:
         command.append(opcode);
         command.append(_T(' '));
         printReference(command, module, argument);
         break;
      case bcReserve:
      case bcPushN:
      case bcPopN:
      case bcOpen:
      case bcQuitN:
      case bcAccCreate:
      //case bcAccTestFlagN:
      //case bcAccCopyN:
      case bcAccAddN:
      case bcGetLen:
      //case bcAccTryN:
      //case bcSelfShiftI:
      //case bcAccShiftI:
      //case bcTryLock:
      //case bcFreeLock:
      //case bcSPTryLock:
      //case bcAccFreeLock:
      case bcJumpAccN:
         command.append(opcode);
         command.append(_T(' '));
         command.appendHex(argument);
         break;
      //case bcAccInc:
      case bcPushI:
         command.append(opcode);
         command.append(_T(' '));
         command.appendHex(argument);
         break;
      case bcPushSI:
      case bcAccLoadSI:
      case bcAccSaveSI:
      case bcSwapSI:
      case bcPopSI:
      case bcMccCopySI:
      case bcIncSI:
      case bcAccSaveDstSI:
      case bcAccSwapSI:
         command.append(opcode);
         command.append(_T(" sp["));
         command.appendInt(argument);
         command.append(_T(']'));
         break;
      case bcAccGetSI:
         command.append(opcode);
         command.append(_T(" acc[sp["));
         command.appendInt(argument);
         command.append(_T("]]"));
         break;
      case bcAccGetFI:
         command.append(opcode);
         command.append(_T(" acc[fp["));
         command.appendInt(argument);
         command.append(_T("]]"));
         break;
      case bcPushFI:
      case bcAccLoadFI:
      case bcPopFI:
      case bcIncFI:
      case bcAccSaveFI:
      case bcMccCopyFI:
      case bcMccCopyPrmFI:
      case bcXAccSaveFI:
         command.append(opcode);
         command.append(_T(" fp["));
         command.appendInt(argument);
         command.append(_T(']'));
         break;
      case bcCallAcc:
         command.append(opcode);
         command.append(_T(" acc::vmt["));
         command.appendInt(argument);
         command.append(_T(']'));
         break;
      //case bcPushAccI:
      //case bcPopAccI:
      //case bcPop2AccI:
      case bcAccLoadAccI:
      case bcMccCopyAccI:
      case bcMccAddAccI:
         command.append(opcode);
         command.append(_T(" acc["));
         command.appendInt(argument);
         command.append(_T(']'));
         break;
      case bcPushSelfI:
      case bcPopSelfI:
      case bcAccSaveSelfI:
      //case bcAccLoadSelfI:
         command.append(opcode);
         command.append(_T(" self["));
         command.appendInt(argument);
         command.append(_T(']'));
         break;
      //case bcIAccCopyN:
      //   command.append(opcode);
      //   command.append(_T(" acc["));
      //   command.appendInt(argument);
      //   command.append(_T("], "));
      //   command.appendHex(argument2);
      //   break;
      case bcIAccCopyR:
         command.append(opcode);
         command.append(_T(" acc["));
         command.appendInt(argument);
         command.append(_T("], "));
         printReference(command, module, argument2);
         break;
      case bcIAccFillR:
         command.append(opcode);
         command.append(_T(' '));
         command.appendInt(argument);
         command.append(_T(", "));
         printReference(command, module, argument2);
         break;
      case bcCreate:
      case bcCreateN:
      case bcAccCreateN:
      case bcAccBoxN:
         command.append(opcode);
         command.append(_T(' '));
         command.appendInt(argument);
         command.append(_T(", "));
         printReference(command, module, argument2);
         break;
      case bcRCallM:
         command.append(opcode);
         command.append(_T(' '));
         printReference(command, module, argument);
         command.append(_T(", "));
         printMessage(command, module, argument2);
         break;
      case bcRCallN:
         command.append(opcode);
         command.append(_T(' '));
         printReference(command, module, argument);
         command.append(_T(", "));
         command.appendInt(argument2);
         break;
      //case bcAccCopyM:
      case bcMccCopyM:
      case bcMccAddM:
         command.append(opcode);
         command.append(_T(' '));
         printMessage(command, module, argument);
         break;
      case bcCallSI:
         command.append(opcode);
         command.append(_T(' '));
         command.appendInt(argument);
         command.append(_T(", "));
         command.appendInt(argument2);
         break;
      default:
         command.append(opcode);
         break;
   }

   print(command);
}

void printByteCodes(_Module* module, _Memory* code, ref_t address, int indent, int pageSize)
{
   MemoryReader codeReader(code, address);

   size_t codeSize = codeReader.getDWord();
   size_t endPos = codeReader.Position() + codeSize;

   int row = 0;
   List<int> labels;
   while(codeReader.Position() < endPos) {
      printCommand(module, codeReader, indent, labels);
      print(_T("\n"));

      row++;
      if (row == pageSize) {
         wprintf(_T("Press any key to continue..."));
         fgetchar();
         wprintf(_T("\n"));

         row = 0;
      }
   }
}

void printMethod(_Module* module, const wchar_t* methodReference, int pageSize)
{
   methodReference = trim(methodReference);

   int separator = StringHelper::find(methodReference, '.');
   if (separator == -1) {
      wprintf(_T("Invalid command"));

      return;
   }

   IdentifierString className(methodReference, separator);

   const wchar16_t* methodName = methodReference + separator + 1;

   // resolve method
   ref_t message = resolveMessage(module, methodName);
   if (message == 0)
      return;

   // find class VMT
   ReferenceNs reference(module->Name(), className);
   _Memory* vmt = findClassVMT(module, reference);
   _Memory* code = findClassCode(module, reference);
   if (vmt == NULL || code == NULL) {
      wprintf(_T("Class %s not found\n"), (const wchar_t*)reference);

      return;
   }

   // find method entry
   MemoryReader vmtReader(vmt);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));
   int vmtSize = vmtReader.getDWord();

   VMTEntry        entry;

   // read VMT while the entry not found
   size -= sizeof(ClassHeader) + 4;
   bool found = false;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(VMTEntry));

      if (entry.message == message) {
         found = true;

         printLine(_T("@method "), methodReference);
         printByteCodes(module, code, entry.address, 4, pageSize);
         print(_T("@end\n"));

         break;
      }

      size -= sizeof(VMTEntry);
   }
   if (!found) {
      wprintf(_T("Method %s not found"), methodName);
   }
}

//void printConstructor(_Module* module, const wchar_t* className, int pageSize)
//{
//   className = trim(className);
//
//   // find class VMT
//   ReferenceNs reference(module->Name(), className);
//   _Memory* vmt = findClassVMT(module, reference);
//   _Memory* code = findClassCode(module, reference);
//   if (vmt == NULL || code == NULL) {
//      wprintf(_T("Class %s not found\n"), (const wchar_t*)reference);
//
//      return;
//   }
//
//   ClassInfo info;
//   loadClassInfo(module, className, info);
//
//   if (info.constructor != 0) {
//      print(_T("@constructor\n"));
//      printByteCodes(module, code, 0, 4, pageSize);
//      print(_T("@end\n"));
//   }
//   else {
//      print(_T("Constructor is not available\n"));
//   }
//}

void printSymbol(_Module* module, const wchar16_t* symbolReference, int pageSize)
{
   // find class VMT
   ReferenceNs reference(module->Name(), symbolReference);
   _Memory* code = findSymbolCode(module, reference);
   if (code == NULL) {
      wprintf(_T("Symbol %s not found\n"), (const wchar16_t*)reference);

      return;
   }

   printLine(_T("@symbol "), symbolReference);
   printByteCodes(module, code, 0, 4, pageSize);
   print(_T("@end\n"));
}

void listClassMethods(_Module* module, const wchar_t* className, int pageSize)
{
   className = trim(className);

   // find class VMT
   ReferenceNs reference(module->Name(), className);
   _Memory* vmt = findClassVMT(module, reference);
   if (vmt == NULL) {
      wprintf(_T("Class %s not found\n"), (const wchar_t*)reference);

      return;
   }

   // list methods
   MemoryReader vmtReader(vmt);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));
   int vmtSize = vmtReader.getDWord();

   VMTEntry        entry;

   size -= sizeof(ClassHeader) + 4;
   IdentifierString temp;
   int row = 0;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(VMTEntry));

      // print the method name
      temp.copy(className);
      temp.append('.');
      printMessage(temp, module, entry.message);
      printLine(_T("@method "), temp);

      row++;
      if (row == pageSize) {
         wprintf(_T("Press any key to continue..."));
         fgetchar();
         wprintf(_T("\n"));

         row = 0;
      }

      size -= sizeof(VMTEntry);
   }
}

//void listClassRoles(_Module* module, const wchar_t* className, int pageSize)
//{
//   className = trim(className);
//
//   ClassInfo info;
//   if (!loadClassInfo(module, className, info)) {
//      return;
//   }
//
//   if (info.roles.Count() > 0) {
//      ClassInfo::FieldMap::Iterator it = info.roles.start();
//      while (!it.Eof()) {
//         wprintf(_T("Role %s:%s\n"), (const wchar_t*)it.key(), module->resolveReference(*it & ~mskAnyRef));
//
//         it++;
//      }
//   }
//   else wprintf(_T("No roles are found\n"));
//}

void listClasses(_Module* module, int pageSize)
{
   const wchar16_t* moduleName = module->Name();

   int row = 0;
   ReferenceMap::Iterator it = ((Module*)module)->References();
   while (!it.Eof()) {
      const wchar16_t* reference = it.key();
      NamespaceName ns(it.key());
      if (StringHelper::compare(moduleName, ns)) {
         ReferenceName name(it.key());
         if (module->mapSection(*it | mskVMTRef, true)) {
            wprintf(_T("class %s\n"), (const wchar16_t*)name);
         }
         else wprintf(_T("symbol %s\n"), (const wchar16_t*)name);

         row++;
         if (row == pageSize) {
            wprintf(_T("Press any key to continue..."));
            fgetchar();
            wprintf(_T("\n"));

            row = 0;
         }
      }

      it++;
   }
}

void setOutputMode(const wchar16_t* path)
{
   if (_writer)
      freeobj(_writer);

   _writer = new TextFileWriter(path, 0, false);
}

void runSession(_Module* module)
{
   char              buffer[MAX_LINE];
   IdentifierString  line;
   int               pageSize = 30;
   while (true) {
      wprintf(_T("\n>"));

      // !! fgets is used instead of fgetws, because there is strange bug in fgetws implementation
      fgets(buffer, MAX_LINE, stdin);
      line.copy(buffer, strlen(buffer));

      while (!emptystr(line) && line[getlength(line) - 1]=='\r' || line[getlength(line) - 1]=='\n')
         line[getlength(line) - 1] = 0;

      while (!emptystr(line) && line[getlength(line) - 1]==' ')
         line[getlength(line) - 1] = 0;

      // execute command
      if (line[0]=='-') {
         switch(line[1]) {
            case 'q':
               return;
            case 'h':
               printHelp();
               break;
            case 'm':
               printMethod(module, line + 2, pageSize);
               break;
            //case 'c':
            //   printConstructor(module, line + 2, pageSize);
            //   break;
            case 's':
               printSymbol(module, line + 2, pageSize);
               break;
            case 'l':
               if (line[2]=='m') {
                  listClassMethods(module, line + 3, pageSize);
               }
               //else if (line[2]=='r') {
               //   listClassRoles(module, line + 3, pageSize);
               //}
               else listClasses(module, pageSize);
               break;
            case 'o':
               setOutputMode(line + 2);
               break;
            default:
               printHelp();
         }
      }
      else printHelp();
   }
}
// === Main Program ===
int main(int argc, char* argv[])
{
	printf("ELENA command line Elena ByteCode Viewer %d.%d.0 (C)2012-2013 by Alexei Rakov\n\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION);

   if (argc<2) {
      printf("ecv <module name> | ecv -p<module path>");
      return 0;
   }

   // prepare library manager
   const wchar16_t* configPath = _T("elc.cfg");
   const wchar16_t* rootPath = NULL;

   // get viewing module name
	IdentifierString moduleName(argv[1]);

   // load config attributes
   IniConfigFile config;
   if (config.load(configPath, feAnsi)) {
      rootPath = config.getSetting(PROJECT_SECTION, ROOTPATH_OPTION, rootPath);
   }

   LibraryManager loader(rootPath, NULL);
   LoadResult result = lrNotFound;
   _Module* module = NULL;

   // if direct path is provieded
   if (moduleName[0]=='-' && moduleName[1]=='p') {
      moduleName = moduleName + 2;

      Path     path;
      path.copyPath(moduleName);

      FileName name(moduleName);

      loader.setPackage(name, path);
      module = loader.loadModule(name, result, false);
   }
   else module = loader.loadModule(moduleName, result, false);

   if (result != lrSuccessful) {
      printLoadError(result);

      return -1;
   }
   else wprintf(_T("%s module loaded\n"), (const wchar16_t*)moduleName);

   loadVerbs(_verbs);

   runSession(module);

   if (_writer)
      freeobj(_writer);

   return 0;
}
