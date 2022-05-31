//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implematioon of the DebugController class and
//    its helpers
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "debugcontroller.h"
#include "module.h"

using namespace elena_lang;

// --- LibraryProvider ---

inline bool isSymbolReference(ustr_t name)
{
   size_t pos = name.findLast('\'');
   if (pos != NOTFOUND_POS) {
      return name[pos + 1] == '#';
   }
   else return false;
}

inline ref_t mapModuleReference(ModuleBase* module, ustr_t referenceName, bool existing)
{
   if (!isWeakReference(referenceName)) {
      return module->mapReference(referenceName + getlength(module->name()), existing);
   }
   else return module->mapReference(referenceName, existing);
}

void DebugInfoProvider :: retrievePath(ustr_t name, PathString& path, path_t extension)
{
   ustr_t package = _model->getPackage();

   // if it is the root package
   if (package.compare(name)) {
      path.copy(*_model->projectPath);
      path.combine(_model->getOutputPath());

      ReferenceName::nameToPath(path, name);
      path.appendExtension(extension);
   }
   // if the class belongs to the project package
   else if (name.compare(package, package.length()) && (name[package.length()] == '\'')) {
      path.copy(*_model->projectPath);
      path.combine(_model->getOutputPath());

      ReferenceName::nameToPath(path, name);
      path.appendExtension(extension);
   }
   else {
      // if file doesn't exist use package root
      path.copy(*_model->paths.libraryRoot);

      ReferenceName::nameToPath(path, name);
      path.appendExtension(extension);
   }
}

ModuleBase* DebugInfoProvider :: loadDebugModule(ustr_t reference)
{
   NamespaceString name(reference);
   PathString      path;

   // check if the module is already loaded
   auto it = _modules.start();
   while (!it.eof()) {
      if (NamespaceString::isIncluded((*it)->name(), reference)) {
         ustr_t properName = reference + (*it)->name().length();
         if ((*it)->mapReference(properName, true))
            return *it;
      }
      it++;
   }

   Module* module = nullptr;
   while (!module && !name.empty()) {
      retrievePath(*name, path, _T("dnl"));

      if (!_modules.exist(*name)) {
         module = new Module();

         FileReader reader(*path, FileRBMode, FileEncoding::Raw, false);
         LoadResult result = module->load(reader);
         if (result == LoadResult::Successful) {
            ustr_t relativeName = reference.str() + module->name().length();
            if (relativeName[0] == '#')
               relativeName = relativeName + 1;

            if (module->mapReference(relativeName, true) != 0) {
               _modules.add(*name, module);
            }
            else {
               delete module;
               module = nullptr;

               name.trimLastSubNs();
            }
         }
         else {
            delete module;
            module = nullptr;

            name.trimLastSubNs();
         }
      }
      else name.trimLastSubNs();

   }
   return module;
}

bool DebugInfoProvider :: loadSymbol(ustr_t reference, StreamReader& addressReader, DebugProcessBase* process)
{
   bool isClass = true;
   ModuleBase* module = nullptr;
   // if symbol
   if (isSymbolReference(reference)) {
      module = loadDebugModule(reference);
      isClass = false;
   }
   else module = loadDebugModule(reference);

   pos_t position = 0;
   /*if (reference.find('@') != NOTFOUND_POS && reference.find('#', 0) > 0) {
      position = (module != NULL) ? mapModuleReference(module, reference + reference.find('\'', 0), true) : 0;
   }
   else */position = (module != nullptr) ? mapModuleReference(module, reference, true) : 0;
   if (position != 0) {
      // place reader on the next after symbol record
      MemoryReader reader(module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true), position);
      MemoryReader stringReader(module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true));

      // map vmt address for a class
      if (isClass) {
         addr_t vmtPtr = 0;
         addressReader.read(&vmtPtr, sizeof(addr_t));

         if (vmtPtr != 0) {
            _classes.add(vmtPtr, (addr_t)reader.address());
            _classNames.add(reference, vmtPtr);
            if (reference.find('@') != NOTFOUND_POS && reference.find('#', 0) > 0) {
               IdentifierString weakName("'$auto", reference + getlength(module->name()));

               _classNames.add(*weakName, vmtPtr);
            }
         }
      }
      // skip symbol entry address
      else {
         addr_t dummy = 0;
         addressReader.read(&dummy, sizeof(addr_t));
      }

      // start to read lineinfo until end symbol
      DebugLineInfo info;
      void* current = nullptr;
      int level = 1;
      while (level > 0 && !reader.eof()) {
         current = reader.address();

         reader.read(&info, sizeof(DebugLineInfo));
         switch (info.symbol) {
            case DebugSymbol::Statement:
            case DebugSymbol::Procedure:
               level++;
               break;
            case DebugSymbol::End:
               level--;
               break;
               //else if (info.symbol == dsField || (info.symbol & ~dsTypeMask) == dsLocal || info.symbol == dsFieldInfo)
               //{
               //   // replace field name reference with the name
               //   stringReader.seek(info.addresses.symbol.nameRef);

               //   ((DebugLineInfo*)current)->addresses.symbol.nameRef = mapDebugPTR32(stringReader.Address());
               //}
            case DebugSymbol::Breakpoint:
            {
               addr_t stepAddress = 0;
               addressReader.read(&stepAddress, sizeof(addr_t));

               ((DebugLineInfo*)current)->addresses.step.address = stepAddress;
               // virtual end of expression should be stepped over automatically by debugger
               //if (info.symbol != dsVirtualEnd)
               process->addStep(stepAddress, (void*)current);

               break;
            }
            default:
               break;
         }
      }
      return true;
   }
   else return false;
}

bool DebugInfoProvider :: load(StreamReader& reader, bool setEntryAddress, DebugProcessBase* process)
{
   IdentifierString reference;
   if (setEntryAddress) {
      // read entry point
      reader.read(&_entryPoint, sizeof(_entryPoint));

      if (_entryPoint != 0)
         setEntryAddress = false;

      // skip project namespace
      reader.readString(reference);
   }

   while (!reader.eof()) {
      // read reference
      reader.readString(reference);

      // define the next record position
      pos_t size = reader.getPos() - 4;
      pos_t nextPosition = reader.position() + size;

      if (setEntryAddress) {
         pos_t pos = reader.position();

         // if entry address was not defined take the first one
         reader.read(&_entryPoint, sizeof(_entryPoint));

         reader.seek(pos);
         setEntryAddress = false;
      }

      //// if it is a VM temporal symbol - skip it
      //if (reference.compare(TAPE_SYMBOL)/* && _debugTape*/) {
      ////      loadTapeDebugInfo(reader, size);
      //}
      // otherwise load standard debug info
      /*else */loadSymbol(*reference, reader, process);

      reader.seek(nextPosition);
   }

   return true;
}

ModuleBase* DebugInfoProvider :: getDebugModule(addr_t address)
{
   ModuleMap::Iterator it = _modules.start();
   while (!it.eof()) {
      MemoryBase* section = (*it)->mapSection(DEBUG_LINEINFO_ID, true);
      if (section != nullptr) {
         addr_t starting = (addr_t)section->get(0);
         addr_t len = section->length();
         if (starting <= address && (address - starting) < len) {
            return *it;
         }
      }
      ++it;
   }
   return nullptr;
}

DebugLineInfo* DebugInfoProvider :: seekDebugLineInfo(addr_t lineInfoAddress, ustr_t& moduleName, ustr_t& sourcePath)
{
   ModuleBase* module = getDebugModule(lineInfoAddress);
   if (module) {
      moduleName = module->name();

      //DebugLineInfo* current = (DebugLineInfo*)lineInfoAddress;
      //while (current->symbol != DebugSymbol::Procedure/* && current->symbol != dsCodeInfo*/)
      //   current = &current[-1];

      //MemoryBase* section = module->mapSection(DEBUG_STRINGS_ID, true);

      //if (section != NULL) {
      //   sourcePath = (const char*)section->get(current->addresses.source.nameRef);
      //}

      return (DebugLineInfo*)lineInfoAddress;
   }
   else return nullptr;
}

DebugLineInfo* DebugInfoProvider :: getNextStep(DebugLineInfo* step, bool stepOverMode)
{
   DebugLineInfo* next = step;
   /*if (stepOverMode) {
      int level = 1;
      while (level > 0) {
         next = &next[1];
         switch (next->symbol) {
            case dsVirtualBlock:
               level++;
               break;
            case dsVirtualEnd:
               level--;
               break;
            case dsEnd:
               return nullptr;
            case dsEOP:
               level = 0;
               break;
            default:
               break;
         }

      }
   }
   else*/ next = &next[1];

   while (next->symbol  != DebugSymbol::Breakpoint) {
      if (next->symbol == DebugSymbol::End)
         return nullptr;

      next = &next[1];
   }

   return next;
}

// --- DebugController ---

DebugController :: DebugController(DebugProcessBase* process, ProjectModel* model, 
   SourceViewModel* sourceModel, NotifierBase* notifier)
   : _provider(model)
{
   _started = false;
   _process = process;
   _running = false;
   _sourceModel = sourceModel;
   _notifier = notifier;
}

void DebugController :: debugThread()
{
   if (!_process->startProgram(_debuggee.str(), _arguments.str())) {
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
   //_currentModule = nullptr;
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
         PathString debugDataPath(_debuggee);
         debugDataPath.changeExtension(_T("dn"));

         FileReader reader(*debugDataPath, FileRBMode, FileEncoding::Raw, false);
         char header[8];
         reader.read(header, 8);
         if (ustr_t(DEBUG_MODULE_SIGNATURE).compare(header, 5)) {
            _provider.setDebugInfo(reader.getDWord(), INVALID_ADDR);

            loadDebugSection(reader, starting);
         }
         else _provider.setDebugInfoSize(4);
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
      //else if (_postponed.gotoMode) {
      //   runToCursor(_postponed.source, _postponed.path.c_str(), _postponed.col, _postponed.row);
      //}
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

      ustr_t moduleName = nullptr;
      ustr_t sourcePath = nullptr;
      DebugLineInfo* lineInfo = _provider.seekDebugLineInfo((addr_t)_process->getState(), moduleName, sourcePath);
      /*if (lineInfo->symbol == dsAssemblyStep) {
         size_t objectPtr = _debugger.Context()->LocalPtr(1);
         int flags = _debugger.Context()->VMTFlags(_debugger.Context()->ClassVMT(objectPtr));
         //if (test(flags, elTapeGroup)) {
         //   loadTapeDebugInfo(objectPtr);
         //   _autoStepInto = true;
         //}
         //else {
            // continue debugging if it is not a tape
         stepInto();
         //}
      }
      else */onCurrentStep(lineInfo, moduleName, sourcePath);
   }
   //if (_debugger.Context()->checkFailed) {
   //   _listener->onCheckPoint(_T("Operation failed"));
   //}
   //if (_debugger.Exception() != NULL) {
   //   ProcessException* exeption = _debugger.Exception();

   //   _listener->onNotification(exeption->Text(), exeption->address, exeption->code);
   //}
}

void DebugController :: onCurrentStep(DebugLineInfo* lineInfo, ustr_t moduleName, ustr_t sourcePath)
{
   if (lineInfo) {
      _sourceModel->setTraceLine(lineInfo->row, true);

      _notifier->notifyModelChange(NOTIFY_SOURCEMODEL);

      //if (!moduleName.compare(_currentModule) || !sourcePath.compare(_currentSource)) {
      //   //!! do we need it at all?
      //   //onLoadModule(moduleName, sourcePath);
      //   _currentModule = moduleName;
      //   _currentSource = sourcePath;
      //}
      //_listener->onStep(moduleName, sourcePath, lineInfo->row, lineInfo->col, lineInfo->length);
   }
}

void DebugController :: onStop()
{
   _started = false;
   _process->reset();
   _provider.clear();

   _sourceModel->clearTraceLine();
   _notifier->notifyModelChange(NOTIFY_SOURCEMODEL);
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
         /*if (nextStep->symbol != dsVirtualEnd) {
            _debugger.processVirtualStep(nextStep);
            processStep();
            return;
         }
         else */_process->setStepMode();
      }
      /*else if (test(lineInfo->symbol, dsAtomicStep)) {
         _debugger.setBreakpoint(nextStep->addresses.step.address, true);
      }*/
      // else set step mode
      else _process->setStepMode();
   }

   _process->setEvent(DEBUG_RESUME);
}

void DebugController :: stepOver()
{
   if (_running || !_process->isStarted())
      return;

   if (!_started) {
      
   }
   else {

   }
}

bool DebugController :: startThread()
{
   _process->initEvents();
   _process->setEvent(DEBUG_SUSPEND);

   _process->startThread(this);

   while (_process->waitForEvent(DEBUG_ACTIVE, 0));

   //_listener->onStart();

   return _process->isStarted();
}

void DebugController :: clearBreakpoints()
{
   
}

bool DebugController :: start(path_t programPath, path_t arguments, bool debugMode)
{
   //_currentModule = NULL;
   _debuggee.copy(programPath);
   _arguments.copy(arguments);

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

void DebugController :: loadDebugSection(StreamReader& reader, bool starting)
{
   // if there are new records in debug section
   if (!reader.eof()) {
      _provider.load(reader, starting, _process);

      //_listener->onDebuggerHook();

      _provider.setDebugInfoSize(reader.position());

      //loadSubjectInfo(reader);
   }
   // otherwise continue
   else _process->setEvent(DEBUG_RESUME);
}
