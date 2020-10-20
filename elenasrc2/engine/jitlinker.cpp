//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class implementation.
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitlinker.h"

using namespace _ELENA_;

constexpr ref_t SIGNATURE_MASK = 0x80000000;

// --- resolveReference resolveMessage

inline void resolveReference(_Memory* image, size_t position, ref_t vaddress, size_t mask, bool virtualMode)
{
   if (!virtualMode) {
      if ((mask & mskImageMask) == mskRelCodeRef) {
         (*image)[position] = vaddress - ((size_t)image->get(0)) - position - 4;
      }
      else (*image)[position] += vaddress;
   }
   // in virtual mode
   else if ((mask & mskImageMask) == mskRelCodeRef) {
      image->addReference(vaddress | mskRelCodeRef, position);
   }
   else image->addReference(vaddress, position);
}

// --- ReferenceLoader::ReferenceHelper ---

SectionInfo JITLinker::ReferenceHelper :: getSection(ref_t reference, _Module* module)
{
   if (!module)
      module = _module;

   ident_t referenceName = module->resolveReference(reference & ~mskAnyRef);

   return _owner->_loader->getSectionInfo(ReferenceInfo(module, referenceName), reference & mskAnyRef, false);
}

SectionInfo JITLinker::ReferenceHelper :: getCoreSection(ref_t reference)
{
  return _owner->_loader->getCoreSectionInfo(reference, 0);
}

ref_t JITLinker::ReferenceHelper :: resolveMessage(ref_t reference, _Module* module)
{
   if (!module)
      module = _module;

   return _owner->resolveMessage(module, reference, _references);
}

void JITLinker::ReferenceHelper :: addBreakpoint(size_t position)
{
   if (!_debug)
      _debug = _owner->_loader->getTargetDebugSection();

   MemoryWriter writer(_debug);

   if (!_owner->_virtualMode) {
      ref_t address = (size_t)_owner->_codeBase;

      writer.writeDWord(address + position);
   }
   else writer.writeRef((ref_t)_owner->_codeBase, position);
}

void JITLinker::ReferenceHelper :: writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module)
{
   ref_t mask = reference & mskAnyRef;
   ref_t refID = reference & ~mskAnyRef;

   if (!module)
      module = _module;

   ref_t position = writer.Position();
   writer.writeDWord(disp);

   // vmt entry offset / address should be resolved later
   if (mask == mskVMTMethodAddress || mask == mskVMTEntryOffset) {
      _references->add(position, RefInfo(reference, module));
      return;
   }

   // try to resolve immediately
   void* vaddress = LOADER_NOTLOADED;
   switch (mask) {
      case mskPreloadCodeRef:
         vaddress = _owner->_compiler->getPreloadedReference(refID);
         mask = mskNativeCodeRef;
         break;
      case mskPreloadRelCodeRef:
         vaddress = _owner->_compiler->getPreloadedReference(refID);
         mask = mskNativeRelCodeRef;
         break;
      case mskCodeRef:
      case mskRelCodeRef:
         vaddress = (void*)refID;
         break;
      default:
         vaddress = _owner->_loader->resolveReference(_owner->_loader->retrieveReference(module, refID, mask), mask);
         break;
   }

   if (vaddress != LOADER_NOTLOADED) {
      resolveReference(writer.Memory(), position, (ref_t)vaddress, mask, _owner->_virtualMode);
   }
   // or resolve later
   else _references->add(position, RefInfo(reference, module));
}

//void JITLinker::ReferenceHelper :: writeXReference(MemoryWriter& writer, ref_t reference, ref64_t disp, _Module* module)
//{
//   ref_t mask = reference & mskAnyRef;
//
//   if (!module)
//      module = _module;
//
//   ref_t position = writer.Position();
//   writer.writeQWord(disp);
//
//   // vmt entry offset / address should be resolved later
//   if (mask == mskVMTXMethodAddress || mask == mskVMTXEntryOffset) {
//      _references->add(position, RefInfo(reference, module));
//      return;
//   }
//   // currently only mskVMTXMethodAddress and mskVMTXEntryOffset supported
//   else throw InternalError("64bit references are not supported");
//}

void JITLinker::ReferenceHelper :: writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp)
{
   if (!_owner->_virtualMode) {
      ref_t address = (ref_t)vaddress;

      // calculate relative address
      if (relative)
         address -= ((ref_t)writer.Address() + 4);

      writer.writeDWord(address + disp);
   }
   else if (relative) {
      writer.writeRef(((ref_t)vaddress | mskRelCodeRef), disp);
   }
   else writer.writeRef((ref_t)vaddress, disp);
}

// --- JITLinker ---

ref_t JITLinker :: mapAction(SectionInfo& messageTable, ident_t actionName, ref_t weakActionRef, ref_t signature)
{
   if (signature == 0u && weakActionRef != 0u)
      return weakActionRef;

   MemoryWriter mdataWriter(messageTable.section);

   ref_t actionRef = mdataWriter.Position() / 8;
   // weak action ref for strong one or the same ref
   if (weakActionRef) {
      mdataWriter.writeDWord(weakActionRef);
   }
   else mdataWriter.writeDWord(0);

   // signature or action name for weak message
   if (signature) {
      mdataWriter.writeRef(mskMessageTableRef | signature, 0);
   }
   else {
      MemoryWriter bodyWriter(_loader->getSectionInfo(ReferenceInfo(MESSAGEBODY_TABLE), mskRDataRef, true).section);

      mdataWriter.writeRef(mskMessageTableRef | bodyWriter.Position(), 0);

      bodyWriter.writeLiteral(actionName, getlength(actionName) + 1);
      bodyWriter.align(4, 0);
   }

   messageTable.module->mapPredefinedAction(actionName, actionRef, signature);

   return actionRef;
}

ref_t JITLinker :: resolveSignature(_Module* module, ref_t signature, bool variadicOne, References* references)
{
   if (!signature)
      return 0;

   ref_t signatures[ARG_COUNT];
   size_t count = module->resolveSignature(signature, signatures);

   // resolve the message
   IdentifierString signatureName;
   for (size_t i = 0; i != count; i++) {
      signatureName.append('$');
      ident_t referenceName = module->resolveReference(signatures[i]);

      if (isWeakReference(referenceName)) {
         if (isTemplateWeakReference(referenceName)) {
            ReferenceInfo refInfo = _loader->retrieveReference(module, signatures[i], mskVMTRef);

            signatureName.append(refInfo.referenceName);
         }
         else {
            signatureName.append(module->Name());
            signatureName.append(referenceName);
         }
      }
      else signatureName.append(referenceName);
   }

   if (count != 0 && variadicOne) {
      // HOTFIX : to tell apart vardiatic signature from normal ones (see further)
      signatureName.append("#params");
   }

   SectionInfo info = _loader->getSectionInfo(ReferenceInfo(MESSAGEBODY_TABLE), mskRDataRef, true);

   ref_t resolvedSignature = info.module->mapAction(signatureName.c_str(), 0u, true) & ~SIGNATURE_MASK;
   if (resolvedSignature == 0) {
      MemoryWriter writer(info.section);
      resolvedSignature = writer.Position();

      IdentifierString typeName;
      for (size_t i = 0; i != count; i++) {
         ident_t referenceName = module->resolveReference(signatures[i]);
         if (isWeakReference(referenceName)) {
            if (isTemplateWeakReference(referenceName)) {
               typeName.copy(referenceName);
            }
            else {
               typeName.copy(module->Name());
               typeName.append(referenceName);
            }
         }
         else typeName.copy(referenceName);

         ref_t typeClassRef = info.module->mapReference(typeName.c_str(), false);
         writer.writeRef(typeClassRef | mskVMTRef, 0);

         // NOTE : indicate weak class reference, to be later resolved if required
         if (references) {
            void* vaddress = _loader->resolveReference(
               _loader->retrieveReference(info.module, typeClassRef, mskVMTRef), mskVMTRef);

            if (vaddress == LOADER_NOTLOADED)
               references->add(INVALID_REF, RefInfo(typeClassRef | mskVMTRef, info.module));
         }
      }

      if (variadicOne) {
         // HOTFIX : vardiatic signature should end with zero for correct multi-dispatching operation
         writer.writeDWord(0);
      }

      // HOTFIX : adding a mask to tell apart a message name from a signature in meta module
      info.module->mapPredefinedAction(signatureName.c_str(), resolvedSignature | SIGNATURE_MASK, 0u);
   }

   return resolvedSignature;
}

ref_t JITLinker :: resolveWeakAction(SectionInfo& messageTable, ident_t actionName)
{
   ref_t resolvedAction = messageTable.module->mapAction(actionName, 0u, true);
   if (!resolvedAction) {
      resolvedAction = mapAction(messageTable, actionName, 0u, 0u);

      messageTable.module->mapPredefinedAction(actionName, resolvedAction, 0u);
   }

   return resolvedAction;
}

ident_t JITLinker :: retrieveResolvedAction(ref_t reference)
{
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);

   ref_t signature;
   return messageTable.module->resolveAction(reference, signature);
}

ref_t JITLinker :: resolveMessage(_Module* module, ref_t message, References* references)
{
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);

   ref_t actionRef, flags;
   int argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   // signature and custom verb should be imported
   ref_t signature;
   ident_t actionName = module->resolveAction(actionRef, signature);

   ref_t resolvedSignature = resolveSignature(module, signature, test(message, VARIADIC_MESSAGE), references);
   ref_t resolvedAction = messageTable.module->mapAction(actionName, resolvedSignature, true);
   if (!resolvedAction) {
      resolvedAction = mapAction(messageTable, actionName, resolveWeakAction(messageTable, actionName), resolvedSignature);
   }

   return encodeMessage(resolvedAction, argCount, flags);
}

void* JITLinker :: calculateVAddress(MemoryWriter* writer, int mask)
{
   return calculateVAddress(writer, mask, VA_ALIGNMENT);
}

void* JITLinker :: calculateVAddress(MemoryWriter* writer, int mask, int alignment)
{
   // align the section
   _compiler->alignCode(writer, alignment, test(mask, mskCodeRef));

   // virtual address - real address in the memory of nonvirtual mode, or section relative address
   return _virtualMode ? (void*)(writer->Position() | mask) : writer->Address();
}

int JITLinker :: resolveVMTMethodAddress(_Module* module, ref_t reference, int messageID)
{
   void* refVAddress = resolve(_loader->retrieveReference(module, reference, mskVMTRef), mskVMTRef, false);

   int address = _staticMethods.get(MethodInfo(refVAddress, messageID));
   if (address == -1) {
      address = getVMTMethodAddress(refVAddress, messageID);

      _staticMethods.add(MethodInfo(refVAddress, messageID), address);
   }

   return address;
}

void JITLinker :: fixReferences(References& references, _Memory* image)
{
   // fix not loaded references
   ref_t currentMask = 0;
   ref_t currentRef = 0;
   References::Iterator it = references.start();
   while (!it.Eof()) {
      RefInfo current = *it;

      currentMask = current.reference & mskAnyRef;
      currentRef = current.reference & ~mskAnyRef;

      // if it is a vmt method address
      if (currentMask == mskVMTMethodAddress) {
         resolve(_loader->retrieveReference(current.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         size_t offset = it.key();
         size_t messageID = (*image)[offset];

         (*image)[offset] = resolveVMTMethodAddress(current.module, currentRef, messageID);
         if (_virtualMode) {
            image->addReference(mskRelCodeRef, offset);
         }
         else (*image)[offset] -= (((size_t)image->get(0)) + offset + 4);
      }
      // if it is a vmt message offset
      else if (currentMask == mskVMTEntryOffset) {
         void* refVAddress = resolve(_loader->retrieveReference(current.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         size_t offset = it.key();
         size_t messageID = (*image)[offset];

         (*image)[offset] = getVMTMethodIndex(refVAddress, messageID);
      }
      //// if it is a vmtx method address
      ////else if (currentMask == mskVMTXMethodAddress) {
      ////   resolve(_loader->retrieveReference(current.module, currentRef, mskVMTRef), mskVMTRef, false);

      ////   // message id should be replaced with an appropriate method address
      ////   size_t offset = it.key();
      ////   ref64_t messageID = (*image)[offset + 4];
      ////   messageID <<= 32;
      ////   messageID |= (*image)[offset];

      ////   (*image)[offset] = resolveVMTMethodAddress(current.module, currentRef, fromMessage64(messageID));
      ////   if (_virtualMode) {
      ////      image->addReference(mskRelCodeRef, offset);
      ////   }
      ////   else (*image)[offset] -= (((size_t)image->get(0)) + offset + 4);
      ////}
      // otherwise
      else {
         void* refVAddress = resolve(_loader->retrieveReference(current.module, currentRef, currentMask), currentMask, false);

         // NOTE : check if it is a weak class reference (used for Message table generation)
         if (it.key() != INVALID_REF)
            resolveReference(image, it.key(), (ref_t)refVAddress, currentMask, _virtualMode);
      }
      it++;
   }
}

void* JITLinker :: getVMTAddress(_Module* module, ref_t reference, References& references)
{
   if (reference != 0) {
      ReferenceInfo referenceInfo = _loader->retrieveReference(module, reference, mskVMTRef);

      void* vaddress = _loader->resolveReference(referenceInfo, mskVMTRef);

      if (vaddress==LOADER_NOTLOADED) {
         // create VMT table without resolving references to prevent circular reference
         vaddress = createBytecodeVMTSection(referenceInfo, mskVMTRef, _loader->getClassSectionInfo(referenceInfo, mskClassRef, mskVMTRef, false), references);

         if (vaddress == LOADER_NOTLOADED)
            throw JITUnresolvedException(referenceInfo.referenceName);
      }
      return vaddress;
   }
   else return NULL;
}

void* JITLinker :: getVMTReference(_Module* module, ref_t reference, References& references)
{
   void* vaddress = getVMTAddress(module, reference, references);

   if (vaddress != NULL) {
      if (_virtualMode) {
         _Memory* image = _loader->getTargetSection(mskVMTRef);

         return image->get((ref_t)vaddress & ~mskAnyRef);
      }
      else return vaddress;
   }
   else return NULL;
}

void* JITLinker :: getVMTAddress(void* vaddress)
{
   void* vmtPtr;
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      vmtPtr = image->get((ref_t)vaddress & ~mskAnyRef);
   }
   else vmtPtr = vaddress;

   return vmtPtr;
}

int JITLinker :: getVMTMethodAddress(void* vaddress, int messageID)
{
   void* entries;
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      entries = image->get((ref_t)vaddress & ~mskAnyRef);
   }
   else entries = vaddress;

   return _compiler->findMethodAddress(entries, messageID, _compiler->findLength(entries));
}

int JITLinker :: getVMTMethodIndex(void* vaddress, int messageID)
{
   void* entries;
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      entries = image->get((ref_t)vaddress & ~mskAnyRef);
   }
   else entries = vaddress;

   return _compiler->findMethodIndex(entries, messageID, _compiler->findLength(entries));
}

size_t JITLinker :: getVMTFlags(void* vaddress)
{
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      return _compiler->findFlags(image->get((ref_t)vaddress & ~mskAnyRef));
   }
   else return _compiler->findFlags(vaddress);
}

size_t JITLinker :: loadMethod(ReferenceHelper& refHelper, MemoryReader& reader, MemoryWriter& writer)
{
   size_t position = writer.Position();

   // method just in time compilation
   _compiler->compileProcedure(refHelper, reader, writer);

   return _virtualMode ? position : (size_t)writer.Memory()->get(position);
}

void* JITLinker :: resolveNativeSection(ReferenceInfo referenceInfo, int mask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == NULL)
      return LOADER_NOTLOADED;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mask);
   MemoryWriter writer(image);

   void* vaddress = calculateVAddress(&writer, mask & mskImageMask);
   size_t position = writer.Position();

   _loader->mapReference(referenceInfo, vaddress, mask);

   // load section into target image
   MemoryReader reader(sectionInfo.section);
   writer.read(&reader, sectionInfo.section->Length());

   // resolve section references
   _ELENA_::RelocationMap::Iterator it(sectionInfo.section->getReferences());
   ref_t currentMask = 0;
   ref_t currentRef = 0;
   while (!it.Eof()) {
      currentMask = it.key() & mskAnyRef;
      currentRef = it.key() & ~mskAnyRef;

      if (currentMask == mskPreloadDataRef) {
         resolveReference(image, *it + position, (ref_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeDataRef, _virtualMode);
      }
      else if (currentMask == mskPreloadCodeRef) {
         resolveReference(image, *it + position, (ref_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeCodeRef, _virtualMode);
      }
      else if (currentMask == mskPreloadRelCodeRef) {
         resolveReference(image, *it + position, (ref_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeRelCodeRef, _virtualMode);
      }
      else {
         void* refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

         resolveReference(image, *it + position, (ref_t)refVAddress, currentMask, _virtualMode);
      }
      it++;
   }
   return vaddress;
}

void* JITLinker :: resolveNativeVariable(ReferenceInfo referenceInfo, int mask)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection((ref_t)mskDataRef);
   MemoryWriter writer(image);

   void* vaddress = calculateVAddress(&writer, mskDataRef, 4);

   _compiler->allocateVariable(writer);

   _loader->mapReference(referenceInfo, vaddress, mask);

   return vaddress;
}

//void* JITLinker :: resolveConstVariable(ident_t reference, int mask)
//{
//   // get target image & resolve virtual address
//   _Memory* image = _loader->getTargetSection((ref_t)mskRDataRef);
//   MemoryWriter writer(image);
//
//   void* vaddress = calculateVAddress(&writer, mskRDataRef, 4);
//
//   _compiler->allocateVariable(writer);
//
//   _loader->mapReference(reference, vaddress, mask);
//
//   return vaddress;
//}

void* JITLinker :: resolveBytecodeSection(ReferenceInfo referenceInfo, int mask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == NULL)
      return LOADER_NOTLOADED;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mask);
   MemoryWriter writer(image);

   void* vaddress = calculateVAddress(&writer, mask & mskImageMask);

   _loader->mapReference(referenceInfo, vaddress, mask);

   // symbol just in time compilation
   References references(RefInfo(0, NULL));
   ReferenceHelper refHelper(this, sectionInfo.module, &references);
   MemoryReader reader(sectionInfo.section);

   // create native debug info header if debug info enabled
   size_t sizePtr = (size_t)-1;
   if (mask == mskClassRef) {
//      // vmt vaddress is 0 for method handler / constructor
//      if (_withDebugInfo)
//         createNativeClassDebugInfo(reference, 0, sizePtr);
//
//      _compiler->compileMethod(refHelper, reader, writer);
   }
   else {
      if (_withDebugInfo)
         createNativeSymbolDebugInfo(referenceInfo, vaddress, sizePtr);

      // !! generate an error if the section length is zero (because the section is created even if the symbol does not exist, see compileDeclarations)

      _compiler->compileSymbol(refHelper, reader, writer);
   }

   if (_withDebugInfo)
      endNativeDebugInfo(sizePtr);

   // fix not loaded references
   fixReferences(references, image);

   if (sectionInfo.attrSection != nullptr) {
      MemoryReader attrReader(sectionInfo.attrSection);

      // generate run-time attributes
      ClassInfo::CategoryInfoMap attributes;
      attributes.read(&attrReader);

      referenceInfo.module = sectionInfo.module;
      createAttributes(referenceInfo, attributes);
   }

   return vaddress;
}

void* JITLinker :: resolveClassName(ReferenceInfo referenceInfo)
{
   IdentifierString relativeName;
   if (!referenceInfo.isRelative()) {
      relativeName.append(referenceInfo.referenceName + getlength(referenceInfo.module->Name()));
   }
   else relativeName.copy(referenceInfo.referenceName);

   return resolve(ReferenceInfo(relativeName.c_str()), mskLiteralRef, false);
}

void* JITLinker :: resolvePackage(_Module* module)
{
   ReferenceNs packageInfo("'", PACKAGE_SECTION);

   return resolve(ReferenceInfo(module, packageInfo.c_str()), mskConstArray, false);
}

void JITLinker :: resolveStaticValues(ReferenceInfo referenceInfo, MemoryReader& vmtReader, MemoryReader& attrReader,
   _Memory* vmtImage, ref_t position)
{
   // fix VMT Static table
   // NOTE : ignore virtual VMT
   ClassInfo::StaticInfoMap staticValues;
   staticValues.read(&vmtReader);

   ref_t currentMask = 0;
   ref_t currentRef = 0;
   for (auto it = staticValues.start(); !it.Eof(); it++) {
      currentMask = *it & mskAnyRef;
      currentRef = *it & ~mskAnyRef;

      void* refVAddress = NULL;
      if (currentMask == mskConstantRef && currentRef == 0) {
         // HOTFIX : ignore read-only sealed static field
      }
      else {
         if (*it == CLASSNAME_CONST) {
            refVAddress = resolveClassName(referenceInfo);

            currentMask = mskLiteralRef;
         }
         else if (*it == PACKAGE_CONST) {
            refVAddress = resolvePackage(referenceInfo.module);

            currentMask = mskConstArray;
         }
         /*if (currentMask == mskStatRef && currentRef == 0) {
            refVAddress = resolveAnonymousStaticVariable();
         }*/
         else if (currentRef != 0)
            refVAddress = resolve(_loader->retrieveReference(referenceInfo.module, currentRef, currentMask), currentMask, false);

         resolveReference(vmtImage, position + it.key() * 4, (ref_t)refVAddress, currentMask, _virtualMode);
      }
   }

   // generate run-time attributes
   ClassInfo::CategoryInfoMap attributes;
   attributes.read(&attrReader);

   createAttributes(referenceInfo, attributes);
}

void* JITLinker :: createBytecodeVMTSection(ReferenceInfo referenceInfo, int mask, ClassSectionInfo sectionInfo, References& references)
{
   if (sectionInfo.codeSection == NULL || sectionInfo.vmtSection == NULL)
      return LOADER_NOTLOADED;

   ReferenceHelper refHelper(this, sectionInfo.module, &references);

   MemoryReader attrReader(sectionInfo.attrSection);

   // VMT just in time compilation
   MemoryReader vmtReader(sectionInfo.vmtSection);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));

   // get target image & resolve virtual address
   _Memory* vmtImage = _loader->getTargetSection(mask);
   _Memory* codeImage = _loader->getTargetSection(mskClassRef);

   MemoryWriter vmtWriter(vmtImage);

   // allocate space and make VTM offset
   _compiler->allocateVMT(vmtWriter, header.flags, header.count, header.staticSize);

   void* vaddress = calculateVAddress(&vmtWriter, mask & mskImageMask);

   _loader->mapReference(referenceInfo, vaddress, mask);

   // if it is a standard VMT
   if (test(header.flags, elStandartVMT)) {
      size_t position = vmtWriter.Position();

      // load parent class
      void* parentVMT = getVMTReference(sectionInfo.module, header.parentRef, references);
      size_t count = _compiler->copyParentVMT(parentVMT, (VMTEntry*)vmtImage->get(position));

      // create native debug info header if debug info enabled
      size_t sizePtr = (size_t)-1;
      if (_withDebugInfo)
         createNativeClassDebugInfo(referenceInfo, vaddress, sizePtr);

      // read and compile VMT entries
      MemoryWriter   codeWriter(codeImage);
      MemoryReader   codeReader(sectionInfo.codeSection);

      size_t          methodPosition;
      VMTEntry        entry;

      size -= sizeof(ClassHeader);
      while (size > 0) {
         vmtReader.read((void*)&entry, sizeof(VMTEntry));

         codeReader.seek(entry.address);
         methodPosition = loadMethod(refHelper, codeReader, codeWriter);

         // NOTE : statically linked message is not added to VMT
         if (test(entry.message, STATIC_MESSAGE)) {
            _staticMethods.add(MethodInfo(vaddress, refHelper.resolveMessage(entry.message)), methodPosition);
         }
         else _compiler->addVMTEntry(refHelper.resolveMessage(entry.message), methodPosition, (VMTEntry*)vmtImage->get(position), count);

         size -= sizeof(VMTEntry);
      }
      if (_withDebugInfo)
         endNativeDebugInfo(sizePtr);

      if (count != header.count)
         throw InternalError("VMT structure is corrupt");

      // load class class
      void* classClassVAddress = getVMTAddress(sectionInfo.module, header.classRef, references);
      void* parentVAddress = NULL;
      if (header.parentRef != 0)
         parentVAddress = resolve(_loader->retrieveReference(sectionInfo.module, header.parentRef, mskVMTRef), mskVMTRef, true);

      // fix VMT
      _compiler->fixVMT(vmtWriter, (pos_t)classClassVAddress, (pos_t)parentVAddress, count, _virtualMode);

      if (!test(header.flags, elVirtualVMT)) {
         // HOTFIX : to presave the string name
         referenceInfo.module = sectionInfo.module;

         resolveStaticValues(referenceInfo, vmtReader, attrReader, vmtImage, position);
      }
   }

   return vaddress;
}

void JITLinker :: generateMetaAttribute(int category, ident_t fullName, void* address)
{
   SectionInfo tableInfo = _loader->getSectionInfo(ReferenceInfo(MATTRIBUTE_TABLE), mskRDataRef, false);

   MemoryWriter writer(tableInfo.section);

   writer.writeDWord(category);
   writer.writeDWord(getlength(fullName) + 9);
   writer.writeLiteral(fullName);

   if (!_virtualMode) {
      writer.writeDWord((size_t)address);
   }
   else writer.writeRef((ref_t)address, 0);
}

void JITLinker :: generateMetaAttribute(int category, ReferenceInfo& referenceInfo, int mask)
{
   IdentifierString fullName;
   if (referenceInfo.isRelative()) {
      fullName.copy(referenceInfo.module->Name());
      fullName.append(referenceInfo.referenceName);
   }
   else fullName.copy(referenceInfo.referenceName);

   void* address = resolve(referenceInfo, mask, false);

   generateMetaAttribute(category, fullName.ident(), address);
}

void JITLinker :: generateOverloadListMetaAttribute(_Module* module, ref_t message, ref_t listRef)
{
   ref_t actionRef, flags;
   int argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   // write the overload list name
   ref_t signature;
   ident_t actionName = module->resolveAction(actionRef, signature);

   IdentifierString fullName;
   fullName.copy(module->Name());
   fullName.append('\'');

   if (test(flags, VARIADIC_MESSAGE)) {
      fullName.append("params#");
   }

   fullName.append(actionName);
   fullName.append('[');
   fullName.appendInt(argCount);
   fullName.append(']');

   ident_t referenceName = module->resolveReference(listRef & ~mskAnyRef);

   // resolve extension overloadlist
   void* address = resolve(ReferenceInfo(module, referenceName), mskConstArray, false);

   generateMetaAttribute(caExtOverloadlist, fullName.ident(), address);
}

void JITLinker :: createAttributes(ReferenceInfo& referenceInfo, ClassInfo::CategoryInfoMap& attributes)
{
   auto it = attributes.start();
   while (!it.Eof()) {
      ClassInfo::Attribute attr = it.key();
      switch (attr.value1) {
         case caInitializer:
            _initializers.add(ModuleReference(referenceInfo.module, *it));
            break;
         case caSymbolSerializable:
            generateMetaAttribute(attr.value1, referenceInfo, mskSymbolRef);
            break;
         case caSerializable:
            generateMetaAttribute(attr.value1, referenceInfo, mskVMTRef);
            break;
         case caExtOverloadlist:
            generateOverloadListMetaAttribute(referenceInfo.module, attr.value2, *it);
            break;
         default:
            break;
      }

      it++;
   }
}

void* JITLinker :: resolveBytecodeVMTSection(ReferenceInfo referenceInfo, int mask, ClassSectionInfo sectionInfo)
{
   // HOTFIX : caching a reference name, the original string could be corrupted
   IdentifierString className(referenceInfo.referenceName);
   referenceInfo.referenceName = className.c_str();
   
   References      references(RefInfo(0, NULL));

   // create VMT
   void* vaddress = createBytecodeVMTSection(referenceInfo, mask, sectionInfo, references);

   //HOTFIX : resolve class symbol as well
   if (_classSymbolAutoLoadMode)
      resolve(referenceInfo, mskSymbolRef, true);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection(mskClassRef));

   return vaddress;
}

void JITLinker :: fixSectionReferences(SectionInfo& sectionInfo,  _Memory* image, size_t position, void* &vmtVAddress,
   bool constArrayMode, References* messageReferences)
{
   // resolve section references
   _ELENA_::RelocationMap::Iterator it(sectionInfo.section->getReferences());
   ref_t currentMask = 0;
   ref_t currentRef = 0;
   while (!it.Eof()) {
      currentMask = it.key() & mskAnyRef;
      currentRef = it.key() & ~mskAnyRef;

      if (currentMask == 0) {
         (*image)[*it + position] = resolveMessage(sectionInfo.module, (*sectionInfo.section)[*it], messageReferences);
      }
      else if (currentMask == mskVMTEntryOffset) {
         void* refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         size_t offset = *it;
         size_t messageID = resolveMessage(sectionInfo.module, (*image)[offset + position], messageReferences);

         (*image)[offset + position] = getVMTMethodIndex(refVAddress, messageID);
      }
      else if (currentMask == mskVMTMethodAddress) {
         resolve(_loader->retrieveReference(sectionInfo.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         size_t offset = *it;
         size_t messageID = resolveMessage(sectionInfo.module, (*image)[offset + position], messageReferences);

         (*image)[offset + position] = resolveVMTMethodAddress(sectionInfo.module, currentRef, messageID);
         if (_virtualMode) {
            image->addReference(mskCodeRef, offset + position);
         }
      }
      else if (constArrayMode && currentMask == mskMessage) {
         (*image)[*it + position] = parseMessage(sectionInfo.module->resolveReference(currentRef), false);
      }
      else if (constArrayMode && currentMask == mskMessageName) {
         (*image)[*it + position] = parseMessage(sectionInfo.module->resolveReference(currentRef), true);
      }
      else {
         void* refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

         if (*it == -4) {
            // resolve the constant vmt reference
            vmtVAddress = refVAddress;
         }
         else resolveReference(image, *it + position, (ref_t)refVAddress, currentMask, _virtualMode);
      }

      it++;
   }
}

void* JITLinker :: resolveConstant(ReferenceInfo referenceInfo, int mask, bool silentMode)
{
   References messageReferences(RefInfo(0, nullptr));

   bool constantValue = true;
   ident_t value = NULL;
   ReferenceInfo vmtReferenceInfo = referenceInfo;
   if (mask == mskLiteralRef) {
      value = vmtReferenceInfo.referenceName;
      vmtReferenceInfo.referenceName = _loader->getLiteralClass();
   }
   else if (mask == mskWideLiteralRef) {
      value = vmtReferenceInfo.referenceName;
      vmtReferenceInfo.referenceName = _loader->getWideLiteralClass();
   }
   else if (mask == mskCharRef) {
      value = vmtReferenceInfo.referenceName;
      vmtReferenceInfo.referenceName = _loader->getCharacterClass();
   }
   else if (mask == mskInt32Ref) {
      value = vmtReferenceInfo.referenceName;
      vmtReferenceInfo.referenceName = _loader->getIntegerClass();
   }
   else if (mask == mskInt64Ref) {
      value = vmtReferenceInfo.referenceName;
      vmtReferenceInfo.referenceName = _loader->getLongClass();
   }
   else if (mask == mskRealRef) {
      value = vmtReferenceInfo.referenceName;
      vmtReferenceInfo.referenceName = _loader->getRealClass();
   }
   else constantValue = false;

   // get constant VMT reference
   void* vmtVAddress = resolve(vmtReferenceInfo, mskVMTRef, true);

   // HOTFIX: if the constant is referred by iself it could be already resolved
   void* vaddress = _loader->resolveReference(referenceInfo, mask);
   if (vaddress != LOADER_NOTLOADED)
      return vaddress;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   int vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(referenceInfo, vaddress, mask);

   size_t position = writer.Position();
   if (mask == mskLiteralRef) {
      _compiler->compileLiteral(&writer, value);
   }
   else if (mask == mskWideLiteralRef) {
      WideString wideValue(value);

      _compiler->compileWideLiteral(&writer, wideValue);
   }
   else if (mask == mskCharRef) {
      _compiler->compileChar32(&writer, value);
   }
   else if (mask == mskInt32Ref) {
      _compiler->compileInt32(&writer, value.toULong(16));
   }
   else if (mask == mskInt64Ref) {
      // a constant starts with a special mark to tell apart from integer constant, so it should be skipped before converting to the number
      _compiler->compileInt64(&writer, value.toLongLong(10, 1));
   }
   else if (mask == mskRealRef) {
      _compiler->compileReal64(&writer, value.toDouble());
   }
   else if (mask == mskConstArray) {
      // resolve constant value
      SectionInfo sectionInfo = _loader->getSectionInfo(referenceInfo, mskRDataRef, silentMode);
      if (!sectionInfo.section)
         return nullptr;

      _compiler->compileCollection(&writer, sectionInfo.section);

      vmtVAddress = NULL; // !! to support dump array
      fixSectionReferences(sectionInfo, image, position, vmtVAddress, true, &messageReferences);
      constantValue = true;
   }
   else if (vmtVAddress == LOADER_NOTLOADED) {
      // resolve constant value
      SectionInfo sectionInfo = _loader->getSectionInfo(referenceInfo, mskRDataRef, false);
      _compiler->compileBinary(&writer, sectionInfo.section);

      fixSectionReferences(sectionInfo, image, position, vmtVAddress, false, &messageReferences);
      constantValue = true;
   }

   if (vmtVAddress == LOADER_NOTLOADED)
      throw JITUnresolvedException(referenceInfo);

   // check if the class could be constant one
   if (!constantValue) {
      // read VMT flags
      size_t flags = getVMTFlags(vmtVAddress);

      if (!test(flags, elStateless))
         throw JITConstantExpectedException(referenceInfo);
   }

   // fix object VMT reference
   resolveReference(image, vmtPosition, (ref_t)vmtVAddress, mskVMTRef, _virtualMode);

   // load message references
   fixReferences(messageReferences, nullptr);

   return vaddress;
}

//void* JITLinker :: resolveAnonymousStaticVariable()
//{
//   // get target image & resolve virtual address
//   MemoryWriter writer(_loader->getTargetSection((ref_t)mskStatRef));
//
//   size_t vaddress = (_virtualMode ? writer.Position() | mskStatRef : (size_t)writer.Address());
//
//   _compiler->allocateVariable(writer);
//
//   _statLength++;
//
//   return (void*)vaddress;
//}

void* JITLinker :: resolveStaticVariable(ReferenceInfo referenceInfo, int mask)
{
   // get target image & resolve virtual address
   MemoryWriter writer(_loader->getTargetSection(mask));

   size_t vaddress = (_virtualMode ? writer.Position() | mask : (size_t)writer.Address());

   _compiler->allocateVariable(writer);

   _statLength++;

   _loader->mapReference(referenceInfo, (void*)vaddress, mask);

   return (void*)vaddress;
}

void* JITLinker :: resolveMessageTable(ReferenceInfo referenceInfo, int mask)
{
   References      references(RefInfo(0, NULL));

   // get target image & resolve virtual address
   MemoryWriter writer(_loader->getTargetSection(mask));

   pos_t position = writer.Position();

   SectionInfo sectionInfo = _loader->getSectionInfo(referenceInfo, mskRDataRef, false);
   SectionInfo bodyInfo = _loader->getSectionInfo(ReferenceInfo(MESSAGEBODY_TABLE), mskRDataRef, false);

   // load table into target image
   MemoryReader reader(sectionInfo.section);
   writer.read(&reader, sectionInfo.section->Length());
   writer.writeDWord(0);
   writer.writeDWord(0);

   ref_t bodyPtr = writer.Position();

   // load table content into target image
   MemoryReader bodyReader(bodyInfo.section);
   writer.read(&bodyReader, bodyInfo.section->Length());

   // resolve section references
   _ELENA_::RelocationMap::Iterator it(sectionInfo.section->getReferences());
   while (!it.Eof()) {
      ref_t key = it.key();

      if ((key & mskAnyRef) == mskMessageTableRef) {
         // fix the reference to the message body buffer
         writer.seek(position + *it);
         writer.writeDWord(bodyPtr + (key & ~mskAnyRef));
      }
      else references.add(position + *it, RefInfo(key, sectionInfo.module));

      it++;
   }
   writer.seekEOF();

   // resolve signature references
   _ELENA_::RelocationMap::Iterator body_it(bodyInfo.section->getReferences());
   while (!body_it.Eof()) {
      references.add(bodyPtr + *body_it, RefInfo(body_it.key(), bodyInfo.module));

      body_it++;
   }

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection(mask));

   return nullptr; // !! should be resolved only once
}

void* JITLinker :: resolveMetaAttributeTable(ReferenceInfo, int mask)
{
   _Memory* asection = _loader->getTargetSection(mask);

   // get target image & resolve virtual address
   MemoryWriter writer(asection);

   SectionInfo bodyInfo = _loader->getSectionInfo(ReferenceInfo(MATTRIBUTE_TABLE), mskRDataRef, false);

   // load table into target image
   MemoryReader reader(bodyInfo.section);
   writer.writeDWord(bodyInfo.section->Length()); // section size
   pos_t offs = writer.Position();
   writer.read(&reader, bodyInfo.section->Length());

   // resolve signature references
   _ELENA_::RelocationMap::Iterator body_it(bodyInfo.section->getReferences());
   while (!body_it.Eof()) {
      asection->addReference(body_it.key(), offs + *body_it);

      body_it++;
   }

   return nullptr; // !! should be resolved only once
}

ref_t JITLinker :: parseMessage(ident_t reference, bool actionOnlyMode)
{
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);

   if (actionOnlyMode) {
      ref_t resolvedAction = messageTable.module->mapAction(reference, 0, true);
      if (!resolvedAction) {
         resolvedAction = resolveWeakAction(messageTable, reference);
      }

      return resolvedAction;
   }
   else {
      ref_t flags = 0;
      int paramCount = reference[0] - '0';

      // signature and custom verb should be imported
      ident_t actionName = reference + 1;

      ref_t resolvedAction = messageTable.module->mapAction(actionName, 0, true);
      if (!resolvedAction) {
         resolvedAction = resolveWeakAction(messageTable, actionName);
      }

      return encodeMessage(resolvedAction, paramCount, flags);
   }
}

void* JITLinker :: resolveExtensionMessage(ReferenceInfo referenceInfo, ident_t vmt)
{
   int dotPos = referenceInfo.referenceName.find('.');

   IdentifierString extensionName(referenceInfo.referenceName, dotPos);
   ref_t messageID = parseMessage(referenceInfo.referenceName + dotPos + 1, false);

   // HOTFIX : extension message should be a function one
   messageID |= FUNCTION_MESSAGE;

   void* vmtExtVAddress = resolve(ReferenceInfo(referenceInfo.module, extensionName), mskVMTRef, false);
   int entryOffset = getVMTMethodAddress(vmtExtVAddress, messageID);;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   int vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   void* vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(referenceInfo, vaddress, mskMessage);

   if (_virtualMode) {
      _compiler->compileInt64(&writer, messageID, mskCodeRef, entryOffset);
   }
   else _compiler->compileInt64(&writer, messageID, entryOffset);

   // get constant VMT reference
   void* vmtVAddress = resolve(vmt, mskVMTRef, false);

   // fix object VMT reference
   resolveReference(image, vmtPosition, (ref_t)vmtVAddress, mskVMTRef, _virtualMode);

   return vaddress;
}

void* JITLinker :: resolveMessage(ReferenceInfo referenceInfo, ident_t vmt, bool actionOnlyMode)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   int vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   void* vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(referenceInfo, vaddress, mskMessage);

   _compiler->compileInt32(&writer, parseMessage(referenceInfo.referenceName, actionOnlyMode));

   // get constant VMT reference
   void* vmtVAddress = resolve(vmt, mskVMTRef, false);

   // fix object VMT reference
   resolveReference(image, vmtPosition, (ref_t)vmtVAddress, mskVMTRef, _virtualMode);

   return vaddress;
}

////void* JITLinker :: resolveThreadSafeVariable(const TCHAR*  reference, int mask)
////{
////   // get target image & resolve virtual address
////   MemoryWriter writer(_loader->getTargetSection(mskTLSRef));
////
////   size_t vaddress = (_virtualMode ? writer.Position() : (size_t)writer.Address()) | mskTLSRef;
////
////   writer.writeDWord(0);
////
////   _loader->mapReference(reference, (void*)vaddress, mskTLSRef);
////
////   return (void*)vaddress;
////}

void JITLinker :: createNativeDebugInfo(ident_t reference, void* param, size_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);
   writer.writeLiteral(reference);

   sizePtr = writer.Position();
   writer.writeDWord(0); // size place holder

   writer.writeDWord((size_t)param);
}

void JITLinker :: createNativeSymbolDebugInfo(ReferenceInfo referenceInfo, void* address, size_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);

   // start with # to distinguish the symbol debug info from the class one
   if (referenceInfo.isRelative()) {
      IdentifierString name(referenceInfo.module->Name());
      if (referenceInfo.referenceName.find(1, '\'', NOTFOUND_POS) != NOTFOUND_POS) {
         name.append(NamespaceName(referenceInfo.referenceName));
      }

      name.append("'#");
      name.append(ReferenceName(referenceInfo.referenceName + 1));

      writer.writeLiteral(name);
   }
   else {
      IdentifierString name(NamespaceName(referenceInfo.referenceName), "'#", ReferenceName(referenceInfo.referenceName));
      writer.writeLiteral(name);
   }

   sizePtr = writer.Position();
   writer.writeDWord(0); // size place holder

   // save symbol entry

   // save VMT address
   if (!_virtualMode || address == NULL) {
      writer.writeDWord((size_t)address);
   }
   else writer.writeRef((ref_t)address, 0);
}

void JITLinker :: createNativeClassDebugInfo(ReferenceInfo referenceInfo, void* vaddress, size_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);
   if (referenceInfo.isRelative()) {
      IdentifierString name(referenceInfo.module->Name(), referenceInfo.referenceName);

      writer.writeLiteral(name.c_str());
   }
   else writer.writeLiteral(referenceInfo.referenceName);

   sizePtr = writer.Position();
   writer.writeDWord(0); // size place holder

   // save VMT address
   if (!_virtualMode || vaddress == NULL) {
      writer.writeDWord((size_t)vaddress);
   }
   else writer.writeRef((ref_t)vaddress, 0);
}

void JITLinker :: endNativeDebugInfo(size_t sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   (*debug)[sizePtr] = debug->Length() - sizePtr;
}

void* JITLinker :: resolveTemporalByteCode(_ReferenceHelper& helper, MemoryReader& reader, ident_t reference, void* param)
{
   _Memory* image = _loader->getTargetSection(mskCodeRef);

   // map dynamic code
   MemoryWriter writer(image);
   void* vaddress = calculateVAddress(&writer, mskCodeRef);

   if (param && _withDebugInfo && !emptystr(reference)) {
      // it it is a debug mode a special debug record is created containing link to tape
      size_t sizePtr = 0;
      createNativeDebugInfo(reference, param, sizePtr);

      //HOTFIX: temporal byte code is compiled as a method
      //because compileSymbol emulates embedded code now
      _compiler->compileProcedure(helper, reader, writer);

      endNativeDebugInfo(sizePtr);
   }
   //HOTFIX: temporal byte code is compiled as a method
   //because compileSymbol emulates embedded code now
   else _compiler->compileProcedure(helper, reader, writer);

   return vaddress;
}

void JITLinker :: onModuleLoad(_Module* module)
{
   IdentifierString initSymbol("'", INITIALIZER_SECTION);
   ref_t initRef = module->mapReference(initSymbol, true);
   if (initRef)
      _initializers.add(ModuleReference(module, initRef));

   if (_withExtDispatchers) {
      IdentifierString extInitSymbol("'", EXT_INITIALIZER_SECTION);
      ref_t extInitRef = module->mapReference(extInitSymbol, true);
      if (extInitRef)
         _initializers.add(ModuleReference(module, extInitRef));
   }
}

void JITLinker :: generateInitTape(MemoryDump& tape)
{
   ReferenceHelper helper(this, NULL, NULL);
   IdentifierString initSymbol("'", INITIALIZER_SECTION);

   auto it = _initializers.start();
   while (!it.Eof()) {
      _Module* module = (*it).value1;
      ref_t initRef = (*it).value2;

      void* initializer = resolveBytecodeSection(ReferenceInfo(module, initSymbol), mskCodeRef, helper.getSection(initRef | mskSymbolRef, module));
      if (initializer != LOADER_NOTLOADED) {
         if (!_virtualMode) {
            // HOTFIX : in VM mode - use relative address
            _Memory* image = _loader->getTargetSection(mskCodeRef);

            _compiler->generateSymbolCall(tape, (void*)((size_t)initializer - (size_t)image->get(0)));
         }
         else _compiler->generateSymbolCall(tape, initializer);
      }

      it++;
   }

   _initializers.clear();
}

void* JITLinker :: resolveEntry(void* programEntry)
{
   MemoryDump   ecodes;

   _compiler->generateProgramStart(ecodes);

   // generate module initializers
   generateInitTape(ecodes);

   _compiler->generateSymbolCall(ecodes, programEntry);

   _compiler->generateProgramEnd(ecodes);

   References references(RefInfo(0, NULL));
   ReferenceHelper refHelper(this, NULL, &references);
   MemoryReader reader(&ecodes);

   void* entry = resolveTemporalByteCode(refHelper, reader, NULL, NULL);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection(mskCodeRef));

   return entry;
}

// NOTE: reference should not be a forward one, otherwise there may be code duplication
void* JITLinker :: resolve(ReferenceInfo referenceInfo, int mask, bool silentMode)
{
   void* vaddress = _loader->resolveReference(referenceInfo, mask);
   if (vaddress==LOADER_NOTLOADED) {
      //printf("%s - %x\n", referenceInfo.referenceName.c_str(), mask);

      switch (mask) {
         case mskSymbolRef:
//         case mskClassRef:
            vaddress = resolveBytecodeSection(referenceInfo, mask, _loader->getSectionInfo(referenceInfo, mask, silentMode));
            break;
         case mskSymbolRelRef:
//         case mskClassRelRef:
            vaddress = resolveBytecodeSection(referenceInfo, mask & ~mskRelCodeRef, _loader->getSectionInfo(referenceInfo, mask & ~mskRelCodeRef, silentMode));
            break;
         case mskInternalRef:
         case mskInternalRelRef:
            vaddress = resolveBytecodeSection(referenceInfo, mask & ~mskRelCodeRef, _loader->getSectionInfo(referenceInfo, 0, silentMode));
            break;
         case mskVMTRef:
            vaddress = resolveBytecodeVMTSection(referenceInfo, mask, _loader->getClassSectionInfo(referenceInfo, mskClassRef, mskVMTRef, silentMode));
            break;
         case mskNativeCodeRef:
         case mskNativeDataRef:
         case mskNativeRDataRef:
            vaddress = resolveNativeSection(referenceInfo, mask, _loader->getSectionInfo(referenceInfo, mask, silentMode));
            break;
         case mskNativeRelCodeRef:
            vaddress = resolveNativeSection(referenceInfo, mskNativeCodeRef, _loader->getSectionInfo(referenceInfo, mskNativeCodeRef, silentMode));
            break;
         case mskConstantRef:
         case mskLiteralRef:
         case mskWideLiteralRef:
         case mskCharRef:
         case mskInt32Ref:
         case mskRealRef:
         case mskInt64Ref:
            vaddress = resolveConstant(referenceInfo, mask, silentMode);
            break;
         case mskConstArray:
            vaddress = resolveConstant(referenceInfo, mask, silentMode);
            break;
         case mskStatSymbolRef:
            vaddress = resolveStaticVariable(referenceInfo, mskStatRef);
            break;
         case mskMessage:
            vaddress = resolveMessage(referenceInfo, _loader->getMessageClass(), false);
            break;
         case mskMessageName:
            vaddress = resolveMessage(referenceInfo, _loader->getMessageNameClass(), true);
            break;
         case mskExtMessage:
            vaddress = resolveExtensionMessage(referenceInfo, _loader->getExtMessageClass());
            break;
         //case mskNativeVariable:
         case mskLockVariable:
            vaddress = resolveNativeVariable(referenceInfo, mask);
            break;
////         case mskConstVariable:
////            vaddress = resolveConstVariable(reference, mask);
////            break;
         case mskMessageTableRef:
            vaddress = resolveMessageTable(referenceInfo, mskMessageTableRef);
            break;
         case mskMetaAttributes:
            vaddress = resolveMetaAttributeTable(referenceInfo, mskMetaAttributes);
            break;
         case mskEntryRef:
            vaddress = resolveEntry(resolve(referenceInfo, mskSymbolRef, silentMode));
            break;
         case mskEntryRelRef:
            vaddress = resolveEntry(resolve(referenceInfo, mskSymbolRelRef, silentMode));
            break;
      }
   }
   if (!silentMode && vaddress == LOADER_NOTLOADED)
      throw JITUnresolvedException(referenceInfo);

   return vaddress;
}

void* JITLinker :: resolve(ident_t reference, int mask, bool silentMode)
{
   return resolve(ReferenceInfo(reference), mask, silentMode);
}

void JITLinker :: prepareCompiler()
{
   References      references(RefInfo(0, NULL));
   ReferenceHelper helper(this, NULL, &references);

   // load predefine messages
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);
   // dispatch message should be the first
   resolveWeakAction(messageTable, DISPATCH_MESSAGE);
   // protected default constructor message should be the second
   // NOTE : protected constructor action can be used only for default constructor due to current implementation
   // (we have to guarantee that default constructor is always the second one)
   resolveWeakAction(messageTable, CONSTRUCTOR_MESSAGE2);
   // constructor message should be the third
   resolveWeakAction(messageTable, CONSTRUCTOR_MESSAGE);

   _compiler->prepareCore(helper, _loader);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection((ref_t)mskCodeRef));
}

void JITLinker :: fixImage(ident_t superClass)
{
   // HOTFIX : setting a reference to the super class class in VOID
   void* superPtr = resolve(superClass, mskVMTRef, true);
   //void* ptr = _compiler->findClassPtr(superPtr);

   _compiler->setVoidParent(_loader, superPtr, _virtualMode);
}
