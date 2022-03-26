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
            if (module->mapReference(reference.str() + module->name().length(), true) != 0) {
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
      while (!reader.eof()) {
         current = reader.address();

         reader.read(&info, sizeof(DebugLineInfo));
         switch (info.symbol) {
            case DebugSymbol::Procedure:
               level++;
               break;
            case DebugSymbol::End:
               if (level == 1) {
                  break;
               }
               else level--;
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


// --- DebugController ---

DebugController :: DebugController(DebugProcessBase* process, ProjectModel* model)
   : _provider(model)
{
   _started = false;
   _process = process;
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

   //_listener->onStop(_process->proceedCheckPoint());
}

void DebugController :: onInitBreakpoint()
{
   bool starting = _provider.getDebugInfoSize() == 0;
   if (starting) {
      DebugReader reader(_process, _process->getBaseAddress(), 0);

      // define if it is a vm client or stand-alone
      char signature[0x10];
      _process->findSignature(reader, signature, 0x10);

      if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) == 0) {
         PathString debugDataPath(_debuggee);
         debugDataPath.changeExtension(_T("dn"));

         FileReader reader(*debugDataPath, FileRBMode, FileEncoding::Raw, false);
         char header[5];
         reader.read(header, 5);
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

   if (_provider.getEntryPoint()) {
      //if (_postponed.stepMode) {
      //   // continue debugging
      //   if (starting) {
      //      _debugger.setBreakpoint(_entryPoint, false);
      //   }
      //   else _debugger.setStepMode();

      //   _events.setEvent(DEBUG_RESUME);
      //}
      //else if (_postponed.gotoMode) {
      //   runToCursor(_postponed.source, _postponed.path.c_str(), _postponed.col, _postponed.row);
      //}
      //// otherwise continue
      /*else */run();
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

   //   ident_t moduleName = NULL;
   //   ident_t sourcePath = NULL;
   //   DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State(), moduleName, sourcePath);
   //   if (lineInfo->symbol == dsAssemblyStep) {
   //      size_t objectPtr = _debugger.Context()->LocalPtr(1);
   //      int flags = _debugger.Context()->VMTFlags(_debugger.Context()->ClassVMT(objectPtr));
   //      //if (test(flags, elTapeGroup)) {
   //      //   loadTapeDebugInfo(objectPtr);
   //      //   _autoStepInto = true;
   //      //}
   //      //else {
   //         // continue debugging if it is not a tape
   //      stepInto();
   //      //}
   //   }
   //   else showCurrentModule(lineInfo, moduleName, sourcePath);
   }
   //if (_debugger.Context()->checkFailed) {
   //   _listener->onCheckPoint(_T("Operation failed"));
   //}
   //if (_debugger.Exception() != NULL) {
   //   ProcessException* exeption = _debugger.Exception();

   //   _listener->onNotification(exeption->Text(), exeption->address, exeption->code);
   //}
}

void DebugController :: run()
{
   if (_running || !_process->isStarted())
      return;

   if (_provider.getDebugInfoPtr() == 0 && _provider.getEntryPoint() != 0) {
      _process->setBreakpoint(_provider.getEntryPoint(), false);
      //_postponed.clear();
   }
   _started = true;

   _process->setEvent(DEBUG_RESUME);

   _process->activate();
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
   _started = false;

   _process->reset();

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
