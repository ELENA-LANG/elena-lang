//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ecviewer.h"
#include "ecvconst.h"
#include "module.h"
#include "langcommon.h"

using namespace elena_lang;

constexpr int TABBING = 20;

// --- trim ---

inline ustr_t trim(ustr_t s)
{
   while (s[0] == ' ')
      s += 1;

   return s;
}

// --- ByteCodeViewer ---

MemoryBase* ByteCodeViewer :: findSymbolCode(ustr_t name)
{
   ReferenceName referenceName(nullptr, name);

   ref_t reference = _module->mapReference(*referenceName, true);
   if (!reference)
      return nullptr;

   return _module->mapSection(reference | mskSymbolRef, true);
}

MemoryBase* ByteCodeViewer :: findClassVMT(ustr_t name)
{
   ReferenceName referenceName(nullptr, name);

   ref_t reference = _module->mapReference(*referenceName, true);
   if (!reference)
      return nullptr;

   return _module->mapSection(reference | mskVMTRef, true);
}

MemoryBase* ByteCodeViewer :: findClassCode(ustr_t name)
{
   ReferenceName referenceName(nullptr, name);

   ref_t reference = _module->mapReference(*referenceName, true);
   if (!reference)
      return nullptr;

   return _module->mapSection(reference | mskClassRef, true);
}

mssg_t ByteCodeViewer :: resolveMessageByIndex(MemoryBase* vmt, int index)
{
   MemoryReader vmtReader(vmt);
   // read tape record size
   pos_t size = vmtReader.getPos();

   // read VMT header
   ClassInfo info;
   vmtReader.read((void*)&info.header, sizeof(ClassHeader));

   MethodEntry entry = {};

   size -= sizeof(ClassHeader);
   IdentifierString prefix;
   IdentifierString line;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(MethodEntry));

      index--;
      if (index == 0) {
         IdentifierString line;
         addMessage(line, entry.message);

         return resolveMessage(*line/*, test(header.flags, elExtension)*/);
      }

      size -= sizeof(MethodEntry);
   }
   return 0;
}

mssg_t ByteCodeViewer :: resolveMessage(ustr_t messageName)
{
   mssg_t message = ByteCodeUtil::resolveMessage(messageName, _module);
   if (message == 0) {
      printLine("Unknown message ", messageName);
   }

   return message;
}

void ByteCodeViewer::printHelp()
{
   _presenter->print("-q                      - quit\n");
   _presenter->print("#<symbol>               - view symbol byte codes\n");
   _presenter->print("?                       - list all classes\n");
}

void ByteCodeViewer::printModuleManifest()
{
   _presenter->print(ECV_MODULE_LOADED, _module->name());
}

void ByteCodeViewer::printLine(ustr_t arg1)
{
   _presenter->print(arg1);
   _presenter->print("\n");
}

void ByteCodeViewer::printLine(ustr_t arg1, ustr_t arg2)
{
   _presenter->print(arg1);
   _presenter->print(" ");
   _presenter->print(arg2);
   _presenter->print("\n");
}

void ByteCodeViewer :: nextRow(int& row, int pageSize)
{
   row++;
   if (row == pageSize - 1 && !_noPaging) {
      getchar();
   }

   _presenter->print("\n");
}

void ByteCodeViewer :: printLineAndCount(ustr_t arg1, ustr_t arg2, int& row, int pageSize)
{
   _presenter->print(arg1);
   _presenter->print(" ");
   _presenter->print(arg2);
   nextRow(row, pageSize);
}

void ByteCodeViewer :: addRArg(arg_t arg, IdentifierString& commandStr)
{
   ref_t mask = arg & mskAnyRef;
   ustr_t referenceName = _module->resolveReference(arg & ~mskAnyRef);
   switch (mask) {
      case mskArrayRef:
         commandStr.append("array:");
         break;
      case mskMetaArrayRef:
         commandStr.append("marray:");
         break;
      case mskSymbolRef:
         commandStr.append("symbol:");
         break;
      case mskVMTRef:
         commandStr.append("class:");
         break;
      default:
         commandStr.append(":");
         break;
   }
   if (isWeakReference(referenceName))
      commandStr.append(_module->name());

   commandStr.append(referenceName);
}

void ByteCodeViewer :: addSecondRArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(", ");

   addRArg(arg, commandStr);
}

void ByteCodeViewer :: addArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(":");
   commandStr.appendInt(arg);
}

void ByteCodeViewer :: addSecondArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(", ");

   addArg(arg, commandStr);
}

void ByteCodeViewer :: addFPArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append("fp:");
   commandStr.appendInt(arg);
}

void ByteCodeViewer::addSecondFPArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(", ");

   addFPArg(arg, commandStr);
}

void ByteCodeViewer :: addSPArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append("sp:");
   commandStr.appendInt(arg);
}

void ByteCodeViewer :: addSecondSPArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(", ");

   addSPArg(arg, commandStr);
}

void ByteCodeViewer :: addCommandArguments(ByteCommand& command, IdentifierString& commandStr)
{
   if (ByteCodeUtil::isDoubleOp(command.code)) {
      switch (command.code) {
         case ByteCode::SetR:
         case ByteCode::CallR:
            addRArg(command.arg1, commandStr);
            break;
         case ByteCode::MovM:
            commandStr.append(":");
            addMessage(commandStr, command.arg1);
            break;
         default:
            addArg(command.arg1, commandStr);
            break;
      }
   }
   else if (!ByteCodeUtil::isSingleOp(command.code)) {
      switch (command.code) {
         case ByteCode::CallExtR:
            addRArg(command.arg1, commandStr);
            addSecondArg(command.arg2, commandStr);
            break;
         case ByteCode::XStoreSIR:
            addArg(command.arg1, commandStr);
            addSecondRArg(command.arg2, commandStr);
            break;
         case ByteCode::MovSIFI:
            addArg(command.arg1, commandStr);
            addSecondFPArg(command.arg2, commandStr);
            break;
         case ByteCode::NewIR:
         case ByteCode::NewNR:
            addArg(command.arg1, commandStr);
            addSecondRArg(command.arg2, commandStr);
            break;
         default:
            addArg(command.arg1, commandStr);
            addSecondArg(command.arg2, commandStr);
            break;
      }
   }
}

void ByteCodeViewer :: addMessage(IdentifierString& commandStr, mssg_t message)
{
   ByteCodeUtil::resolveMessageName(commandStr, _module, message);
}

void ByteCodeViewer :: printCommand(ByteCommand& command, int indent)
{
   IdentifierString commandLine;
   for (int i = 0; i < indent; i++)
      commandLine.append(" ");

   ByteCodeUtil::decode(command.code, commandLine);

   // HOTFIX : remove tailing double colon
   if (commandLine[commandLine.length() - 1] == ':') {
      commandLine.truncate(commandLine.length() - 2);
   }


   size_t tabbing = TABBING;
   while (commandLine.length() < tabbing) {
      commandLine.append(" ");
   }

   addCommandArguments(command, commandLine);

   _presenter->print(commandLine.str());
}

void ByteCodeViewer :: printByteCodes(MemoryBase* section, pos_t address, int indent, int pageSize)
{
   MemoryReader reader(section, address);

   ByteCommand command;
   pos_t size = reader.getPos();
   pos_t endPos = reader.position() + size;
   int row = 1;
   while (reader.position() < endPos) {
      ByteCodeUtil::read(reader, command);

      printCommand(command, indent);
      nextRow(row, pageSize);
   }
}

void ByteCodeViewer :: printSymbol(ustr_t name)
{
   // find symbol section
   MemoryBase* code = findSymbolCode(name);
   if (code == nullptr) {
      _presenter->print(ECV_SYMBOL_NOTFOUND, name);

      return;
   }

   printLine("@symbol", name);
   printByteCodes(code, 0, 4, _pageSize);
   printLine("@end");
}

void ByteCodeViewer :: printFlags(ref_t flags, int& row, int pageSize)
{
   if (test(flags, elClassClass)) {
      printLineAndCount("@flag ", "elClassClass", row, pageSize);
   }
   if (test(flags, elRole)) {
      printLineAndCount("@flag ", "elRole", row, pageSize);
   }
   if (test(flags, elSealed)) {
      printLineAndCount("@flag ", "elSealed", row, pageSize);
   }
   if (test(flags, elClosed)) {
      printLineAndCount("@flag ", "elClosed", row, pageSize);
   }
   if (test(flags, elStateless)) {
      printLineAndCount("@flag ", "elStateless", row, pageSize);
   }
   if (test(flags, elNonStructureRole)) {
      printLineAndCount("@flag ", "elNonStructureRole", row, pageSize);
   }
   if (test(flags, elStructureRole)) {
      printLineAndCount("@flag ", "elStructureRole", row, pageSize);
   }
   if (test(flags, elWrapper)) {
      printLineAndCount("@flag ", "elWrapper", row, pageSize);
   }
   if (test(flags, elReadOnlyRole)) {
      printLineAndCount("@flag ", "elReadOnlyRole", row, pageSize);
   }
}

void ByteCodeViewer :: printFields(ClassInfo& classInfo, int& row, int pageSize)
{
   IdentifierString line;

   auto it = classInfo.fields.start();
   while (!it.eof()) {
      auto fieldInfo = *it;

      line.copy(it.key());
      if (isPrimitiveRef(fieldInfo.typeRef)) {
         switch (fieldInfo.typeRef) {
            case V_INT32:
               line.append(" of __int[4]");
               break;
         }
      }
      else if (fieldInfo.typeRef) {
         ustr_t typeName = _module->resolveReference(fieldInfo.typeRef);

         line.append(" of ");
         line.append(typeName);
      }

      printLineAndCount("@field ", *line, row, pageSize);

      ++it;
   }
}

void ByteCodeViewer::printMethod(ustr_t name)
{
   name = trim(name);

   size_t separator = name.find('.');
   if (separator == NOTFOUND_POS) {
      printf("Invalid command");

      return;
   }

   IdentifierString className(name, separator);

   ustr_t methodName = name.str() + separator + 1;
   mssg_t message = 0;

   // find class VMT
   MemoryBase* vmt = findClassVMT(*className);
   MemoryBase* code = findClassCode(*className);
   if (vmt == nullptr || code == nullptr) {
      printLine("Class not found: ", *className);

      return;
   }

   MemoryReader vmtReader(vmt);
   // read tape record size
   pos_t size = vmtReader.getPos();

   // read VMT header
   ClassInfo info;
   vmtReader.read((void*)&info.header, sizeof(ClassHeader));

   // resolve method
   if (methodName[0] >= '0' && methodName[0] <= '9') {
      message = resolveMessageByIndex(vmt, StrConvertor::toInt(methodName.str(), 10));
   }
   else message = resolveMessage(methodName/*, test(header.flags, elExtension)*/);

   if (message == 0)
      return;

   bool found = false;
   MethodEntry entry = {};

   size -= sizeof(ClassHeader);
   IdentifierString prefix;
   IdentifierString line;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(MethodEntry));

      if (entry.message == message) {
         found = true;

         IdentifierString line;
         line.copy(*className);
         line.append('.');
         addMessage(line, message);

         printLine("@method ", *line);
         printByteCodes(code, entry.codeOffset, 4, _pageSize);
         printLine("@end");

         break;
      }

      size -= sizeof(MethodEntry);
   }

}

void ByteCodeViewer :: printClass(ustr_t name, bool fullInfo)
{
   name = trim(name);

   MemoryBase* vmt = findClassVMT(name);
   if (vmt == nullptr) {
      _presenter->print(ECV_CLASS_NOTFOUND, name);

      return;
   }

   MemoryReader vmtReader(vmt);
   // read tape record size
   pos_t size = vmtReader.getPos();

   // read VMT header
   ClassInfo info;
   vmtReader.read((void*)&info.header, sizeof(ClassHeader));

   int row = 1;
   if (fullInfo) {
      if (info.header.parentRef) {
         printLineAndCount("@parent ", _module->resolveReference(info.header.parentRef), row, _pageSize);
         row++;
      }

      printFlags(info.header.flags, row, _pageSize);
      printFields(info, row, _pageSize);
   }

   MethodEntry entry = {};

   size -= sizeof(ClassHeader);
   IdentifierString prefix;
   IdentifierString line;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(MethodEntry));

      line.copy(name);
      line.append('.');
      addMessage(line, entry.message);

      prefix.copy("@method ");

      printLineAndCount(*prefix, *line, row, _pageSize);

      size -= sizeof(MethodEntry);
   }
}

bool ByteCodeViewer :: load(path_t path)
{
   _module = new Module();

   FileReader reader(path, FileRBMode, FileEncoding::Raw, false);
   auto retVal = static_cast<Module*>(_module)->load(reader);

   if (retVal == LoadResult::Successful) {
      printModuleManifest();

      return true;
   }
   else return false;
}

void ByteCodeViewer :: listMembers()
{
   _module->forEachReference(_presenter, [](ModuleBase* module, ref_t reference, void* arg)
      {
         auto referenceName = module->resolveReference(reference);
         if (isWeakReference(referenceName)) {
            if (module->mapSection(reference | mskVMTRef, true)) {
               ((PresenterBase*)arg)->print("class %s\n", referenceName.str());
            }
            else if (module->mapSection(reference | mskSymbolRef, true)) {
               ((PresenterBase*)arg)->print("symbol %s\n", referenceName.str());
            }
         }
      });
}

void ByteCodeViewer :: runSession()
{
   char              buffer[LINE_LEN];
   while (true) {
      _presenter->print("\n>");

      _presenter->readLine(buffer, LINE_LEN);

      size_t len = getlength(buffer);
      while (len > 0 && (buffer[len - 1] == '\r' || buffer[len - 1] == '\n')) {
         buffer[--len] = 0;
      }
      while (len > 0 && buffer[len - 1] == ' ') {
         buffer[--len] = 0;
      }

      if (buffer[0] == '?') {
         if (buffer[1] == 0) {
            listMembers();
         }
         else printHelp();
      }
      else if (buffer[0] == '-') {
         switch (buffer[1]) {
            case 'q':
               return;
            case 'o':
               _presenter->setOutputMode(buffer + 2);
               break;
            default:
               printHelp();
               break;
         }
      }
      else if (buffer[0] == '#') {
         printSymbol(buffer + 1);
      }
      else if (ustr_t(buffer).find('.') != NOTFOUND_POS) {
         printMethod(buffer);
      }
      else printClass(buffer, true);
   }
}
