//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image class implementations
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "image.h"
#include "errors.h"

using namespace _ELENA_;

// Virtual machine client built-in references
#define VM_LOADER          "$native'core_vm'console_vm_start"
//#define VM_GUI_LOADER      "$package'core_vm'gui_vm_start"

#define VM_TAPE            "'vm_tape"
#define VM_HOOK            "'vm_hook"

// --- ExecutableImage ---

ExecutableImage::ExecutableImage(Project* project, _JITCompiler* compiler, _Helper& helper)
   : Image(true)
{
   _project = project;

   // load default forwards
   _literal.copy(project->resolveForward(WSTR_FORWARD));
   _character.copy(project->resolveForward(WCHAR_FORWARD));
   _int.copy(project->resolveForward(INT_FORWARD));
   _long.copy(project->resolveForward(LONG_FORWARD));
   _real.copy(project->resolveForward(REAL_FORWARD));
   _message.copy(project->resolveForward(MESSAGE_FORWARD));
   _signature.copy(project->resolveForward(SIGNATURE_FORWARD));
   _verb.copy(project->resolveForward(VERB_FORWARD));

   JITLinker linker(dynamic_cast<_JITLoader*>(this), compiler, true, (void*)mskCodeRef);

   helper.beforeLoad(compiler, *this);

   // initialize compiler inline code
   linker.prepareCompiler();

   // load starting symbol (it shouldn't be forward)
   const wchar16_t* entry = project->StrSetting(opEntry);

   // create the image
   _entryPoint = linker.resolve(entry, mskNativeCodeRef, true);
   if(_entryPoint == LOADER_NOTLOADED)
      throw JITUnresolvedException(project->StrSetting(opEntry));

  // fix up static table size
   compiler->setStaticRootCounter(this, linker.getStaticCount(), true);

   helper.afterLoad(*this);
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
      case mskDebugRef:
         return &_debug;
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

SectionInfo ExecutableImage :: getCoreSectionInfo(ref_t reference, size_t mask)
{
   SectionInfo sectionInfo;

   sectionInfo.module = _project->resolveCore(reference, true);
   if (sectionInfo.module == NULL) {
      throw InternalError(errCommandSetAbsent);
   }
   else sectionInfo.section = sectionInfo.module->mapSection(reference | mask, true);

   if (sectionInfo.section == NULL) {
      throw InternalError(errCommandSetAbsent);
   }

   return sectionInfo;
}

ClassSectionInfo ExecutableImage :: getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask, bool silentMode)
{
   ClassSectionInfo sectionInfo;

   ref_t referenceID = 0;
   sectionInfo.module = _project->resolveModule(reference, referenceID, silentMode);
   if (sectionInfo.module == NULL || referenceID == 0) {
      if (!silentMode)
         throw JITUnresolvedException(reference);
   }
   else {
      sectionInfo.codeSection = sectionInfo.module->mapSection(referenceID | codeMask, true);
      sectionInfo.vmtSection = sectionInfo.module->mapSection(referenceID | vmtMask, true);
   }
   return sectionInfo;
}

size_t ExecutableImage :: getLinkerConstant(int id)
{
   switch (id) {
      case lnGCMGSize:
         return _project->IntSetting(opGCMGSize);
      case lnGCYGSize:
         return _project->IntSetting(opGCYGSize);
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

const wchar16_t* ExecutableImage :: getCharacterClass()
{
   return _character;
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

const wchar16_t* ExecutableImage :: getMessageClass()
{
   return _message;
}

const wchar16_t* ExecutableImage :: getSignatureClass()
{
   return _signature;
}

const wchar16_t* ExecutableImage :: getVerbClass()
{
   return _verb;
}

const wchar16_t* ExecutableImage :: getNamespace()
{
   return _project->StrSetting(opNamespace);
}

const wchar16_t* ExecutableImage :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref || mask == mskCharRef) {
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
   tape.writeDWord(command);
   tape.writeDWord(0);
}

inline void writeTapeRecord(size_t base, MemoryWriter& tape, size_t command, const wchar16_t* value)
{
   tape.writeDWord(command);

   if (!emptystr(value)) {
      tape.writeDWord((getlength(value) + 1) << 1);
      tape.writeWideLiteral(value, getlength(value) + 1);
   }
   else tape.writeDWord(0);
}

inline void writeTapeRecord(size_t base, MemoryWriter& tape, size_t command, const wchar16_t* value1, const wchar16_t* value2)
{
   tape.writeDWord(command);
   // write total length including equal sign
   tape.writeDWord((getlength(value1) + getlength(value2) + 2) << 1);
   if (!emptystr(value1)) {
      tape.writeWideLiteral(value1, getlength(value1));
      tape.writeWideChar('=');
   }
   if (!emptystr(value2)) {
      tape.writeWideLiteral(value2);
   }
   else tape.writeWideChar(0);
}

VirtualMachineClientImage :: VirtualMachineClientImage(Project* project, _JITCompiler* compiler, const tchar_t* appPath)
   : Image(false), _exportReferences((size_t)-1)
{
//   _project = project;

   MemoryWriter   data(&_data);
   MemoryWriter   code(&_text);

   // setup debugger hook record
   _bss.writeDWord(0, 0);               // reserve place for debug_mode, debug_address fields
   _bss.writeDWord(4, 0);

   size_t vmHook = data.Position();
   data.writeRef(mskNativeDataRef, 0); // hook address

//   int type = project->IntSetting(opPlatform, ptWin32Console);

   ref_t reference = 0;
   _Module* module = project->resolveModule(ReferenceNs(/*test(type, mtUIMask, mtGUI) ? VM_GUI_LOADER : */VM_LOADER), reference, true);
   _Memory* section = NULL;

   if (module != NULL) {      
      section = module->mapSection(reference | mskNativeCodeRef, true);
   }
   if (section == NULL)
      throw InternalError("Cannnot load vm client loader");

   // setup startup VM script
   ReferenceMap consts((size_t)-1);
   VMClientHelper helper(this, &consts, &data, module);

   consts.add(ConstantIdentifier(VM_TAPE), createTape(data, project));
   consts.add(ConstantIdentifier(VM_HOOK), vmHook);

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
   ForwardIterator it = project->getForwardIt();
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
      size_t offset = _references->get(reference);

      if ((int)offset == -1) {
         offset = _dataWriter->Position();

         _Memory* constant = _module->mapSection(_module->mapReference(reference, true) | mask, true);
         if (constant != NULL) {
            _dataWriter->write((char*)constant->get(0), constant->Length());
         }
         else throw InternalError("Cannnot load vm client loader");
      }

      writer.writeRef(mskRDataRef, offset);
   }
}
