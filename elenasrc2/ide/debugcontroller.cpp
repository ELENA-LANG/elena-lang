//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implematioon of the DebugController class and
//      its helpers
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "debugcontroller.h"
//#include "messages.h"

using namespace _ELENA_;

// --- verbs ---

//MessageMap         _verbs;
//ConstantIdentifier _tapeSymbol(TAPE_SYMBOL);

// --- DebugController ---

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
            _events.resetEvent(DEBUG_ACTIVE);
            _events.resetEvent(DEBUG_SUSPEND);
            _running = false;
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

   onStop(_debugger.proceedCheckPoint());
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

DebugLineInfo* DebugController :: seekDebugLineInfo(size_t lineInfoAddress, const wchar16_t* &moduleName, const wchar16_t* &sourcePath)
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
            sourcePath = (const wchar16_t*)section->get(current->addresses.source.nameRef);
         }

         return (DebugLineInfo*)lineInfoAddress;
      }
      else return NULL;
//   }
}

DebugLineInfo* DebugController :: seekClassInfo(size_t address, const wchar16_t* &className, int& flags)
{
   // read class VMT address
   size_t vmtPtr = _debugger.Context()->ClassVMT(address);
   if (vmtPtr==0)
      return NULL;

   // if it is role, read the role owner
   flags = _debugger.Context()->VMTFlags(vmtPtr);
   //if (test(flags, elRoleVMT)) {
   //   vmtPtr = _debugger.Context()->ClassVMT(vmtPtr);
   //}

   // get class debug info address
   size_t position = _classes.get(vmtPtr);
   _Module* module = getDebugModule(position);
   _Memory* section = (module != NULL) ? module->mapSection(DEBUG_LINEINFO_ID, true) : NULL;

   if (position != 0 && section != NULL) {
      // to resolve class name we need offset in the section rather then the real address
      className = module->resolveReference(position - (size_t)section->get(0));

      return (DebugLineInfo*)position;
   }
   return NULL;
}

DebugLineInfo* DebugController :: getNextStep(DebugLineInfo* step)
{
   if (step->symbol != dsEnd && step->symbol != dsEOP) {
      DebugLineInfo* next = &step[1];
      int level = 0;
      while (level > 0 || (next->symbol & dsDebugMask) != dsStep) {
         switch (next->symbol) {
            case dsVirtualBlock:
               level++;
               break;
            case dsEOP:
            case dsVirtualEnd:
               level--;
               break;
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
         }

         next = &next[1];
      }

      return getNextStep(next);
   }
   return NULL;
}

size_t DebugController :: findNearestAddress(_Module* module, const tchar_t* path, size_t row, size_t col)
{
   _Memory* section = module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true);
   _Memory* strings = module->mapSection(DEBUG_STRINGS_ID, true);

   DebugLineInfo* info = (DebugLineInfo*)section->get(4);
   int count = section->Length() / sizeof(DebugLineInfo);

   // scanning the array of debug lines until closest step is found
   size_t address = (size_t)-1;
   int nearestCol = -1;
   bool skipping = true;
   for (size_t i = 0 ; i < count ; i++) {
      if (info[i].symbol == dsProcedure) {
         const wchar16_t* procPath = (const wchar16_t*)strings->get(info[i].addresses.source.nameRef);
         if (StringHelper::compare(procPath, path)) {
            skipping = false;
         }
      }
      else if (info[i].symbol == dsEnd) {
         skipping = true;
      }
      else if (!skipping && (info[i].symbol & dsDebugMask) == dsStep) {
         if (row == info[i].row) {
            int lineCol = info[i].col & 0xFFFF;
            if (nearestCol == -1 || ( (size_t)lineCol >= col && lineCol < nearestCol)) {
               nearestCol = lineCol;
               address = info[i].addresses.step.address;
            }
         }
      }
   }
   return address;
}

void DebugController :: processStep()
{
   if (_debugger.isTrapped()) {
      if (_debugger.isVMBreakpoint()) {
         onVMBreakpoint();

         return;
      }

      const wchar16_t* moduleName = NULL;
      const wchar16_t* sourcePath = NULL;
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State(), moduleName, sourcePath);
      showCurrentModule(lineInfo, moduleName, sourcePath);
   }
   if (_debugger.Context()->checkFailed) {
      onCheckPoint(_T("Operation failed"));
   }
   if (_debugger.Exception()!=NULL) {
      ProcessException* exeption = _debugger.Exception();

      onNotification(exeption->Text(), exeption->address, exeption->code);
   }
}

bool DebugController :: start()
{
   _events.init();
   _events.setEvent(DEBUG_SUSPEND);

   _debugger.startThread(this);

   while (_events.waitForEvent(DEBUG_ACTIVE, 0));

   onStart();

   return _debugger.isStarted();
}

bool  DebugController :: loadDebugData(const tchar_t* path)
{
   clearDebugInfo();

   FileReader	file(path, _T("rb+"), feRaw, false);

   if (file.isOpened()) {
      // load signature
      char signature[10];
      file.read(signature, strlen(DEBUG_MODULE_SIGNATURE));
      if (strncmp(signature, DEBUG_MODULE_SIGNATURE, strlen(DEBUG_MODULE_SIGNATURE)) != 0)
         return false;

      // load entry point
      file.readDWord(_entryPoint);

      // load debug info
      return loadDebugData(file);
   }
   else return false;
}

bool DebugController :: loadSymbolDebugInfo(const wchar16_t* reference, StreamReader&  addressReader)
{
   bool isClass = true;
   _Module* module = NULL;
   // if symbol
   if (reference[0]=='#') {
      module = loadDebugModule(reference + 1);
      isClass = false;
   }
   else module = loadDebugModule(reference);

   size_t position = (module != NULL) ? module->mapReference(reference, true) : 0;
   if (position != 0) {
      // place reader on the next after symbol record
      MemoryReader reader(module->mapSection(DEBUG_LINEINFO_ID | mskDataRef, true), position);
      MemoryReader stringReader(module->mapSection(DEBUG_STRINGS_ID | mskDataRef, true));

      // map vmt address for a class
      if (isClass) {
         ref_t vmtPtr = addressReader.getDWord();

         if (vmtPtr != 0) {
            _classes.add(vmtPtr, (size_t)reader.Address());
         }
      }

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

            ((DebugLineInfo*)current)->addresses.symbol.nameRef = (ref_t)stringReader.Address();
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

bool DebugController :: loadTapeDebugInfo(StreamReader& reader, size_t size)
{
   //// if tape debugging is allowed, debugger should switch to step mode
   //_postponed.setStepMode();

   //void* tape = (void*)reader.getDWord();
   //size -= 4;

   //// create temporal document
   //int sourcePos = generateTape(tape, reader, size >> 2);
   //onLoadTape(_tapeSymbol, sourcePos);

   return true;
}

bool DebugController :: loadDebugData(StreamReader& reader)
{
   IdentifierString reference;
   while (!reader.Eof()) {
      // read reference
      reader.readWideString(reference);

      // define the next record position
      int size = reader.getDWord() - 4;
      int nextPosition = reader.Position() + size;

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

void DebugController :: onVMBreakpoint()
{
   DebugReader reader(&_debugger, _vmDebugPtr, _vmDebugSize);

   // if there are new records in debug section
   if (!reader.Eof()) {
      // if it is a first time the debug data is loaded
      if (_vmDebugSize == 0) {
         // skip idle entry address
         reader.getDWord();
      }

      loadDebugData(reader);

      _vmDebugSize = reader.Position();

      // continue debugging
      if (_postponed.stepMode) {
         _debugger.setStepMode();
         _events.setEvent(DEBUG_RESUME);
      }
      else if (_postponed.gotoMode) {
         runToCursor(_postponed.source, _postponed.path, _postponed.col, _postponed.row);
      }
      else run();
   }
   // otherwise continue
   else _events.setEvent(DEBUG_RESUME);
}

void DebugController :: loadBreakpoints(List<Breakpoint>& breakpoints)
{
   List<Breakpoint>::Iterator breakpoint = breakpoints.start();
   while (!breakpoint.Eof()) {
      _Module* module = _modules.get((*breakpoint).module);
      if (module != NULL) {
         size_t address = findNearestAddress(module, (*breakpoint).source, (*breakpoint).row, 0);
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
         size_t address = findNearestAddress(module, breakpoint.source, breakpoint.row, 0);
         if (address != 0xFFFFFFFF) {
            if (adding) {
               _debugger.addBreakpoint(address);
            }
            else _debugger.removeBreakpoint(address);
         }
      }
   }
}

void DebugController :: clearBreakpoints()
{
   _debugger.clearBreakpoints();
}

bool DebugController :: start(const tchar_t* programPath, const tchar_t* arguments, DebugMode debugMode, List<Breakpoint>& breakpoints)
{
   _currentModule = NULL;
   _started = false;

   _debugger.reset();

   _debuggee.copy(programPath);
   _arguments.copy(arguments);

   if (debugMode == dbmStandalone) {
      Path debugDataPath(programPath);
      debugDataPath.changeExtension(_T("dn"));

      if (!loadDebugData(debugDataPath))
         return false;

      loadBreakpoints(breakpoints);
   }
   else if (debugMode == dbmElenaVM) {
      _entryPoint = 0x401000; // !! is it possible there could be another entry address?

      _vmHookMode = true;
      _debugger.setVMHook();
   }
   else clearBreakpoints();

   return start();
}

void DebugController :: run()
{
   if (_running || !_debugger.isStarted())
      return;

   if (_vmHookMode) {
      _debugger.setBreakpoint(_entryPoint, false);
      _postponed.clear();
   }
   _started = true;

   _events.setEvent(DEBUG_RESUME);

   _debugger.activate();
}

void DebugController :: runToCursor(const tchar_t* name, const tchar_t* path, int col, int row)
{
   if (_running || !_debugger.isStarted())
      return;

   // if it is VM client the debug info should be loaded at first
   if (_vmHookMode) {
      _debugger.setBreakpoint(_entryPoint, false);

      _postponed.setGotoMode(col, row, name, path);
   }
   // !! temporal
   //// if it is a temporal tape
   //else if (ConstIdentifier::compare(name, TAPE_SYMBOL)) {
   //   size_t address = findNearestAddress((DebugLineInfo*)_tape.get(4), _tape[0], row, col);
   //   if (address != 0xFFFFFFFF) {
   //      _debugger.setBreakpoint(address, false);
   //   }
   //}
   else {
      _Module* module = _modules.get(name);
      if (module != NULL) {
         size_t address = findNearestAddress(module, path, row, col);
         if (address != 0xFFFFFFFF) {
            _debugger.setBreakpoint(address, false);
         }
      }
      else _postponed.setGotoMode(col, row, name, path);
   }

   _started = true;
   _events.setEvent(DEBUG_RESUME);
}

void DebugController :: stepOverLine()
{
   if (_running || !_debugger.isStarted())
      return;

   if (!_started) {
      _started = true;

      if (_vmHookMode)
         _postponed.setStepMode();

      _debugger.setBreakpoint(_entryPoint, false);
   }
   else {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());

      // if next step is available set the breakpoint
      DebugLineInfo* nextStep = getEndStep(lineInfo);

      if (nextStep) {
         _debugger.setBreakpoint(nextStep->addresses.step.address, true);
      }
      // else set step mode
      else _debugger.setStepMode();
   }
   _events.setEvent(DEBUG_RESUME);
}

void DebugController :: stepInto()
{
   if (_running || !_debugger.isStarted())
      return;

   if (!_started) {
      _started = true;

      if (_vmHookMode)
         _postponed.setStepMode();

      _debugger.setBreakpoint(_entryPoint, false);
   }
   else {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());

      // if debugger should notify on the step result
      if (test(lineInfo->symbol, dsProcedureStep))
         _debugger.setCheckMode();

      DebugLineInfo* nextStep = getNextStep(lineInfo);
      // if the address is the same perform the virtual step
      if (nextStep && nextStep->addresses.step.address == lineInfo->addresses.step.address) {
         _debugger.processVirtualStep(nextStep);
         processStep();
         return;
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

      if (_vmHookMode)
         _postponed.setStepMode();

      _debugger.setBreakpoint(_entryPoint, false);
   }
   else {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());

      // if debugger should notify on the step result
      if (test(lineInfo->symbol, dsProcedureStep))
         _debugger.setCheckMode();

      DebugLineInfo* nextStep = getNextStep(lineInfo);
      // if next step is available set the breakpoint
      if (nextStep) {
         // if the address is the same perform the virtual step
         if (nextStep->addresses.step.address == lineInfo->addresses.step.address) {
            _debugger.processVirtualStep(nextStep);
            processStep();
            return;
         }
         _debugger.setBreakpoint(nextStep->addresses.step.address, true);
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

const wchar_t* DebugController :: getValue(size_t address, wchar16_t* value, size_t length)
{
   _debugger.Context()->readDump(address, (char*)value, length * 2);

   return value;
}

int DebugController :: getEIP()
{
   return _debugger.Context()->EIP();
}

void DebugController :: readFields(_DebuggerWatch* watch, DebugLineInfo* info, size_t address)
{
   int index = 1;
   while (info[index].symbol == dsField) {
      const wchar16_t* fieldName = (const wchar16_t*)info[index].addresses.field.nameRef;
      int size = info[index].addresses.field.size;
      // if it is a data field
      if (size != 0) {
         if (size == 4) {
            watch->write(this, 0, fieldName, (int)_debugger.Context()->readDWord(address));
         }
         else if (size == 2) {
            watch->write(this, 0, fieldName, (int)_debugger.Context()->readWord(address));
         }

         address += size;
      }
      else {
         size_t fieldPtr = _debugger.Context()->ObjectPtr(address);
         if (fieldPtr==0) {
            watch->write(this, fieldPtr, fieldName, _T("<nil>"));
         }
         else {
            int flags = 0;
            const wchar16_t* className = NULL;
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
            else watch->write(this, fieldPtr, fieldName, _T("<unknown>"));
         }

         address += 4;
      }

      index++;
   }
}

void DebugController :: readList(_DebuggerWatch* watch, int* list, int length)
{
   String<tchar_t, 10> index;
   for (int i = 0 ; i < length ; i++)  {
      index.copy(_T("["));
      index.appendInt(i);
      index.append(_T("]"));
      size_t memberPtr = list[i];
      if (memberPtr==0) {
         watch->write(this, memberPtr, index, _T("<nil>"));
      }
      else {
         int flags = 0;
         const wchar16_t* className = NULL;
         DebugLineInfo* item = seekClassInfo(memberPtr, className, flags);
         if (item) {
            watch->write(this, memberPtr, index, className);
         }
         //// if unknown check if it is a dynamic subject
         //else if (test(flags, elDynamicSubjectRole)) {
         //   watch->write(this, memberPtr, index, _T("<subject>"));
         //}
         //// if unknown check if it is a group object
         //else if (test(flags, elGroup)) {
         //   watch->write(this, memberPtr, index, test(flags, elCastGroup) ? _T("<broadcast group>") : _T("<group>"));
         //}
         else watch->write(this, memberPtr, index, _T("<unknown>"));
      }
   }
}

void DebugController :: readByteArray(_DebuggerWatch* watch, size_t address, const wchar16_t* name)
{
   char list[DEBUG_MAX_ARRAY_LENGTH];
   int  length = 0;

   // get bytearray size
   getValue(address - 8, (char*)&length, 4);
   length = -length;

   if (length > DEBUG_MAX_LIST_LENGTH)
      length = DEBUG_MAX_LIST_LENGTH;

   getValue(address, (char*)list, length);

   watch->write(this, address, name, list, length);
}

void DebugController :: readShortArray(_DebuggerWatch* watch, size_t address, const wchar16_t* name)
{
   short list[DEBUG_MAX_ARRAY_LENGTH];
   int  length = 0;

   // get bytearray size
   getValue(address - 8, (char*)&length, 4);
   length = -length >> 1;

   if (length > DEBUG_MAX_LIST_LENGTH)
      length = DEBUG_MAX_LIST_LENGTH;

   getValue(address, (char*)list, length);

   watch->write(this, address, name, list, length);
}

void DebugController :: readMessage(_DebuggerWatch* watch, ref_t reference)
{
   String<tchar_t, 20> messageValue(_T("<"));
   messageValue.appendHex(reference);
   messageValue.append(_T('>'));

   watch->write(this, reference, _T("$message"), messageValue);
}

void DebugController :: readObject(_DebuggerWatch* watch, ref_t address, const wchar16_t* name, bool ignoreInline)
{
   if (address != 0) {
      const wchar16_t* className = NULL;
      int flags = 0;
      DebugLineInfo* info = seekClassInfo(address, className, flags);
      if (info != NULL) {
         /*// do not show self for embedded inline classes
         if (ignoreInline && test(flags, elInlineClass) && !test(flags, elSymbol)) {
            readFields(watch, info, address);
         }
         else*/ watch->write(this, address, name, className);
      }
      /*// if unknown check if it is a group object
      else if (test(flags, elGroup)) {
         watch->write(this, address, name, test(flags, elCastGroup) ? _T("<broadcast group>") : _T("<group>"));
      }*/
      /*// if unknown check if it is a dynamic subject
      else if (test(flags, elDynamicSubjectRole)) {
         watch->write(this, address, name, _T("<subject>"));
      }*/
      else watch->write(this, address, name, _T("<unknown>"));
   }
   else watch->write(this, address, name, _T("<nil>"));
}

void DebugController :: readLocalInt(_DebuggerWatch* watch, ref_t address, const wchar16_t* name)
{
   if (address != 0) {
      const wchar16_t* className = NULL;
      int flags = 0;
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
   else watch->write(this, address, name, _T("<nil>"));
}

void DebugController :: readLocalLong(_DebuggerWatch* watch, ref_t address, const wchar16_t* name)
{
   if (address != 0) {
      const wchar16_t* className = NULL;
      int flags = 0;
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
   else watch->write(this, address, name, _T("<nil>"));
}

void DebugController :: readLocalReal(_DebuggerWatch* watch, ref_t address, const wchar16_t* name)
{
   if (address != 0) {
      const wchar16_t* className = NULL;
      int flags = 0;
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
   else watch->write(this, address, name, _T("<nil>"));
}

void DebugController :: readParams(_DebuggerWatch* watch, ref_t address, const wchar16_t* name, bool ignoreInline)
{
   if (address != 0) {

      String<tchar_t, 255> index;
      for (int i = 0 ; i < 255 ; i++)  {
         index.copy(name);
         index.append(_T("["));
         index.appendInt(i);
         index.append(_T("]"));

         size_t memberPtr = 0;
         getValue(address, (char*)&memberPtr, 4);
         // break if zero is found
         if (memberPtr == 0)
            break;

         int flags = 0;
         const wchar16_t* className = NULL;
         DebugLineInfo* item = seekClassInfo(memberPtr, className, flags);
         if (item) {
            watch->write(this, memberPtr, index, className);
         }
         //// if unknown check if it is a dynamic subject
         //else if (test(flags, elDynamicSubjectRole)) {
         //   watch->write(this, memberPtr, index, _T("<subject>"));
         //}
         //// if unknown check if it is a group object
         //else if (test(flags, elGroup)) {
         //   watch->write(this, memberPtr, index, test(flags, elCastGroup) ? _T("<broadcast group>") : _T("<group>"));
         //}
         else watch->write(this, memberPtr, index, _T("<unknown>"));

         address += 4;
      }
   }
   else watch->write(this, address, name, _T("<nil>"));
}

void DebugController :: readAutoContext(_DebuggerWatch* watch)
{
   if (_debugger.isStarted()) {
      DebugLineInfo* lineInfo = seekDebugLineInfo((size_t)_debugger.Context()->State());
      int index = 0;
      while (lineInfo[index].symbol != dsProcedure) {
         if (lineInfo[index].symbol == dsLocal) {
            // write local variable
            int localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readObject(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef, false);
         }
         else if (lineInfo[index].symbol == dsIntLocal) {
            // write stack allocated local variable
            int localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readLocalInt(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef);
         }
         else if (lineInfo[index].symbol == dsLongLocal) {
            // write stack allocated local variable
            int localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readLocalLong(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef);
         }
         else if (lineInfo[index].symbol == dsRealLocal) {
            // write stack allocated local variable
            int localPtr = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readLocalReal(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef);
         }  
         else if (lineInfo[index].symbol == dsParamsLocal) {
            // write stack allocated local variable
            int localPtr = _debugger.Context()->Local(lineInfo[index].addresses.local.level);
            readParams(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef, false);
         }
         else if (lineInfo[index].symbol == dsByteArrayLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->readDWord(_debugger.Context()->Local(lineInfo[index].addresses.local.level));

            readByteArray(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef);
         }
         else if (lineInfo[index].symbol == dsShortArrayLocal) {
            // write stack allocated local variable
            size_t localPtr = _debugger.Context()->readDWord(_debugger.Context()->Local(lineInfo[index].addresses.local.level));

            readShortArray(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef);
         }
         else if (lineInfo[index].symbol == dsMessage) {
            // write local variable
            int message = _debugger.Context()->LocalPtr(lineInfo[index].addresses.local.level);
            readMessage(watch, message);
         }
         else if (lineInfo[index].symbol == dsStack) {
            // write local variable
            int localPtr = _debugger.Context()->CurrentPtr(lineInfo[index].addresses.local.level);
            readObject(watch, localPtr, (const wchar16_t*)lineInfo[index].addresses.local.nameRef, false);
         }
         index--;
      }
   }
}

void DebugController :: readContext(_DebuggerWatch* watch, size_t selfPtr)
{
   if (_debugger.isStarted()) {
      int flags = 0;
      const wchar16_t* className = NULL;
      DebugLineInfo* info = seekClassInfo(selfPtr, className, flags);
   //   if (test(flags, elGroup)) {
   //      int list[DEBUG_MAX_LIST_LENGTH];
   //      int length = 0;

   //      // get group size
   //      getValue(selfPtr - 8, (char*)&length, 4);

   //      if (length > sizeof(list))
   //         length = sizeof(list);

   //      getValue(selfPtr, (char*)list, length);

   //      length >>= 2;
   //      readList(watch, list, length);
   //   }
      if (info) {
         int type = info->addresses.symbol.flags & elDebugMask;
         if (type==elDebugLiteral) {
            wchar16_t value[DEBUG_MAX_STR_LENGTH + 1];
            int length = 0;
            getValue(selfPtr - 8, (char*)&length, 4);

            length = -length;
            length >>= 1;

            if (length > DEBUG_MAX_STR_LENGTH) {
               length = DEBUG_MAX_STR_LENGTH;
            }
            getValue(selfPtr, value, length);
            value[length] = 0;
            watch->write(this, value);
         }
         else if (type==elDebugDWORD) {
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

            int s = sizeof(list);
            if (length > sizeof(list))
               length = sizeof(list);

            getValue(selfPtr, (char*)list, length);

            length >>= 2;
            readList(watch, list, length);
         }
         else if (type==elDebugBytes) {
            //readByteArray(watch, selfPtr);
         }
         else if (type==elDebugChars) {
            int list[DEBUG_MAX_LIST_LENGTH];
            int length = 0;

            // get array size
            getValue(selfPtr - 8, (char*)&length, 4);

            int s = sizeof(list);
            if (length > sizeof(list))
               length = sizeof(list);

            getValue(selfPtr, (char*)list, length);

            length >>= 2;
            readList(watch, list, length);
         }
         else if (ConstantIdentifier::compare(className, NIL_CLASS)) {
            watch->write(this, _T("<nil>"));
         }
         else readFields(watch, info, selfPtr);
      }
   }
}

void DebugController :: readPString(size_t address, IdentifierString& string)
{
   string.clear();
   wchar_t ch = 0;
   do {
      _debugger.Context()->readDump(address, (char*)&ch, 2);
      address += 2;

      string.append(ch);
   } while (ch != 0);
}

//void DebugController :: readRegisters(_DebuggerWatch* watch)
//{
//   watch->write(this, _debugger.Context()->EIP(), _T("EIP"), EMPTY_STRING);
//}

void DebugController :: showCurrentModule(DebugLineInfo* lineInfo, const wchar16_t* moduleName, const wchar16_t* sourcePath)
{
   if (lineInfo) {
      if (!StringHelper::compare(_currentModule, moduleName) || StringHelper::compare(_currentSource, sourcePath)) {
         //!! do we need it at all?
         //onLoadModule(moduleName, sourcePath);
         _currentModule = moduleName;
         _currentSource = sourcePath;
      }
      onStep(moduleName, sourcePath, lineInfo->row, lineInfo->col, lineInfo->length);
   }
}

inline void writeStatement(MemoryWriter& writer, const wchar16_t* command)
{
   writer.writeWideLiteral(command, getlength(command));
   writer.writeWideChar('\n');
}

inline void writeCommand(MemoryWriter& writer, const wchar16_t* command, const wchar16_t* param)
{
   writer.writeWideLiteral(_T("   "), 3);

   writer.writeWideLiteral(command, getlength(command));
   writer.writeWideChar(' ');
   writer.writeWideLiteral(param, getlength(param));
   writer.writeWideChar('\n');
}

inline void writeCommand(MemoryWriter& writer, const wchar16_t* command, const wchar16_t* paramPrefix, const wchar16_t* param,
                         const wchar16_t* paramPostfix)
{
   writer.writeWideLiteral(_T("   "), 3);

   writer.writeWideLiteral(command, getlength(command));
   writer.writeWideChar(' ');
   writer.writeWideLiteral(paramPrefix, getlength(paramPrefix));
   writer.writeWideLiteral(param, getlength(param));
   writer.writeWideLiteral(paramPostfix, getlength(paramPostfix));
   writer.writeWideChar('\n');
}

inline void writeCommand(MemoryWriter& writer, const wchar16_t* command, const wchar16_t* paramPrefix, int param,
                         const wchar16_t* paramPostfix)
{
   writer.writeWideLiteral(_T("   "), 3);

   String<wchar16_t, 12> str;
   str.appendHex(param);

   writer.writeWideLiteral(command, getlength(command));
   writer.writeWideChar(' ');
   writer.writeWideLiteral(paramPrefix, getlength(paramPrefix));
   writer.writeWideLiteral(str, getlength(str));
   writer.writeWideLiteral(paramPostfix, getlength(paramPostfix));
   writer.writeWideChar('\n');
}

inline void writeCommand(MemoryWriter& writer, const wchar16_t* command, const wchar16_t* param1, const wchar16_t* paramPrefix,
                         int param2, const wchar16_t* paramPostfix)
{
   String<wchar16_t, 12> str;
   str.appendHex(param2);

   writer.writeWideLiteral(_T("   "), 3);
   writer.writeWideLiteral(command, getlength(command));
   writer.writeWideChar(' ');
   writer.writeWideLiteral(param1, getlength(param1));
   writer.writeWideLiteral(paramPrefix, getlength(paramPrefix));
   writer.writeWideLiteral(str, getlength(str));
   writer.writeWideLiteral(paramPostfix, getlength(paramPostfix));
   writer.writeWideChar('\n');
}

// !! could the code be refactored to reuse part of the code used in ELT as well
int DebugController :: generateTape(void* tape, StreamReader& reader, int breakpointCount)
{
//   // load verbs global dictionary
//   if (_verbs.Count() == 0)
//      _ELENA_::loadVerbs(_verbs);
//
//   // save the length of debug info structure
//   _tape.writeDWord(0, breakpointCount + 3);
//
//   int debugPos = _tape.Length();
//
//   // reserve place for a debug section: number of breakpoints + header + current + previous
//   _tape.writeBytes(debugPos, 0, (breakpointCount + 3)*sizeof(DebugLineInfo));
//
//   int textPos = _tape.Length();
//
//   int row = 0;
//   MemoryWriter writer(&_tape, debugPos);
//   MemoryWriter textWriter(&_tape);
//
//   // a tape beginning
//   writeStatement(textWriter, _T("$tape"));
//
//   DebugLineInfo begin(dsProcedure, 0, 0, row);
//   writer.write(&begin, sizeof(DebugLineInfo));
//
//   // a tape current variables
//   DebugLineInfo current(dsStack, 0, 0, row);
//   current.addresses.local.level = 0;
//   current.addresses.local.nameRef = (int)_T("current");
//
//   writer.write(&current, sizeof(DebugLineInfo));
//
//   // a tape previous variables
//   current.addresses.local.level = 1;
//   current.addresses.local.nameRef = (int)_T("previous");
//
//   writer.write(&current, sizeof(DebugLineInfo));
//
//   // a tape body
//   IdentifierString prefix;
//   IdentifierString reference;
//
//   size_t base    = (size_t)tape;
//   size_t command = _debugger.Context()->readDWord(base);
//   size_t tapePtr = 0;
//   while (command != 0) {
//      size_t param = _debugger.Context()->readDWord(base + tapePtr + 4);
//      tapePtr = _debugger.Context()->readDWord(base + tapePtr + 8);         // goes to the next record
//
//      // skip VM commands
//      if (!test(command, VM_MASK)) {
//         if (command != PREFIX_TAPE_MESSAGE_ID) {
//            // write a tape step record
//            DebugLineInfo info(dsStep, 0, 0, ++row);
//            info.addresses.step.address = reader.getDWord();
//
//            size_t position = writer.Position();
//            writer.write(&info, sizeof(DebugLineInfo));
//
//            // !! should we add a flag to indicate temporal tape?
//            _debugger.addStep(info.addresses.step.address, (void*)position);
//         }
//
//         bool invoke = false;
//         switch(command) {
//            case PREFIX_TAPE_MESSAGE_ID:
//               readPString(base + param, prefix);
//               break;
//            case PUSH_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("push"), reference);
//               break;
//            case PUSH_EMPTY_MESSAGE_ID:
//               writeCommand(textWriter, _T("push <empty>"), NULL);
//               break;
//            case PUSHS_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("push"), _T("\""), reference, _T("\""));
//               break;
//            case PUSHN_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("push"), NULL, reference, NULL);
//               break;
//            case PUSHR_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("push"), NULL, reference, _T("r"));
//               break;
//            case PUSHL_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("push"), NULL, reference, _T("l"));
//               break;
//            case COPY_TAPE_MESSAGE_ID:
//               writeCommand(textWriter, _T("push"), _T("sp ["), param, _T("h]"));
//               break;
//            case GET_TAPE_MESSAGE_ID:
//               writeCommand(textWriter, _T("push"), _T("fp ["), param, _T("h]"));
//               break;
//            case CALL_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("call"), reference);
//               break;
//            case NEW_TAPE_MESSAGE_ID:
//               writeCommand(textWriter, _T("new"), prefix, _T("["), param, _T("h]"));
//               prefix.clear();
//               break;
//            case NEW_ARG_MESSAGE_ID:
//               writeCommand(textWriter, _T("arg"), prefix, _T("["), param, _T("h]"));
//               prefix.clear();
//               break;
//            case GROUP_TAPE_MESSAGE_ID:
//               readPString(base + param, reference);
//               writeCommand(textWriter, _T("group-add"), reference);
//               break;
//            case POP_TAPE_MESSAGE_ID:
//               writeCommand(textWriter, _T("pop"), NULL, param, _T("h"));
//               break;
////            case INVOKE_TAPE_MESSAGE_ID:
////               invoke = true;
//            default:
//            {
//               const wchar16_t* message = retrieveKey(_verbs.start(), invoke ? param : command, (const wchar16_t*)NULL);
//
//               if(invoke) {
//                  writeCommand(textWriter, _T("send"), _T("%0"), _T("."), message);
//               }
//               else if (!emptystr(prefix)) {
//                  writeCommand(textWriter, _T("send"), prefix, _T("."), message);
//
//                  prefix.clear();
//               }
//               else writeCommand(textWriter, _T("send"), message);
//               break;
//            }
//         }
//      }
//
//      command = _debugger.Context()->readDWord(base + tapePtr);
//   }
//
//   // a tape end
//   // write an end of tape record
//   DebugLineInfo eop(dsEOP, 0, 0, ++row);
//   eop.addresses.step.address = reader.getDWord();
//
//   size_t position = writer.Position();
//   writer.write(&eop, sizeof(DebugLineInfo));
//   _debugger.addStep(eop.addresses.step.address, (void*)position);
//
//   writeStatement(textWriter, _T("end"));
//   textWriter.writeWideChar(0);
//
//   return textPos;

   return 0; // !! temporal
}
