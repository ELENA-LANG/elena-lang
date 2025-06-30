//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implematioon of the DebugController class and
//    its helpers
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "debugcontroller.h"
#include "module.h"

#ifdef _MSC_VER

#include <tchar.h>

#endif

using namespace elena_lang;

DebugSymbol operator & (const DebugSymbol& l, const DebugSymbol& r)
{
   return (DebugSymbol)((ref_t)l & (ref_t)r);
}

// --- LibraryProvider ---

void DebugInfoProvider :: retrievePath(ustr_t name, PathString& path, path_t extension)
{
   ustr_t package = _model->getPackage();

   // if it is the project package
   if (isEqualOrSubSetNs(package, name)) {
      defineModulePath(name, path, *_model->projectPath, _model->getOutputPath(), extension);
   }
   else {
      // check external libraries
      for (auto ref_it = _model->referencePaths.start(); !ref_it.eof(); ++ref_it) {
         ustr_t extPackage = ref_it.key();
         if (isEqualOrSubSetNs(extPackage, name)) {
            path.copy(*_model->projectPath);
            path.combine(*ref_it);

            ReferenceName::nameToPath(path, name);
            path.appendExtension(extension);

            return;
         }
      }

      // if file doesn't exist use package root
      path.copy(*_model->paths.libraryRoot);

      ReferenceName::nameToPath(path, name);
      path.appendExtension(extension);
   }
}

void DebugInfoProvider :: fixNamespace(NamespaceString& name)
{
   PathString      path;
   retrievePath(*name, path, _T("dnl"));

   while (name.length() != 0) {
      retrievePath(*name, path, _T("dnl"));

      if (!PathUtil::ifExist(*path)) {
         name.trimLastSubNs();
      }
      else break;
   }
}

addr_t DebugInfoProvider :: getClassAddress(ustr_t name)
{
   addr_t address = _classNames.get(name);
   if (address == INVALID_ADDR) {
      if (name.findStr(TEMPLATE_PREFIX_NS) != NOTFOUND_POS) {
         size_t index = name.findLast('\'');
         name = name + index + 1;

         // bad luck : we have to go through the list
         ustr_t resolvedName = _classNames.retrieve<ustr_t>(name, name,
            [](ustr_t arg, ustr_t ref, addr_t)
            {
               size_t index = ref.findLast('\'');
               if (index != NOTFOUND_POS) {
                  ustr_t currentName = ref + index + 1;

                  return currentName.compare(arg);
               }
               return false;
            }
         );

         if (!resolvedName.empty()) {
            address = _classNames.get(resolvedName);
         }
      }
   }

   return address;
}

ModuleBase* DebugInfoProvider :: resolveModule(ustr_t ns)
{
   ModuleMap::Iterator it = _modules.start();
   while (!it.eof()) {
      auto name = (*it)->name();

      if (NamespaceString::isIncluded(name, ns)) {
         IdentifierString virtualRef(ns + name.length_pos());
         while (true) {
            virtualRef.append('\'');
            virtualRef.append(NAMESPACE_REF);
            if ((*it)->mapReference(*virtualRef, true))
               return *it;

            break;
         }
      }
      ++it;
   }
   return nullptr;
}

addr_t DebugInfoProvider :: findNearestAddress(ModuleBase* module, ustr_t path, int row)
{
   MemoryBase* lineInfos = module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true);
   MemoryBase* strings = module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true);

   DebugLineInfo* info = (DebugLineInfo*)lineInfos->get(sizeof(pos_t));
   size_t count = lineInfos->length() / sizeof(DebugLineInfo);

   addr_t address = INVALID_ADDR;
   int nearestRow = 0;
   bool skipping = true;
   for (size_t i = 0; i < count; i++) {
      if (info[i].symbol == DebugSymbol::Procedure && info[i].addresses.source.nameRef != INVALID_ADDR) {
         ustr_t procPath = (const char*)strings->get(info[i].addresses.source.nameRef);
         if (procPath.compare(path)) {
            skipping = false;
         }
      }
      else if (info[i].symbol == DebugSymbol::End) {
         skipping = true;
      }
      else if (!skipping && (info[i].symbol & DebugSymbol::DebugMask) == DebugSymbol::Breakpoint) {
         if (_abs(nearestRow - row) > _abs(info[i].row - row)) {
            nearestRow = info[i].row;

            address = info[i].addresses.step.address;
            if (info[i].row == row)
               break;
         }
      }
   }
   return address;
}

DebugLineInfo* DebugInfoProvider :: getNextStep(DebugLineInfo* step, bool stepOverMode)
{
   DebugLineInfo* next = step;
   if (stepOverMode) {
      int level = 1;
      while (level > 0) {
         next = &next[1];
         switch (next->symbol) {
            case DebugSymbol::Statement:
               level++;
               break;
            case DebugSymbol::EndOfStatement:
               level--;
               break;
            case DebugSymbol::End:
               level = 0;
               break;
            default:
               break;
         }
      }
   }
   else next = &next[1];

   while ((next->symbol & DebugSymbol::DebugMask) != DebugSymbol::Breakpoint) {
      if (next->symbol == DebugSymbol::End)
         return nullptr;

      next = &next[1];
   }

   return next;
}

DebugLineInfo* DebugInfoProvider :: seekClassInfo(addr_t address, IdentifierString& className, addr_t vmtAddress, ref_t flags)
{
   if (!vmtAddress || vmtAddress == INVALID_ADDR)
      return nullptr;

   addr_t position = _classes.get(vmtAddress);
   ModuleBase* module = getDebugModule(position);
   MemoryBase* section = (module != nullptr) ? module->mapSection(DEBUG_LINEINFO_ID, true) : nullptr;
   if (position != 0 && section != nullptr) {
      // to resolve class name we need offset in the section rather then the real address
      ref_t ref = (ref_t)(position - (addr_t)section->get(0));

      ustr_t name = module->resolveReference(ref);
      if (isWeakReference(name)) {
         className.copy(module->name());
         className.append(name);
      }
      else className.copy(name);

      return (DebugLineInfo*)position;
   }

   return nullptr;
}

// --- DebugController ---

DebugController :: DebugController(IDEDebugProcessBase* process, ProjectModel* model,
   SourceViewModel* sourceModel, DebugSourceController* sourceController)
   : _provider(model), _startUpSettings({})
{
   _started = false;
   _process = process;
   _running = false;
   _sourceModel = sourceModel;
   _model = model;
   _currentPath = nullptr;
   _sourceController = sourceController;
}

void DebugController :: debugThread()
{
   if (!_process->startProgram(_debuggee.str(), _arguments.str(), *_model->paths.appPath, _startUpSettings)) {
      //HOTFIX : to inform the listening thread
      _process->resetEvent(DEBUG_ACTIVE);

      return;
   }

   _running = true;
   while (_process->isStarted()) {
      int event = _process->waitForAnyEvent();

      switch (event) {
         case DEBUG_ACTIVE:
            if (_process->Exception()) {
               processStep();
               _process->resetException();
               _process->run();
            }
            else if (!_process->proceed(100)) {
               _process->resetEvent(DEBUG_ACTIVE);
               _running = false;
               processStep();
            }
            else _process->run();

            break;
         case DEBUG_RESUME:
            _running = true;
            _process->run();
            _process->resetEvent(DEBUG_RESUME);
            _process->setEvent(DEBUG_ACTIVE);

            break;
         case DEBUG_SUSPEND:
            _running = false;
            _process->resetEvent(DEBUG_SUSPEND);
            _process->resetEvent(DEBUG_ACTIVE);
            break;
         case DEBUG_CLOSE:
            _process->stop();
            _process->setEvent(DEBUG_ACTIVE);
            _process->resetEvent(DEBUG_CLOSE);
            break;
         default:
            break;
      }
   }
   _running = false;
   _currentModule.clear();
   _process->clearEvents();

   onStop();
}

void DebugController :: onInitBreakpoint()
{
   bool starting = _provider.getDebugInfoSize() == 0;
   if (starting) {
      //HOTFIX : due to current implementation - setting position as a base address
      DebugReader reader(_process, 0, (pos_t)_process->getBaseAddress());

      // define if it is a vm client or stand-alone
      char signature[0x10];
      _process->findSignature(reader, signature, 0x10);

      if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) == 0) {
         if (DebugInfoProviderBase::loadDebugInfo(*_debuggee, &_provider, _process)) {
            _sourceController->traceStart(_model);
         }
         // otherwise continue
         else {
            _provider.setDebugInfoSize(4);
            _process->setEvent(DEBUG_RESUME);
         }
      }
   //   else if (strncmp(signature, ELENACLIENT_SIGNITURE, strlen(ELENACLIENT_SIGNITURE)) == 0) {
   //      reader.seek(_debugger.getBaseAddress());

   //      if (!_debugger.initDebugInfo(false, reader, _debugInfoPtr)) {
   //         // continue debugging
   //         _events.setEvent(DEBUG_RESUME);
   //         return;
   //      }

   //      DebugReader reader(&_debugger, _debugInfoPtr, 0);
   //      // the debug section starts with size field
   //      reader.setSize(reader.getDWord());

   //      _debugInfoSize = 4;
   //      reader.seek(_debugInfoSize);

   //      loadDebugSection(reader, starting);
   //   }
      // !! notify if the executable is not supported
      else _provider.setDebugInfoSize(4);
   }

   addr_t entryAddr = _provider.getEntryPoint();
   if (_provider.getEntryPoint()) {
      if (_postponed.stepMode) {
         // continue debugging
         if (starting) {
            _process->setBreakpoint(entryAddr, false);
         }
         else _process->setStepMode();

         _process->setEvent(DEBUG_RESUME);
      }
      else if (_postponed.gotoMode) {
         runToCursor(*_postponed.source, *_postponed.path, _postponed.row);
      }
      // otherwise continue
      else run();
   }
   else _process->setEvent(DEBUG_RESUME);
}

void DebugController :: processStep()
{
   if (_process->isTrapped()) {
      if (_process->isInitBreakpoint()) {
         onInitBreakpoint();

         return;
      }

      IdentifierString moduleName;
      ustr_t sourcePath = nullptr;
      DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState(), moduleName, sourcePath);

      if (_postponed.autoNextLine) {
         if (lineInfo->row == _postponed.row) {
            autoStepOver();

            return;
         }

         _postponed.autoNextLine = false;
      }
      onCurrentStep(lineInfo, *moduleName, sourcePath);
   }
}

void DebugController :: onCurrentStep(DebugLineInfo* lineInfo, ustr_t moduleName, ustr_t sourcePath)
{
   if (lineInfo) {
      bool found = true;
      if (!_currentModule.compare(moduleName) || !sourcePath.compare(_currentPath)) {
         _currentModule.copy(moduleName);
         _currentPath = sourcePath;

         PathString path(sourcePath);
         found = _sourceController->selectSource(_model, _sourceModel, moduleName, *path);
      }

      _sourceController->traceStep(_sourceModel, found, lineInfo->row);
   }
   else _sourceController->traceStep(_sourceModel, false, -1);
}

void DebugController :: onStop()
{
   _started = false;
   _process->reset();
   _provider.clear();

   _currentModule.clear();
   _currentPath = nullptr;

   _sourceController->traceFinish(_sourceModel);
}

void DebugController :: run()
{
   if (_running || !_process->isStarted())
      return;

   if (_provider.getDebugInfoPtr() == 0 && _provider.getEntryPoint() != 0) {
      _process->setBreakpoint(_provider.getEntryPoint(), false);
      _postponed.clear();
   }
   _started = true;

   _process->setEvent(DEBUG_RESUME);

   _process->activate();
}

void DebugController :: runToCursor(ustr_t ns, ustr_t path, int row)
{
   if (_running || !_process->isStarted())
      return;

   if (!_started) {
      if (_provider.getDebugInfoPtr() == 0 && _provider.getEntryPoint() != 0) {
         _process->setBreakpoint(_provider.getEntryPoint(), false);
         _postponed.setGotoMode(0, row, ns, path);
      }
      _started = true;
   }
   else {
      ModuleBase* currentModule = _provider.resolveModule(ns);
      if (currentModule != nullptr) {
         addr_t address = _provider.findNearestAddress(currentModule, path, row);
         if (address != INVALID_ADDR) {
            _process->setBreakpoint(address, false);
         }
      }
   }

   _process->setEvent(DEBUG_RESUME);
}

void DebugController :: toggleBreakpoint(Breakpoint* bp, bool adding)
{
   ModuleBase* currentModule = _provider.resolveModule(*bp->module);
   if (currentModule != nullptr) {
      addr_t address = _provider.findNearestAddress(currentModule, *bp->source, bp->row);
      if (address != INVALID_ADDR) {
         if (adding) {
            _process->addBreakpoint(address);
         }
         else _process->removeBreakpoint(address);
      }
   }
}

bool isNeedAutoStep(DebugLineInfo* currentStep, DebugLineInfo* nextStep)
{
   return nextStep->addresses.step.address == currentStep->addresses.step.address || nextStep->row == currentStep->row || nextStep->symbol == DebugSymbol::VirtualBreakpoint;
}

void DebugController :: stepInto()
{
   if (_running || !_process->isStarted())
      return;

   if (!_started) {
      if (_provider.getDebugInfoPtr() == 0 && _provider.getEntryPoint() != 0) {
         _process->setBreakpoint(_provider.getEntryPoint(), false);
         _postponed.setStepMode();
      }
      _started = true;
   }
   else {
      DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState());

      //// if debugger should notify on the step result
      //if (test(lineInfo->symbol, dsProcedureStep))
      //   _debugger.setCheckMode();

      DebugLineInfo* nextStep = _provider.getNextStep(lineInfo, false);
      // if the address is the same perform the virtual step
      if (nextStep && nextStep->addresses.step.address == lineInfo->addresses.step.address) {
         _process->setStepMode();
      }
      /*else if (test(lineInfo->symbol, dsAtomicStep)) {
         _debugger.setBreakpoint(nextStep->addresses.step.address, true);
      }*/
      // else set step mode
      else _process->setStepMode();
   }

   _process->setEvent(DEBUG_RESUME);
}

void DebugController :: autoStepOver()
{
   stepOver();
}

void DebugController :: stepOver()
{
   if (_running || !_process->isStarted())
      return;

   if (!_started) {
      if (_provider.getDebugInfoPtr() == 0 && _provider.getEntryPoint() != 0) {
         _process->setBreakpoint(_provider.getEntryPoint(), false);
         _postponed.setStepMode();
      }
      _started = true;
   }
   else {
      DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState());
      DebugLineInfo* nextStep = _provider.getNextStep(lineInfo, true);
      if (nextStep) {
         if (nextStep && isNeedAutoStep(lineInfo, nextStep)) {
            _postponed.autoNextLine = true;
            _postponed.row = lineInfo->row;

            if (nextStep->symbol == DebugSymbol::VirtualBreakpoint) {
               _process->setBreakpoint(nextStep->addresses.step.address, true);
            }
            else _process->setStepMode();
         }
         else _process->setBreakpoint(nextStep->addresses.step.address, true);
      }
      // else set step mode
      else _process->setStepMode();
   }

   _process->setEvent(DEBUG_RESUME);
}

bool DebugController :: startThread()
{
   _process->initEvents();
   _process->setEvent(DEBUG_SUSPEND);

   _process->startThread(this);

   while (_process->waitForEvent(DEBUG_ACTIVE, 0));

   return _process->isStarted();
}

void DebugController :: clearBreakpoints()
{

}

bool DebugController :: start(path_t programPath, path_t arguments, bool debugMode, StartUpSettings startUpSettings)
{
   _currentModule.clear();
   _debuggee.copy(programPath);
   _arguments.copy(arguments);
   _startUpSettings = startUpSettings;

   if (debugMode) {
      addr_t entryPoint = _process->findEntryPoint(programPath);
      if (entryPoint == INVALID_ADDR)
         return false;

      _provider.setEntryPoint(entryPoint);

      _process->initHook();
   }
   else {
      _provider.setEntryPoint(0);

      clearBreakpoints();
   }

   return startThread();
}

void DebugController :: stop()
{
   if (_process->isStarted()) {
      _process->setEvent(DEBUG_CLOSE);
   }
}

void DebugController :: readObjectContent(ContextBrowserBase* watch, void* item, addr_t address, int level,
   DebugLineInfo* info, addr_t vmtAddress)
{
   WatchContext context = { item, address };

   if (!vmtAddress)
      vmtAddress = _process->getClassVMT(address);

   int flags = _process->getClassFlags(vmtAddress);

   int type = flags & elDebugMask;
   switch (type) {
      case elDebugDWORD:
         watch->populateDWORD(&context, _process->getDWORD(address));
         break;
      case elDebugQWORD:
         watch->populateQWORD(&context, _process->getQWORD(address));
         break;
      case elDebugFLOAT64:
         watch->populateFLOAT64(&context, _process->getFLOAT64(address));
         break;
      case elDebugLiteral:
      {
         char value[DEBUG_MAX_STR_LENGTH + 1];
         size_t length = _min(_process->getArrayLength(address), DEBUG_MAX_STR_LENGTH);
         _process->readDump(address, value, (pos_t)length);
         value[length] = 0;
         watch->populateString(&context, value);
         break;
      }
      case elDebugWideLiteral:
      {
         wide_c value[DEBUG_MAX_STR_LENGTH + 1];
         size_t length = _min(_process->getArrayLength(address), DEBUG_MAX_STR_LENGTH) >> 1;
         _process->readDump(address, (char*)value, (pos_t)(length << 1));
         value[length] = 0;
         watch->populateWideString(&context, value);
         break;
      }
      case elDebugArray:
         readObjectArray(watch, item, address, level, info);
         break;
      case elDebugDWORDS:
         readIntArrayLocal(watch, item, address, "content", level);
         break;
      case elDebugFLOAT64S:
         readRealArrayLocal(watch, item, address, "content", level);
         break;
      default:
         readFields(watch, item, address, level, info);
         break;
   }
}

void* DebugController :: readObject(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level, ustr_t className, addr_t vmtAddress)
{
   if (address != 0) {
      // read class VMT address if not provided
      IdentifierString classNameStr;

      if (!vmtAddress)
         vmtAddress = _process->getClassVMT(address);

      ref_t flags = 0;
      if (!emptystr(className)) {
         classNameStr.copy(className);
      }

      DebugLineInfo* info = _provider.seekClassInfo(address, classNameStr, vmtAddress, flags);

      WatchContext context = { parent, address };
      void* item = watch->addOrUpdate(&context, name, *classNameStr);

      if (level > 0)
         readObjectContent(watch, item, address, level, info, vmtAddress);

      return item;
   }
   else {
      WatchContext context = { parent, 0 };
      void* item = watch->addOrUpdate(&context, name, nullptr);

      return item;
   }
}

void* DebugController :: readByteArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      size_t length = _min(_process->getArrayLength(address), 100);

      WatchContext context = { parent, address };
      void* item = watch->addOrUpdate(&context, name, "<bytearray>");

      IdentifierString value;
      for (size_t i = 0; i < length; i++) {
         unsigned char b = _process->getBYTE(address + i);

         value.copy("[");
         value.appendInt(i);
         value.append("]");

         WatchContext context = { item, address + i };
         watch->addOrUpdateBYTE(&context, *value, b);
      }

      return item;
   }
   else return nullptr;
}

void* DebugController :: readShortArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      size_t length = _min(_process->getArrayLength(address) >> 1, 100);

      WatchContext context = { parent, address };
      void* item = watch->addOrUpdate(&context, name, "<shortarray>");

      IdentifierString value;
      for (size_t i = 0; i < length; i++) {
         unsigned short b = _process->getWORD(address + i * 2);

         value.copy("[");
         value.appendInt(i);
         value.append("]");

         WatchContext context = { item, address + i * 2};
         watch->addOrUpdateWORD(&context, *value, b);
      }

      return item;
   }
   else return nullptr;
}

void* DebugController :: readIntArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      size_t length = _min(_process->getArrayLength(address) >> 2, 100);

      WatchContext context = { parent, address };
      void* item = watch->addOrUpdate(&context, name, "<intarray>");

      IdentifierString value;
      for (size_t i = 0; i < length; i++) {
         unsigned int b = _process->getDWORD(address + i * 4);

         value.copy("[");
         value.appendInt((int)i);
         value.append("]");

         WatchContext context = { item, address + i * 4};
         watch->addOrUpdateDWORD(&context, *value, b);
      }

      return item;
   }
   else return nullptr;
}

void* DebugController :: readRealArrayLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      size_t length = _min(_process->getArrayLength(address) >> 3, 100);

      WatchContext context = { parent, address };
      void* item = watch->addOrUpdate(&context, name, "<realarray>");

      IdentifierString value;
      for (size_t i = 0; i < length; i++) {
         double b = _process->getFLOAT64(address + i * 8);

         value.copy("[");
         value.appendInt((int)i);
         value.append("]");

         WatchContext context = { item, address + i * 8 };
         watch->addOrUpdateFLOAT64(&context, *value, b);
      }

      return item;
   }
   else return nullptr;
}

void* DebugController :: readIntLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      int value = _process->getDWORD(address);

      WatchContext context = { parent, address };
      return watch->addOrUpdateDWORD(&context, name, value);
   }
   else return nullptr;
}

void* DebugController :: readUIntLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      unsigned int value = _process->getDWORD(address);

      WatchContext context = { parent, address };
      return watch->addOrUpdateUINT(&context, name, value);
   }
   else return nullptr;
}

void* DebugController :: readByteLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      unsigned int value = _process->getBYTE(address);

      WatchContext context = { parent, address };
      return watch->addOrUpdateDWORD(&context, name, value);
   }
   else return nullptr;
}

void* DebugController :: readShortLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      unsigned int value = _process->getWORD(address);

      WatchContext context = { parent, address };
      return watch->addOrUpdateDWORD(&context, name, value);
   }
   else return nullptr;
}

void* DebugController :: readLongLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      long long value = _process->getQWORD(address);

      WatchContext context = { parent, address };
      return watch->addOrUpdateQWORD(&context, name, value);
   }
   else return nullptr;
}

void* DebugController :: readRealLocal(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level)
{
   if (level > 0) {
      double value = _process->getFLOAT64(address);

      WatchContext context = { parent, address };
      return watch->addOrUpdateFLOAT64(&context, name, value);
   }
   else return nullptr;
}

void DebugController :: readObjectArray(ContextBrowserBase* watch, void* parent, addr_t address, int level, DebugLineInfo* info)
{
   if (level <= 0)
      return;

   int length = (int)_min(_process->getArrayLength(address) / sizeof(addr_t), 100);
   IdentifierString value;
   for (int i = 0; i < length; i++) {
      addr_t itemAddress = _process->getField(address, i);

      value.copy("[");
      value.appendInt(i);
      value.append("]");

      readObject(watch, parent, itemAddress, *value, level - 1, nullptr);
   }
}

void* DebugController :: readFieldValue(ContextBrowserBase* watch, void* parent, addr_t address, ustr_t name, int level, int size, ustr_t className)
{
   if (level > 0) {
      if (size == 4) {
         return readUIntLocal(watch, parent, address, name, level);
      }
      else if (size == 2) {
         return readShortLocal(watch, parent, address, name, level);
      }
      else if (size == 1) {
         return readByteLocal(watch, parent, address, name, level);
      }
      else if (size == 8) {
         return readLongLocal(watch, parent, address, name, level);
      }
      else {
         WatchContext context = { parent, address };
         void* item = watch->addOrUpdate(&context, name, className);

         return item;
      }
   }
   else return nullptr;
}

void DebugController :: readFields(ContextBrowserBase* watch, void* parent, addr_t address, int level, DebugLineInfo* info)
{
   if (level <= 0 || info == nullptr)
      return;

   size_t index = 1;
   while (info[index].symbol == DebugSymbol::Field || info[index].symbol == DebugSymbol::FieldAddress) {
      bool isStruct = info[index].symbol == DebugSymbol::FieldAddress;
      ustr_t name = (const char*)info[index].addresses.field.nameRef;
      addr_t fieldAddress = 0;
      ustr_t className = nullptr;
      if (!isStruct) {
         fieldAddress = _process->getField(address, info[index].addresses.field.offset);
      }
      else {
         fieldAddress = _process->getFieldAddress(address, info[index].addresses.field.offset);
      }
      index++;
      int size = 0;
      if (info[index].symbol == DebugSymbol::FieldInfo) {
         className = (const char*)info[index].addresses.info.nameRef;
         size = info[index].addresses.info.size;

         index++;
      }

      if (isStruct && size != 0) {
         readFieldValue(watch, parent, fieldAddress, name, level - 1, size, className);
      }
      else readObject(watch, parent, fieldAddress, name, level - 1, className);
   }
}

void DebugController :: readContext(ContextBrowserBase* watch, void* parentItem, addr_t address, int level)
{
   if (_process->isStarted() && level > 0) {
      addr_t vmtAddress = _process->getClassVMT(address);
      ref_t flags = vmtAddress ? _process->getClassFlags(vmtAddress) : 0;

      IdentifierString className;
      DebugLineInfo* info = _provider.seekClassInfo(address, className, vmtAddress, flags);

      readObjectContent(watch, parentItem, address, level, info, vmtAddress);
   }
}

inline int getFPOffset(int argument, int argOffset)
{
   return (argument - (argument < 0 ? argOffset : 0));
}

inline disp_t getFrameDisp(DebugLineInfo& frameInfo, disp_t offset)
{
   if (frameInfo.symbol == DebugSymbol::FrameInfo && frameInfo.addresses.offset.disp != 0) {
      return frameInfo.addresses.offset.disp + offset;
   }
   else return 0;
}

inline const char* getClassInfo(DebugLineInfo& frameInfo)
{
   if (frameInfo.symbol == DebugSymbol::ParameterInfo || frameInfo.symbol == DebugSymbol::LocalInfo) {
      return (const char*)frameInfo.addresses.source.nameRef;
   }
   else return nullptr;
}

void DebugController :: readAutoContext(ContextBrowserBase* watch, int level, WatchItems* refreshedItems)
{
   if (_process->isStarted()) {
      DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState());
      if (lineInfo == nullptr)
         return;

      int index = 0;
      while (lineInfo[index].symbol != DebugSymbol::Procedure) {
         void* item = nullptr;
         switch (lineInfo[index].symbol) {
            case DebugSymbol::Local:
               item = readObject(watch, nullptr, _process->getStackItem(lineInfo[index].addresses.local.offset),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::IntLocalAddress:
               item = readIntLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::UIntLocalAddress:
               item = readUIntLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::ByteArrayAddress:
               item = readByteArrayLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::ByteArrayParameter:
               item = readByteArrayLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::ShortArrayAddress:
               item = readShortArrayLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::ShortArrayParameter:
               item = readShortArrayLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::IntArrayAddress:
               item = readIntArrayLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::RealArrayAddress:
               item = readRealArrayLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::IntArrayParameter:
               item = readIntArrayLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::RealArrayParameter:
               item = readRealArrayLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::LongLocalAddress:
               item = readLongLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::RealLocalAddress:
               item = readRealLocal(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::Parameter:
               item = readObject(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::IntParameterAddress:
               item = readIntLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                     (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::LongParameterAddress:
               item = readLongLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                     (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::RealParameterAddress:
               item = readRealLocal(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                     (const char*)lineInfo[index].addresses.local.nameRef, level - 1);
               break;
            case DebugSymbol::ParameterAddress:
            {
               ustr_t className = getClassInfo(lineInfo[index + 2]);
               addr_t vmtAddress = _provider.getClassAddress(className);

               item = readObject(watch, nullptr,
                  _process->getStackItem(
                     lineInfo[index].addresses.local.offset, -getFrameDisp(lineInfo[index + 1], _process->getDataOffset() * 2) - _process->getDataOffset()),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1,
                  className, vmtAddress);
               break;
            }
            case DebugSymbol::LocalAddress:
            {
               ustr_t className = getClassInfo(lineInfo[index + 1]);
               addr_t vmtAddress = _provider.getClassAddress(className);

               item = readObject(watch, nullptr,
                  _process->getStackItemAddress(getFPOffset(lineInfo[index].addresses.local.offset, _process->getDataOffset())),
                  (const char*)lineInfo[index].addresses.local.nameRef, level - 1,
                  className, vmtAddress);
               break;
            }
            default:
               break;
         }

         if (item)
            refreshedItems->add(item);

         index--;
      }
   }
}

void DebugController::resolveNamespace(NamespaceString& ns)
{
   _provider.fixNamespace(ns);
}
