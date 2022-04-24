//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class implementation.
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitlinker.h"
#include "langcommon.h"

using namespace elena_lang;

constexpr ref_t SIGNATURE_MASK = 0x80000000;

// --- JITLinkerReferenceHelper ---

inline void writeVAddress32(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp, 
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 4);
      image->addReference(reference, position);
   }
   else {
      if ((addressMask & mskRefType) == mskRelRef32) {
         unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0) - position - 4) + disp;
         image->write(position, &offs, 4);
      }
      else {
         vaddress += disp;
         image->write(position, &vaddress, 4);
      }
   }
}

inline void writeRelAddress32(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 4);
      image->addReference(reference, position);
   }
   else {
      unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0) - position - 4) + disp;
      image->write(position, &offs, 4);
   }
}

inline void writeVAddress64(MemoryBase* image, pos_t position, addr_t vaddress, pos64_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 8);
      image->addReference(reference, position);
   }
   else {
      //if ((addressMask & mskRefType) == mskRelRef32) {
      //   unsigned long long offs = (unsigned int)(vaddress - (addr_t)image->get(0) - position - 4) + disp;
      //   image->write(position, &offs, 4);
      //}
      //else {
         vaddress += (addr_t)disp;
         image->write(position, &vaddress, 8);
      //}
   }
}

inline void writeDisp32Hi(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 2);
      image->addReference(reference, position);
   }
   else {
      unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0)) + disp;
      offs >>= 16;
      image->write(position, &offs, 2);
   }
}

inline void writeDisp32Lo(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 2);
      image->addReference(reference, position);
   }
   else {
      unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0)) + disp;
      image->write(position, &offs, 2);

   }
}

inline void writeRef32Lo(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 2);
      image->addReference(reference, position);
   }
   else {
      // save the lowest part of 32 bit address
      vaddress += disp;
      image->write(position, &vaddress, 2);
   }
}

inline void writeRef32Hi(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 2);
      image->addReference(reference, position);
   }
   else {
      // save the highest part of 32 bit address
      vaddress += disp;
      vaddress >>= 16;
      image->write(position, &vaddress, 2);

   }
}

JITLinker::JITLinkerReferenceHelper :: JITLinkerReferenceHelper(JITLinker* owner, ModuleBase* module, VAddressMap* references)
{
   this->_owner = owner;
   this->_module = module;
   this->_references = references;
   this->_debug = nullptr;
}

void JITLinker::JITLinkerReferenceHelper :: addBreakpoint(MemoryWriter& codeWriter)
{
   if (!_debug)
      _debug = _owner->_imageProvider->getTargetDebugSection();

   MemoryWriter writer(_debug);

   _owner->_compiler->addBreakpoint(writer, codeWriter, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeSectionReference(MemoryBase* image, pos_t imageOffset, 
   ref_t reference, MemoryBase* section, pos_t sectionOffset)
{
   ref_t currentMask = reference & mskAnyRef;
   ref_t currentRef = reference & ~mskAnyRef;

   if (currentMask == 0) {
      MemoryBase::writeDWord(image, imageOffset,
         _owner->createMessage(_module, MemoryBase::getDWord(section, sectionOffset), *_references));
   }
   //else if (currentMask == mskVMTEntryOffset) {
   //   lvaddr_t refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, mskVMTRef), mskVMTRef, false);

   //   // message id should be replaced with an appropriate method address
   //   pos_t offset = *it;
   //   mssg_t messageID = resolveMessage(sectionInfo.module, (*sectionInfo.section)[offset], messageReferences);

   //   (*image)[imageOffset] = getVMTMethodIndex(refVAddress, messageID);
   //}
   //else if (currentMask == mskVMTMethodAddress) {
   //   resolve(_loader->retrieveReference(sectionInfo.module, currentRef, mskVMTRef), mskVMTRef, false);

   //   // message id should be replaced with an appropriate method address
   //   pos_t offset = *it;
   //   mssg_t messageID = resolveMessage(sectionInfo.module, (*sectionInfo.section)[offset], messageReferences);

   //   //lvaddr_t address = resolveVMTMethodAddress(sectionInfo.module, currentRef, messageID);
   //   //if (_virtualMode) {
   //   //   (*image)[imageOffset] = (pos_t)address;
   //   //   
   //   //   // TODO : maybe mskRelCodeRef should be used?
   //   //   image->addReference(mskCodeRef/*mskRelCodeRef*/, (pos_t)imageOffset);
   //   //}
   //   //else (*image)[imageOffset] = (pos_t)address;

   //   writeVAddr(image, imageOffset, resolveVMTMethodAddress(sectionInfo.module, currentRef, messageID));
   //   if (_virtualMode) {
   //      image->addReference(mskCodeRef, imageOffset);
   //   }
   //}
   //else if (constArrayMode && currentMask == mskMessage) {
   //   (*image)[imageOffset] = parseMessage(sectionInfo.module->resolveReference(currentRef));
   //}
   //else if (constArrayMode && currentMask == mskMessageName) {
   //   (*image)[imageOffset] = parseAction(sectionInfo.module->resolveReference(currentRef));
   //}
   //else {
   //   lvaddr_t refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

   //   if (*it == -4) {
   //      // resolve the constant vmt reference
   //      vmtVAddress = refVAddress;
   //   }
   //   else resolveReference(image, imageOffset, refVAddress, currentMask, _virtualMode);
   //}

}

void JITLinker::JITLinkerReferenceHelper :: writeVMTMethodReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp, mssg_t message,
   ref_t addressMask, ModuleBase* module)
{
   if (module == nullptr)
      module = _module;

   // vmt entry offset / address should be resolved later
   _references->add(position, { reference, module, addressMask, disp, message });
}

void JITLinker::JITLinkerReferenceHelper :: writeReference(MemoryBase& target, pos_t position, ref_t reference, pos_t disp,
   ref_t addressMask, ModuleBase* module)
{
   ref_t mask = reference & mskAnyRef;
   ref_t refID = reference & ~mskAnyRef;

   if (module == nullptr)
      module = _module;

   addr_t vaddress = INVALID_ADDR;
   //switch (mask) {
   //   default:
   //
   vaddress = _owner->_mapper->resolveReference(
      _owner->_loader->retrieveReferenceInfo(module, refID, mask, _owner->_forwardResolver), mask);
   //
   //      break;
   //}

   if (vaddress != INVALID_ADDR) {
      switch (addressMask & mskRefType) {
         case mskRelRef32:
            ::writeRelAddress32(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskRef32:
            ::writeVAddress32(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskRef64:
            ::writeVAddress64(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskDisp32Hi:
            ::writeDisp32Hi(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskDisp32Lo:
            ::writeDisp32Lo(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskRef32Hi:
            ::writeRef32Hi(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskRef32Lo:
            ::writeRef32Lo(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         default:
            // to make compiler happy
            break;
      }
   }
   else _references->add(position, VAddressInfo(reference, module, addressMask, disp));
}

void JITLinker::JITLinkerReferenceHelper :: writeVAddress32(MemoryBase& target, pos_t position, addr_t vaddress, 
   pos_t disp, ref_t addressMask)
{
   ::writeVAddress32(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeRelAddress32(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp, ref_t addressMask)
{
   ::writeRelAddress32(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeVAddress64(MemoryBase& target, pos_t position, addr_t vaddress, pos64_t disp,
   ref_t addressMask)
{
   ::writeVAddress64(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeVAddress32Hi(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask)
{
   ::writeRef32Hi(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeVAddress32Lo(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask)
{
   ::writeRef32Lo(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper ::writeDisp32Hi(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask)
{
   ::writeDisp32Hi(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeDisp32Lo(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask)
{
   ::writeDisp32Lo(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

mssg_t JITLinker::JITLinkerReferenceHelper :: importMessage(mssg_t message, ModuleBase* module)
{
   if (!module)
      module = _module;

   return _owner->createMessage(module, message, *_references);
}

// --- JITLinker ---

addr_t JITLinker :: getVMTAddress(ModuleBase* module, ref_t reference, VAddressMap& references)
{
   if (reference) {
      auto referenceInfo = _loader->retrieveReferenceInfo(module, reference, mskVMTRef, _forwardResolver);

      addr_t vaddress = _mapper->resolveReference(referenceInfo, mskVMTRef);
      if (vaddress == INVALID_ADDR) {
         // create VMT table without resolving references to prevent circular reference
         vaddress = createVMTSection(referenceInfo, 
            _loader->getClassSections(referenceInfo, mskVMTRef, mskClassRef, false),
            references);

         if (vaddress == INVALID_ADDR)
            throw JITUnresolvedException(referenceInfo);
      }
      return vaddress;
   }
   else return 0;
}

void* JITLinker :: getVMTPtr(addr_t address)
{
   if (!address)
      return nullptr;

   if (_virtualMode) {
      MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);

      return image->get(address & ~mskAnyRef);
   }
   else return (void*)address;
}

addr_t JITLinker :: calculateVAddress(MemoryWriter& writer, ref_t targetMask)
{
   // align the section
   _compiler->alignCode(writer, _alignment, (targetMask & mskImageType) == mskCodeRef);

#ifdef _DEBUG
   if (writer.position() >= ~mskAnyRef)
      throw InternalError(errReferenceOverflow);
#endif

   return _virtualMode ? (writer.position() | targetMask) : (addr_t)writer.address();
}

void JITLinker :: fixOffset(pos_t position, ref_t offsetMask, int offset, MemoryBase* image)
{
   MemoryWriter writer(image);
   writer.seek(position);

   switch (offsetMask) {
      case mskRef32:
         _compiler->writeImm32(&writer, offset);
         break;
      case mskRef32Lo12:
         _compiler->writeImm12(&writer, offset, 0);
         break;
      case mskRef32Lo:
         _compiler->writeImm16(&writer, offset, 0);
         break;
      default:
         break;
   }
}

void JITLinker :: fixReferences(VAddressMap& relocations, MemoryBase* image)
{
   for (auto it = relocations.start(); !it.eof(); ++it) {
      VAddressInfo info = *it;

      ref_t currentRef = info.reference & ~mskAnyRef;
      ref_t currentMask = info.reference & mskAnyRef;

      addr_t vaddress = 0;
      switch (currentMask) {
         case mskVMTMethodAddress:
         {
            resolve(_loader->retrieveReferenceInfo(info.module, currentRef, mskVMTRef,
               _forwardResolver), mskVMTRef, false);

            vaddress = resolveVMTMethodAddress(info.module, currentRef, info.message);
            break;
         }
         case mskVMTMethodOffset:
         {
            resolve(_loader->retrieveReferenceInfo(info.module, currentRef, mskVMTRef,
               _forwardResolver), mskVMTRef, false);
            pos_t offset = resolveVMTMethodOffset(info.module, currentRef, info.message);
            fixOffset(it.key(), info.addressMask, offset, image);

            info.addressMask = 0; // clear because it is already fixed
            break;
         }
         default:
            vaddress = resolve(_loader->retrieveReferenceInfo(info.module, currentRef, currentMask,
               _forwardResolver), currentMask, false);
            break;
      }

      switch (info.addressMask & mskRefType) {
         case mskRelRef32:
            ::writeRelAddress32(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskRef32:
            ::writeVAddress32(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskRef64:
            ::writeVAddress64(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskDisp32Hi:
            ::writeDisp32Hi(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskDisp32Lo:
            ::writeDisp32Lo(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskRef32Hi:
            ::writeRef32Hi(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskRef32Lo:
            ::writeRef32Lo(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         default:
            // to make compiler happy
            break;
      }
   }
}

addr_t JITLinker :: getVMTMethodAddress(addr_t vmtAddress, mssg_t message)
{
   void* entries = getVMTPtr(vmtAddress);

   return _compiler->findMethodAddress(entries, message);
}

addr_t JITLinker :: resolveVMTMethodAddress(ModuleBase* module, ref_t reference, mssg_t message)
{
   addr_t vmtAddress = resolve(_loader->retrieveReferenceInfo(module, reference, mskVMTRef, _forwardResolver), mskVMTRef, false);

   addr_t vaddress = _staticMethods.get({ vmtAddress, message });
   if (vaddress == INVALID_ADDR) {
      vaddress = getVMTMethodAddress(vmtAddress, message);

      _staticMethods.add({ vmtAddress, message }, vaddress);
   }

   if (_virtualMode)
      vaddress |= mskCodeRef;

   return vaddress;
}

pos_t JITLinker :: resolveVMTMethodOffset(ModuleBase* module, ref_t reference, mssg_t message)
{
   addr_t vmtAddress = resolve(_loader->retrieveReferenceInfo(module, reference, mskVMTRef, _forwardResolver), mskVMTRef, false);

   void* vmtPtr = getVMTPtr(vmtAddress);

   pos_t offset = _compiler->findMethodOffset(vmtPtr, message);

   return offset;
}

addr_t JITLinker :: loadMethod(ReferenceHelperBase& refHelper, MemoryReader& reader, MemoryWriter& writer)
{
   pos_t position = writer.position();

   // method just in time compilation
   // NOTE : LabelHelper parameter should be overriden inside the specific CPU compiler
   _compiler->compileProcedure(&refHelper, reader, writer, nullptr);

   return _virtualMode ? position : (addr_t)writer.Memory()->get(position);
}

ref_t JITLinker::resolveWeakAction(ustr_t actionName)
{
   ref_t resolvedAction = _mapper->resolveAction(actionName, 0u);
   if (!resolvedAction) {
      resolvedAction = createAction(actionName, 0u, 0u);

      _mapper->mapAction(actionName, resolvedAction, 0u);
   }

   return resolvedAction;
}

ref_t JITLinker :: createAction(ustr_t actionName, ref_t weakActionRef, ref_t signature)
{
   if (signature == 0u && weakActionRef != 0u)
      return weakActionRef;

   MemoryWriter mdataWriter(_imageProvider->getMDataSection());
   MemoryWriter mbdataWriter(_imageProvider->getMBDataSection());

   ref_t actionRef = _compiler->addActionEntry(mdataWriter, mbdataWriter, 
      actionName, weakActionRef, signature);

   if (!actionName.empty())
      _mapper->mapAction(actionName, actionRef, signature);

   return actionRef;
}

ref_t JITLinker :: createSignature(ModuleBase* module, ref_t signature, VAddressMap& references)
{
   if (!signature)
      return 0;

   ref_t signReferences[ARG_COUNT];
   size_t count = module->resolveSignature(signature, signReferences);

   // resolve the signature name
   IdentifierString signatureName;
   for (size_t i = 0; i < count; i++) {
      signatureName.append('$');
      auto referenceInfo = _loader->retrieveReferenceInfo(module, signReferences[i], mskVMTRef, _forwardResolver);
      if (referenceInfo.module != nullptr) {
         signatureName.append(referenceInfo.module->name());
         signatureName.append(referenceInfo.referenceName);
      }
      else signatureName.append(referenceInfo.referenceName);
   }

   ref_t resolvedSignature = _mapper->resolveAction(*signatureName, 0) & ~SIGNATURE_MASK;
   if (resolvedSignature == 0) {
      Section* messageBody = _imageProvider->getMBDataSection();
      MemoryWriter writer(messageBody);

      resolvedSignature = writer.position();

      IdentifierString typeName;
      for (size_t i = 0; i < count; i++) {
         // NOTE : indicate weak class reference, to be later resolved if required
         ref_t addressMask = 0;
         auto typeClassRef = _mapper->resolveReference(
            _loader->retrieveReferenceInfo(module, signReferences[i], mskVMTRef, _forwardResolver),
            mskVMTRef);

         pos_t position = _compiler->addSignatureEntry(writer, typeClassRef, addressMask, _virtualMode);
         if (typeClassRef == INVALID_ADDR) {
            _mapper->addLazyReference(
               { mskMessageBodyRef, position, module, signReferences[i] | mskVMTRef, addressMask });
         }
      }

      // HOTFIX : adding a mask to tell apart a message name from a signature in meta module
      _mapper->mapAction(*signatureName, resolvedSignature | SIGNATURE_MASK, 0u);
   }

   return resolvedSignature;
}

mssg_t JITLinker :: createMessage(ModuleBase* module, mssg_t message, VAddressMap& references)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;

   decodeMessage(message, actionRef, argCount, flags);

   // a signature and a verb should be imported
   ref_t signature = 0;
   ustr_t actionName = module->resolveAction(actionRef, signature);

   ref_t importedSignature = createSignature(module, signature, references);
   ref_t importedAction = _mapper->resolveAction(actionName, importedSignature);
   if (!importedAction) {
      importedAction = createAction(actionName, resolveWeakAction(actionName), importedSignature);
   }

   return encodeMessage(importedAction, argCount, flags);
}

addr_t JITLinker :: createVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo,
   VAddressMap& references)
{
   if (sectionInfo.vmtSection == nullptr || sectionInfo.codeSection == nullptr)
      return INVALID_ADDR;

   referenceInfo.module = sectionInfo.module;
   referenceInfo.referenceName = referenceInfo.module->resolveReference(sectionInfo.reference);

   JITLinkerReferenceHelper helper(this, sectionInfo.module, &references);

   // VMT just in time compilation
   MemoryReader vmtReader(sectionInfo.vmtSection);
   // read vmt record size
   pos_t size = vmtReader.getPos();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));

   // get target image & resolve virtual address
   MemoryBase* vmtImage = _imageProvider->getTargetSection(mskRDataRef);
   MemoryBase* codeImage = _imageProvider->getTargetSection(mskCodeRef);
   MemoryWriter vmtWriter(vmtImage);

   // allocate space and make VTM offset
   _compiler->allocateVMT(vmtWriter, header.flags, header.count);

   addr_t vaddress = calculateVAddress(vmtWriter, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, mskVMTRef);

   if (test(header.flags, elStandartVMT)) {
      pos_t position = vmtWriter.position();

      // load the parent class
      addr_t parentAddress = getVMTAddress(sectionInfo.module, header.parentRef, references);
      pos_t count = _compiler->copyParentVMT(getVMTPtr(parentAddress), vmtImage->get(position));

      pos_t debugPosition = INVALID_POS;
      if (_withDebugInfo)
         debugPosition = createNativeClassDebugInfo(referenceInfo, vaddress);

      // read and compile VMT entries
      MemoryWriter   codeWriter(codeImage);
      MemoryReader   codeReader(sectionInfo.codeSection);

      addr_t      methodPosition = 0;
      MethodEntry entry = { };

      size -= sizeof(ClassHeader);
      while (size > 0) {
         vmtReader.read((void*)&entry, sizeof(MethodEntry));

         if (entry.codeOffset == INVALID_POS) {
            methodPosition = 0;
         }
         else {
            codeReader.seek(entry.codeOffset);
            methodPosition = loadMethod(helper, codeReader, codeWriter);
         }

         // NOTE : statically linked message is not added to VMT
         if (test(entry.message, STATIC_MESSAGE)) {
            _staticMethods.add(
               { vaddress, helper.importMessage(entry.message) }, methodPosition);
         }
         else _compiler->addVMTEntry(helper.importMessage(entry.message), methodPosition, 
            vmtImage->get(position), count);

         size -= sizeof(MethodEntry);
      }

      if (_withDebugInfo)
         endNativeDebugInfo(debugPosition);

      if (count != header.count)
         throw InternalError(errCorruptedVMT);

      // load the class class
      addr_t classClassAddress = getVMTAddress(sectionInfo.module, header.classRef, references);

      // update VMT
      _compiler->updateVMTHeader(vmtWriter, parentAddress, classClassAddress, header.flags, header.count, _virtualMode);
   }

   return vaddress;
}

addr_t JITLinker :: resolveVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo)
{
   VAddressMap references(VAddressInfo(0, nullptr, 0, 0));

   // create VMT
   addr_t vaddress = createVMTSection(referenceInfo, sectionInfo, references);

   //HOTFIX : resolve class symbol as well
   if (_classSymbolAutoLoadMode)
      resolve(referenceInfo, mskSymbolRef, true);

   // fix not loaded references
   fixReferences(references, _imageProvider->getTargetSection(mskCodeRef));

   return vaddress;
}

pos_t JITLinker :: createNativeSymbolDebugInfo(ReferenceInfo referenceInfo, addr_t vaddress)
{
   MemoryBase* debug = _imageProvider->getTargetDebugSection();
   MemoryWriter writer(debug);

   // start with # to distinguish the symbol debug info from the class one
   if (referenceInfo.isRelative()) {
      IdentifierString name(referenceInfo.module->name());
      if (referenceInfo.referenceName.findSub(1, '\'', NOTFOUND_POS) != NOTFOUND_POS) {
         NamespaceString ns(referenceInfo.referenceName);

         name.append(*ns);
      }
      ReferenceProperName properName(referenceInfo.referenceName);

      name.append("'#");
      name.append(*properName);

      writer.writeString(*name);
   }
   else {
      NamespaceString ns(referenceInfo.referenceName);
      ReferenceProperName properName(referenceInfo.referenceName);

      IdentifierString name(*ns, "'#", *properName);
      writer.writeString(*name);
   }

   pos_t position = writer.position();
   writer.writePos(0); // size place holder

   // save symbol address
   _compiler->addBreakpoint(writer, vaddress, _virtualMode);

   return position;
}

pos_t JITLinker :: createNativeClassDebugInfo(ReferenceInfo referenceInfo, addr_t vaddress)
{
   MemoryBase* debug = _imageProvider->getTargetDebugSection();

   MemoryWriter writer(debug);
   if (referenceInfo.isRelative()) {
      IdentifierString name(referenceInfo.module->name(), referenceInfo.referenceName);

      writer.writeString(*name);
   }
   else writer.writeString(referenceInfo.referenceName);

   pos_t position = writer.position();
   writer.writePos(0); // size place holder

   _compiler->addBreakpoint(writer, vaddress, _virtualMode);

   return position;
}


void JITLinker :: endNativeDebugInfo(pos_t position)
{
   MemoryBase* debug = _imageProvider->getTargetDebugSection();

   pos_t size = debug->length() - position;
   debug->write(position, &size, sizeof(size));
}

addr_t JITLinker :: resolveBytecodeSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == nullptr)
      return INVALID_ADDR;

   MemoryBase*  image = _imageProvider->getTargetSection(mskCodeRef);
   MemoryWriter writer(image);

   addr_t vaddress = calculateVAddress(writer, mskCodeRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   VAddressMap references(VAddressInfo(0, nullptr, 0, 0));
   JITLinkerReferenceHelper helper(this, sectionInfo.module, &references);
   MemoryReader bcReader(sectionInfo.section);

   if (_withDebugInfo) {
      pos_t sizePosition = createNativeSymbolDebugInfo(referenceInfo, vaddress);

      // NOTE : LabelHelper parameter should be overriden inside the specific CPU compiler
      _compiler->compileSymbol(&helper, bcReader, writer, nullptr);

      endNativeDebugInfo(sizePosition);
   }
   // NOTE : LabelHelper parameter should be overriden inside the specific CPU compiler
   else _compiler->compileSymbol(&helper, bcReader, writer, nullptr);

   // fix not loaded references
   fixReferences(references, image);

   return vaddress;
}

addr_t JITLinker :: resolveMetaSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == nullptr)
      return INVALID_ADDR;

   MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   addr_t vaddress = calculateVAddress(writer, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   VAddressMap references(VAddressInfo(0, nullptr, 0, 0));
   JITLinkerReferenceHelper helper(this, sectionInfo.module, &references);
   MemoryReader bcReader(sectionInfo.section);

   _compiler->compileMetaList(&helper, bcReader, writer, sectionInfo.section->length() >> 2);

   // fix not loaded references
   fixReferences(references, image);

   return vaddress;
}

inline ReferenceInfo retrieveConstantVMT(SectionInfo info)
{
   for (auto it = RelocationMap::Iterator(info.section->getReferences()); !it.eof(); ++it)
   {
      if ((*it) == (pos_t)-4) {
         return { info.module, info.module->resolveReference(it.key()) };
      }
   }

   return {};
}

addr_t JITLinker :: resolveConstantArray(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode)
{
   VAddressMap references({ 0, nullptr, 0, 0 });
   ReferenceInfo vmtReferenceInfo;

   // get target image & resolve virtual address
   MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   bool structMode = false;

   // resolve constant value
   SectionInfo sectionInfo = _loader->getSection(referenceInfo, sectionMask, silentMode);
   if (!sectionInfo.section)
      return 0;

   vmtReferenceInfo = retrieveConstantVMT(sectionInfo);

   int size = sectionInfo.section->length() >> 2;

   // get constant VMT reference
   addr_t vmtVAddress = resolve(vmtReferenceInfo, mskVMTRef, true);

   // allocate object header
   _compiler->allocateHeader(writer, vmtVAddress, size, structMode, _virtualMode);
   addr_t vaddress = calculateVAddress(writer, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   JITLinkerReferenceHelper helper(this, sectionInfo.module, &references);
   _compiler->writeCollection(&helper, writer, sectionInfo.section);

   fixReferences(references, image);

   return vaddress;
}

addr_t JITLinker :: resolveConstant(ReferenceInfo referenceInfo, ref_t sectionMask)
{
   ReferenceInfo vmtReferenceInfo = referenceInfo;
   ustr_t value = referenceInfo.referenceName;
   int size = 0;
   bool structMode = false;
   switch (sectionMask) {
      case mskIntLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.intLiteralClass;
         size = 4;
         structMode = true;
         break;
      case mskLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.literalClass;
         size = value.length() + 1;
         structMode = true;
         break;
      default:
         break;
   }

   // get constant VMT reference
   addr_t vmtVAddress = resolve(vmtReferenceInfo, mskVMTRef, true);

   // HOTFIX: if the constant is referred by iself it could be already resolved
   addr_t vaddress = _mapper->resolveReference(referenceInfo, sectionMask);
   if (vaddress != INVALID_ADDR)
      return vaddress;

   // get target image & resolve virtual address
   MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   _compiler->allocateHeader(writer, vmtVAddress, size, structMode, _virtualMode);
   vaddress = calculateVAddress(writer, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   switch (sectionMask) {
      case mskIntLiteralRef:
         _compiler->writeInt32(writer, StrConvertor::toUInt(value, 16));
         break;
      case mskLiteralRef:
         _compiler->writeLiteral(writer, value);
         break;
      default:
         break;
   }

   return vaddress;
}

void JITLinker :: prepare(JITCompilerBase* compiler)
{
   _compiler = compiler;

   _withDebugInfo = _compiler->isWithDebugInfo();

   VAddressMap references(VAddressInfo(0, nullptr, 0, 0));
   JITLinkerReferenceHelper helper(this, nullptr, &references);

   // --- message tables ---
   // should start with empty entry
   createAction(nullptr, 0u, 0u);
   // dispatch message should be the next one - to make sure it is always
   // the first entry in the class VMT
   resolveWeakAction(DISPATCH_MESSAGE);
   //// protected default constructor message should be the second
   //// NOTE : protected constructor action can be used only for default constructor due to current implementation
   //// (we have to guarantee that default constructor is always the second one)
   //resolveWeakAction(CONSTRUCTOR_MESSAGE2);
   // constructor message should be the third
   resolveWeakAction(CONSTRUCTOR_MESSAGE);

   // prepare jit compiler
   _compiler->prepare(_loader, _imageProvider, &helper, nullptr, _jitSettings);

   // fix not loaded references
   fixReferences(references, _imageProvider->getTextSection());
}

void JITLinker :: complete(JITCompilerBase* compiler)
{
   // fix message body references
   Section* mbSection = _imageProvider->getMBDataSection();
   VAddressMap mbReferences({});
   _mapper->forEachLazyReference<VAddressMap*>(&mbReferences, [](VAddressMap* mbReferences, LazyReferenceInfo info)
   {
      if (info.mask == mskMessageBodyRef) {
         mbReferences->add(info.position, { info.reference, info.module, info.addressMask, 0 });
      }
   });

   fixReferences(mbReferences, mbSection);
}

addr_t JITLinker :: resolve(ustr_t referenceName, ref_t sectionMask, bool silentMode)
{
   ReferenceInfo referenceInfo = _loader->retrieveReferenceInfo(referenceName, _forwardResolver);

   return resolve(referenceInfo, sectionMask, silentMode);
}

addr_t JITLinker :: resolve(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode)
{
   addr_t address = _mapper->resolveReference(referenceInfo, sectionMask);
   if (address == INVALID_ADDR) {
      switch (sectionMask) {
         case mskSymbolRef:
            address = resolveBytecodeSection(referenceInfo, sectionMask, 
               _loader->getSection(referenceInfo, sectionMask, silentMode));
            break;
         case mskVMTRef:
            address = resolveVMTSection(referenceInfo, 
               _loader->getClassSections(referenceInfo, mskVMTRef, mskClassRef, silentMode));
            break;
         case mskMetaArrayRef:
            address = resolveMetaSection(referenceInfo, sectionMask, 
               _loader->getSection(referenceInfo, sectionMask, silentMode));
            break;
         case mskIntLiteralRef:
         case mskLiteralRef:
            address = resolveConstant(referenceInfo, sectionMask);
            break;
         case mskConstArray:
            address = resolveConstantArray(referenceInfo, sectionMask, silentMode);
            break;
         default:
            // to make compiler happy
            break;
      }
   }

   if (!silentMode && address == INVALID_ADDR)
      throw JITUnresolvedException(referenceInfo);

   return address; // !! temporal
}
