//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Image class implementations
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "image.h"
#include "errors.h"
#include "bytecode.h"

using namespace _ELENA_;

// --- ExecutableImage ---

ExecutableImage :: ExecutableImage(bool standAlone, Project* project, _JITCompiler* compiler, _Helper& helper)
   : Image(standAlone)
{
   _project = project;
   _objectHeaderSize = compiler->getObjectHeaderSize();

  // create a special module containing message and meta attribute data
   _Module* messages = _project->createModule(META_MODULE);
   compiler->allocateMetaInfo(messages);

  // load default forwards
   _literal = project->resolveForward(STR_FORWARD);
   _wideLiteral = project->resolveForward(WIDESTR_FORWARD);
   _character = project->resolveForward(CHAR_FORWARD);
   _int = project->resolveForward(INT_FORWARD);
   _long = project->resolveForward(LONG_FORWARD);
   _real = project->resolveForward(REAL_FORWARD);
   _message = project->resolveForward(MESSAGE_FORWARD);
   _ext_message = project->resolveForward(EXT_MESSAGE_FORWARD);
   _messageName = project->resolveForward(MESSAGENAME_FORWARD);

   //resolveExternal()

   JITLinker linker(dynamic_cast<_JITLoader*>(this), compiler, true, (ref_t)mskCodeRef,
      project->BoolSetting(opExtDispatchers),
      project->BoolSetting(opClassSymbolAutoLoad));

  // save root namespace
   _debug.writeLiteral(_debug.Length(), getNamespace());

   helper.beforeLoad(compiler, *this);

  // initialize compiler inline code
   linker.prepareCompiler();

  // create the image
   ident_t entryName = project->resolveForward(SYSTEM_ENTRY);
   _entryPoint = emptystr(entryName) ? LOADER_NOTLOADED : linker.resolve(entryName, mskSymbolRef, true);
   if(_entryPoint == LOADER_NOTLOADED)
      throw JITUnresolvedException(ReferenceInfo(SYSTEM_ENTRY));

   linker.fixImage(project->resolveForward(SUPER_FORWARD));

  // fix up static table size
   compiler->setStaticRootCounter(this, linker.getStaticCount(), true);

  // resolve message & action tables
   linker.resolve(MESSAGE_TABLE, mskMessageTableRef, true);

  // resolve attribute table
   linker.resolve(MATTRIBUTE_TABLE, mskMetaAttributes, true);

   helper.afterLoad(*this);
}

ref_t ExecutableImage :: getDebugEntryPoint()
{
   ident_t starter = _project->resolveForward(PROGRAM_ENTRY);
   while (isForwardReference(starter)) {
      starter = _project->resolveForward(starter + FORWARD_PREFIX_NS_LEN);
   }

   return (ref_t)resolveReference(starter, mskSymbolRef) & ~mskAnyRef;
}

_Memory* ExecutableImage :: getTargetSection(ref_t mask)
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
      case mskMetaRef:
         switch (mask) {
            case mskMessageTableRef:
               return &_mdata;
            case mskMetaAttributes:
               return &_adata;
            default:
               return &_debug;
         }
      default:
         return NULL;
   }
}

SectionInfo ExecutableImage :: getSectionInfo(ReferenceInfo referenceInfo, ref_t mask, bool silentMode)
{
   SectionInfo sectionInfo;

   ref_t referenceID = 0;
   if (referenceInfo.isRelative()) {
      sectionInfo.module = referenceInfo.module;
      referenceID = referenceInfo.module->mapReference(referenceInfo.referenceName, true);
   }
   else sectionInfo.module = _project->resolveModule(referenceInfo.referenceName, referenceID);

   if (sectionInfo.module != NULL && referenceID != 0) {
      sectionInfo.section = sectionInfo.module->mapSection(referenceID | mask, true);
      sectionInfo.attrSection = sectionInfo.module->mapSection(referenceID | mskSymbolAttributeRef, true);
   }

   if (sectionInfo.section == NULL && !silentMode) {
      throw JITUnresolvedException(referenceInfo);
   }

   return sectionInfo;
}

SectionInfo ExecutableImage :: getCoreSectionInfo(ref_t reference, ref_t mask)
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

ClassSectionInfo ExecutableImage :: getClassSectionInfo(ReferenceInfo referenceInfo, ref_t codeMask, ref_t vmtMask, bool silentMode)
{
   ClassSectionInfo sectionInfo;

   ref_t referenceID = 0;
   if (referenceInfo.isRelative()) {
      if (isTemplateWeakReference(referenceInfo.referenceName)) {
         sectionInfo.module = _project->resolveModule(referenceInfo.referenceName, referenceID, silentMode);
      }
      else {
         sectionInfo.module = referenceInfo.module;
         referenceID = referenceInfo.module->mapReference(referenceInfo.referenceName, true);
      }
   }
   else sectionInfo.module = _project->resolveModule(referenceInfo.referenceName, referenceID, silentMode);

   if (sectionInfo.module == NULL || referenceID == 0) {
      if (!silentMode)
         throw JITUnresolvedException(referenceInfo);
   }
   else {
      sectionInfo.codeSection = sectionInfo.module->mapSection(referenceID | codeMask, true);
      sectionInfo.vmtSection = sectionInfo.module->mapSection(referenceID | vmtMask, true);
      sectionInfo.attrSection = sectionInfo.module->mapSection(referenceID | mskAttributeRef, true);
   }

   return sectionInfo;
}

pos_t ExecutableImage :: getLinkerConstant(int id)
{
   switch (id) {
      case lnGCMGSize:
         return _project->IntSetting(opGCMGSize);
      case lnGCYGSize:
         return _project->IntSetting(opGCYGSize);
      case lnGCPERMSize:
         return _project->IntSetting(opGCPERMSize);
      case lnThreadCount:
         return _project->IntSetting(opThreadMax);
      case lnObjectSize:
         return _objectHeaderSize;
      default:
         return 0;
   }
}

ident_t ExecutableImage :: getLiteralClass()
{
   return _literal;
}

ident_t ExecutableImage :: getWideLiteralClass()
{
   return _wideLiteral;
}

ident_t ExecutableImage :: getCharacterClass()
{
   return _character;
}

ident_t ExecutableImage::getIntegerClass()
{
   return _int;
}

ident_t ExecutableImage :: getLongClass()
{
   return _long;
}

ident_t ExecutableImage :: getRealClass()
{
   return _real;
}

ident_t ExecutableImage :: getMessageClass()
{
   return _message;
}

ident_t ExecutableImage :: getExtMessageClass()
{
   return _ext_message;
}

ident_t ExecutableImage :: getMessageNameClass()
{
   return _messageName;
}

ident_t ExecutableImage :: getNamespace()
{
   return _project->StrSetting(opNamespace);
}

ident_t ExecutableImage :: resolveTemplateWeakReference(ident_t referenceName)
{
   ident_t resolvedName = _project->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
   if (emptystr(resolvedName)) {
      if (referenceName.endsWith(CLASSCLASS_POSTFIX)) {
         // HOTFIX : class class reference should be resolved simultaneously with class one
         IdentifierString classReferenceName(referenceName, getlength(referenceName) - getlength(CLASSCLASS_POSTFIX));

         classReferenceName.copy(resolveTemplateWeakReference(classReferenceName.c_str()));
         classReferenceName.append(CLASSCLASS_POSTFIX);

         _project->addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, classReferenceName.c_str());

         return _project->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }

      // COMPILER MAGIC : try to find a template implementation
      ref_t resolvedRef = 0;
      _Module* refModule = _project->resolveWeakModule(referenceName + TEMPLATE_PREFIX_NS_LEN, resolvedRef, true);
      if (refModule != nullptr) {
         ident_t resolvedReferenceName = refModule->resolveReference(resolvedRef);
         if (isWeakReference(resolvedReferenceName)) {
            IdentifierString fullName(refModule->Name(), resolvedReferenceName);

            _project->addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, fullName);
         }
         else _project->addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, resolvedReferenceName);

         referenceName = _project->resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }
      else throw JITUnresolvedException(referenceName);
   }
   else referenceName = resolvedName;

   return referenceName;
}

ReferenceInfo ExecutableImage :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref || mask == mskCharRef || mask == mskWideLiteralRef) {
      return module->resolveConstant(reference);
   }
   // if it is a message
   else if (mask == 0) {
      ref_t signRef = 0;
      return module->resolveAction(reference, signRef);
   }
   else {
      ident_t referenceName = module->resolveReference(reference);
      while (isForwardReference(referenceName)) {
         ident_t resolvedName = _project->resolveForward(referenceName + FORWARD_PREFIX_NS_LEN);
         if (!emptystr(resolvedName)) {
            referenceName = resolvedName;
         }
         else throw JITUnresolvedException(referenceName);
      }

      if (isWeakReference(referenceName)) {
         if (isTemplateWeakReference(referenceName)) {
            referenceName = resolveTemplateWeakReference(referenceName);
         }

         return ReferenceInfo(module, referenceName);
      }
      return ReferenceInfo(referenceName);
   }
}

// --- ExecutableImage::_Helper ---

inline void writeTapeRecord(MemoryWriter& tape, size_t command)
{
   tape.writeDWord(command);
   tape.writeDWord(0);
}

inline void writeTapeRecord(MemoryWriter& tape, size_t command, ident_t value, bool forward = false)
{
   tape.writeDWord(command);

   if (!emptystr(value)) {
      if (forward) {
         tape.writeDWord(getlength(value) + 1 + FORWARD_PREFIX_NS_LEN);
         tape.writeLiteral(FORWARD_PREFIX_NS, FORWARD_PREFIX_NS_LEN);
         tape.writeLiteral(value, getlength(value) + 1);
      }
      else {
         tape.writeDWord(getlength(value) + 1);
         tape.writeLiteral(value, getlength(value) + 1);
      }
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
   else tape.writeChar((char)0);
}

void ExecutableImage::_Helper :: createTape(_Memory& tape, Project* project, bool withNewConsole)
{
   MemoryWriter data(&tape);

   // write tape

   // USE_VM_MESSAGE_ID path, package
   writeTapeRecord(data, USE_VM_MESSAGE_ID, project->StrSetting(opNamespace), project->StrSetting(opOutputPath));

   // LOAD_VM_MESSAGE_ID name
   writeTapeRecord(data, LOAD_VM_MESSAGE_ID, project->StrSetting(opTemplate));

   // { MAP_VM_MESSAGE_ID fwrd, ref }*
   ForwardIterator it = project->getForwardIt();
   while (!it.Eof()) {
      ident_t fwd = *it;

      writeTapeRecord(data, MAP_VM_MESSAGE_ID, it.key(), fwd);

      it++;
   }

   if (withNewConsole) {
      writeTapeRecord(data, OPEN_VM_CONSOLE);
   }

   // START_VM_MESSAGE_ID debugMode ??
   writeTapeRecord(data, START_VM_MESSAGE_ID);

   // CALL_TAPE_MESSAGE_ID 'program
   writeTapeRecord(data, CALL_TAPE_MESSAGE_ID, PROGRAM_ENTRY, true);

   data.writeDWord(0);
}

//// --- VirtualMachineClientImage ---
//
//VirtualMachineClientImage :: VirtualMachineClientImage(Project* project, _JITCompiler* compiler, bool guiMode)
//   : Image(false), _exportReferences((size_t)-1)
//{
////   _project = project;
////
////
////   MemoryWriter   code(&_text);
////
////   // setup debugger hook record
////   _bss.writeDWord(0, 0);               // reserve place for debug_mode, debug_address fields
////   _bss.writeDWord(4, 0);
////
////   size_t vmHook = data.Position();
////   data.writeRef((ref_t)mskNativeDataRef, 0); // hook address
////
//////   int type = project->IntSetting(opPlatform, ptWin32Console);
////
////   ref_t reference = 0;
////   _Module* module = project->resolveModule(ReferenceNs(/*test(type, mtUIMask, mtGUI) ? VM_GUI_LOADER : */VM_LOADER), reference, true);
////   _Memory* section = NULL;
////
////   if (module != NULL) {
////      section = module->mapSection(reference | mskNativeCodeRef, true);
////   }
////   if (section == NULL)
////      throw InternalError("Cannnot load vm client loader");
////
////   // setup startup VM script
////   ReferenceMap consts((size_t)-1);
////   VMClientHelper helper(this, &consts, &data, module);
////
////   consts.add(VM_TAPE, createTape(data, project, guiMode));
////   consts.add(VM_HOOK, vmHook);
////
////   compiler->loadNativeCode(helper, code, module, section);
//}
//
//// --- VirtualMachineClientImage::VMClientHelper ---
//
//void VirtualMachineClientImage::VMClientHelper :: writeReference(MemoryWriter& writer, ident_t reference, int mask)
//{
//   if (mask == mskImportRef) {
//      writer.writeRef(_owner->resolveExternal(reference), 0);
//   }
//   else {
//      size_t offset = _references->get(reference);
//
//      if ((int)offset == -1) {
//         offset = _dataWriter->Position();
//
//         _Memory* constant = _module->mapSection(_module->mapReference(reference, true) | mask, true);
//         if (constant != NULL) {
//            _dataWriter->write((char*)constant->get(0), constant->Length());
//         }
//         else throw InternalError("Cannnot load vm client loader");
//      }
//
//      writer.writeRef(mskRDataRef, offset);
//   }
//}
