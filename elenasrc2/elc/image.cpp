//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image class implementations
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "image.h"

using namespace _ELENA_;

// Virtual machine client built-in references
#define VM_LOADER      _T("$package'core_vm'console_vm_start")

#define VM_TAPE            _T("'vm_tape")
#define VM_PATH            _T("'vm_path")
#define VM_PROCEDURE       _T("'vm_procedure")
#define VM_DEBUGPROCEDURE  _T("'vm_debugprocedure")
#define VM_HOOK            _T("'vm_hook")
#define VM_ERR_DLLLOAD     _T("'vm_dllnotfound")
#define VM_ERR_DLLINVALID  _T("'vm_dllinvalid")
#define VM_ERR_ERRPROC     _T("'vm_errproc")

// --- ExecutableImage ---

ExecutableImage :: ExecutableImage(Project* project, _JITCompiler* compiler)
   : Image(true)
{
   _project = project;

   // load default forwards
   _literal.copy(WSTR_CLASS);
   _int.copy(INT_CLASS);
   _long.copy(LONG_CLASS);
   _real.copy(REAL_CLASS);

   JITLinker linker(dynamic_cast<_JITLoader*>(this), compiler, true, (void*)mskCodeRef, project->BoolSetting(opDebugMode));

   // compile TLS section if it is a multi-threading app
   if (_project->IntSetting(opThreadMax) > 1) {
      compiler->compileTLS(dynamic_cast<_JITLoader*>(this));

      // load GC thread table, should be allocated before static roots
      // thread table contains TLS reference
      compiler->compileThreadTable(dynamic_cast<_JITLoader*>(this), _project->IntSetting(opThreadMax));
   }

   // initialize compiler inline code
   linker.prepareCompiler(
      project->loadPrimitive(CORE_MODULE, false),
      project->loadPrimitive(COMMAND_MODULE, false));

   // load starting symbol (it shouldn't be forward)
   const wchar16_t* entry = project->StrSetting(opEntry);

   _entryPoint = linker.resolve(entry, mskNativeCodeRef, true);
   if(_entryPoint == LOADER_NOTLOADED)
      throw JITUnresolvedException(project->StrSetting(opEntry));

  // fix up static table size
   compiler->setStaticRootCounter(this, linker.getStaticCount(), true);
}

ref_t ExecutableImage :: getDebugEntryPoint()
{
   const wchar16_t* starter = _project->resolveForward(STARTUP_CLASS);
   while (isWeakReference(starter)) {
      starter = _project->resolveForward(starter);
   }

   return (ref_t)resolveReference(starter, mskSymbolRef) & ~mskAnyRef;
}

_Memory* ExecutableImage :: getTargetSection(size_t mask)
{
   switch(mask & mskImageMask)
   {
      case mskCodeRef:
      case mskRelCodeRef:
         return &_text;
      case mskRDataRef:
         return &_data;
      case mskStatRef:
         return &_stat;
      case mskDataRef:
         return &_bss;
      case mskTLSRef:
         return &_tls;
      default:
         return NULL;
   }
}

SectionInfo ExecutableImage :: getSectionInfo(const wchar16_t* reference, size_t mask)
{
   SectionInfo sectionInfo;

   ref_t referenceID = 0;
   sectionInfo.module = _project->resolveModule(reference, referenceID);
   if (sectionInfo.module == NULL || referenceID == 0) {
      throw JITUnresolvedException(reference);
   }
   else sectionInfo.section = sectionInfo.module->mapSection(referenceID | mask, true);

   if (sectionInfo.section == NULL) {
      throw JITUnresolvedException(reference);
   }

   return sectionInfo;
}

ClassSectionInfo ExecutableImage :: getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask)
{
   ClassSectionInfo sectionInfo;

   ref_t referenceID = 0;
   sectionInfo.module = _project->resolveModule(reference, referenceID);
   if (sectionInfo.module == NULL || referenceID == 0)
      throw JITUnresolvedException(reference);
   else {
      sectionInfo.codeSection = sectionInfo.module->mapSection(referenceID | codeMask, true);
      sectionInfo.vmtSection = sectionInfo.module->mapSection(referenceID | vmtMask, true);
   }
   return sectionInfo;
}

size_t ExecutableImage :: getLinkerConstant(int id)
{
   switch (id) {
      case lnGCSize:
         return _project->IntSetting(opGCHeapSize);
      case lnYGRatio:
         return _project->IntSetting(opYGRatio);
      case lnThreadCount:
         return _project->IntSetting(opThreadMax);
      case lnObjectSize:
         return _project->IntSetting(opGCObjectSize);
      default:
         return 0;
   }
}

const wchar16_t* ExecutableImage :: getLiteralClass()
{
   return _literal;
}

const wchar16_t* ExecutableImage :: getIntegerClass()
{
   return _int;
}

const wchar16_t* ExecutableImage :: getLongClass()
{
   return _long;
}

const wchar16_t* ExecutableImage :: getRealClass()
{
   return _real;
}

const wchar16_t* ExecutableImage :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref) {
      return module->resolveConstant(reference);
   }
   // if it is a message
   else if (mask == 0) {
      return module->resolveSubject(reference);
   }
   else {
      const wchar16_t* referenceName = module->resolveReference(reference);
      while (isWeakReference(referenceName)) {
         const wchar_t* resolvedName = _project->resolveForward(referenceName);

         if (!emptystr(resolvedName))  {
            referenceName = resolvedName;
         }
         else throw JITUnresolvedException(referenceName);
      }
      return referenceName;
   }
}

// --- VirtualMachineClientImage ---

inline void writeTapeRecord(size_t base, MemoryWriter& tape, size_t command)
{
   size_t pos = tape.Position() - base;

   tape.writeDWord(command);
   tape.writeDWord(0);
   tape.writeDWord(pos + 12);
}

inline void writeTapeRecord(size_t base, MemoryWriter& tape, size_t command, const wchar16_t* value)
{
   size_t pos = tape.Position() - base;

   tape.writeDWord(command);
   tape.writeDWord(pos + 12);
   tape.writeDWord(0);        // next place holder

   tape.writeWideLiteral(value);

   // fix next field
   size_t nextPos = tape.Position() - base;
   tape.seek(base + pos + 8);
   tape.writeDWord(nextPos);

   tape.seek(nextPos + base);
}

inline void writeTapeRecord(size_t base, MemoryWriter& tape, size_t command, const wchar16_t* value1, const wchar16_t* value2)
{
   size_t pos = tape.Position() - base;

   tape.writeDWord(command);
   tape.writeDWord(pos + 12);
   tape.writeDWord(0);        // next place holder

   if (!emptystr(value1)) {
      tape.writeWideLiteral(value1, getlength(value1));
      tape.writeWideLiteral(_T("="), 1);
   }
   if (!emptystr(value2)) {
      tape.writeWideLiteral(value2);
   }
   else tape.writeWideChar(0);

   // fix next field
   size_t nextPos = tape.Position() - base;
   tape.seek(base + pos + 8);
   tape.writeDWord(nextPos);

   tape.seek(nextPos + base);
}

//inline void writeLiteralRef(MemoryWriter& tape, MemoryWriter& data, const wchar16_t* s)
//{
//   if (!emptystr(s)) {
//      tape.writeRef(mskRDataRef, data.Position());
//
//      data.writeWideLiteral(s);
//   }
//   else tape.writeDWord(0);
//}

inline ref_t writeLiteral(MemoryWriter& data, const wchar16_t* s)
{
   ref_t position = data.Position();
   data.writeWideLiteral(s);

   return position;
}

inline ref_t writeAnsiLiteral(MemoryWriter& data, const wchar16_t* s)
{
   ref_t position = data.Position();
   data.writeAsciiLiteral(s);

   return position;
}

inline ref_t writeErrorMessage(MemoryWriter& data, const wchar16_t* s)
{
   ref_t position = data.Position();
   data.writeDWord(getlength(s));
   data.writeWideLiteral(s);

   return position;
}

inline ref_t writeErrorMessage(MemoryWriter& data, const wchar16_t* s1, const wchar16_t* s2, const wchar16_t* s3)
{
   String<wchar16_t, 255> message;
   message.copy(s1);
   message.append(s2);
   message.append(s3);

   return writeErrorMessage(data, message);
}

VirtualMachineClientImage :: VirtualMachineClientImage(Project* project, _JITCompiler* compiler, const _path_t* appPath)
   : Image(false), _exportReferences((size_t)-1)
{
   _project = project;

   // setup virtual machine path
   _rootPath.copy(project->StrSetting(opVMPath));
   _rootPath.combine(_T("elenavm.dll"));

   MemoryWriter   data(&_data);
   MemoryWriter   code(&_text);

   // setup debugger hook record
   _bss.writeDWord(0, 0);               // reserve place for debug_mode, debug_address fields
   _bss.writeDWord(4, 0);

   size_t vmHook = data.Position();
   data.writeRef(mskNativeDataRef, 0); // hook address

   // setup startup VM script
   ReferenceMap consts((size_t)-1);
   VMClientHelper helper(this, &consts);

   consts.add(VM_TAPE, createTape(data, project));
   consts.add(VM_PATH, writeLiteral(data, _rootPath));
   consts.add(VM_PROCEDURE, writeAnsiLiteral(data, _T("InterpretLVM")));
   consts.add(VM_DEBUGPROCEDURE, writeAnsiLiteral(data, _T("SetDebugMode")));
   consts.add(VM_HOOK, vmHook);
   consts.add(VM_ERR_ERRPROC, writeAnsiLiteral(data, _T("GetLVMStatus")));
   consts.add(VM_ERR_DLLLOAD, writeErrorMessage(data, _T("Cannot load "), _rootPath, _T("\n")));
   consts.add(VM_ERR_DLLINVALID , writeErrorMessage(data, _T("Incorrect elenavm.dll\n")));

   _Memory* section = NULL;
   _Module* module = project->loadPrimitive(CORE_VM_MODULE, false);
   if (module != NULL) {
      //int type = project->IntSetting(opApplicationType, stConsole);
      ref_t reference = /*type == stGUI ? STD_VM_GUI_LOADER : */module->mapReference(VM_LOADER, true);
      section = module->mapSection(reference | mskNativeCodeRef, true);
   }
   if (section == NULL)
      throw InternalError("Cannnot load vm client loader");

   compiler->loadNativeCode(helper, code, module, section);
}

ref_t VirtualMachineClientImage :: createTape(MemoryWriter& data, Project* project)
{
   int tapeRef = data.Position();

   // write tape

   // USE_VM_MESSAGE_ID path, package
   writeTapeRecord(tapeRef, data, USE_VM_MESSAGE_ID, project->StrSetting(opNamespace), project->StrSetting(opOutputPath));

   // LOAD_VM_MESSAGE_ID name
   writeTapeRecord(tapeRef, data, LOAD_VM_MESSAGE_ID, project->StrSetting(opTemplate));

   // { MAP_VM_MESSAGE_ID fwrd, ref }*
   SourceIterator it = project->getForwardIt();
   while (!it.Eof()) {
      writeTapeRecord(tapeRef, data, MAP_VM_MESSAGE_ID, it.key(), *it);

      it++;
   }

   // START_VM_MESSAGE_ID debugMode ??
   writeTapeRecord(tapeRef, data, START_VM_MESSAGE_ID);

   // CALL_TAPE_MESSAGE_ID 'program
   writeTapeRecord(tapeRef, data, CALL_TAPE_MESSAGE_ID, ConstantIdentifier(STARTUP_CLASS));

   // SEND_MESSAGE 
   IdentifierString verb("0#");
   verb.append(0x20 + EVAL_MESSAGE_ID);
   writeTapeRecord(tapeRef, data, SEND_TAPE_MESSAGE_ID, verb);
   
   data.writeDWord(0);

   return tapeRef;
}

// --- VirtualMachineClientImage::VMClientHelper ---

void VirtualMachineClientImage::VMClientHelper :: writeReference(MemoryWriter& writer, const wchar16_t* reference, int mask)
{
   if (mask == mskImportRef) {
      writer.writeRef(_owner->resolveExternal(reference), 0);
   }
   else {
      int offset = _references->get(reference);

      if (offset != -1)
         writer.writeRef(mskRDataRef, offset);
   }
}
