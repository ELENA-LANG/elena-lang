//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implematioon of the DebugController class and
//      its helpers
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "debugcontroller.h"
#include "rtman.h"
//#include "messages.h"
#include "module.h"

using namespace _ELENA_;

// --- DebugController ---

pos_t DebugController :: mapDebugPTR32(void* address)
{
#ifdef _WIN64
   //!! temporal : does not work if the address > 2GB
   return (pos_t)(size_t)address;
#else
   return (pos_t)address;
#endif
}

void* DebugController :: unmapDebugPTR32(pos_t position)
{
#ifdef _WIN64
   //!! temporal : does not work if the address > 2GB
   return (void*)(size_t)position;
#else
   return (void*)position;
#endif
}

void DebugController :: debugThread()
{
   if (!_debugger.start(_debuggee, _arguments))
      return;

   _running = true;
   while (_debugger.isStarted()) {
      int event = _events.waitForAnyEvent();

      switch (event) {
         case DEBUG_ACTIVE:
            if (_debugger.Exception()) {
               processStep();
               _debugger.resetException();
               _debugger.run();
            }
            else if (!_debugger.proceed(100)) {
               _events.resetEvent(DEBUG_ACTIVE);
               _running = false;
               processStep();
            }
            else _debugger.run();

            break;
         case DEBUG_RESUME:
            _running = true;
            _debugger.run();
            _events.resetEvent(DEBUG_RESUME);
            _events.setEvent(DEBUG_ACTIVE);
            break;
         case DEBUG_SUSPEND:
            _running = false;
            _events.resetEvent(DEBUG_SUSPEND);
            _events.resetEvent(DEBUG_ACTIVE);
            break;
         case DEBUG_CLOSE:
            _debugger.stop();
            _events.setEvent(DEBUG_ACTIVE);
            _events.resetEvent(DEBUG_CLOSE);
            break;
      }
   }
   _running = false;
   _currentModule = NULL;
   _events.close();

   _listener->onStop(_debugger.proceedCheckPoint());
}

_Module* DebugController :: getDebugModule(size_t address)
{
   ModuleMap::Iterator it = _modules.start();
   while (!it.Eof()) {
      _Memory* section = (*it)->mapSection(DEBUG_LINEINFO_ID, true);
      if (section != NULL) {
         size_t starting = (size_t)section->get(0);
         if (starting <= address && (address - starting) < section->Length()) {
            return *it;
         }
      }
      it++;
   }
   return NULL;
}

DebugLineInfo* DebugController :: seekDebugLineInfo(size_t lineInfoAddress, ident_t &moduleName, ident_t &sourcePath)
{
   //// if it is a temporal tape line info
   //if (lineInfoAddress < _tape.Length()) {
   //   moduleName = _tapeSymbol;

   //   return (DebugLineInfo*)_tape.get(lineInfoAddress);
   //}
   //// otherwise standard approach is used: find the module where debug line info record is located
   //else {
      _Module* module = getDebugModule(lineInfoAddress);
      if (module) {
         moduleName = module->Name();

         DebugLineInfo* current = (DebugLineInfo*)lineInfoAddress;
         while (current->symbol != dsProcedure)
            current = &current[-1];

         _Memory* section = module->mapSection(DEBUG_STRINGS_ID, true);

         if (section != NULL) {
            sourcePath = (const char*)section->get(current->addresses.source.nameRef);
         }

         return (DebugLineInfo*)lineInfoAddress;
      }
      else return NULL;
//   }
}

DebugLineInfo* DebugController :: seekClassInfo(size_t address, ident_t &className, size_t& flags, size_t vmtPtr)
{
   // read class VMT address if not provided
   if (vmtPtr == 0) {
      vmtPtr = _debugger.Context()->ClassVMT(address);
   }

   // exit if the class is unknown
   if (vmtPtr==0)
      return NULL;

   // if it is role, read the role owner
   flags = _debugger.Context()->VMTFlags(vmtPtr);

   // get class debug info address
   size_t position = _classes.get(vmtPtr);
   _Module* module = getDebugModule(position);
   _Memory* section = (module != NULL) ? module->mapSection(DEBUG_LINEINFO_ID, true) : NULL;

   if (position != 0 && section != NULL) {
      // to resolve class name we need offset in the section rather then the real address
      className = module->resolveReference((ref_t)(position - (size_t)section->get_st(0)));

      return (DebugLineInfo*)position;
   }
   return NULL;
}

DebugLineInfo* DebugController :: getNextStep(DebugLineInfo* step, bool stepOverMode)
{
   DebugLineInfo* next = &step[1];

   // HOTFIX : nested brackets
   if (step->symbol == dsEOP && next->symbol == dsVirtualEnd)
      step = next;

   if (step->symbol != dsEnd && step->symbol != dsEOP) {
      next = &step[1];
      while ((next->symbol & dsDebugMask) != dsStep) {
         // step over virtual block if required
         if (next->symbol == dsVirtualBlock && stepOverMode) {
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
                     return NULL;
                  //case dsEOP:
                  //   level = 0;
                  //   break;
                  default:
                     break;
               }
            }
            return next;
         }
         next = &next[1];
      }

      return next;
   }
   return NULL;
}

DebugLineInfo* DebugController :: getEndStep(DebugLineInfo* step)
{
   if (step->symbol != dsEnd) {
      DebugLineInfo* next = &step[1];
      // go to the end of statement or procedure
      int level = 0;
      // NOTE : end-statements should be ignored if they are inside virtual blocks
      while (level > 0 || next->symbol != dsStatement) {
         switch (next->symbol) {
            case dsVirtualBlock:
               level++;
               break;
            case dsEOP:
            case dsEnd:
            case dsVirtualEnd:
               // NOTE : ends should be ignored if they are inside virtual blocks
               if (level == 0) {
                  return (next->symbol == dsEnd) ? NULL : next;
               }
               else level--;

               break;
            default:
               break;
         }

         next = &next[1];
      }

      return getNextStep(next, false);
   }
   return NULL;
}

DebugLineInfo* DebugController :: seekLineInfo(size_t address, ident_t &moduleName, ident_t &className,
   ident_t &methodName, ident_t &procPath)
{
   ModuleMap::Iterator it = _modules.start();
   while (!it.Eof()) {
      moduleName = (*it)->Name();

      _Memory* section = (*it)->mapSection(DEBUG_LINEINFO_ID, true);
      _Memory* strings = (*it)->mapSection(DEBUG_STRINGS_ID, true);
      if (section != NULL) {
         DebugLineInfo* info = (DebugLineInfo*)section->get(4);
         int count = section->Length() / sizeof(DebugLineInfo);
         int prev = 0;
         bool loaded = false;
         for (int i = 0 ; i < count ; i++) {
            if (info[i].symbol == dsClass || info[i].symbol == dsSymbol) {
               className = (const char*)strings->get(info[i].addresses.symbol.nameRef);
               if (info[i].symbol == dsClass) {
                  loaded = _classNames.exist(className);
               }
               else loaded = true;
            }
            else if (!loaded) {
               // skip not loaded classes
            }
            else if (info[i].symbol == dsProcedure) {
               procPath = (const char*)strings->get(info[i].addresses.source.nameRef);
               methodName = NULL;
               prev = 0;
            }
            else if (info[i].symbol == dsMessage) {
               methodName = (const char*)strings->get(info[i].addresses.source.nameRef);
            }
            else if ((info[i].symbol & dsDebugMask) == dsStep) {
               // if it is a exact match
               if (info[i].addresses.step.address == address) {
                  if (info[i].row < 0) {
                     // search for the lastest row info
                     while (i > 0 && info[i].row < 0)
                        i--;

                     return info + i;
                  }
                  else return info + i;
               }
               else if (prev != 0 && info[prev].addresses.step.address < address && info[i].addresses.step.address >= address && info[i].row >= 0) {
                  return info + i;
               }
               /*if (info[i].symbol == dsEOP) {
                  prev = 0;
               }
               else */prev = i;
            }
         }
      }
      it++;
   }
   return NULL;
}

size_t DebugController :: findNearestAddress(_Module* module, ident_t path, int row)
{
   _Memory* section = module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true);
   _Memory* strings = module->mapSection(DEBUG_STRINGS_ID, true);

   DebugLineInfo* info = (DebugLineInfo*)section->get(4);
   int count = section->Length() / sizeof(DebugLineInfo);

   // scanning the array of debug lines until closest step is found
   size_t address = (size_t)-1;
   int nearestRow = 0;
   bool skipping = true;
   for (int i = 0 ; i < count ; i++) {
      if (info[i].symbol == dsProcedure) {
         ident_t procPath = (const char*)strings->get(info[i].addresses.source.nameRef);
         if (procPath.compare(path)) {
            skipping = false;
         }
      }
      else if (info[i].symbol == dsEnd) {
         skipping = true;
      }
      else if (!skipping && (info[i].symbol & dsDebugMask) == dsStep) {
         if (__abs(nearestRow - row) > __abs(info[i].row - row)) {
            nearestRow = info[i].row;

            address = info[i].addresses.step.address;
            if (info[i].row == row)
               break;
         }
      }
   }
   return address;
}

void DebugController :: processStep()
{
   if (_debugger.isTrapped()) {
      if (_debugger.isInitBreakpoint()) {
         onInitBreakpoint();

         return;
      }

      ident_t moduleName = NULL;
      ident_t sourcePath = NULL;
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State(), moduleName, sourcePath);
      if (lineInfo->symbol == dsAssemblyStep) {
         size_t objectPtr = _debugger.Context()->LocalPtr(1);
         int flags = _debugger.Context()->VMTFlags(_debugger.Context()->ClassVMT(objectPtr));
         if (test(flags, elTapeGroup)) {
            loadTapeDebugInfo(objectPtr);
         }
         else {
            // continue debugging if it is not a tape
            stepInto();
         }
      }
      else showCurrentModule(lineInfo, moduleName, sourcePath);
   }
   if (_debugger.Context()->checkFailed) {
      _listener->onCheckPoint(_T("Operation failed"));
   }
   if (_debugger.Exception()!=NULL) {
      ProcessException* exeption = _debugger.Exception();

      _listener->onNotification(exeption->Text(), exeption->address, exeption->code);
   }
}

bool DebugController :: start()
{
   _events.init();
   _events.setEvent(DEBUG_SUSPEND);

   _debugger.startThread(this);

   while (_events.waitForEvent(DEBUG_ACTIVE, 0));

   _listener->onStart();

   return _debugger.isStarted();
}

void DebugController :: onInitBreakpoint()
{
   bool starting = _debugInfoSize == 0;
   if (starting) {
      DebugReader reader(&_debugger, 0, _debugger.getBaseAddress());

      // define if it is a vm client or stand-alone
      char signature[0x10];
      _debugger.findSignature(reader, signature);

      if (strcmp(signature, ELENACLIENT_SIGNITURE) == 0) {
         if (!_debugger.initDebugInfo(false, reader, _debugInfoPtr))
         {
            // continue debugging
            _events.setEvent(DEBUG_RESUME);
            return;
         }
      }
      else if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) == 0) {
         DebugReader reader(&_debugger);

         _debugger.initDebugInfo(true, reader, _debugInfoPtr);
      }
      // !! notify if the executable is not supported
      _debugInfoSize = 4;
   }

   DebugReader reader(&_debugger, _debugInfoPtr, 0);
   // the debug section starts with size field
   reader.setSize(reader.getDWord());
   reader.seek(_debugInfoSize);

   // if there are new records in debug section
   if (!reader.Eof()) {
      loadDebugData(reader, starting);

      _listener->onDebuggerHook();

      _debugInfoSize = reader.Position();

      // continue debugging
      if (_postponed.stepMode) {
         if (starting) {
            _debugger.setBreakpoint(_entryPoint, false);
         }
         else _debugger.setStepMode();

         _events.setEvent(DEBUG_RESUME);
      }
      else if (_postponed.gotoMode) {
         runToCursor(_postponed.source, _postponed.path.c_str(), _postponed.col, _postponed.row);
      }
      else run();
   }
   // otherwise continue
   else _events.setEvent(DEBUG_RESUME);
}
//
bool DebugController :: loadSymbolDebugInfo(ident_t reference, StreamReader&  addressReader)
{
   bool isClass = true;
   _Module* module = NULL;
   // if symbol
   if (reference[0]=='#') {
      module = loadDebugModule(reference + 1);
      isClass = false;
   }
   else module = loadDebugModule(reference);

   pos_t position = (module != NULL) ? module->mapReference(reference, true) : 0;
   if (position != 0) {
      // place reader on the next after symbol record
      MemoryReader reader(module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true), position);
      MemoryReader stringReader(module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true));

      // map vmt address for a class
      if (isClass) {
         ref_t vmtPtr = addressReader.getDWord();

         if (vmtPtr != 0) {
            _classes.add(vmtPtr, (size_t)reader.Address());
            _classNames.add(reference, vmtPtr);
         }
      }
      // skip symbol entry address
      else addressReader.getDWord();

      // start to read lineinfo until end symbol
      DebugLineInfo info;
      void* current = NULL;
      int level = 1;
      while (!reader.Eof()) {
         current =  reader.Address();

         reader.read(&info, sizeof(DebugLineInfo));
         if (info.symbol == dsProcedure) {
            level++;
         }
         else if (info.symbol == dsEnd) {
            if (level == 1) {
               break;
            }
            else level--;
         }
         else if (info.symbol == dsField || (info.symbol & ~dsTypeMask) == dsLocal)
         {
            // replace field name reference with the name
            stringReader.seek(info.addresses.symbol.nameRef);

            ((DebugLineInfo*)current)->addresses.symbol.nameRef = mapDebugPTR32(stringReader.Address());
         }
         else if ((info.symbol & dsDebugMask) == dsStep) {
            ref_t stepAddress = addressReader.getDWord();

            ((DebugLineInfo*)current)->addresses.step.address = stepAddress;
            // virtual end of expression should be stepped over automatically by debugger
            if (info.symbol != dsVirtualEnd)
               _debugger.addStep(stepAddress, (void*)current);
         }
      }
      return true;
   }
   else return false;
}

bool DebugController :: loadTapeDebugInfo(size_t selfPtr)
{
   // if tape debugging is allowed, debugger should switch to step mode
   //_postponed.setStepMode();

   int row = _debugger.Context()->LocalPtr(-2);

   int sourcePos = _tapeBookmarks.get(selfPtr);
   if (sourcePos == -1) {
      int list[DEBUG_MAX_LIST_LENGTH];
      int length = 0;
      getValue(selfPtr - 8, (char*)&length, 4);

      if (length > sizeof(list))
         length = sizeof(list);

      getValue(selfPtr, (char*)list, length);

      // create temporal document
      sourcePos = generateTape(list, length >> 2);

      _tapeBookmarks.add(selfPtr, sourcePos);
   }

   _listener->onLoadTape(TAPE_SYMBOL, row + 1, sourcePos, 0);

   return true;
}

bool DebugController :: loadDebugData(StreamReader& reader, bool setEntryAddress)
{
   IdentifierString reference;
   if (setEntryAddress) {
      // read entry point
      _entryPoint = reader.getDWord();

      if (_entryPoint != 0)
         setEntryAddress = false;

      // skip project namespace
      reader.readString(reference);
   }

   while (!reader.Eof()) {
      // read reference
      reader.readString(reference);

      // define the next record position
      pos_t size = reader.getDWord() - 4;
      pos_t nextPosition = reader.Position() + size;

      if (setEntryAddress) {
         // if entry address was not defined take the first one
         _entryPoint = reader.getDWord();

         reader.seek(reader.Position() - 4);
         setEntryAddress = false;
      }

   //   // if it is a VM temporal symbol and tape debugging as allowed
   //   if (ConstantIdentifier::compare(reference, TAPE_SYMBOL) && _debugTape) {
   //      loadTapeDebugInfo(reader, size);
   //   }
      // otherwise load standard debug info
   /*   else */loadSymbolDebugInfo(reference, reader);

      reader.seek(nextPosition);
   }

   return true;
}

void DebugController :: loadBreakpoints(List<Breakpoint>& breakpoints)
{
   List<Breakpoint>::Iterator breakpoint = breakpoints.start();
   while (!breakpoint.Eof()) {
      _Module* module = _modules.get((*breakpoint).module);
      if (module != NULL) {
         size_t address = findNearestAddress(module, (*breakpoint).source, (*breakpoint).row);
         if (address != 0xFFFFFFFF) {
            _debugger.addBreakpoint(address);
         }
      }
      breakpoint++;
   }
}

void DebugController :: toggleBreakpoint(Breakpoint& breakpoint, bool adding)
{
   if (_debugger.isStarted()) {
      _Module* module = _modules.get(breakpoint.module);
      if (module != NULL) {
         size_t address = findNearestAddress(module, breakpoint.source, breakpoint.row);
         if (address != 0xFFFFFFFF) {
            if (adding) {
               _debugger.addBreakpoint(address);
            }
            else _debugger.removeBreakpoint(address);
         }
      }
   }
}

_Module* DebugController :: loadDebugModule(ident_t reference)
{
   _ELENA_::NamespaceName name(reference);
   _ELENA_::Path          path;

   _manager->retrievePath(name, path, _T("dnl"));

   Module* module = (Module*)_modules.get(name);
   if (module == NULL) {
      module = new Module();

      _ELENA_::FileReader reader(path.c_str(), _ELENA_::feRaw, false);
      _ELENA_::LoadResult result = module->load(reader);
      if (result != _ELENA_::lrSuccessful) {
         delete module;

         return NULL;
      }
      _modules.add(name, module);
   }
   return module;
}

void DebugController :: clearBreakpoints()
{
   _debugger.clearBreakpoints();
}

bool DebugController :: start(path_t programPath, path_t arguments, bool debugMode, List<Breakpoint>& breakpoints)
{
   _currentModule = NULL;
   _started = false;

   _debugger.reset();

   _debuggee.copy(programPath);
   _arguments.copy(arguments);

   if (debugMode) {
      _entryPoint = _debugger.findEntryPoint(programPath);
      if (_entryPoint == (size_t)-1)
         return false;

      _debugger.initHook();
   }
   else {
      _entryPoint = 0;

      clearBreakpoints();
   }

   return start();
}

void DebugController :: run()
{
   if (_running || !_debugger.isStarted())
      return;

   if (_debugInfoPtr == 0 && _entryPoint != 0) {
      _debugger.setBreakpoint(_entryPoint, false);
      _postponed.clear();
   }
   _started = true;

   _events.setEvent(DEBUG_RESUME);

   _debugger.activate();
}

void DebugController :: runToCursor(ident_t name, path_t path, int col, int row)
{
   if (_running || !_debugger.isStarted())
      return;

   // the debug info should be loaded at first
   if (_debugInfoPtr == 0 && _entryPoint != 0) {
      _debugger.setBreakpoint(_entryPoint, false);

      _postponed.setGotoMode(col, row, name, path);
   }
   else {
      _Module* module = _modules.get(name);
      if (module != NULL) {
         size_t address = findNearestAddress(module, IdentifierString(path), row);
         if (address != 0xFFFFFFFF) {
            _debugger.setBreakpoint(address, false);
         }
      }
      else _postponed.setGotoMode(col, row, name, path);
   }

   _started = true;
   _events.setEvent(DEBUG_RESUME);
}

void DebugController :: stepInto()
{
   if (_running || !_debugger.isStarted())
      return;

   if (!_started) {
      _started = true;

      if (_debugInfoPtr == 0 && _entryPoint != 0)
         _postponed.setStepMode();

      _debugger.setBreakpoint(_entryPoint, false);
   }
   else {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());

      // if debugger should notify on the step result
      if (test(lineInfo->symbol, dsProcedureStep))
         _debugger.setCheckMode();

      DebugLineInfo* nextStep = getNextStep(lineInfo, false);
      // if the address is the same perform the virtual step
      if (nextStep && nextStep->addresses.step.address == lineInfo->addresses.step.address) {
         if (nextStep->symbol != dsVirtualEnd) {
            _debugger.processVirtualStep(nextStep);
            processStep();
            return;
         }
         else _debugger.setStepMode();
      }
      else if (test(lineInfo->symbol, dsAtomicStep)) {
         _debugger.setBreakpoint(nextStep->addresses.step.address, true);
      }
      // else set step mode
      else _debugger.setStepMode();
   }

   _events.setEvent(DEBUG_RESUME);
}

void DebugController :: stepOver()
{
   if (_running || !_debugger.isStarted())
      return;

   if (!_started) {
      _started = true;

      _postponed.setStepMode();

      _debugger.setBreakpoint(_entryPoint, false);
   }
   else {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());
      // if debugger should notify on the step result
      if (test(lineInfo->symbol, dsProcedureStep))
         _debugger.setCheckMode();

      DebugLineInfo* nextStep = getNextStep(lineInfo, true);
      // if next step is available set the breakpoint
      if (nextStep) {
         // if the address is the same perform the virtual step
         if (nextStep->addresses.step.address == lineInfo->addresses.step.address) {
            _debugger.processVirtualStep(nextStep);
            processStep();
            return;
         }
         else _debugger.setBreakpoint(nextStep->addresses.step.address, true);
      }
      // else set step mode
      else _debugger.setStepMode();
   }
   _events.setEvent(DEBUG_RESUME);
}

void DebugController :: stop()
{
   if (_debugger.isStarted()) {
      _events.setEvent(DEBUG_CLOSE);
   }
}

const char* DebugController :: getValue(size_t address, char* value, size_t length)
{
   _debugger.Context()->readDump(address, value, length);

   return value;
}

const wide_c* DebugController :: getValue(size_t address, wide_c* value, size_t length)
{
   _debugger.Context()->readDump(address, (char*)value, length * 2);

   return value;
}

//int DebugController :: getEIP()
//{
//   return _debugger.Context()->EIP();
//}

void DebugController :: readCallStack(_DebuggerCallStack* watch)
{
   MemoryDump retPoints;

   MemoryWriter writer(&retPoints);
   DebugReader reader(&_debugger);

   RTManager manager;
   manager.readCallStack(reader, _debugger.Context()->Frame(), _debugger.Context()->IP(), writer);

   for(pos_t pos = 0 ; pos < retPoints.Length(); pos += 4) {
      ident_t path = NULL;
      ident_t moduleName = NULL;
      ident_t className = NULL;
      ident_t methodName = NULL;

      DebugLineInfo* info = seekLineInfo(_debugger.readRetAddress(retPoints, pos), moduleName, className, methodName, path);
      if (info != NULL) {
         watch->write(moduleName, className, methodName, path, info->col + 1, info->row + 1, _debugger.readRetAddress(retPoints, pos));
      }
      else watch->write(_debugger.readRetAddress(retPoints, pos));
   }
}

void DebugController :: readFields(_DebuggerWatch* watch, DebugLineInfo* info, size_t address)
{
   int index = 1;
   while (info[index].symbol == dsField) {
      ident_t fieldName = (const char*)unmapDebugPTR32(info[index].addresses.field.nameRef);
      int size = info[index].addresses.field.size;
      // if it is a data field
      if (size != 0) {
         if (size == 4) {
            watch->write(this, 0, fieldName, (int)_debugger.Context()->readDWord(address));
         }
         else if (size == 2) {
            watch->write(this, 0, fieldName, (int)_debugger.Context()->readWord(address));
         }
         else if (size == 1) {
            watch->write(this, 0, fieldName, (int)_debugger.Context()->readByte(address));
         }

         address += size;
      }
      else {
         size_t fieldPtr = _debugger.Context()->ObjectPtr(address);
         if (fieldPtr==0) {
            watch->write(this, fieldPtr, fieldName, "<nil>");
         }
         else {
            size_t flags = 0;
            ident_t className = NULL;
            DebugLineInfo* field = seekClassInfo(fieldPtr, className, flags);
            if (field) {
               watch->write(this, fieldPtr, fieldName, className);
            }
            //// if unknown check if it is a dynamic subject
            //else if (test(flags, elDynamicSubjectRole)) {
            //   watch->write(this, fieldPtr, fieldName, _T("<subject>"));
            //}
            //// if unknown check if it is a group object
            //else if (test(flags, elGroup)) {
            //   watch->write(this, fieldPtr, fieldName, test(flags, elCastGroup) ? _T("<broadcast group>") : _T("<group>"));
            //}
            else watch->write(this, fieldPtr, fieldName, "<unknown>");
         }

         address += 4;
      }

      index++;
   }
}

void DebugController :: readList(_DebuggerWatch* watch, int* list, int length)
{
   String<char, 10> index;
   for (int i = 0 ; i < length ; i++)  {
      index.copy("[");
      index.appendInt(i);
      index.append("]");
      size_t memberPtr = list[i];
      if (memberPtr==0) {
         watch->write(this, memberPtr, ident_t(index), "<nil>");
      }
      else {
         size_t flags = 0;
         ident_t className = NULL;
         DebugLineInfo* item = seekClassInfo(memberPtr, className, flags);
         if (item) {
            watch->write(this, memberPtr, ident_t(index), className);
         }
         //// if unknown check if it is a dynamic subject
         //else if (test(flags, elDynamicSubjectRole)) {
         //   watch->write(this, memberPtr, index, _T("<subject>"));
         //}
         //// if unknown check if it is a group object
         //else if (test(flags, elGroup)) {
         //   watch->write(this, memberPtr, index, test(flags, elCastGroup) ? _T("<broadcast group>") : _T("<group>"));
         //}
         else watch->write(this, memberPtr, ident_t(index), "<unknown>");
      }
   }
}

void DebugController::readByteArray(_DebuggerWatch* watch, size_t address, ident_t name)
{
   char list[DEBUG_MAX_ARRAY_LENGTH];
   int  length = 0;

   // get bytearray size
   getValue(address - 8, (char*)&length, 4);
   length &= 0xFFFFF;

   if (length > DEBUG_MAX_LIST_LENGTH)
      length = DEBUG_MAX_LIST_LENGTH;

   getValue(address, (char*)list, length);

   watch->write(this, address, name, list, length);
}

void DebugController::readShortArray(_DebuggerWatch* watch, size_t address, ident_t name)
{
   short list[DEBUG_MAX_ARRAY_LENGTH];
   int  length = 0;

   // get bytearray size
   getValue(address - 8, (char*)&length, 4);
   length &= 0xFFFFF;
   length = length >> 1;

   if (length > DEBUG_MAX_LIST_LENGTH)
      length = DEBUG_MAX_LIST_LENGTH;

   getValue(address, (char*)list, length << 1);

   watch->write(this, address, name, list, length);
}

void DebugController::readIntArray(_DebuggerWatch* watch, size_t address, ident_t name)
{
   int list[DEBUG_MAX_ARRAY_LENGTH];
   int length = 0;

   // get bytearray size
   getValue(address - 8, (char*)&length, 4);
   length &= 0xFFFFF;
   length = length >> 2;

   if (length > DEBUG_MAX_LIST_LENGTH)
      length = DEBUG_MAX_LIST_LENGTH;

   getValue(address, (char*)list, length << 2);

   watch->write(this, address, name, list, length);
}

void DebugController :: readMessage(_DebuggerWatch* watch, ref_t reference)
{
   String<char, 20> messageValue("<");
   messageValue.appendHex(reference);
   messageValue.append('>');

   watch->write(this, reference, "$message", (const char*)messageValue);
}

void DebugController :: readObject(_DebuggerWatch* watch, size_t address, ident_t className, ident_t name)
{
   if (!emptystr(className)) {
      watch->write(this, address, name, className);
   }
   else watch->write(this, address, name, "<unknown>");
}

void DebugController :: readObject(_DebuggerWatch* watch, size_t address, ident_t name)
{
   if (address != 0) {
      ident_t className = NULL;
      size_t flags = 0;
      DebugLineInfo* info = seekClassInfo(address, className, flags);

      readObject(watch, address, className, name);
   }
   else watch->write(this, address, name, "<nil>");
}

void DebugController::readLocalInt(_DebuggerWatch* watch, size_t address, ident_t name)
{
   if (address != 0) {
      ident_t className = NULL;
      size_t flags = 0;
      DebugLineInfo* info = seekClassInfo(address, className, flags);
      if (info != NULL) {
         watch->write(this, address, name, className);
      }
      else {
         int value = 0;
         getValue(address, (char*)&value, 4);

         watch->write(this, address, name, value);
      }
   }
   else watch->write(this, address, name, "<nil>");
}

void DebugController::readLocalLong(_DebuggerWatch* watch, size_t address, ident_t name)
{
   if (address != 0) {
      ident_t className = NULL;
      size_t flags = 0;
      DebugLineInfo* info = seekClassInfo(address, className, flags);
      if (info != NULL) {
         watch->write(this, address, name, className);
      }
      else {
         long long value = 0;
         getValue(address, (char*)&value, 8);

         watch->write(this, address, name, value);
      }
   }
   else watch->write(this, address, name, "<nil>");
}

void DebugController::readLocalReal(_DebuggerWatch* watch, size_t address, ident_t name)
{
   if (address != 0) {
      ident_t className = NULL;
      size_t flags = 0;
      DebugLineInfo* info = seekClassInfo(address, className, flags);
      if (info != NULL) {
         watch->write(this, address, name, className);
      }
      else {
         double value = 0;
         getValue(address, (char*)&value, 8);

         watch->write(this, address, name, value);
      }
   }
   else watch->write(this, address, name, "<nil>");
}

void DebugController::readParams(_DebuggerWatch* watch, size_t address, ident_t name, bool ignoreInline)
{
   if (address != 0) {

      String<char, 255> index;
      for (int i = 0 ; i < 255 ; i++)  {
         index.copy(name);
         index.append("[");
         index.appendInt(i);
         index.append("]");

         size_t memberPtr = 0;
         getValue(address, (char*)&memberPtr, 4);
         // break if zero is found
         if (memberPtr == 0)
            break;

         size_t flags = 0;
         ident_t className = NULL;

         DebugLineInfo* item = seekClassInfo(memberPtr, className, flags);
         if (item) {
            watch->write(this, memberPtr, ident_t(index), className);
         }
         //// if unknown check if it is a dynamic subject
         //else if (test(flags, elDynamicSubjectRole)) {
         //   watch->write(this, memberPtr, index, _T("<subject>"));
         //}
         //// if unknown check if it is a group object
         //else if (test(flags, elGroup)) {
         //   watch->write(this, memberPtr, index, test(flags, elCastGroup) ? _T("<broadcast group>") : _T("<group>"));
         //}
         else watch->write(this, memberPtr, ident_t(index), "<unknown>");

         address += 4;
      }
   }
   else watch->write(this, address, name, "<nil>");
}

void DebugController :: readAutoContext(_DebuggerWatch* watch)
{
   if (_debugger.isStarted()) {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());
      if (lineInfo == NULL)
         return;

      int index = 0;
      while (lineInfo[index].symbol != dsProcedure) {
         if (lineInfo[index].symbol == dsLocal) {
            // write local variable
            size_t localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readObject(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsIntLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readLocalInt(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsIntLocalPtr) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);
            readLocalInt(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsLongLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readLocalLong(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsLongLocalPtr) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);
            readLocalLong(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsRealLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readLocalReal(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsRealLocalPtr) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);
            readLocalReal(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsParamsLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);
            readParams(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef), false);
         }
         else if (lineInfo[index].symbol == dsByteArrayLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->readDWord(_debugger.Context()->Local(lineInfo[index].addresses.local.level));

            readByteArray(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsByteArrayLocalPtr) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);

            readByteArray(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsShortArrayLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->readDWord(_debugger.Context()->Local(lineInfo[index].addresses.local.level));

            readShortArray(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsShortArrayLocalPtr) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);

            readShortArray(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsIntArrayLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->readDWord(_debugger.Context()->Local(lineInfo[index].addresses.local.level));

            readIntArray(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         else if (lineInfo[index].symbol == dsIntArrayLocalPtr) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);

            readIntArray(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         //else if (lineInfo[index].symbol == dsMessage) {
         //   // write local variable
         //   int message = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
         //   readMessage(watch, message);
         //}
         else if (lineInfo[index].symbol == dsStructPtr) {
            size_t localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);
            ref_t classPtr = _classNames.get((const char*)unmapDebugPTR32(lineInfo[index + 1].addresses.source.nameRef));
            if (classPtr != 0) {
               readObject(watch, localPtr, 
                  (const char*)unmapDebugPTR32(lineInfo[index + 1].addresses.source.nameRef), 
                  (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));

               watch->append(this, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef), localPtr, classPtr);
            }
         }
         else if (lineInfo[index].symbol == dsLocalPtr) {
            size_t localPtr = _debugger.Context()->readDWord(_debugger.Context()->Local(lineInfo[index].addresses.local.level));
            ref_t classPtr = _classNames.get((const char*)unmapDebugPTR32(lineInfo[index + 1].addresses.source.nameRef));

            readObject(watch, localPtr, 
               (const char*)unmapDebugPTR32(lineInfo[index + 1].addresses.source.nameRef), 
               (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));

            watch->append(this, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef), localPtr, classPtr);
         }
         else if (lineInfo[index].symbol == dsStack) {
            // write local variable
            size_t localPtr = _debugger.Context()->CurrentPtr(lineInfo[index].addresses.local.level);
            readObject(watch, localPtr, (const char*)unmapDebugPTR32(lineInfo[index].addresses.local.nameRef));
         }
         index--;
      }
   }
}

void DebugController :: readContext(_DebuggerWatch* watch, size_t selfPtr, size_t classPtr)
{
   if (_debugger.isStarted()) {
      size_t flags = 0;
      ident_t className = NULL;
      DebugLineInfo* info = seekClassInfo(selfPtr, className, flags, classPtr);
      if (info) {
         int type = info->addresses.symbol.flags & elDebugMask;
         if (type==elDebugLiteral) {
            char value[DEBUG_MAX_STR_LENGTH + 1];
            int length = 0;
            getValue(selfPtr - 8, (char*)&length, 4);

            length &= 0xFFFFF;

            if (length > DEBUG_MAX_STR_LENGTH) {
               length = DEBUG_MAX_STR_LENGTH;
            }
            getValue(selfPtr, value, length);
            value[length] = 0;
            watch->write(this, value);
         }
         if (type == elDebugWideLiteral) {
            wide_c value[DEBUG_MAX_STR_LENGTH + 1];
            int length = 0;
            getValue(selfPtr - 8, (char*)&length, 4);

            length &= 0xFFFFF;
            length >>= 1;

            if (length > DEBUG_MAX_STR_LENGTH) {
               length = DEBUG_MAX_STR_LENGTH;
            }
            getValue(selfPtr, value, length);
            value[length] = 0;
            watch->write(this, value);
         }
         else if (type == elDebugDWORD || type == elDebugSubject) {
            char value[4];
            getValue(selfPtr, value, 4);

            watch->write(this, *(int*)value);
         }
         else if (type==elDebugReal64) {
            char value[8];
            getValue(selfPtr, value, 8);

            watch->write(this, *(double*)value);
         }
         else if (type==elDebugQWORD) {
            char value[8];
            getValue(selfPtr, value, 8);

            watch->write(this, *(long long*)value);
         }
         else if (type==elDebugArray) {
            int list[DEBUG_MAX_LIST_LENGTH];
            int length = 0;

            // get array size
            getValue(selfPtr - 8, (char*)&length, 4);
            if (length == 0x800000)
               length = 0;

            if (length > sizeof(list))
               length = sizeof(list);

            getValue(selfPtr, (char*)list, length);

            length >>= 2;
            readList(watch, list, length);
         }
         else if (type==elDebugBytes) {
            readByteArray(watch, selfPtr, NULL);
         }
         else if (type==elDebugShorts) {
            readShortArray(watch, selfPtr, NULL);
         }
         else if (type == elDebugIntegers) {
            readIntArray(watch, selfPtr, NULL);
         }
         else if (className.compare("system'nil")) {
            watch->write(this, "<nil>");
         }
         else readFields(watch, info, selfPtr);
      }
   }
}

//void DebugController :: readPString(size_t address, IdentifierString& string)
//{
//   string.clear();
//   wchar_t ch = 0;
//   do {
//      _debugger.Context()->readDump(address, (char*)&ch, 2);
//      address += 2;
//
//      string.append(ch);
//   } while (ch != 0);
//}
//
////void DebugController :: readRegisters(_DebuggerWatch* watch)
////{
////   watch->write(this, _debugger.Context()->EIP(), _T("EIP"), EMPTY_STRING);
////}

void DebugController :: showCurrentModule(DebugLineInfo* lineInfo, ident_t moduleName, ident_t sourcePath)
{
   if (lineInfo) {
      if (!moduleName.compare(_currentModule) || sourcePath.compare(_currentSource)) {
         //!! do we need it at all?
         //onLoadModule(moduleName, sourcePath);
         _currentModule = moduleName;
         _currentSource = sourcePath;
      }
      _listener->onStep(moduleName, sourcePath, lineInfo->row, lineInfo->col, lineInfo->length);
   }
}

inline void writeStatement(MemoryWriter& writer, text_t command)
{
   writer.writeLiteral(command, getlength(command));
   writer.writeChar((text_c)'\n');
}

#ifdef _WIN32

inline void writeStatement(MemoryWriter& writer, ident_t command)
{
   WideString caption(command);

   writer.writeLiteral(caption, getlength(caption));
   writer.writeChar((text_c)'\n');
}

inline void writeStatement(MemoryWriter& writer, ident_t command, int value)
{
   WideString caption(command);
   caption.append('<');
   caption.appendHex(value);
   caption.append('>');

   writer.writeLiteral(caption, getlength(caption));
   writer.writeChar((text_c)'\n');
}

inline void writeStatement(MemoryWriter& writer, ident_t command, ident_t value)
{
   WideString caption(command);
   caption.append('<');
   caption.append(WideString(value));
   caption.append('>');

   writer.writeLiteral(caption, getlength(caption));
   writer.writeChar((text_c)'\n');
}

#endif // _WIN32

int DebugController :: generateTape(int* list, int length)
{
   int textPos = _tape.Length();

   MemoryWriter textWriter(&_tape);

   // a tape beginning
   writeStatement(textWriter, _T("$tape"));

   for (int i = 0; i < length; i++) {
      size_t memberPtr = list[i];
      if (memberPtr == 0) {
         writeStatement(textWriter, _T("$nil"));
      }
      else {
         size_t flags = 0;
         ident_t className = NULL;
         DebugLineInfo* item = seekClassInfo(memberPtr, className, flags);
         if (item) {
            switch (flags & elDebugMask)
            {
               case elDebugDWORD:
                  writeStatement(textWriter, className, _debugger.Context()->readDWord(memberPtr));
                  break;
               case elDebugLiteral:
               {
                  char value[DEBUG_MAX_STR_LENGTH + 1];
                  int length = 0;
                  getValue(memberPtr - 8, (char*)&length, 4);

                  length &= 0xFFFFF;

                  if (length > DEBUG_MAX_STR_LENGTH) {
                     length = DEBUG_MAX_STR_LENGTH;
                  }
                  getValue(memberPtr, value, length);
                  value[length] = 0;

                  writeStatement(textWriter, className, value);
                  break;
               }
               case elDebugMessage:
                  writeStatement(textWriter, className, _debugger.Context()->readDWord(memberPtr));
                  break;
               default:
                  writeStatement(textWriter, className);
                  break;
            }            
         }
         else writeStatement(textWriter, _T("<unknown>"));
      }
   }

   writeStatement(textWriter, _T("end"));
   textWriter.writeChar((text_c)0);

   return textPos;
}
