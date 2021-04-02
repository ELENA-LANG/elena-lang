//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitlinker.h"

using namespace _ELENA_;

constexpr ref_t SIGNATURE_MASK = 0x80000000;

// --- resolveReference ---

inline bool writeVAddr(_Memory* image, pos_t position, lvaddr_t addr)
{
   return image->write(position, &addr, sizeof(lvaddr_t));
}

inline bool writeVAddrOffset(_Memory* image, pos_t position, pos_t addrOffset)
{
   return image->write(position, &addrOffset, 4);
}

inline bool updateVAddr(_Memory* image, pos_t position, lvaddr_t addr)
{
   lvaddr_t val = 0;
   image->read(position, &val, sizeof(lvaddr_t));
   addr += val;

   return image->write(position, &addr, sizeof(lvaddr_t));
}

inline void resolveReference(_Memory* image, pos_t position, lvaddr_t vaddress, ref_t mask, bool virtualMode)
{
   if (!virtualMode) {
      switch (mask & mskImageMask) {
         case mskRelCodeRef:
         case mskRelStatRef:
         case mskRelDataRef:
            // NOTE : the offset should be written
            writeVAddrOffset(image, position, (pos_t)(vaddress - ((lvaddr_t)image->get(0)) - position - 4));
            break;
         case mskRelExternakRef:
            // NOTE : the offset should be written
            vaddress &= ~mskRelImportRef;
            writeVAddrOffset(image, position, (pos_t)(vaddress - ((lvaddr_t)image->get(0)) - position - 4));
            break;
         default:
            updateVAddr(image, position, vaddress);
            break;
      }
   }
   else {
      switch (mask & mskImageMask) {
         case mskRelCodeRef:
            image->addReference((ref_t)vaddress | mskRelCodeRef, position);
            break;
         case mskRelStatRef:
            image->addReference((ref_t)vaddress | mskRelStatRef, position);
            break;
         case mskRelDataRef:
            image->addReference((ref_t)vaddress | mskRelDataRef, position);
            break;
         default:
            image->addReference((ref_t)vaddress, position);
            break;
      }
   }
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

mssg_t JITLinker::ReferenceHelper :: resolveMessage(mssg_t reference, _Module* module)
{
   if (!module)
      module = _module;

   return _owner->resolveMessage(module, reference, _references);
}

void JITLinker::ReferenceHelper :: addBreakpoint(pos_t position)
{
   if (!_debug)
      _debug = _owner->_loader->getTargetDebugSection();

   MemoryWriter writer(_debug);

   if (!_owner->_virtualMode) {
      writer.writeDWord(_owner->_codeBase + position);
   }
   else writer.writeRef(_owner->_codeBase, position);
}

void JITLinker::ReferenceHelper :: writeReference(MemoryWriter& writer, ref_t reference, pos_t disp, _Module* module)
{
   ref_t mask = reference & mskAnyRef;
   ref_t refID = reference & ~mskAnyRef;

   if (!module)
      module = _module;

   pos_t position = writer.Position();
   writer.writeDWord(disp);

   // vmt entry offset / address should be resolved later
   if (mask == mskVMTMethodAddress || mask == mskVMTEntryOffset) {
      _references->add(position, RefInfo(reference, module));
      return;
   }

   // try to resolve immediately
   lvaddr_t vaddress = LOADER_NOTLOADED;
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
         vaddress = (lvaddr_t)refID;
         break;
      default:
         vaddress = _owner->_loader->resolveReference(_owner->_loader->retrieveReference(module, refID, mask), mask);
         break;
   }

   if (vaddress != LOADER_NOTLOADED) {
      resolveReference(writer.Memory(), position, vaddress, mask, _owner->_virtualMode);
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

void JITLinker::ReferenceHelper :: writeVAddress(MemoryWriter& writer, lvaddr_t vaddress, pos_t disp)
{
   if (!_owner->_virtualMode) {
      ref_t address = vaddress;

      writer.writeDWord(address + disp);
   }
   else writer.writeRef(vaddress, disp);
}

void JITLinker::ReferenceHelper::writeRelVAddress(MemoryWriter& writer, lvaddr_t vaddress, ref_t mask, pos_t disp)
{
   if (!_owner->_virtualMode) {
      // calculate relative address
      vaddress -= ((lvaddr_t)writer.Address() + 4);

      ref_t address = vaddress;

      writer.writeDWord(address + disp);
   }
   else writer.writeRef((vaddress | mask), disp);
}

// --- JITLinker ---

ref_t JITLinker :: mapAction(SectionInfo& messageTable, ident_t actionName, ref_t weakActionRef, ref_t signature)
{
   if (signature == 0u && weakActionRef != 0u)
      return weakActionRef;

   MemoryWriter mdataWriter(messageTable.section);
   MemoryWriter bodyWriter(_loader->getSectionInfo(ReferenceInfo(MESSAGEBODY_TABLE), mskRDataRef, true).section);

   ref_t actionRef = _compiler->allocateActionEntry(mdataWriter, bodyWriter, actionName, weakActionRef, signature);

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
         _compiler->allocateSignatureEntry(writer, typeClassRef);

         // NOTE : indicate weak class reference, to be later resolved if required
         if (references) {
            lvaddr_t vaddress = _loader->resolveReference(
               _loader->retrieveReference(info.module, typeClassRef, mskVMTRef), mskVMTRef);

            if (vaddress == LOADER_NOTLOADED)
               references->add(INVALID_REF, RefInfo(typeClassRef | mskVMTRef, info.module));
         }
      }

      if (variadicOne) {
         // HOTFIX : vardiatic signature should end with zero for correct multi-dispatching operation
         _compiler->allocateSignatureEntry(writer, 0);
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

mssg_t JITLinker :: resolveMessage(_Module* module, mssg_t message, References* references)
{
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);

   ref_t actionRef, flags;
   pos_t argCount = 0;
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

lvaddr_t JITLinker :: calculateVAddress(MemoryWriter* writer, ref_t mask)
{
   return calculateVAddress(writer, mask, VA_ALIGNMENT);
}

lvaddr_t JITLinker :: calculateVAddress(MemoryWriter* writer, ref_t mask, int alignment)
{
   // align the section
   _compiler->alignCode(writer, alignment, test(mask, mskCodeRef));

   // virtual address - real address in the memory of nonvirtual mode, or section relative address
   return _virtualMode ? (writer->Position() | mask) : (lvaddr_t)writer->Address();
}

lvaddr_t JITLinker :: resolveVMTMethodAddress(_Module* module, ref_t reference, mssg_t messageID)
{
   lvaddr_t refVAddress = resolve(_loader->retrieveReference(module, reference, mskVMTRef), mskVMTRef, false);

   lvaddr_t address = _staticMethods.get(MethodInfo(refVAddress, messageID));
   if (address == INVALID_VADDR) {
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
         pos_t offset = it.key();
         mssg_t messageID = (*image)[offset];

         lvaddr_t address = resolveVMTMethodAddress(current.module, currentRef, messageID);
         if (_virtualMode) {
            (*image)[offset] = (pos_t)address;

            image->addReference(mskRelCodeRef, (pos_t)offset);
         }
         else {
            address -= ((lvaddr_t)image->get(0)) + offset + 4;

            (*image)[offset] = (pos_t)address;
         }
      }
      // if it is a vmt message offset
      else if (currentMask == mskVMTEntryOffset) {
         lvaddr_t refVAddress = resolve(_loader->retrieveReference(current.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         pos_t offset = it.key();
         mssg_t messageID = (*image)[offset];

         (*image)[offset] = getVMTMethodIndex(refVAddress, messageID);
      }
      // otherwise
      else {
         lvaddr_t refVAddress = resolve(_loader->retrieveReference(current.module, currentRef, currentMask), currentMask, false);

         // NOTE : check if it is a weak class reference (used for Message table generation)
         if (it.key() != INVALID_REF)
            resolveReference(image, it.key(), refVAddress, currentMask, _virtualMode);
      }
      it++;
   }
}

lvaddr_t JITLinker :: getVMTAddress(_Module* module, ref_t reference, References& references)
{
   if (reference != 0) {
      ReferenceInfo referenceInfo = _loader->retrieveReference(module, reference, mskVMTRef);

      lvaddr_t vaddress = _loader->resolveReference(referenceInfo, mskVMTRef);

      if (vaddress==LOADER_NOTLOADED) {
         // create VMT table without resolving references to prevent circular reference
         vaddress = createBytecodeVMTSection(referenceInfo, mskVMTRef, 
            _loader->getClassSectionInfo(referenceInfo, mskClassRef, mskVMTRef, false), references);

         if (vaddress == LOADER_NOTLOADED)
            throw JITUnresolvedException(referenceInfo.referenceName);
      }
      return vaddress;
   }
   else return 0;
}

void* JITLinker :: getVMTReference(_Module* module, ref_t reference, References& references)
{
   lvaddr_t vaddress = getVMTAddress(module, reference, references);

   if (vaddress != 0) {
      if (_virtualMode) {
         _Memory* image = _loader->getTargetSection(mskVMTRef);

         return image->get((pos_t)vaddress & ~mskAnyRef);
      }
      else return (void*)vaddress;
   }
   else return nullptr;
}

//lvaddr_t JITLinker :: getVMTAddress(ref_t address)
//{
//   lvaddr_t vmtPtr;
//   if (_virtualMode) {
//      _Memory* image = _loader->getTargetSection(mskVMTRef);
//
//      vmtPtr = (lvaddr_t)image->get(address & ~mskAnyRef);
//   }
//   else vmtPtr = address;
//
//   return vmtPtr;
//}

lvaddr_t JITLinker :: getVMTMethodAddress(lvaddr_t address, mssg_t messageID)
{
   void* entries;
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      entries = image->get(address & ~mskAnyRef);
   }
   else entries = (void*)address;

   return _compiler->findMethodAddress(entries, messageID, _compiler->findLength(entries));
}

pos_t JITLinker :: getVMTMethodIndex(lvaddr_t address, mssg_t messageID)
{
   void* entries;
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      entries = image->get(address & ~mskAnyRef);
   }
   else entries = (void*)address;

   return _compiler->findMethodIndex(entries, messageID, _compiler->findLength(entries));
}

ref_t JITLinker :: getVMTFlags(lvaddr_t vaddress)
{
   if (_virtualMode) {
      _Memory* image = _loader->getTargetSection(mskVMTRef);

      return _compiler->findFlags(image->get((vaddress & ~mskAnyRef)));
   }
   else return _compiler->findFlags((void*)vaddress);
}

lvaddr_t JITLinker :: loadMethod(ReferenceHelper& refHelper, MemoryReader& reader, MemoryWriter& writer)
{
   lvaddr_t position = writer.Position();

   // method just in time compilation
   _compiler->compileProcedure(refHelper, reader, writer);

   return _virtualMode ? position : (lvaddr_t)writer.Memory()->get(position);
}

lvaddr_t JITLinker :: resolveNativeSection(ReferenceInfo referenceInfo, ref_t mask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == NULL)
      return LOADER_NOTLOADED;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mask);
   MemoryWriter writer(image);

   lvaddr_t vaddress = calculateVAddress(&writer, mask & mskImageMask);
   pos_t position = writer.Position();

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
         resolveReference(image, *it + position, (uintptr_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeDataRef, _virtualMode);
      }
      else if (currentMask == mskPreloadRelDataRef) {
         resolveReference(image, *it + position, (uintptr_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeRelDataRef, _virtualMode);
      }
      else if (currentMask == mskPreloadCodeRef) {
         resolveReference(image, *it + position, (uintptr_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeCodeRef, _virtualMode);
      }
      else if (currentMask == mskPreloadRelCodeRef) {
         resolveReference(image, *it + position, (uintptr_t)_compiler->getPreloadedReference(currentRef), (ref_t)mskNativeRelCodeRef, _virtualMode);
      }
      else {
         lvaddr_t refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

         resolveReference(image, *it + position, (uintptr_t)refVAddress, currentMask, _virtualMode);
      }
      it++;
   }
   return vaddress;
}

lvaddr_t JITLinker :: resolveNativeVariable(ReferenceInfo referenceInfo, ref_t mask)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection((ref_t)mskDataRef);
   MemoryWriter writer(image);

   lvaddr_t vaddress = calculateVAddress(&writer, mskDataRef, 4);

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

lvaddr_t JITLinker :: resolveBytecodeSection(ReferenceInfo referenceInfo, ref_t mask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == NULL)
      return LOADER_NOTLOADED;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mask);
   MemoryWriter writer(image);

   lvaddr_t vaddress = calculateVAddress(&writer, mask & mskImageMask);

   _loader->mapReference(referenceInfo, vaddress, mask);

   // symbol just in time compilation
   References references(RefInfo(0, NULL));
   ReferenceHelper refHelper(this, sectionInfo.module, &references);
   MemoryReader reader(sectionInfo.section);

   // create native debug info header if debug info enabled
   pos_t sizePtr = (pos_t)-1;
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

lvaddr_t JITLinker :: resolveClassName(ReferenceInfo referenceInfo)
{
   IdentifierString relativeName;
   if (!referenceInfo.isRelative()) {
      relativeName.append(referenceInfo.referenceName + getlength(referenceInfo.module->Name()));
   }
   else relativeName.copy(referenceInfo.referenceName);

   return resolve(ReferenceInfo(relativeName.c_str()), mskLiteralRef, false);
}

lvaddr_t JITLinker :: resolvePackage(_Module* module)
{
   ReferenceNs packageInfo("'", PACKAGE_SECTION);

   return resolve(ReferenceInfo(module, packageInfo.c_str()), mskConstArray, false);
}

void JITLinker :: resolveStaticValues(ReferenceInfo referenceInfo, MemoryReader& vmtReader, MemoryReader& attrReader,
   _Memory* vmtImage, pos_t position)
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

      lvaddr_t refVAddress = 0;
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

         resolveReference(vmtImage, position + _compiler->findMemberPosition(it.key()), refVAddress, currentMask, _virtualMode);
      }
   }

   // generate run-time attributes
   ClassInfo::CategoryInfoMap attributes;
   attributes.read(&attrReader);

   createAttributes(referenceInfo, attributes);
}

lvaddr_t JITLinker :: createBytecodeVMTSection(ReferenceInfo referenceInfo, ref_t mask, ClassSectionInfo sectionInfo, 
   References& references)
{
   if (sectionInfo.codeSection == NULL || sectionInfo.vmtSection == NULL)
      return LOADER_NOTLOADED;

   ReferenceHelper refHelper(this, sectionInfo.module, &references);

   MemoryReader attrReader(sectionInfo.attrSection);

   // VMT just in time compilation
   MemoryReader vmtReader(sectionInfo.vmtSection);
   // read tape record size
   pos_t size = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));
   bool abstractOne = test(header.flags, elAbstract);

   // get target image & resolve virtual address
   _Memory* vmtImage = _loader->getTargetSection(mask);
   _Memory* codeImage = _loader->getTargetSection(mskClassRef);

   MemoryWriter vmtWriter(vmtImage);

   // allocate space and make VTM offset
   _compiler->allocateVMT(vmtWriter, header.flags, header.count, header.staticSize);

   lvaddr_t vaddress = calculateVAddress(&vmtWriter, mask & mskImageMask);

   _loader->mapReference(referenceInfo, vaddress, mask);

   // if it is a standard VMT
   if (test(header.flags, elStandartVMT)) {
      pos_t position = vmtWriter.Position();

      // load parent class
      void* parentVMT = getVMTReference(sectionInfo.module, header.parentRef, references);
      pos_t count = _compiler->copyParentVMT(parentVMT, (VMTEntry*)vmtImage->get(position));

      // create native debug info header if debug info enabled
      pos_t sizePtr = (pos_t)-1;
      if (_withDebugInfo)
         createNativeClassDebugInfo(referenceInfo, vaddress, sizePtr);

      // read and compile VMT entries
      MemoryWriter   codeWriter(codeImage);
      MemoryReader   codeReader(sectionInfo.codeSection);

      lvaddr_t        methodPosition;
      VMTEntry       entry;

      size -= sizeof(ClassHeader);
      while (size > 0) {
         vmtReader.read((void*)&entry, sizeof(VMTEntry));

         if (entry.address == INVALID_REF) {
            methodPosition = 0;
         }
         else {
            codeReader.seek(entry.address);
            methodPosition = loadMethod(refHelper, codeReader, codeWriter);
         }

         // NOTE : statically linked message is not added to VMT
         if (test(entry.message, STATIC_MESSAGE)) {
            _staticMethods.add(MethodInfo(vaddress, refHelper.resolveMessage(entry.message)), methodPosition);
         }
         else _compiler->addVMTEntry(refHelper.resolveMessage(entry.message), methodPosition, 
            (VMTEntry*)vmtImage->get(position), count);

         size -= sizeof(VMTEntry);
      }
      if (_withDebugInfo)
         endNativeDebugInfo(sizePtr);

      if (count != header.count)
         throw InternalError("VMT structure is corrupt");

      // load class class
      lvaddr_t classClassVAddress = getVMTAddress(sectionInfo.module, header.classRef, references);
      lvaddr_t parentVAddress = 0;
      if (header.parentRef != 0)
         parentVAddress = resolve(_loader->retrieveReference(sectionInfo.module, header.parentRef, mskVMTRef), mskVMTRef, true);

      // fix VMT
      _compiler->fixVMT(vmtWriter, classClassVAddress, parentVAddress, count, _virtualMode, abstractOne);

      if (!test(header.flags, elVirtualVMT)) {
         // HOTFIX : to presave the string name
         referenceInfo.module = sectionInfo.module;

         resolveStaticValues(referenceInfo, vmtReader, attrReader, vmtImage, position);
      }
   }

   return vaddress;
}

void JITLinker :: generateMetaAttribute(int category, ident_t fullName, lvaddr_t address)
{
   SectionInfo tableInfo = _loader->getSectionInfo(ReferenceInfo(MATTRIBUTE_TABLE), mskRDataRef, false);

   MemoryWriter writer(tableInfo.section);
   _compiler->compileMAttribute(writer, category, fullName, address, _virtualMode);
}

void JITLinker :: generateMetaAttribute(int category, ReferenceInfo& referenceInfo, ref_t mask)
{
   IdentifierString fullName;
   if (referenceInfo.isRelative()) {
      fullName.copy(referenceInfo.module->Name());
      fullName.append(referenceInfo.referenceName);
   }
   else fullName.copy(referenceInfo.referenceName);

   lvaddr_t address = resolve(referenceInfo, mask, false);

   generateMetaAttribute(category, fullName.ident(), address);
}

void JITLinker :: generateOverloadListMetaAttribute(_Module* module, mssg_t message, ref_t listRef)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   // write the overload list name
   ref_t signature;
   ident_t actionName = module->resolveAction(actionRef, signature);

   IdentifierString fullName;
   fullName.copy(module->Name());
   fullName.append('\'');

   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      fullName.append("params#");
   }

   fullName.append(actionName);
   fullName.append('[');
   fullName.appendInt(argCount);
   fullName.append(']');

   ident_t referenceName = module->resolveReference(listRef & ~mskAnyRef);

   // resolve extension overloadlist
   lvaddr_t address = resolve(ReferenceInfo(module, referenceName), mskConstArray, false);

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

lvaddr_t JITLinker :: resolveBytecodeVMTSection(ReferenceInfo referenceInfo, ref_t mask, ClassSectionInfo sectionInfo)
{
   // HOTFIX : caching a reference name, the original string could be corrupted
   IdentifierString className(referenceInfo.referenceName);
   referenceInfo.referenceName = className.c_str();
   
   References      references(RefInfo(0, NULL));

   // create VMT
   lvaddr_t vaddress = createBytecodeVMTSection(referenceInfo, mask, sectionInfo, references);

   //HOTFIX : resolve class symbol as well
   if (_classSymbolAutoLoadMode)
      resolve(referenceInfo, mskSymbolRef, true);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection(mskClassRef));

   return vaddress;
}

void JITLinker :: fixSectionReferences(SectionInfo& sectionInfo,  _Memory* image, pos_t position, lvaddr_t &vmtVAddress,
   bool constArrayMode, References* messageReferences)
{
   // resolve section references
   _ELENA_::RelocationMap::Iterator it(sectionInfo.section->getReferences());
   ref_t currentMask = 0;
   ref_t currentRef = 0;
   while (!it.Eof()) {
      currentMask = it.key() & mskAnyRef;
      currentRef = it.key() & ~mskAnyRef;

      pos_t imageOffset = (constArrayMode ? _compiler->findMemberPosition(*it >> 2) : *it) + position;

      if (currentMask == 0) {
         (*image)[imageOffset] = resolveMessage(sectionInfo.module, (*sectionInfo.section)[*it], messageReferences);
      }
      else if (currentMask == mskVMTEntryOffset) {
         lvaddr_t refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         pos_t offset = *it;
         mssg_t messageID = resolveMessage(sectionInfo.module, (*sectionInfo.section)[offset], messageReferences);

         (*image)[imageOffset] = getVMTMethodIndex(refVAddress, messageID);
      }
      else if (currentMask == mskVMTMethodAddress) {
         resolve(_loader->retrieveReference(sectionInfo.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         pos_t offset = *it;
         mssg_t messageID = resolveMessage(sectionInfo.module, (*sectionInfo.section)[offset], messageReferences);

         //lvaddr_t address = resolveVMTMethodAddress(sectionInfo.module, currentRef, messageID);
         //if (_virtualMode) {
         //   (*image)[imageOffset] = (pos_t)address;
         //   
         //   // TODO : maybe mskRelCodeRef should be used?
         //   image->addReference(mskCodeRef/*mskRelCodeRef*/, (pos_t)imageOffset);
         //}
         //else (*image)[imageOffset] = (pos_t)address;

         writeVAddr(image, imageOffset, resolveVMTMethodAddress(sectionInfo.module, currentRef, messageID));
         if (_virtualMode) {
            image->addReference(mskCodeRef, imageOffset);
         }
      }
      else if (constArrayMode && currentMask == mskMessage) {
         (*image)[imageOffset] = parseMessage(sectionInfo.module->resolveReference(currentRef));
      }
      else if (constArrayMode && currentMask == mskMessageName) {
         (*image)[imageOffset] = parseAction(sectionInfo.module->resolveReference(currentRef));
      }
      else {
         lvaddr_t refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

         if (*it == -4) {
            // resolve the constant vmt reference
            vmtVAddress = refVAddress;
         }
         else resolveReference(image, imageOffset, refVAddress, currentMask, _virtualMode);
      }

      it++;
   }
}

lvaddr_t JITLinker :: resolveConstant(ReferenceInfo referenceInfo, ref_t mask, bool silentMode)
{
   //if (referenceInfo.referenceName.find("rusLat") != NOTFOUND_POS)
   //   mask |= 0;

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
   lvaddr_t vmtVAddress = resolve(vmtReferenceInfo, mskVMTRef, true);

   // HOTFIX: if the constant is referred by iself it could be already resolved
   lvaddr_t vaddress = _loader->resolveReference(referenceInfo, mask);
   if (vaddress != LOADER_NOTLOADED)
      return vaddress;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   pos_t vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

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
         return 0;

      _compiler->compileCollection(&writer, sectionInfo.section);

      vmtVAddress = 0; // !! to support dump array
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
      ref_t flags = getVMTFlags(vmtVAddress);

      if (!test(flags, elStateless))
         throw JITConstantExpectedException(referenceInfo);
   }

   // fix object VMT reference
   resolveReference(image, vmtPosition, vmtVAddress, mskVMTRef, _virtualMode);

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

lvaddr_t JITLinker :: resolveStaticVariable(ReferenceInfo referenceInfo, ref_t mask)
{
   // get target image & resolve virtual address
   MemoryWriter writer(_loader->getTargetSection(mask));

   lvaddr_t vaddress = _virtualMode ? writer.Position() | mask : (lvaddr_t)writer.Address();

   _compiler->allocateVariable(writer);

   _statLength++;

   _loader->mapReference(referenceInfo, vaddress, mask);

   return vaddress;
}

lvaddr_t JITLinker :: resolveMessageTable(ReferenceInfo referenceInfo, ref_t mask)
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

   pos_t bodyPtr = writer.Position();

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

   return 0; // !! should be resolved only once
}

lvaddr_t JITLinker :: resolveMetaAttributeTable(ReferenceInfo, ref_t mask)
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

   return 0; // !! should be resolved only once
}

mssg_t JITLinker :: parseMessage(ident_t reference)
{
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);

   ref_t flags = 0;
   size_t paramCount = reference[0] - '0';

   // signature and custom verb should be imported
   ident_t actionName = reference + 1;

   ref_t resolvedAction = messageTable.module->mapAction(actionName, 0, true);
   if (!resolvedAction) {
      resolvedAction = resolveWeakAction(messageTable, actionName);
   }

   return encodeMessage(resolvedAction, paramCount, flags);
}

ref_t JITLinker :: parseAction(ident_t reference)
{
   SectionInfo messageTable = _loader->getSectionInfo(ReferenceInfo(MESSAGE_TABLE), mskRDataRef, true);

   ref_t resolvedAction = messageTable.module->mapAction(reference, 0, true);
   if (!resolvedAction) {
      resolvedAction = resolveWeakAction(messageTable, reference);
   }

   return resolvedAction;
}

lvaddr_t JITLinker :: resolveExtensionMessage(ReferenceInfo referenceInfo, ident_t vmt)
{
   int dotPos = referenceInfo.referenceName.find('.');

   IdentifierString extensionName(referenceInfo.referenceName, dotPos);
   mssg_t messageID = parseMessage(referenceInfo.referenceName + dotPos + 1);

   // HOTFIX : extension message should be a function one
   messageID |= FUNCTION_MESSAGE;

   lvaddr_t vmtExtVAddress = resolve(ReferenceInfo(referenceInfo.module, extensionName), mskVMTRef, false);
   lvaddr_t entryOffset = getVMTMethodAddress(vmtExtVAddress, messageID);;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   pos_t vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   lvaddr_t vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(referenceInfo, vaddress, mskMessage);

   if (_virtualMode) {
      _compiler->compileMssgExtension(&writer, messageID, mskCodeRef, entryOffset);
   }
   else _compiler->compileMssgExtension(&writer, messageID, entryOffset);

   // get constant VMT reference
   lvaddr_t vmtVAddress = resolve(vmt, mskVMTRef, false);

   // fix object VMT reference
   resolveReference(image, vmtPosition, vmtVAddress, mskVMTRef, _virtualMode);

   return vaddress;
}

lvaddr_t JITLinker :: resolveAction(ReferenceInfo referenceInfo, ident_t vmt)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   pos_t vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   lvaddr_t vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(referenceInfo, vaddress, mskMessage);

   _compiler->compileAction(&writer, parseAction(referenceInfo.referenceName));

   // get constant VMT reference
   lvaddr_t vmtVAddress = resolve(vmt, mskVMTRef, false);

   // fix object VMT reference
   resolveReference(image, vmtPosition, vmtVAddress, mskVMTRef, _virtualMode);

   return vaddress;
}

lvaddr_t JITLinker :: resolveMessage(ReferenceInfo referenceInfo, ident_t vmt)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   pos_t vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   lvaddr_t vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(referenceInfo, vaddress, mskMessage);

   _compiler->compileMessage(&writer, parseMessage(referenceInfo.referenceName));

   // get constant VMT reference
   lvaddr_t vmtVAddress = resolve(vmt, mskVMTRef, false);

   // fix object VMT reference
   resolveReference(image, vmtPosition, vmtVAddress, mskVMTRef, _virtualMode);

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

void JITLinker :: createNativeDebugInfo(ident_t reference, void* param, pos_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);
   writer.writeLiteral(reference);

   sizePtr = writer.Position();
   writer.writeDWord(0); // size place holder

   writer.writeDWord((lvaddr_t)param);
}

void JITLinker :: createNativeSymbolDebugInfo(ReferenceInfo referenceInfo, lvaddr_t address, pos_t& sizePtr)
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
   if (!_virtualMode || address == 0) {
      writer.writeDWord(address);
   }
   else writer.writeRef((ref_t)address, 0);
}

void JITLinker :: createNativeClassDebugInfo(ReferenceInfo referenceInfo, lvaddr_t vaddress, pos_t& sizePtr)
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
      writer.writeDWord(vaddress);
   }
   else writer.writeRef((ref_t)vaddress, 0);
}

void JITLinker :: endNativeDebugInfo(size_t sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   (*debug)[sizePtr] = debug->Length() - sizePtr;
}

lvaddr_t JITLinker :: resolveTemporalByteCode(_ReferenceHelper& helper, MemoryReader& reader, ident_t reference, void* param)
{
   _Memory* image = _loader->getTargetSection(mskCodeRef);

   // map dynamic code
   MemoryWriter writer(image);
   lvaddr_t vaddress = calculateVAddress(&writer, mskCodeRef);

   if (param && _withDebugInfo && !emptystr(reference)) {
      // it it is a debug mode a special debug record is created containing link to tape
      pos_t sizePtr = 0;
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

      lvaddr_t initializer = resolveBytecodeSection(ReferenceInfo(module, initSymbol), mskCodeRef, helper.getSection(initRef | mskSymbolRef, module));
      if (initializer != LOADER_NOTLOADED) {
         if (!_virtualMode) {
            // HOTFIX : in VM mode - use relative address
            _Memory* image = _loader->getTargetSection(mskCodeRef);

            _compiler->generateSymbolCall(tape, initializer - (lvaddr_t)image->get(0));
         }
         else _compiler->generateSymbolCall(tape, initializer);
      }

      it++;
   }

   _initializers.clear();
}

lvaddr_t JITLinker :: resolveEntry(lvaddr_t programEntry)
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

   lvaddr_t entry = resolveTemporalByteCode(refHelper, reader, nullptr, nullptr);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection(mskCodeRef));

   return entry;
}

// NOTE: reference should not be a forward one, otherwise there may be code duplication
lvaddr_t JITLinker :: resolve(ReferenceInfo referenceInfo, ref_t mask, bool silentMode)
{
   lvaddr_t vaddress = _loader->resolveReference(referenceInfo, mask);
   if (vaddress==LOADER_NOTLOADED) {
      //printf("%s - %x\n", referenceInfo.referenceName.c_str(), mask);

      switch (mask) {
         case mskSymbolRef:
//         case mskClassRef:
            vaddress = resolveBytecodeSection(referenceInfo, mask, _loader->getSectionInfo(referenceInfo, mask, silentMode));
            break;
         case mskSymbolRelRef:
//         case mskClassRelRef:
            vaddress = resolveBytecodeSection(referenceInfo, mask & ~mskRelRef, _loader->getSectionInfo(referenceInfo, mask & ~mskRelRef, silentMode));
            break;
         case mskInternalRef:
         case mskInternalRelRef:
            vaddress = resolveBytecodeSection(referenceInfo, mask & ~mskRelRef, _loader->getSectionInfo(referenceInfo, mskCodeRef, silentMode));
            break;
         case mskVMTRef:
            vaddress = resolveBytecodeVMTSection(referenceInfo, mask, _loader->getClassSectionInfo(referenceInfo, mskClassRef, mskVMTRef, silentMode));
            break;
         case mskNativeCodeRef:
         case mskNativeDataRef:
         case mskNativeRDataRef:
            vaddress = resolveNativeSection(referenceInfo, mask, _loader->getSectionInfo(referenceInfo, mask, silentMode));
            break;
         //case mskNativeRelDataRef:
         //   vaddress = resolveNativeSection(referenceInfo, mask, _loader->getSectionInfo(referenceInfo, mskNativeDataRef, silentMode));
         //   break;
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
         case mskStatSymbolRelRef:
            vaddress = resolveStaticVariable(referenceInfo, mskStatRef);
            break;
         case mskMessage:
            vaddress = resolveMessage(referenceInfo, _loader->getMessageClass());
            break;
         case mskMessageName:
            vaddress = resolveAction(referenceInfo, _loader->getMessageNameClass());
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

lvaddr_t JITLinker :: resolve(ident_t reference, ref_t mask, bool silentMode)
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
   lvaddr_t superPtr = resolve(superClass, mskVMTRef, true);
   //void* ptr = _compiler->findClassPtr(superPtr);

   _compiler->setVoidParent(_loader, superPtr, _virtualMode);
}
