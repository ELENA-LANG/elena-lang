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
   _objectHeaderSize = compiler->getObjectHeaderSize();

   // load default forwards
   _literal = project->resolveForward(WSTR_FORWARD);
   _character = project->resolveForward(WCHAR_FORWARD);
   _int = project->resolveForward(INT_FORWARD);
   _long = project->resolveForward(LONG_FORWARD);
   _real = project->resolveForward(REAL_FORWARD);
   _message = project->resolveForward(MESSAGE_FORWARD);
   _signature = project->resolveForward(SIGNATURE_FORWARD);
   _verb = project->resolveForward(VERB_FORWARD);

   JITLinker linker(dynamic_cast<_JITLoader*>(this), compiler, true, (void*)mskCodeRef);

   helper.beforeLoad(compiler, *this);

   // initialize compiler inline code
   linker.prepareCompiler();

   // load starting symbol (it shouldn't be forward)
   ident_t entry = project->StrSetting(opEntry);

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
   ident_t starter = _project->resolveForward(STARTUP_CLASS);
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

SectionInfo ExecutableImage :: getSectionInfo(ident_t reference, size_t mask)
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

ClassSectionInfo ExecutableImage :: getClassSectionInfo(ident_t reference, size_t codeMask, size_t vmtMask, bool silentMode)
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
         return _objectHeaderSize;
      default:
         return 0;
   }
}

ident_t ExecutableImage::getLiteralClass()
{
   return _literal;
}

ident_t ExecutableImage::getCharacterClass()
{
   return _character;
}

ident_t ExecutableImage::getIntegerClass()
{
   return _int;
}

ident_t ExecutableImage::getLongClass()
{
   return _long;
}

ident_t ExecutableImage::getRealClass()
{
   return _real;
}

ident_t ExecutableImage::getMessageClass()
{
   return _message;
}

ident_t ExecutableImage::getSignatureClass()
{
   return _signature;
}

ident_t ExecutableImage::getVerbClass()
{
   return _verb;
}

ident_t ExecutableImage :: getNamespace()
{
   return _project->StrSetting(opNamespace);
}

ident_t ExecutableImage :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref || mask == mskCharRef) {
      return module->resolveConstant(reference);
   }
   // if it is a message
   else if (mask == 0) {
      return module->resolveSubject(reference);
   }
   else {
      ident_t referenceName = module->resolveReference(reference);
      while (isWeakReference(referenceName)) {
         ident_t resolvedName = _project->resolveForward(referenceName);

         if (!emptystr(resolvedName))  {
            referenceName = resolvedName;
         }
         else throw JITUnresolvedException(referenceName);
      }
      return referenceName;
   }
}

// --- VirtualMachineClientImage ---

inline void writeTapeRecord(MemoryWriter& tape, size_t command)
{
   tape.writeDWord(command);
   tape.writeDWord(0);
}

inline void writeTapeRecord(MemoryWriter& tape, size_t command, ident_t value)
{
   tape.writeDWord(command);

   if (!emptystr(value)) {
      tape.writeDWord(getlength(value) + 1);
      tape.writeLiteral(value, getlength(value) + 1);
   }
   else tape.writeDWord(0);
}

inline void writeTapeRecord(MemoryWriter& tape, size_t command, ident_t value1, ident_t value2)
{
   tape.writeDWord(command);
   // write total length including equal sign
   tape.writeDWord(getlength(value1) + getlength(value2) + 2);
   if (!emptystr(value1)) {
      tape.writeLiteral(value1, getlength(value1));
      tape.writeChar('=');
   }
   if (!emptystr(value2)) {
      tape.writeLiteral(value2);
   }
   else tape.writeChar((ident_c)0);
}

VirtualMachineClientImage :: VirtualMachineClientImage(Project* project, _JITCompiler* compiler)
   : Image(false), _exportReferences((size_t)-1)
{
   _project = project;

   MemoryWriter   data(&_data);
   MemoryWriter   code(&_text);

   // setup debugger hook record
   _bss.writeDWord(0, 0);               // reserve place for debug_mode, debug_address fields
   _bss.writeDWord(4, 0);

   size_t vmHook = data.Position();
   data.writeRef((ref_t)mskNativeDataRef, 0); // hook address

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

   consts.add(VM_TAPE, createTape(data, project));
   consts.add(VM_HOOK, vmHook);

   compiler->loadNativeCode(helper, code, module, section);
}

ref_t VirtualMachineClientImage :: createTape(MemoryWriter& data, Project* project)
{
   size_t tapeRef = data.Position();

   // write tape

   // USE_VM_MESSAGE_ID path, package
   writeTapeRecord(data, USE_VM_MESSAGE_ID, project->StrSetting(opNamespace), project->StrSetting(opOutputPath));

   // LOAD_VM_MESSAGE_ID name
   writeTapeRecord(data, LOAD_VM_MESSAGE_ID, project->StrSetting(opTemplate));

   // { MAP_VM_MESSAGE_ID fwrd, ref }*
   ForwardIterator it = project->getForwardIt();
   while (!it.Eof()) {
      writeTapeRecord(data, MAP_VM_MESSAGE_ID, it.key(), *it);

      it++;
   }

   // START_VM_MESSAGE_ID debugMode ??
   writeTapeRecord(data, START_VM_MESSAGE_ID);

   // CALL_TAPE_MESSAGE_ID 'program
   writeTapeRecord(data, CALL_TAPE_MESSAGE_ID, STARTUP_CLASS);

   // SEND_MESSAGE
   IdentifierString verb("0#");
   verb.append(0x20 + START_MESSAGE_ID);
   writeTapeRecord(data, SEND_TAPE_MESSAGE_ID, verb);

   data.writeDWord(0);

   return tapeRef;
}

// --- VirtualMachineClientImage::VMClientHelper ---

void VirtualMachineClientImage::VMClientHelper :: writeReference(MemoryWriter& writer, ident_t reference, int mask)
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
