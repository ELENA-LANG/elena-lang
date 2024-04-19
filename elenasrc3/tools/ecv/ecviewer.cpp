//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Tools
//
//		This is a main file containing ecode viewer code
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ecviewer.h"
#include "ecvconst.h"
#include "module.h"
#include "langcommon.h"

using namespace elena_lang;

constexpr int TABBING = 15;
constexpr int INDENT  = 11;

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

MemoryBase* ByteCodeViewer :: findProcedureCode(ustr_t name)
{
   ReferenceName referenceName(nullptr, name);

   ref_t reference = _module->mapReference(*referenceName, true);
   if (!reference)
      return nullptr;

   return _module->mapSection(reference | mskProcedureRef, true);
}

MemoryBase* ByteCodeViewer :: findClassVMT(ustr_t name)
{
   ReferenceName referenceName(nullptr, name);

   ref_t reference = _module->mapReference(*referenceName, true);
   if (!reference)
      return nullptr;

   return _module->mapSection(reference | mskVMTRef, true);
}

bool ByteCodeViewer :: findClassInfo(ustr_t name, ClassInfo& info)
{
   ReferenceName referenceName(nullptr, name);

   ref_t reference = _module->mapReference(*referenceName, true);
   if (!reference)
      return false;

   auto section = _module->mapSection(reference | mskMetaClassInfoRef, true);
   MemoryReader reader(section);

   info.load(&reader);

   return true;
}

bool ByteCodeViewer::findMethodInfo(ustr_t referenceName, mssg_t message, MethodInfo& info)
{
   ClassInfo classInfo;
   if (!findClassInfo(referenceName, classInfo))
      return false;

   info = classInfo.methods.get(message);
   return true;
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
   printLine("Message index is too big");

   return 0;
}

mssg_t ByteCodeViewer :: resolveMessage(ustr_t messageName)
{
   mssg_t message = ByteCodeUtil::resolveMessage(messageName, _module, true);
   if (message == 0) {
      printLine("Unknown message ", messageName);
   }

   return message;
}

void ByteCodeViewer :: printHelp()
{
   _presenter->print("\n");
   _presenter->print("list of commands\n");
   _presenter->print("?                       - list all classes / symbols\n");
   _presenter->print("?~<filter>              - list classes / symbols matching a filter\n");
   _presenter->print("<class>                 - view class members\n");
   _presenter->print("<class>.~<filter>       - view class members matching a filter\n");
   _presenter->print("<class>.<message>       - view a method byte codes\n");
   _presenter->print("<class>.<index>         - view a method specified by an index byte codes\n");
   _presenter->print("#<symbol>               - view symbol byte codes\n");
   _presenter->print("-b                      - toggle bytecode mode\n");
   _presenter->print("-h                      - toggle method hints mode\n");
   _presenter->print("-p                      - toggle pagination mode\n");
   _presenter->print("-q                      - quit\n");
   _presenter->print("-t                      - toggle ignore-breakpoint mode\n");
}

const char* manifestParameters[4] = { "namespace","name     ","version  ","author   " };

void ByteCodeViewer :: printModuleManifest()
{
   _presenter->print(ECV_MODULE_LOADED, _module->name());

   ReferenceName sectionName("", PACKAGE_SECTION);

   MemoryBase* section = _module->mapSection(
      _module->mapReference(*sectionName, true) | mskConstArray, true);
   if (section != nullptr) {
      ref_t currentMask = 0;
      ref_t currentRef = 0;
      for (auto it = RelocationMap::Iterator(section->getReferences()); !it.eof(); ++it) {
         int i = *it >> 2;
         currentMask = it.key() & mskAnyRef;
         currentRef = it.key() & ~mskAnyRef;

         if (currentMask == mskLiteralRef) {
            ustr_t value = _module->resolveConstant(currentRef);

            IdentifierString line(manifestParameters[i]);
            line.append(" : ");
            line.append(value);
            line.append("\n");

            _presenter->print(*line);
         }
      }
   }
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
   if (row == pageSize - 2 && !_noPaging) {
      row = 1;

      _presenter->print("\nPress any key to continue..");
      getchar();
   }
   else _presenter->print("\n");
}

void ByteCodeViewer :: printLineAndCount(ustr_t arg1, ustr_t arg2, int& row, int pageSize)
{
   _presenter->print(arg1);
   _presenter->print(" ");
   _presenter->print(arg2);
   nextRow(row, pageSize);
}

inline void appendPrefix(IdentifierString& commandStr, ustr_t prefix, bool withTabbing)
{
   if (withTabbing && commandStr.length() > prefix.length()) {
      commandStr.truncate(commandStr.length() - prefix.length() + 1);      
   }
   commandStr.append(prefix);
}

void ByteCodeViewer :: addRArg(arg_t arg, IdentifierString& commandStr, bool withTabbing)
{
   ustr_t referenceName = nullptr;
   ref_t mask = arg & mskAnyRef;
   switch (mask) {
      case mskMssgLiteralRef:
      case mskExtMssgLiteralRef:
      case mskMssgNameLiteralRef:
         referenceName = arg ? _module->resolveConstant(arg & ~mskAnyRef) : nullptr;
         break;
      default:
         referenceName = arg ? _module->resolveReference(arg & ~mskAnyRef) : nullptr;
         break;
   }

   switch (mask) {
      case mskArrayRef:
         appendPrefix(commandStr, "array:", withTabbing);
         break;
      case mskTypeListRef:
         appendPrefix(commandStr, "marray:", withTabbing);
         break;
      case mskSymbolRef:
         appendPrefix(commandStr, "symbol:", withTabbing);
         break;
      case mskVMTRef:
         appendPrefix(commandStr, "class:", withTabbing);
         break;
      case mskLongLiteralRef:
         appendPrefix(commandStr, "longconst:", withTabbing);
         referenceName = _module->resolveConstant(arg & ~mskAnyRef);
         break;
      case mskRealLiteralRef:
         appendPrefix(commandStr, "realconst:", withTabbing);
         referenceName = _module->resolveConstant(arg & ~mskAnyRef);
         break;
      case mskIntLiteralRef:
         appendPrefix(commandStr, "intconst:", withTabbing);
         referenceName = _module->resolveConstant(arg & ~mskAnyRef);
         break;
      case mskLiteralRef:
         appendPrefix(commandStr, "strconst:", withTabbing);
         referenceName = _module->resolveConstant(arg & ~mskAnyRef);
         break;
      case mskWideLiteralRef:
         appendPrefix(commandStr, "wideconst:", withTabbing);
         referenceName = _module->resolveConstant(arg & ~mskAnyRef);
         break;
      case mskCharacterRef:
         appendPrefix(commandStr, "charconst:", withTabbing);
         referenceName = _module->resolveConstant(arg & ~mskAnyRef);
         break;
      case mskStaticVariable:
         appendPrefix(commandStr, "static:", withTabbing);
         break;
      case mskProcedureRef:
         appendPrefix(commandStr, "procedure:", withTabbing);
         break;
      case mskMssgLiteralRef:
      case mskExtMssgLiteralRef:
         appendPrefix(commandStr, "mssg:", withTabbing);
         break;
      case mskMssgNameLiteralRef:
         appendPrefix(commandStr, "mssgname:", withTabbing);
         break;
      default:
         commandStr.append(":");
         break;
   }

   if (!arg) {
      commandStr.append("0");
   }
   else if (!referenceName.empty()) {
      if (isWeakReference(referenceName))
         commandStr.append(_module->name());

      commandStr.append(referenceName);
   }
   else commandStr.appendUInt(arg, 16);

}

void ByteCodeViewer :: addSecondRArg(arg_t arg, IdentifierString& commandStr, List<pos_t>& labels)
{
   commandStr.append(", ");

   if ((arg & mskAnyRef) == mskLabelRef) {
      addLabel(arg & ~mskAnyRef, commandStr, labels);
   }
   else addRArg(arg, commandStr, false);
}

void ByteCodeViewer :: addArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(":");
   commandStr.appendInt(arg);
}

void ByteCodeViewer :: addIArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append("i:");
   commandStr.appendInt(arg);
}

void ByteCodeViewer :: addSecondArg(arg_t arg, IdentifierString& commandStr)
{
   commandStr.append(", ");

   addArg(arg, commandStr);
}

void ByteCodeViewer :: addSecondArg(arg_t arg, ustr_t prefix, IdentifierString& commandStr)
{
   commandStr.append(", ");
   commandStr.append(prefix);

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

inline int getLabelIndex(pos_t label, List<pos_t>& labels)
{
   int index = 0;
   auto it = labels.start();
   while (!it.eof()) {
      if (*it == label)
         return index;

      index++;
      ++it;
   }

   return -1;
}

void ByteCodeViewer :: addLabel(arg_t labelPosition, IdentifierString& commandStr, List<pos_t>& labels)
{
   int index = getLabelIndex(labelPosition, labels);

   if (index == -1) {
      index = labels.count();

      labels.add(labelPosition);
   }

   commandStr.append("Lab");
   if (index < 10) {
      commandStr.append('0');
   }
   commandStr.appendInt(index);
}

void ByteCodeViewer :: addCommandArguments(ByteCommand& command, IdentifierString& commandStr, 
   List<pos_t>& labels, pos_t commandPosition)
{
   if (ByteCodeUtil::isDoubleOp(command.code)) {
      switch (command.code) {
         case ByteCode::SetR:
         case ByteCode::CallR:
         case ByteCode::CmpR:
         case ByteCode::PeekR:
         case ByteCode::StoreR:
         case ByteCode::CreateR:
         case ByteCode::XCreateR:
            addRArg(command.arg1, commandStr);
            break;
         case ByteCode::MovM:
         case ByteCode::TstM:
         case ByteCode::XRedirectM:
            commandStr.append(":");
            addMessage(commandStr, command.arg1);
            break;
         case ByteCode::Jump:
         case ByteCode::Jeq:
         case ByteCode::Jne:
         case ByteCode::Jlt:
         case ByteCode::Jge:
         case ByteCode::Jgr:
         case ByteCode::Jle:
            addLabel(command.arg1 + commandPosition + 5, commandStr, labels);
            break;
         case ByteCode::AssignI:
            addArg(command.arg1, commandStr);
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
            if (test(command.arg2, (int)0x80000000)) {
               addSecondArg(command.arg2 & 0x7FFFFFFF, "long ", commandStr);
            }
            else addSecondArg(command.arg2, commandStr);
            break;
         case ByteCode::XStoreSIR:
         case ByteCode::XStoreFIR:
            addArg(command.arg1, commandStr);
            addSecondRArg(command.arg2, commandStr, labels);
            break;
         case ByteCode::MovSIFI:
            addArg(command.arg1, commandStr);
            addSecondFPArg(command.arg2, commandStr);
            break;
         case ByteCode::NewIR:
         case ByteCode::NewNR:
         case ByteCode::XNewNR:
         case ByteCode::CreateNR:
            addArg(command.arg1, commandStr);
            addSecondRArg(command.arg2, commandStr, labels);
            break;
         case ByteCode::CallMR:
         case ByteCode::VCallMR:
         case ByteCode::JumpMR:
         case ByteCode::VJumpMR:
         case ByteCode::DispatchMR:
            commandStr.append(":");
            addMessage(commandStr, command.arg1);
            addSecondRArg(command.arg2, commandStr, labels);
            break;
         case ByteCode::XDispatchMR:
            if (command.arg2) {
               commandStr.append("mssg:");
               addMessage(commandStr, command.arg1);
               addSecondRArg(command.arg2, commandStr, labels);
            }
            else {
               commandStr.append("i:");
               commandStr.appendInt(getArgCount(command.arg1));
            }
            break;
         case ByteCode::SelEqRR:
         case ByteCode::SelLtRR:
         case ByteCode::SelULtRR:
         case ByteCode::SelGrRR:
            addRArg(command.arg1, commandStr);
            addSecondRArg(command.arg2, commandStr, labels);
            break;
         case ByteCode::XHookDPR:
         case ByteCode::XLabelDPR:
            addArg(command.arg1, commandStr);
            addSecondRArg(command.arg2, commandStr, labels);
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
   if (!ByteCodeUtil::resolveMessageName(commandStr, _module, message)) {
      commandStr.append("invalid ");
      commandStr.appendUInt(message);
   }      
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

   command.appendUInt(hex, 16);
   command.append('h');
}

void ByteCodeViewer :: printCommand(ByteCommand& command, int indent, 
   List<pos_t>& labels, pos_t commandPosition)
{
   IdentifierString commandLine;

   if (_showBytecodes) {
      if ((int)command.code < 0x10)
         commandLine.append('0');

      commandLine.appendUInt((int)command.code, 16);
      commandLine.append(' ');

      if (command.code > ByteCode::MaxDoubleOp) {
         appendHex32(commandLine, command.arg1);
         commandLine.append(' ');

         appendHex32(commandLine, command.arg2);
         commandLine.append(' ');
      }
      else if (command.code > ByteCode::MaxSingleOp) {
         appendHex32(commandLine, command.arg1);
         commandLine.append(' ');
      }

      size_t tabbing = command.code == ByteCode::Nop ? 26 : 33;
      while (commandLine.length() < tabbing) {
         commandLine.append(' ');
      }
   }

   if (command.code == ByteCode::Nop) {
      addLabel(commandPosition, commandLine, labels);
      commandLine.append(':');
      commandLine.append("     ");

      ByteCodeUtil::decode(command.code, commandLine);
   }
   else {
      for (int i = 0; i < indent; i++)
         commandLine.append(" ");

      size_t startIndex = commandLine.length();

      ByteCodeUtil::decode(command.code, commandLine);

      // HOTFIX : remove tailing double colon
      if (commandLine[commandLine.length() - 1] == ':') {
         commandLine.truncate(commandLine.length() - 2);
      }

      size_t tabbing = startIndex + TABBING;
      size_t spaceIndex = (*commandLine).findSub(startIndex, ' ', commandLine.length());
      while (commandLine.length() < tabbing) {
         commandLine.insert(" ", spaceIndex);
      }

      addCommandArguments(command, commandLine, labels, commandPosition);
   }

   _presenter->print(commandLine.str());
}

void ByteCodeViewer :: printByteCodes(MemoryBase* section, pos_t address, int indent, int pageSize)
{
   List<pos_t> labels(INVALID_POS);
   MemoryReader reader(section, address);

   ByteCommand command;
   pos_t size = reader.getPos();
   pos_t endPos = reader.position() + size;
   int row = 1;
   while (reader.position() < endPos) {
      pos_t position = reader.position();

      ByteCodeUtil::read(reader, command);

      // ignore a breakpoint if required
      if (command.code == ByteCode::Breakpoint && _ignoreBreakpoints)
         continue;

      printCommand(command, indent, labels, position);
      nextRow(row, pageSize);
   }
}

void ByteCodeViewer :: printMethodInfo(MethodInfo& info)
{
   IdentifierString flags("          @hints:");
   flags.appendHex(info.hints);
   if (test(info.hints, (ref_t)MethodHint::Abstract))
      flags.append(" @Abstract");
   if (test(info.hints, (ref_t)MethodHint::Constant))
      flags.append(" @Constant");
   if (test(info.hints, (ref_t)MethodHint::Conversion))
      flags.append(" @Conversion");
   if (test(info.hints, (ref_t)MethodHint::Extension))
      flags.append(" @Extension");

   printLine(*flags);
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
   printByteCodes(code, 0, INDENT, _pageSize);
   printLine("@end");
}

void ByteCodeViewer :: printProcedure(ustr_t name)
{
   // find symbol section
   MemoryBase* code = findProcedureCode(name);
   if (code == nullptr) {
      _presenter->print(ECV_SYMBOL_NOTFOUND, name);

      return;
   }

   printLine("@procedure", name);
   printByteCodes(code, 0, INDENT, _pageSize);
   printLine("@end");
}

void ByteCodeViewer :: printFlags(ref_t flags, int& row, int pageSize)
{
   if (test(flags, elAbstract)) {
      printLineAndCount("@flag ", "elAbstract", row, pageSize);
   }
   if (test(flags, elClassClass)) {
      printLineAndCount("@flag ", "elClassClass", row, pageSize);
   }
   if (test(flags, elClosed)) {
      printLineAndCount("@flag ", "elClosed", row, pageSize);
   }
   if (test(flags, elDynamicRole)) {
      printLineAndCount("@flag ", "elDynamicRole", row, pageSize);
   }
   if (test(flags, elExtension)) {
      printLineAndCount("@flag ", "elExtension", row, pageSize);
   }
   if (test(flags, elFinal)) {
      printLineAndCount("@flag ", "elFinal", row, pageSize);
   }
   if (test(flags, elGroup)) {
      printLineAndCount("@flag ", "elGroup", row, pageSize);
   }
   if (test(flags, elNestedClass)) {
      printLineAndCount("@flag ", "elNestedClass", row, pageSize);
   }
   if (test(flags, elNoCustomDispatcher)) {
      printLineAndCount("@flag ", "elNoCustomDispatcher", row, pageSize);
   }
   if (test(flags, elNonStructureRole)) {
      printLineAndCount("@flag ", "elNonStructureRole", row, pageSize);
   }
   if (test(flags, elMessage)) {
      printLineAndCount("@flag ", "elMessage", row, pageSize);
   }
   if (test(flags, elPacked)) {
      printLineAndCount("@flag ", "elPacked", row, pageSize);
   }
   if (test(flags, elReadOnlyRole)) {
      printLineAndCount("@flag ", "elReadOnlyRole", row, pageSize);
   }
   if (test(flags, elRole)) {
      printLineAndCount("@flag ", "elRole", row, pageSize);
   }
   if (test(flags, elSealed)) {
      printLineAndCount("@flag ", "elSealed", row, pageSize);
   }
   if (test(flags, elStateless)) {
      printLineAndCount("@flag ", "elStateless", row, pageSize);
   }
   if (test(flags, elStructureRole)) {
      printLineAndCount("@flag ", "elStructureRole", row, pageSize);
   }
   if (test(flags, elStructureWrapper)) {
      printLineAndCount("@flag ", "elStructureWrapper", row, pageSize);
   }
   if (test(flags, elTemplatebased)) {
      printLineAndCount("@flag ", "elTemplatebased", row, pageSize);
   }
   if (test(flags, elWithCustomDispatcher)) {
      printLineAndCount("@flag ", "elWithCustomDispatcher", row, pageSize);
   }
   if (test(flags, elWithGenerics)) {
      printLineAndCount("@flag ", "elWithGenerics", row, pageSize);
   }
   if (test(flags, elWithVariadics)) {
      printLineAndCount("@flag ", "elWithVariadics", row, pageSize);
   }
   if (test(flags, elWrapper)) {
      printLineAndCount("@flag ", "elWrapper", row, pageSize);
   }

   ref_t debugFlag = flags & elDebugMask;
   switch (debugFlag) {
      case elDebugDWORD:
         printLineAndCount("@flag ", "elDebugDWORD", row, pageSize);
         break;
      case elDebugFLOAT64:
         printLineAndCount("@flag ", "elDebugFLOAT64", row, pageSize);
         break;
      case elDebugQWORD:
         printLineAndCount("@flag ", "elDebugQWORD", row, pageSize);
         break;
      case elDebugDWORDS:
         printLineAndCount("@flag ", "elDebugDWORDS", row, pageSize);
         break;
      case elDebugLiteral:
         printLineAndCount("@flag ", "elDebugLiteral", row, pageSize);
         break;
      case elDebugWideLiteral:
         printLineAndCount("@flag ", "elDebugWideLiteral", row, pageSize);
         break;
      case elDebugArray:
         printLineAndCount("@flag ", "elDebugArray", row, pageSize);
         break;
      default:
         break;
   }
}

void ByteCodeViewer :: printFields(ClassInfo& classInfo, int& row, int pageSize)
{
   bool structOne = test(classInfo.header.flags, elStructureRole);

   IdentifierString line;

   auto it = classInfo.fields.start();
   while (!it.eof()) {
      auto fieldInfo = *it;

      line.clear();
      if (structOne) {
         line.append("+");
         line.appendHex(fieldInfo.offset);
         line.append("h: ");
      }

      line.append(it.key());
      if (isPrimitiveRef(fieldInfo.typeInfo.typeRef)) {
         switch (fieldInfo.typeInfo.typeRef) {
            case V_INT32:
               line.append(" of __int[4]");
               break;
            case V_UINT32:
               line.append(" of __uint[4]");
               break;
            case V_PTR32:
               line.append(" of __ptr[4]");
               break;
            case V_PTR64:
               line.append(" of __ptr[8]");
               break;
            case V_INT8:
               line.append(" of __int[1]");
               break;
            case V_UINT8:
               line.append(" of __uint[1]");
               break;
            case V_INT16:
               line.append(" of __int[2]");
               break;
            case V_UINT16:
               line.append(" of __uint[2]");
               break;
            case V_WORD32:
               line.append(" of __word[4]");
               break;
         }
      }
      else if (fieldInfo.typeInfo.typeRef) {
         ustr_t typeName = _module->resolveReference(fieldInfo.typeInfo.typeRef);

         line.append(" of ");
         line.append(typeName);
      }

      printLineAndCount("@field ", *line, row, pageSize);

      ++it;
   }
}

inline ustr_t getMethodPrefix(bool isFunction)
{
   if (isFunction) {
      return "@function";
   }
   return "@method";
}

void ByteCodeViewer::printMethod(ustr_t name, bool fullInfo)
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

//   bool found = false;
   MethodEntry entry = {};

   size -= sizeof(ClassHeader);
   IdentifierString prefix;
   IdentifierString line;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(MethodEntry));

      if (entry.message == message) {
         MethodInfo methodInfo = {};
         if (fullInfo) {
            findMethodInfo(*className, message, methodInfo);
         }

         IdentifierString line;
         line.copy(*className);
         line.append('.');
         addMessage(line, message);
         if (methodInfo.outputRef) {
            line.append("->");
            line.append(_module->resolveReference(methodInfo.outputRef));
         }
         if (_showMethodInfo && _showBytecodes) {
            line.append(" [");
            line.appendHex(message);
            line.append("]");
         }

         printLine(getMethodPrefix(test(entry.message, FUNCTION_MESSAGE)), *line);

         if (_showMethodInfo)
            printMethodInfo(methodInfo);

         if (test(methodInfo.hints, (ref_t)MethodHint::Abstract)) {
            printLine("  <abstract>");
         }
         else printByteCodes(code, entry.codeOffset, INDENT, _pageSize);

         printLine("@end");

         break;
      }

      size -= sizeof(MethodEntry);
   }
}

void ByteCodeViewer :: printClass(ustr_t name, bool fullInfo, ustr_t filterMask)
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
      findClassInfo(name, info);

      if (info.header.parentRef) {
         printLineAndCount("@parent ", _module->resolveReference(info.header.parentRef), row, _pageSize);
         row++;
      }

      if (test(info.header.flags, elExtension)) {
         ref_t extensionRef = info.attributes.get({ 0, ClassAttribute::ExtensionRef });
         if (extensionRef)
            printLineAndCount("@target ", _module->resolveReference(extensionRef), row, _pageSize);
      }

      printFlags(info.header.flags, row, _pageSize);
      printFields(info, row, _pageSize);
   }

   MethodEntry entry = {};

   size -= sizeof(ClassHeader);
   IdentifierString prefix;
   IdentifierString line;
   int counter = 1;
   while (size > 0) {
      vmtReader.read((void*)&entry, sizeof(MethodEntry));

      line.copy(name);
      line.append('.');
      addMessage(line, entry.message);

      if (filterMask.empty() || (*line).findStr(filterMask) != NOTFOUND_POS) {
         prefix.copy("#");
         prefix.appendInt(counter);
         prefix.append(": ");
         prefix.append(getMethodPrefix(test(entry.message, FUNCTION_MESSAGE)));

         printLineAndCount(*prefix, *line, row, _pageSize);
      }


      size -= sizeof(MethodEntry);
      counter++;
   }
}

bool ByteCodeViewer :: load(path_t path)
{
   _module = new Module();
   _pathMode = true;

   FileReader reader(path, FileRBMode, FileEncoding::Raw, false);
   auto retVal = static_cast<Module*>(_module)->load(reader);

   if (retVal == LoadResult::Successful) {
      printModuleManifest();

      return true;
   }
   else return false;
}

bool ByteCodeViewer :: loadByName(ustr_t name)
{
   _module = _provider->loadModule(name);

   if (_module != nullptr) {
      printModuleManifest();

      return true;
   }

   return false;
}

struct BCVSessionInfo
{
   ByteCodeViewer* viewer;
   int             row;
   int             pageSize;
   ustr_t          filter;
};

void ByteCodeViewer :: listMembers(ustr_t filter)
{
   BCVSessionInfo info = { this, 1, _pageSize, filter };

   _module->forEachReference(&info, [](ModuleBase* module, ref_t reference, void* arg)
      {
         auto referenceName = module->resolveReference(reference);

         BCVSessionInfo* info = (BCVSessionInfo*)arg;

         if (isWeakReference(referenceName)) {
            if (!info->filter.empty() && referenceName.findStr(info->filter) == NOTFOUND_POS)
               return;

            if (module->mapSection(reference | mskVMTRef, true)) {
               info->viewer->printLineAndCount("class ", referenceName, 
                  info->row, info->pageSize);
            }
            else if (module->mapSection(reference | mskSymbolRef, true)) {
               info->viewer->printLineAndCount("symbol #", referenceName,
                  info->row, info->pageSize);
            }
            else if (module->mapSection(reference | mskProcedureRef, true)) {
               info->viewer->printLineAndCount("procedure @", referenceName,
                  info->row, info->pageSize);
            }
         }
      });
}

void ByteCodeViewer :: runSession()
{
   printHelp();

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
         if (buffer[1] == 0 || buffer[1] == '~') {
            listMembers(buffer[1] == 0 ? nullptr : buffer + 2);
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
            case 'b':
               _showBytecodes = !_showBytecodes;
               _presenter->print("Bytecode mode is %s", _showBytecodes ? "true" : "false");
               break;
            case 't':
               _ignoreBreakpoints= !_ignoreBreakpoints;
               _presenter->print("Ignore breakpoint mode is %s", _ignoreBreakpoints ? "true" : "false");
               break;
            case 'h':
               _showMethodInfo = !_showMethodInfo;
               _presenter->print("Method hint mode is %s", _showMethodInfo ? "true" : "false");
               break;
            default:
               printHelp();
               break;
         }
      }
      else if (buffer[0] == '#') {
         printSymbol(buffer + 1);
      }
      else if (buffer[0] == '@') {
         printProcedure(buffer + 1);
      }
      else if (ustr_t(buffer).find('.') != NOTFOUND_POS) {
         size_t index = ustr_t(buffer).find('.');
         if (buffer[index + 1] == '~') {
            IdentifierString filter(buffer + index + 2);
            buffer[index] = 0;

            printClass(buffer, true, *filter);
         }
         else printMethod(buffer, true);
      }
      else printClass(buffer, true, nullptr);
   }
}
