//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                              (C)2012-2021, by Alexei Rakov
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
#include "compilercommon.h"

#ifdef _WIN32

#include "winapi/consolehelper.h"
#include <windows.h>

#endif // _WIN32

#define PROJECT_SECTION "project"
#define ROOTPATH_OPTION "libpath"

#define MAX_LINE           256
#define REVISION_VERSION   99

using namespace _ELENA_;

// === Variables ===
bool    _ignoreBreakpoints = true;
bool    _showBytecodes = false;
bool    _noPaging = false;
bool    _noComments = true;

TextFileWriter* _writer;

// === Helper functions ===

// --- trim ---

inline ident_t trim(ident_t s)
{
   while (s[0]==' ')
      s+=1;

   return s;
}

// --- commands ---

#ifdef _WIN32
void print(ident_t line)
{
   wprintf(WideString(line));
   if (_writer)
      _writer->writeLiteral(line);
}
#else
void print(ident_t line)
{
   vprintf(line.c_str());
   if (_writer)
      _writer->writeLiteral(line);
}
#endif

void printLine(ident_t line1, ident_t line2)
{
   print(line1);
   print(line2);
   printf("\n");

   if (_writer) {
      _writer->writeNewLine();
   }
}

void printLine(ident_t line1, ident_t line2, ident_t line3, ident_t line4)
{
   print(line1);
   print(line2);
   print(line3);
   print(line4);

   printf("\n");
   if (_writer) {
      _writer->writeNewLine();
   }
}

void printLine()
{
   printf("\n");

   if (_writer) {
      _writer->writeNewLine();
   }
}

void nextRow(int& row, int pageSize)
{
   row++;
   if (row == pageSize - 1 && !_noPaging) {
      print("Press any key to continue...");
#ifdef _WIN32
      _fgetchar();
#else
      getchar();
#endif
      printf("\n");

      row = 0;
   }
}

void printLine(ident_t line1, ident_t line2, ident_t line3, ident_t line4, int& row, int pageSize)
{
   printLine(line1, line2, line3, line4);
   nextRow(row, pageSize);
}

void printLine(ident_t line1, ident_t line2, int& row, int pageSize)
{
   printLine(line1, line2);
   nextRow(row, pageSize);
}

void printLoadError(LoadResult result)
{
   switch(result)
   {
   case lrNotFound:
      print("Module not found\n");
      break;
   case lrWrongStructure:
      print("Invalid module\n");
      break;
   case lrWrongVersion:
      print("Module out of date\n");
      break;
   }
}

void printHelp()
{
   printf("-c                      - hide / show byte codes\n");
   printf("-b                      - hide / show breakpoints\n");
   printf("-p                      - turn off / turn on pagination\n");
   printf("-i                      - turn off / turn on desciptions\n");
   printf("-q                      - quit\n");
   printf("-h                      - help\n");
   printf("<class>.<method name>   - view method byte codes\n");
   printf("<class>                 - list all class methods\n");
   printf("#<symbol>               - view symbol byte codes\n");
   printf("-o<path>                - save the output\n");
   printf("-l                      - list all classes with methods\n");
   printf("?                       - list all classes\n");
}

_Memory* findClassMetaData(_Module* module, ident_t referenceName)
{
   IdentifierString name("'", referenceName);

   ref_t reference = module->mapReference(name, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskMetaRDataRef, true);
}

_Memory* findClassVMT(_Module* module, ident_t referenceName)
{
   IdentifierString name("'", referenceName);

   ref_t reference = module->mapReference(name, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskVMTRef, true);
}

_Memory* findClassCode(_Module* module, ident_t referenceName)
{
   IdentifierString name("'", referenceName);

   ref_t reference = module->mapReference(name, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskClassRef, true);
}

_Memory* findSymbolCode(_Module* module, ident_t referenceName)
{
   IdentifierString name("'", referenceName);

   ref_t reference = module->mapReference(name, true);
   if (reference == 0) {
      return NULL;
   }
   return module->mapSection(reference | mskSymbolRef, true);
}

ref_t resolveMessage(_Module* module, ident_t method, bool extension)
{
   int argCount = 0;
   ref_t actionRef = 0;
   ref_t flags = 0;

   if (extension)
      flags |= FUNCTION_MESSAGE;

   if (method.startsWith("params#")) {
      flags |= VARIADIC_MESSAGE;

      method = method.c_str() + getlength("params#");
   }

   if (method.startsWith("prop#")) {
      flags |= PROPERTY_MESSAGE;

      method = method.c_str() + getlength("prop#");
   }
   if (method.startsWith("#invoke") || method.startsWith("#new")) {
      flags |= FUNCTION_MESSAGE;
   }
   if (method.startsWith("#static&")) {
      flags |= STATIC_MESSAGE;

      method = method.c_str() + getlength("#static&");
   }
   if (method.compare("#init")) {
      flags |= FUNCTION_MESSAGE;
   }
   if (method.startsWith("#cast")) {
      flags |= CONVERSION_MESSAGE;
   }
   if (method.startsWith("#cast&")) {
      flags |= CONVERSION_MESSAGE;

      method = method.c_str() + getlength("#cast&");
   }

   IdentifierString actionName;
   int paramIndex = method.find('[', -1);
   if (paramIndex != -1) {
      actionName.copy(method, paramIndex);

      IdentifierString countStr(method + paramIndex + 1, getlength(method) - paramIndex - 2);
      argCount = countStr.ident().toInt();
   }
   else actionName.copy(method);

   ref_t signature = 0;
   size_t index = actionName.ident().find('<');
   if (index != NOTFOUND_POS) {
      ref_t references[ARG_COUNT];
      size_t end = actionName.ident().find('>');
      size_t len = 0;
      size_t i = index + 1;
      while (i < end) {
         size_t j = actionName.ident().find(i, ',', end);

         IdentifierString temp(actionName.c_str() + i, j-i);
         references[len++] = module->mapReference(temp, true);

         i = j + 1;
      }

      signature = module->mapSignature(references, len, true);

      actionName.truncate(index);
   }

   actionRef = module->mapAction(actionName, signature, true);
   if (actionRef == 0) {
      printLine("Unknown subject ", actionName);

      return 0;
   }

   return encodeMessage(actionRef, argCount, flags);
}

inline void appendHex32(IdentifierString& command, unsigned int hex)
{
   unsigned int n = hex / 0x10;
   int len = 7;
   while (n > 0) {
      n = n / 0x10;

      len--;
   }

   while (len > 0) {
      command.append('0');
      len--;
   }

   command.appendHex(hex);
   command.append('h');
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

   command.append("Lab");
   if (index < 10) {
      command.append('0');
   }
   command.appendInt(index);
}

void parseMessageConstant(IdentifierString& message, ident_t reference)
{
   // message constant: nverb&signature

   int verbId = 0;
   int signatureId = 0;

   // read the param counter
   int count = reference[0] - '0';

   // skip the param counter
   reference+=1;

   int index = reference.find('&');
   //HOTFIX: for generic GET message we have to ignore ampresand
   if (reference[index + 1] == 0)
      index = -1;

   if (index != -1) {
      //HOTFIX: for GET message we have &&, so the second ampersand should be used
      if (reference[index + 1] == 0 || reference[index + 1] == '&')
         index++;

      IdentifierString verb(reference, index);
      ident_t signature = reference + index + 1;

      // if it is a predefined verb
      if (verb[0] == '#') {
         verbId = verb[1] - 0x20;
      }

      message.append(signature);
   }
   else {
      message.append(reference);
   }
}

void printExternReference(IdentifierString& command, _Module* module, size_t reference)
{
   ident_t ref = module->resolveReference(reference & ~mskAnyRef);
   if (ref.startsWith("$dlls'$rt.")) {
      command.append("extern : ");
      command.append(ref + 10);
   }
   else command.append(ref);
}

void printReference(IdentifierString& command, _Module* module, size_t reference)
{
   ident_t postfix;
   bool literalConstant = false;
   bool messageConstant = false;
   bool quote = false;
   int mask = reference & mskAnyRef;
   if (mask == mskInt32Ref) {
      command.append("const : ");
      literalConstant = true;
   }
   else if (mask == mskInt64Ref) {
      command.append("const : ");
      literalConstant = true;
      postfix = "l";
   }
   else if (mask == mskLiteralRef) {
      command.append("const : ");
      quote = literalConstant = true;
   }
   else if (mask == mskWideLiteralRef) {
      command.append("const : ");
      quote = literalConstant = true;
      postfix = "w";
   }
   else if (mask == mskRealRef) {
      command.append("const : ");
      literalConstant = true;
      postfix = "w";
   }
   else if (mask == mskCharRef) {
      command.append("const : ");
      quote = literalConstant = true;
      postfix = "c";
   }
   else if (mask == mskConstantRef) {
      command.append("const : ");
   }
   else if (mask == mskMessage) {
      messageConstant = true;

      command.append("mssgconst : ");
   }
   else if (mask == mskMessageName) {
      command.append("subjconst : ");
   }
   else if (mask == mskVMTRef) {
      command.append("class : ");
   }
   else if (reference == 0) {
      command.append("0");
   }
   else if (reference == -1) {
      command.append("terminal");
   }

   if (reference == 0 || reference == -1) {
   }
   else if (literalConstant) {
      if (quote)
         command.append("\"");

      command.append(module->resolveConstant(reference & ~mskAnyRef));

      if (quote)
         command.append("\"");

      command.append(postfix);
   }
   else if (messageConstant) {
      command.append("\"");

      ident_t mssg = module->resolveReference(reference & ~mskAnyRef);
      command.append(mssg + 1);
      command.append("[");
      command.appendInt(mssg[0] - '0');
      command.append("]");

      command.append("\"");
   }
   else {
      command.append(module->resolveReference(reference & ~mskAnyRef));
   }
}

void printMessage(IdentifierString& command, _Module* module, size_t reference)
{
   ByteCodeCompiler::resolveMessageName(command, module, reference);
}

void printCommand(IdentifierString& command, const char* opcode)
{
   command.append(opcode);
   size_t tabbing = _showBytecodes ? 46 : 15;
   while (getlength(command) < tabbing) {
      command.append(' ');
   }
}

void printFArgument(IdentifierString& command, int argument)
{
   command.appendInt(argument);
}

void printFPArgument(IdentifierString& command, int argument)
{
   command.appendInt(argument);
}

bool printCommand(_Module* module, MemoryReader& codeReader, int indent, List<int>& labels)
{
   // read bytecode + arguments
   int position = codeReader.Position();
   unsigned char code = codeReader.getByte();

   // ignore a breakpoint if required
   if (code == bcBreakpoint && _ignoreBreakpoints)
      return false;

   char opcode[0x30];
   ByteCodeCompiler::decode((ByteCode)code, opcode);

   IdentifierString command;
   while (indent > 0) {
      command.append(" ");

      indent--;
   }

   int argument = 0;
   int argument2 = 0;
   if (code > MAX_DOUBLE_ECODE) {
      argument = codeReader.getDWord();
      argument2 = codeReader.getDWord();
   }
   else if (code > MAX_SINGLE_ECODE) {
      argument = codeReader.getDWord();
   }

   if (_showBytecodes) {
      if (code < 0x10)
         command.append('0');

      command.appendHex((int)code);
      command.append(' ');

      if (code > MAX_DOUBLE_ECODE) {
         appendHex32(command, argument);
         command.append(' ');

         appendHex32(command, argument2);
         command.append(' ');
      }
      else if (code > MAX_SINGLE_ECODE) {
         appendHex32(command, argument);
         command.append(' ');
      }

      size_t tabbing = code == bcNop ? 26 : 33;
      while (getlength(command) < tabbing) {
         command.append(' ');
      }
   }

   switch(code)
   {
      case bcSaveF:
      case bcLoadF:
      case bcMovF:
      case bcNAddF:
      case bcNSubF:
      case bcNMulF:
      case bcNDivF:
      case bcNAndF:
      case bcNOrF:
      case bcNXorF:
      case bcCloneF:
      case bcPushF:
      case bcLAddF:
      case bcLSubF:
      case bcLMulF:
      case bcLDivF:
      case bcLShlF:
      case bcLShrF:
      case bcLAndF:
      case bcLOrF:
      case bcLXorF:
      case bcRAddF:
      case bcRSubF:
      case bcRMulF:
      case bcRDivF:
      case bcRAddNF:
      case bcRSubNF:
      case bcRMulNF:
      case bcRDivNF:
      case bcRIntF:
      case bcAddF:
      case bcSubF:
      case bcXRSaveF:
      case bcXAddF:
         printCommand(command, opcode);
         printFArgument(command, argument);
         break;
      case bcCopyF:
      case bcCopyToF:
      case bcXSaveF:
      case bcXSaveLenF:
         printCommand(command, opcode);
         printFArgument(command, argument);
         command.append(", ");
         command.appendInt(argument2);
         break;
      case bcPushSIP:
      case bcPushSI:
      case bcPeekSI:
      case bcStoreSI:
      case bcSaveSI:
      case bcMovSIP:
         printCommand(command, opcode);
         command.appendInt(argument);
         break;
      case bcSaveFI:
      case bcPushFI:
      case bcEqualFI:
      case bcStoreFI:
      case bcLoadFI:
      case bcPeekFI:
      case bcPushFIP:
      case bcMovFIP:
      case bcMovFIPD:
         printCommand(command, opcode);
         printFPArgument(command, argument);
         break;
      case bcCopyToFI:
         printCommand(command, opcode);
         printFPArgument(command, argument);
         command.append(", ");
         command.appendInt(argument2);
         break;
      case bcJump:
      case bcHook:
      case bcIf:
      case bcIfCount:
      case bcElse:
      case bcIfHeap:
      case bcNotLess:
      case bcNotGreater:
      case bcCheckSI:
      case bcXRedirect:
      case bcXVRedirect:
         printCommand(command, opcode);
         printLabel(command, position + argument + 5, labels);
         break;
      case bcElseR:
      case bcIfR:
         printCommand(command, opcode);
         printReference(command, module, argument);
         command.append(", ");
         printLabel(command, position + argument2 + 9, labels);
         break;
      case bcIfN:
      case bcElseN:
      case bcLessN:
      case bcNotLessN:
      case bcGreaterN:
      case bcNotGreaterN:
         printCommand(command, opcode);
         printLabel(command, position + argument2 + 9, labels);
         command.append(", ");
         command.appendHex(argument);
         command.append('h');
         break;
      case bcNop:
         printLabel(command, position + argument, labels);
         command.append(':');
         command.append("     ");
         command.append(opcode);
         break;
      case bcPushR:
      case bcPeekR:
      case bcCallR:
      case bcStoreR:
      case bcMovR:
      case bcXSetR:
      case bcCreate:
      case bcFillR:
      case bcCoalesceR:
      case bcXCreate:
         printCommand(command, opcode);
         printReference(command, module, argument);
         break;
      case bcCallExtR:
         printCommand(command, opcode);
         printExternReference(command, module, argument);
         command.append(", ");
         command.appendHex(argument2);
         break;
      case bcReserve:
      case bcRestore:
      case bcPushN:
      case bcFreeI:
      case bcAllocI:
      case bcSetFrame:
      case bcQuitN:
      case bcDec:
      case bcInc:
      case bcMul:
      case bcAnd:
      case bcOr:
      case bcLoadI:
      case bcSaveI:
      case bcLoadSI:
      case bcGetI:
      case bcMovN:
      case bcShl:
      case bcShr:
      case bcCopyTo:
      case bcXWrite:
         printCommand(command, opcode);
         command.appendHex(argument);
         command.append('h');
         break;
      case bcJumpVI:
      case bcCallVI:
      case bcJumpI:
      case bcCallI:
         printCommand(command, opcode);
         command.appendInt(argument);
         break;
      case bcPushAI:
         //      case bcALoadAI:
         printCommand(command, opcode);
         command.appendInt(argument);
         break;
      case bcSetI:
      case bcXSetI:
//      case bcAXSaveBI:
//      case bcALoadBI:
         printCommand(command, opcode);
         command.appendInt(argument);
         break;
      case bcNew:
         printCommand(command, opcode);
         printReference(command, module, argument);
         command.append(", ");
         command.appendInt(argument2);
         break;
      case bcCallRM:
      case bcJumpRM:
      case bcVJumpRM:
      case bcVCallRM:
      case bcMTRedirect:
      case bcXMTRedirect:
         printCommand(command, opcode);
         printReference(command, module, argument);
         command.append(" mssgconst : \"");
         printMessage(command, module, argument2);
         command.append("\"");
         break;
      case bcMovM:
         printCommand(command, opcode);
         command.append("mssgconst : \"");
         printMessage(command, module, argument);
         command.append("\"");
         break;
      case bcMovV:
         printCommand(command, opcode);
         command.append("messagename : \"");
         printMessage(command, module, encodeAction(argument));
         command.append("\"");
         break;
      case bcSelect:
      case bcXSelectR:
         printCommand(command, opcode);
         printReference(command, module, argument);
         command.append(", ");
         printReference(command, module, argument2);
         break;
      case bcNewN:
      case bcFillRI:
      case bcCreateN:
      case bcAllocN:
         printCommand(command, opcode);
         printReference(command, module, argument);
         command.append(", ");
         command.appendInt(argument2);
         break;
      case bcCopyFI:
      case bcCopyToAI:
      case bcXSetFI:
      case bcReadToF:
      case bcMove:
      case bcMoveTo:
      case bcXSaveAI:
      case bcCopyAI:
      case bcXSaveSI:
         printCommand(command, opcode);
         command.appendInt(argument);
         command.append(", ");
         command.appendInt(argument2);
         break;
      default:
         printCommand(command, opcode);
         break;
   }

   print(command);
   return true;
}

void printByteCodes(_Module* module, _Memory* code, ref_t address, int indent, int pageSize)
{
   MemoryReader codeReader(code, address);

   size_t codeSize = codeReader.getDWord();
   size_t endPos = codeReader.Position() + codeSize;

   int row = 1;
   List<int> labels;
   while(codeReader.Position() < endPos) {
      if (printCommand(module, codeReader, indent, labels)) {
         print("\n");

         nextRow(row, pageSize);
      }
   }
}

ref_t resolveMessageByIndex(_Module* module, ident_t className, int index)
{
   // find class VMT
   _Memory* vmt = findClassVMT(module, className);
   if (vmt == NULL) {
      return 0;
   }

   // list methods
   MemoryReader vmtReader(vmt);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));

   VMTEntry        entry;

   size -= sizeof(ClassHeader);
   IdentifierString temp;
   int row = 0;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(VMTEntry));

      index--;
      if (index == 0) {
         IdentifierString temp;
         printMessage(temp, module, entry.message);

         return resolveMessage(module, temp.c_str(), test(header.flags, elExtension));
      }

      size -= sizeof(VMTEntry);
   }

   return 0;
}

void printMethod(_Module* module, ident_t methodReference, int pageSize)
{
   methodReference = trim(methodReference);

   int separator = methodReference.find('.');
   if (separator == -1) {
      printf("Invalid command");

      return;
   }

   IdentifierString className(methodReference, separator);

   ident_t methodName = methodReference + separator + 1;
   ref_t message = 0;

   // find class VMT
   _Memory* vmt = findClassVMT(module, className);
   _Memory* code = findClassCode(module, className);
   if (vmt == NULL || code == NULL) {
      printLine("Class not found: ", className);

      return;
   }

   // find method entry
   MemoryReader vmtReader(vmt);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));

   // resolve method
   if (methodName[0] >= '0' && methodName[0] <= '9') {
      message = resolveMessageByIndex(module, className.ident(), methodName.toInt());
   }
   else message = resolveMessage(module, methodName, test(header.flags, elExtension));

   if (message == 0)
      return;

   VMTEntry        entry;

   // read VMT while the entry not found
   size -= sizeof(ClassHeader);
   bool found = false;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(VMTEntry));

      if (entry.message == message) {
         found = true;

         IdentifierString temp;
         temp.copy(className);
         temp.append('.');
         printMessage(temp, module, entry.message);
         printLine("@method ", temp);

         printByteCodes(module, code, entry.address, 4, pageSize);
         print("@end\n");

         break;
      }

      size -= sizeof(VMTEntry);
   }
   if (!found) {
      printLine("Method not found:", methodName);
   }
}

void printSymbol(_Module* module, ident_t symbolReference, int pageSize)
{
   // find class VMT
   _Memory* code = findSymbolCode(module, symbolReference);
   if (code == NULL) {
      printLine("Symbol not found:", symbolReference);

      return;
   }

   printLine("@symbol ", symbolReference);
   printByteCodes(module, code, 0, 4, pageSize);
   print("@end\n");
}

bool loadClassInfo(_Module* module, ident_t reference, ClassInfo& info)
{
   // find class meta data
   _Memory* data = findClassMetaData(module, reference);
   if (data == NULL) {
      printLine("Class not found:", reference);

      return false;
   }

   MemoryReader reader(data);
   info.load(&reader);

   return true;
}

void listFields(_Module* module, ClassInfo& info, int& row, int pageSize)
{
   ClassInfo::FieldMap::Iterator it = info.fields.start();
   while (!it.Eof()) {
      ref_t type = info.fieldTypes.get(*it).value1;
      if (type != 0 && !isPrimitiveRef(type)) {
         ident_t typeName = module->resolveReference(type);

         printLine("@field ", (const char*)it.key(), " of ", typeName, row, pageSize);
      }
      else printLine("@field ", (const char*)it.key(), row, pageSize);

      it++;
   }
}

void listFlags(int flags, int& row, int pageSize)
{
   if (test(flags, elNestedClass)) {
      printLine("@flag ", "elNestedClass", row, pageSize);
   }

   if (test(flags, elDynamicRole)) {
      printLine("@flag ", "elDynamicRole", row, pageSize);
   }

   if (test(flags, elStructureRole)) {
      printLine("@flag ", "elStructureRole", row, pageSize);
   }

   if (test(flags, elSealed)) {
      printLine("@flag ", "elSealed", row, pageSize);
   }
   else if (test(flags, elFinal)) {
      printLine("@flag ", "elFinal", row, pageSize);
   }
   else if (test(flags, elClosed)) {
      printLine("@flag ", "elClosed", row, pageSize);
   }

   if (test(flags, elWrapper)) {
      printLine("@flag ", "elWrapper", row, pageSize);
   }

   if (test(flags, elStateless)) {
      printLine("@flag ", "elStateless", row, pageSize);
   }

   //if (test(flags, elGroup)) {
   //   printLine("@flag ", "elGroup", row, pageSize);
   //}

   if (test(flags, elWithGenerics)) {
      printLine("@flag ", "elWithGenerics", row, pageSize);
   }

   if (test(flags, elWithVariadics))
      printLine("@flag ", "elWithVariadics", row, pageSize);

   if (test(flags, elReadOnlyRole))
      printLine("@flag ", "elReadOnlyRole", row, pageSize);

   if (test(flags, elNonStructureRole))
      printLine("@flag ", "elNonStructureRole", row, pageSize);

   if (test(flags, elSubject))
      printLine("@flag ", "elSubject", row, pageSize);

   if (test(flags, elAbstract))
      printLine("@flag ", "elAbstract", row, pageSize);

   if (test(flags, elRole))
      printLine("@flag ", "elRole", row, pageSize);

   if (test(flags, elExtension))
      printLine("@flag ", "elExtension", row, pageSize);

   if (test(flags, elMessage))
      printLine("@flag ", "elMessage", row, pageSize);

   if (test(flags, elExtMessage))
      printLine("@flag ", "elExtMessage", row, pageSize);

   //if (test(flags, elSymbol))
   //   printLine("@flag ", "elSymbol", row, pageSize);


   if (test(flags, elClassClass))
      printLine("@flag ", "elClassClass", row, pageSize);

   if (test(flags, elNoCustomDispatcher))
      printLine("@flag ", "elNoCustomDispatcher", row, pageSize);

   switch (flags & elDebugMask) {
      case elDebugDWORD:
         printLine("@flag ", "elDebugDWORD", row, pageSize);
         break;
      case elDebugReal64:
         printLine("@flag ", "elDebugReal64", row, pageSize);
         break;
      case elDebugLiteral:
         printLine("@flag ", "elDebugLiteral", row, pageSize);
         break;
      case elDebugIntegers:
         printLine("@flag ", "elDebugIntegers", row, pageSize);
         break;
      case elDebugArray:
         printLine("@flag ", "elDebugArray", row, pageSize);
         break;
      case elDebugQWORD:
         printLine("@flag ", "elDebugQWORD", row, pageSize);
         break;
      case elDebugBytes:
         printLine("@flag ", "elDebugBytes", row, pageSize);
         break;
      case elDebugShorts:
         printLine("@flag ", "elDebugShorts", row, pageSize);
         break;
      case elDebugPTR:
         printLine("@flag ", "elDebugPTR");
         break;
      case elDebugWideLiteral:
         printLine("@flag ", "elDebugWideLiteral", row, pageSize);
         break;
   //   case elDebugReference:
   //      printLine("@flag ", "elDebugReference", row, pageSize);
   //      break;
      case elDebugSubject:
         printLine("@flag ", "elDebugSubject", row, pageSize);
         break;
   //////   //case elDebugReals:
   //////   //   printLine("@flag ", "elDebugReals");
   //////   //   break;
   //   case elDebugMessage:
   //      printLine("@flag ", "elDebugMessage", row, pageSize);
   //      break;
   //////   //case elDebugDPTR:
   //////   //   printLine("@flag ", "elDebugDPTR");
   //////   //   break;
   ////   case elEnumList:
   ////      printLine("@flag ", "elEnumList", row, pageSize);
   ////      break;
   }
}

void printParents(_Module* module, ref_t reference)
{
   if (!reference)
      return;

   _Memory* vmt = module->mapSection(reference | mskVMTRef, true);
   if (!vmt) {
      ident_t refName = module->resolveReference(reference);
      if (isTemplateWeakReference(refName)) {
         ref_t resolvedReference = module->mapReference(refName.c_str() + getlength(TEMPLATE_PREFIX_NS) - 1);

         vmt = module->mapSection(resolvedReference | mskVMTRef, true);
         if (!vmt)
            return;
      }
      else return;
   }

   MemoryReader vmtReader(vmt);
   // read tape record size
   vmtReader.getDWord();

   // read VMT info
   ClassInfo info;
   vmtReader.read((void*)& info.header, sizeof(ClassHeader));

   printParents(module, info.header.parentRef);

   printLine("@parent ", module->resolveReference(reference));
}

void listClassMethods(_Module* module, ident_t className, int pageSize, bool fullInfo, bool apiMode)
{
   className = trim(className);

   // find class VMT
   _Memory* vmt = findClassVMT(module, className);
   if (vmt == NULL) {
      printLine("Class not found:", className);

      return;
   }

   // list methods
   MemoryReader vmtReader(vmt);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read VMT info
   ClassInfo info;
   vmtReader.read((void*)& info.header, sizeof(ClassHeader));

   int row = 0;
   if (fullInfo) {
      if (!loadClassInfo(module, className, info)) {
         printLine("Class not found:", className);

         return;
      }

      if (apiMode) {
         if (test(info.header.flags, elExtension)) {
            IdentifierString title(className);
            title.append(" of ");

            auto target = info.fieldTypes.get(-1);
            if (target.value1) {
               ident_t targetName = module->resolveReference(target.value1);

               title.append(targetName);
            }
            else title.append("system'Object");

            printLine("extension '", title.c_str());
         }
         else printLine("class '", className);
      }

      if (info.header.parentRef) {
         if (apiMode) {
            printParents(module, info.header.parentRef);
         }
         else printLine("@parent ", module->resolveReference(info.header.parentRef));
         row++;
      }

      listFlags(info.header.flags, row, pageSize);
      listFields(module, info, row, pageSize);

      ref_t val = info.mattributes.get(ClassInfo::Attribute(caInfo, 0));
      if (val) {
         ReferenceNs sectionName("'", METAINFO_SECTION);
         _Memory* section = module->mapSection(module->mapReference(sectionName, true) | mskMetaRDataRef, true);

         ident_t desc = (const char*)section->get(val);

         printLine("@classinfo ", desc);
      }
   }

   VMTEntry        entry;

   size -= sizeof(ClassHeader);
   IdentifierString temp;
   IdentifierString prefix;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(VMTEntry));

      int hints = info.methodHints.get(ClassInfo::Attribute(entry.message, maHint));
      bool isAbstract = test(hints, tpAbstract);
      bool isMultidispatcher = test(hints, tpMultimethod);
      bool isInternal = test(hints, tpInternal);
      bool isPrivate = (hints & tpMask) == tpPrivate;
      bool isProteced = test(hints, tpProtected);
      bool isFunction = test(entry.message, FUNCTION_MESSAGE);
      bool isConversion = (entry.message & PREFIX_MESSAGE_MASK) == CONVERSION_MESSAGE;

      // print the method name
      temp.copy(className);
      temp.append('.');
      printMessage(temp, module, entry.message);

      if (isPrivate) {
         int p_index = temp.ident().find("#static&");
         if (p_index != NOTFOUND_POS)
            temp.cut(p_index, getlength("#static&"));
      }

      ref_t retType = info.methodHints.get(ClassInfo::Attribute(entry.message, maReference));
      if (retType) {
         temp.append(" of ");
         ident_t typeName = module->resolveReference(retType);
         temp.append(typeName);
      }

      if (!_noComments) {
         temp.append(";;");
         for (auto param_it = info.mattributes.getIt(ClassInfo::Attribute(caParamName, entry.message));
            !param_it.Eof(); param_it = info.mattributes.getNextIt(ClassInfo::Attribute(caParamName, entry.message), param_it))
         {
            ReferenceNs sectionName("'", METAINFO_SECTION);
            _Memory* section = module->mapSection(module->mapReference(sectionName, true) | mskMetaRDataRef, true);

            ident_t paramName = (const char*)section->get(*param_it);

            temp.append(paramName);
            temp.append('|');
         }

         ref_t val = info.mattributes.get(ClassInfo::Attribute(caInfo, entry.message));
         if (val) {
            ReferenceNs sectionName("'", METAINFO_SECTION);
            _Memory* section = module->mapSection(module->mapReference(sectionName, true) | mskMetaRDataRef, true);

            ident_t desc = (const char*)section->get(val);

            temp.append(desc);
         }
         if (temp.ident().endsWith(";;"))
            temp.truncate(temp.Length() - 2);
      }

      prefix.copy("@method ");
      if (isProteced)
         prefix.append("@protected ");
      if (isAbstract)
         prefix.append("@abstract ");
      if (isMultidispatcher)
         prefix.append("@multidispatcher ");
      if (isInternal)
         prefix.append("@internal ");
      if (isPrivate)
         prefix.append("@private ");
      if (isFunction)
         prefix.append("@function ");

      printLine(prefix, temp);

      nextRow(row, pageSize);

      size -= sizeof(VMTEntry);
   }
}

inline bool isTemplateBased(ident_t reference)
{
   for (size_t i = 0; i < getlength(reference); i++) {
      if (reference[i] == '#' && reference[i + 1] >= '0' && reference[i + 1] <= '9')
         return true;
   }

   return false;
}

void printSymbolInfo(_Module* module, ident_t refName, ref_t ref)
{
   IdentifierString name(refName);

   _Memory* metaData = module->mapSection(ref | mskMetaRDataRef, true);
   if (metaData != NULL && metaData->Length() == sizeof(SymbolExpressionInfo)) {
      SymbolExpressionInfo info;

      MemoryReader reader(metaData);
      info.load(&reader);

      if (info.type == SymbolExpressionInfo::Type::Constant) {
         name.append(" of ");
         name.append(module->resolveReference(info.exprRef));
      }
   }

   printLine("symbol ", name.c_str());
}

void printAPI(_Module* module, int pageSize, bool publicOnly)
{
   ReferenceMap::Iterator it = ((Module*)module)->References();
   while (!it.Eof()) {
      ident_t reference = it.key();
      bool publicOne = true;
      if (reference.find(INLINE_CLASSNAME) == 1)
         publicOne = false;
      if (reference.startsWith(PRIVATE_PREFIX_NS))
         publicOne = false;

      if (reference[0] == '\'' && (!publicOnly || publicOne)) {
         if (module->mapSection(*it | mskVMTRef, true)) {
            if (isTemplateBased(reference)) {
               if (reference.find("$private@T1") != NOTFOUND_POS) {
                  listClassMethods(module, reference.c_str() + 1, pageSize, true, true);
                  printLine();
               }
            }
            else {
               listClassMethods(module, reference.c_str() + 1, pageSize, true, true);
               printLine();
            }
         }
         else if (module->mapSection(*it | mskSymbolRef, true)) {
            printSymbolInfo(module, reference, *it);
            printLine();
         }
      }

      it++;
   }
}

void listClasses(_Module* module, int pageSize)
{
   ident_t moduleName = module->Name();

   int row = 0;
   ReferenceMap::Iterator it = ((Module*)module)->References();
   while (!it.Eof()) {
      ident_t reference = it.key();
      if (isWeakReference(reference)) {
         if (module->mapSection(*it | mskVMTRef, true)) {
            printLine("class ", reference + 1, row, pageSize);
         }
         else if (module->mapSection(*it | mskSymbolRef, true)) {
            printLine("symbol ", reference + 1, row, pageSize);
         }
      }

      it++;
   }
}

void setOutputMode(path_t path)
{
   if (_writer)
      freeobj(_writer);

   _writer = new TextFileWriter(path, 0, false);
}

void runSession(_Module* module, int pageSize)
{
   char              buffer[MAX_LINE];
   IdentifierString  line;
   while (true) {
      printf("\n>");

      // !! fgets is used instead of fgetws, because there is strange bug in fgetws implementation
      fgets(buffer, MAX_LINE, stdin);
      line.copy(buffer, strlen(buffer));

      while (!emptystr(line) && line[getlength(line) - 1]=='\r' || line[getlength(line) - 1]=='\n')
         line[getlength(line) - 1] = 0;

      while (!emptystr(line) && line[getlength(line) - 1]==' ')
         line[getlength(line) - 1] = 0;

      // execute command
      if (line[0]=='?') {
         if (line[1]==0) {
            listClasses(module, pageSize);
         }
         else printHelp();
      }
      else if (line[0]=='-') {
         switch(line[1]) {
            case 'q':
               return;
            case 'h':
               printHelp();
               break;
            case 'l':
               printAPI(module, pageSize, true);
               break;
            case 'b':
               _ignoreBreakpoints = !_ignoreBreakpoints;
               break;
            case 'p':
               _noPaging = !_noPaging;
               break;
            case 'c':
               _showBytecodes = !_showBytecodes;
               break;
            case 'i':
               _noComments = !_noComments;
               break;
            case 'o':
            {
               Path path(line + 2);
               setOutputMode(path.c_str());
               break;
            }
            default:
               printHelp();
         }
      }
      else if (line[0] == '#') {
         printSymbol(module, line + 1, pageSize);
      }
      else {
         if (line.ident().find('.') != NOTFOUND_POS) {
            printMethod(module, line, pageSize);
         }
         else listClassMethods(module, line, pageSize, true, false);
      }
   }
}

const char* manifestParameters[4] = { "namespace","name     ","version  ","author   " };

void printManifest(_Module* module)
{
   ReferenceNs name(module->Name(), PACKAGE_SECTION);

   _Memory* section = module->mapSection(module->mapReference(name, false) | mskRDataRef, true);
   if (section != NULL) {

      _ELENA_::RelocationMap::Iterator it(section->getReferences());
      ref_t currentMask = 0;
      ref_t currentRef = 0;
      while (!it.Eof()) {
         int i = *it >> 2;
         currentMask = it.key() & mskAnyRef;
         currentRef = it.key() & ~mskAnyRef;

         if (currentMask == mskLiteralRef) {
            //printf(manifestParameters[i]);
            ident_t value = module->resolveConstant(currentRef);
            printf("%s : %s\n", manifestParameters[i], value.c_str());
         }
         it++;
      }
   }
}

void getAppPath(_ELENA_::Path& appPath)
{
#ifdef _WIN32
   wchar_t path[MAX_PATH + 1];

   ::GetModuleFileName(NULL, path, MAX_PATH);

   appPath.copySubPath(path);
   appPath.lower();
#endif
}

// === Main Program ===
int main(int argc, char* argv[])
{
   printf("ELENA command line ByteCode Viewer %d.%d.%d (C)2011-2021 by Alexei Rakov\n", ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, REVISION_VERSION);

   if (argc<2) {
      printf("ecv <module name> | ecv -p<module path>");
      return 0;
   }

   // define the console size for pagination
   int columns = 0, rows = 30;
   ConsoleHelper::getConsoleSize(columns, rows);

   // prepare library manager
   Path configPath;
   getAppPath(configPath);
   configPath.combine("templates\\lib.cfg");

   Path rootPath;
   getAppPath(rootPath);
   rootPath.combine("..\\lib50");

   // get viewing module name
   IdentifierString buffer(argv[1]);
   ident_t moduleName = buffer;

   //// load config attributes
   //IniConfigFile config;
   //if (config.load(configPath, feUTF8)) {
   //   Path::loadPath(rootPath, config.getSetting(PROJECT_SECTION, ROOTPATH_OPTION, DEFAULT_STR));
   //}

   LibraryManager loader(rootPath.c_str(), NULL);
   LoadResult result = lrNotFound;
   _Module* module = NULL;

   // if direct path is provieded
   bool pathMode = false;
   if (moduleName[0]=='-' && moduleName[1]=='p' || moduleName.find('.') != NOTFOUND_POS) {
      if (moduleName[0] == '-')
         moduleName += 2;

      Path path;
      path.copySubPath(moduleName);
      FileName fileName(moduleName);

      IdentifierString name(fileName);
      loader.setNamespace(name, path.c_str());
      module = loader.loadModule(name, result, false);
      pathMode = true;
   }
   else module = loader.loadModule(moduleName, result, false);

   if (result != lrSuccessful) {
      printLoadError(result);

      return -1;
   }
   else {
      printLine(moduleName, " module loaded");

      printManifest(module);
   }

   if (argc == 3 && ident_t(argv[2]).compare("l")) {
      _noPaging = true;

      Path path(moduleName);
      if (pathMode)
         path.changeExtension("out");

      setOutputMode(path.c_str());
      _noComments = false;

      printAPI(module, 0, true);

      _noComments = true;

      return 0;
   }

   runSession(module, rows);

   if (_writer)
      freeobj(_writer);

   return 0;
}
