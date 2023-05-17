//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class implementation.
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitlinker.h"
#include "langcommon.h"
#include "bytecode.h"
#include "module.h"

//#define FULL_OUTOUT_INFO 1

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
      // !! invalid - should be an offset from an appropriate image
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
      // !! invalid - should be an offset from an appropriate image
      unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0)) + disp;
      image->write(position, &offs, 2);

   }
}

inline void writeXDisp32Hi(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 2);
      image->addReference(reference, position);
   }
   else {
      // !! invalid - should be an offset from rdata
      unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0)) + disp;
      offs >>= 16;
      image->write(position, &offs, 2);
   }
}

inline void writeXDisp32Lo(MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      image->write(position, &disp, 2);
      image->addReference(reference, position);
   }
   else {
      // !! invalid - should be an offset from rdata
      unsigned int offs = (unsigned int)(vaddress - (addr_t)image->get(0)) + disp;
      image->write(position, &offs, 2);

   }
}

inline void writeRef32Lo(JITCompilerBase* compiler, MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   MemoryWriter writer(image, position);
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      if (disp)
         compiler->writeImm16(&writer, disp, 0);

      image->addReference(reference, position);
   }
   else {
      // save the lowest part of 32 bit address
      vaddress += disp;
      compiler->writeImm16(&writer, (int)vaddress, 0);
   }
}

inline void writeRef32Hi(JITCompilerBase* compiler, MemoryBase* image, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask, bool virtualMode)
{
   MemoryWriter writer(image, position);
   if (virtualMode) {
      // in the virtual mode vaddress is an image offset - plus address mask
      ref_t reference = (ref_t)vaddress | addressMask;

      if (disp)
         compiler->writeImm16(&writer, disp, 0);

      image->addReference(reference, position);
   }
   else {
      // save the highest part of 32 bit address
      vaddress += disp;
      compiler->writeImm16Hi(&writer, (int)vaddress, 0);
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
   ref_t reference, SectionInfo* sectionInfo, pos_t sectionOffset, ref_t addressMask)
{
   MemoryBase* section = sectionInfo->section;

   ref_t currentMask = reference & mskAnyRef;
   ref_t currentRef = reference & ~mskAnyRef;

   if (currentMask == 0) {
      MemoryBase::writeDWord(image, imageOffset,
         _owner->createMessage(_module, MemoryBase::getDWord(section, sectionOffset), *_references));
   }
   else if (currentMask == mskVMTMethodOffset) {
      _owner->resolve(
         _owner->_loader->retrieveReferenceInfo(sectionInfo->module, currentRef, mskVMTRef,
            _owner->_forwardResolver), mskVMTRef, false);

      // message id should be replaced with an appropriate method address
      mssg_t message = _owner->createMessage(sectionInfo->module, 
         MemoryBase::getDWord(section, sectionOffset), *_references);

      pos_t offset = _owner->resolveVMTMethodOffset(sectionInfo->module, currentRef, message);
      _owner->fixOffset(imageOffset, addressMask, offset, image);
   }
   else if (currentMask == mskVMTMethodAddress) {
      _owner->resolve(
         _owner->_loader->retrieveReferenceInfo(sectionInfo->module, currentRef, mskVMTRef,
            _owner->_forwardResolver), mskVMTRef, false);

      // message id should be replaced with an appropriate method address
      mssg_t message = _owner->createMessage(sectionInfo->module,
         MemoryBase::getDWord(section, sectionOffset), *_references);

      addr_t vaddress = _owner->resolveVMTMethodAddress(sectionInfo->module, currentRef, message);

      switch (addressMask & mskRefType) {
         case mskRef32:
            ::writeVAddress32(image, imageOffset, vaddress, 0, addressMask, _owner->_virtualMode);
            break;
         case mskRef64:
            ::writeVAddress64(image, imageOffset, vaddress, 0, addressMask, _owner->_virtualMode);
            break;
         default:
            // to make compiler happy
            break;
      }
   }
   else {
      addr_t vaddress = _owner->resolve(_owner->_loader->retrieveReferenceInfo(sectionInfo->module, currentRef, currentMask,
         _owner->_forwardResolver), currentMask, false);

      switch (addressMask & mskRefType) {
         case mskRef32:
            ::writeVAddress32(image, imageOffset, vaddress, 0, addressMask, _owner->_virtualMode);
            break;
         case mskRef64:
            ::writeVAddress64(image, imageOffset, vaddress, 0, addressMask, _owner->_virtualMode);
            break;
         default:
            // to make compiler happy
            break;
      }
   }
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
   switch (mask) {
      case mskAutoSymbolRef:
         // HOTFIX : preloaded symbols should be resolved later
         _owner->_mapper->addLazyReference({ mask, position, module, reference, addressMask });
         return;
      default:
         vaddress = _owner->_mapper->resolveReference(
            _owner->_loader->retrieveReferenceInfo(module, refID, mask, 
               _owner->_forwardResolver), mask);
         break;
   }

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
         case mskXDisp32Hi:
            ::writeXDisp32Hi(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskXDisp32Lo:
            ::writeXDisp32Lo(&target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskRef32Hi:
            ::writeRef32Hi(_owner->_compiler, &target, position, vaddress, disp, addressMask, _owner->_virtualMode);
            break;
         case mskRef32Lo:
            ::writeRef32Lo(_owner->_compiler, &target, position, vaddress, disp, addressMask, _owner->_virtualMode);
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
   ::writeRef32Hi(_owner->_compiler, &target, position, vaddress, disp, addressMask, _owner->_virtualMode);
}

void JITLinker::JITLinkerReferenceHelper :: writeVAddress32Lo(MemoryBase& target, pos_t position, addr_t vaddress, pos_t disp,
   ref_t addressMask)
{
   ::writeRef32Lo(_owner->_compiler, &target, position, vaddress, disp, addressMask, _owner->_virtualMode);
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

void JITLinker::JITLinkerReferenceHelper :: resolveLabel(MemoryWriter& writer, ref_t mask, pos_t position)
{
   _owner->_compiler->resolveLabelAddress(&writer, mask, position, _owner->_virtualMode);
}

addr_t JITLinker::JITLinkerReferenceHelper :: resolveMDataVAddress()
{
   if (_owner->_virtualMode) {
      // for the virtual mode, zero should be returned - indicating the start of the section
      return 0;
   }
   return (addr_t)_owner->_imageProvider->getMDataSection()->get(0);
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
      case mskRef64:
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
         case mskMssgNameLiteralRef:
         {
            ref_t dummy = 0;
            vaddress = resolve({ info.module, info.module->resolveAction(currentRef, dummy) }, currentMask, false);
            break;
         }
         case mskMssgLiteralRef:
         case mskExtMssgLiteralRef:
            vaddress = resolve({ info.module, info.module->resolveConstant(currentRef) }, currentMask, false);
            break;
         //case mskNameLiteralRef:
         //case mskPathLiteralRef:
         //   //NOTE : Zero reference is considered to be the reference to itself
         //   if (currentRef) {
         //      vaddress = resolveName(_loader->retrieveReferenceInfo(info.module, currentRef, 
         //         currentMask, _forwardResolver), currentMask == mskPathLiteralRef);
         //   }
         //   else vaddress = resolveName(ownerReferenceInfo, currentMask == mskPathLiteralRef);
         //   break;
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
         case mskXDisp32Hi:
            ::writeXDisp32Hi(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskXDisp32Lo:
            ::writeXDisp32Lo(image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskRef32Hi:
            ::writeRef32Hi(_compiler, image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
            break;
         case mskRef32Lo:
            ::writeRef32Lo(_compiler, image, it.key(), vaddress, info.disp, info.addressMask, _virtualMode);
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
      actionName, weakActionRef, signature, _virtualMode);

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
      if (referenceInfo.module != nullptr && isWeakReference(referenceInfo.referenceName)) {
         signatureName.append(referenceInfo.module->name());
         signatureName.append(referenceInfo.referenceName);
      }
      else signatureName.append(referenceInfo.referenceName);
   }

   ref_t resolvedSignature = _mapper->resolveAction(*signatureName, 0) & ~SIGNATURE_MASK;
   if (resolvedSignature == 0) {
      MemoryBase* messageBody = _imageProvider->getMBDataSection();
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

void JITLinker :: resolveStaticFields(ReferenceInfo& referenceInfo, MemoryReader& vmtReader, FieldAddressMap& staticValues)
{
   ClassInfo::StaticFieldMap statics({});
   ClassInfo::loadStaticFields(&vmtReader, statics);

   for (auto it = statics.start(); !it.eof(); ++it) {
      auto fieldInfo = *it;
      ref_t mask = fieldInfo.valueRef & mskAnyRef;

      if (fieldInfo.valueRef && fieldInfo.offset < 0) {
         addr_t vaddress = INVALID_REF;

         switch (fieldInfo.valueRef) {
            case mskNameLiteralRef:
               vaddress = resolveName(referenceInfo, false);
               break;
            case mskPathLiteralRef:
               vaddress = resolveName(referenceInfo, true);
               break;
            default:
               vaddress = resolve(
                  _loader->retrieveReferenceInfo(referenceInfo.module, fieldInfo.valueRef & ~mskAnyRef,
                     mask, _forwardResolver),
                  mskVMTRef, false);
         }

         assert(vaddress != INVALID_REF);
         staticValues.add(fieldInfo.offset, vaddress);
      }
   }
}

addr_t JITLinker :: createVMTSection(ReferenceInfo referenceInfo, ClassSectionInfo sectionInfo,
   VAddressMap& references)
{
   if (sectionInfo.vmtSection == nullptr || sectionInfo.codeSection == nullptr)
      return INVALID_ADDR;

#ifdef FULL_OUTOUT_INFO
   if (referenceInfo.referenceName)
      printf("linking %s\n", referenceInfo.referenceName.str());

   if (referenceInfo.referenceName.compare("'$inline0"))
      referenceInfo.module = referenceInfo.module;

#endif // FULL_OUTOUT_INFO

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
   _compiler->allocateVMT(vmtWriter, header.flags, header.count, header.staticSize);

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
         mssg_t message = helper.importMessage(entry.message);
         if (test(entry.message, STATIC_MESSAGE)) {
            _staticMethods.add(
               { vaddress, message }, methodPosition);
         }
         else _compiler->addVMTEntry(message, methodPosition,
            vmtImage->get(position), count);

         if (_addressMapper && methodPosition)
            _addressMapper->addMethod(vaddress, message, methodPosition);

         size -= sizeof(MethodEntry);
      }

      if (_withDebugInfo)
         endNativeDebugInfo(debugPosition);

      if (count != header.count)
         throw InternalError(errCorruptedVMT);

      // load the class class
      addr_t classClassAddress = getVMTAddress(sectionInfo.module, header.classRef, references);

      // load the static values
      FieldAddressMap staticValues(INVALID_REF);
      resolveStaticFields(referenceInfo, vmtReader, staticValues);

      resolveClassGlobalAttributes(referenceInfo, vmtReader, vaddress);

      // update VMT
      _compiler->updateVMTHeader(vmtWriter, parentAddress, classClassAddress, header.flags, header.count, staticValues, _virtualMode);
   }

   return vaddress;
}

void JITLinker :: resolveClassGlobalAttributes(ReferenceInfo referenceInfo, MemoryReader& vmtReader, addr_t vaddress)
{
   pos_t attrCount = vmtReader.getPos();
   while (attrCount > 0) {
      ClassAttribute attr = (ClassAttribute)vmtReader.getDWord();
      switch (attr) {
         case ClassAttribute::RuntimeLoadable:
            if (referenceInfo.isRelative()) {
               IdentifierString fullName(referenceInfo.module->name(), referenceInfo.referenceName);

               createGlobalAttribute(GA_CLASS_NAME, *fullName, vaddress);
            }
            else createGlobalAttribute(GA_CLASS_NAME, referenceInfo.referenceName, vaddress);
            break;
         case ClassAttribute::Initializer:
         {
            ref_t symbolRef = vmtReader.getDWord();
            attrCount -= sizeof(unsigned int);

            _mapper->addLazyReference({ mskAutoSymbolRef, INVALID_POS, referenceInfo.module, symbolRef, 0 });

            break;
         }
         default:
            break;
      }

      attrCount -= sizeof(unsigned int);
   }
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

      name.append("'");
      name.append(*properName);
      name.append("#sym");

      writer.writeString(*name);
   }
   else {
      NamespaceString ns(referenceInfo.referenceName);
      ReferenceProperName properName(referenceInfo.referenceName);

      IdentifierString name(*ns, "'", *properName);
      name.append("#sym");
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

void JITLinker :: createGlobalAttribute(int category, ustr_t value, addr_t address)
{
   MemoryWriter writer(_imageProvider->getADataSection());
   _compiler->writeAttribute(writer, category, value, address, _virtualMode);
}

void JITLinker :: resolveSymbolAttributes(ReferenceInfo referenceInfo, addr_t vaddress, SectionInfo sectionInfo)
{
   MemoryReader reader(sectionInfo.metaSection);
   SymbolInfo info;
   info.load(&reader);

   if (info.loadableInRuntime) {
      if (referenceInfo.isRelative()) {
         IdentifierString refName(referenceInfo.module->name(), referenceInfo.referenceName);

         createGlobalAttribute(GA_SYMBOL_NAME, *refName, vaddress);
      }
      else createGlobalAttribute(GA_SYMBOL_NAME, referenceInfo.referenceName, vaddress);
   }
}

addr_t JITLinker :: resolveBytecodeSection(ReferenceInfo referenceInfo, ref_t sectionMask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == nullptr)
      return INVALID_ADDR;

   MemoryBase*  image = _imageProvider->getTargetSection(mskCodeRef);
   MemoryWriter writer(image);

   addr_t vaddress = calculateVAddress(writer, mskCodeRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   if (_addressMapper)
      _addressMapper->addSymbol(vaddress, writer.position());

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

   if (sectionInfo.metaSection)
      resolveSymbolAttributes(referenceInfo, vaddress, sectionInfo);

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

ReferenceInfo JITLinker :: retrieveConstantVMT(SectionInfo sectionInfo)
{
   if (sectionInfo.module) {
      for (auto it = RelocationMap::Iterator(sectionInfo.section->getReferences()); !it.eof(); ++it) {
         if ((*it) == (pos_t)-4) {
            return _loader->retrieveReferenceInfo(sectionInfo.module, it.key() & ~mskAnyRef, it.key() & mskAnyRef,
               _forwardResolver);
         }
      }
   }

   return {};
}

addr_t JITLinker :: resolveConstantArray(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode)
{
   VAddressMap references({ 0, nullptr, 0, 0 });
   ReferenceInfo vmtReferenceInfo;

   bool structMode = false;

   // resolve constant value
   SectionInfo sectionInfo = _loader->getSection(referenceInfo, sectionMask, 0, silentMode);
   if (!sectionInfo.section)
      return 0;

   vmtReferenceInfo = retrieveConstantVMT(sectionInfo);

   int size = sectionInfo.section->length() >> 2;

   // get constant VMT reference
   addr_t vmtVAddress = INVALID_ADDR;
   if (!vmtReferenceInfo.referenceName.empty()) {
      vmtVAddress = resolve(vmtReferenceInfo, mskVMTRef, true);
   }

   // get target image & resolve virtual address
   MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   _compiler->allocateHeader(writer, vmtVAddress, size, structMode, _virtualMode);
   addr_t vaddress = calculateVAddress(writer, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   JITLinkerReferenceHelper helper(this, sectionInfo.module, &references);
   _compiler->writeCollection(&helper, writer, &sectionInfo);

   fixReferences(references, image);

   return vaddress;
}

addr_t JITLinker :: resolveConstantDump(ReferenceInfo referenceInfo, SectionInfo sectionInfo, ref_t sectionMask)
{
   // get target image & resolve virtual address
   MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   ReferenceInfo  vmtReferenceInfo;
   VAddressMap    references({ 0, nullptr, 0, 0 });

   vmtReferenceInfo = retrieveConstantVMT(sectionInfo);

   int size = sectionInfo.section->length();

   // get constant VMT reference
   addr_t vmtVAddress = INVALID_ADDR;
   if (!vmtReferenceInfo.referenceName.empty()) {
      vmtVAddress = resolve(vmtReferenceInfo, mskVMTRef, true);
   }

   // allocate object header
   _compiler->allocateHeader(writer, vmtVAddress, size, true, _virtualMode);
   addr_t vaddress = calculateVAddress(writer, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   _compiler->writeDump(writer, &sectionInfo);

   fixReferences(references, image);

   return vaddress;
}

addr_t JITLinker :: resolveRawConstant(ReferenceInfo referenceInfo)
{
   // get target image & resolve virtual address
   MemoryBase* image = _imageProvider->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   addr_t vaddress = calculateVAddress(writer, mskRDataRef);

   _compiler->writeLiteral(writer, referenceInfo.referenceName);

   return vaddress;
}

addr_t JITLinker :: resolveConstantDump(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode)
{
   // resolve constant value
   SectionInfo sectionInfo = _loader->getSection(referenceInfo, sectionMask, 0, silentMode);
   if (!sectionInfo.section)
      return 0;

   return resolveConstantDump(referenceInfo, sectionInfo, sectionMask);
}

addr_t JITLinker :: resolveName(ReferenceInfo referenceInfo, bool onlyPath)
{
   IdentifierString fullName;
   if (referenceInfo.module && isWeakReference(referenceInfo.referenceName)) {
      fullName.copy(referenceInfo.module->name());
   }
   fullName.append(referenceInfo.referenceName);

   if (onlyPath) {
      NamespaceString ns(*fullName);

      return resolve(*ns, mskLiteralRef, false);
   }
   else return resolve(*fullName, mskLiteralRef, false);
}

mssg_t JITLinker :: parseMessageLiteral(ustr_t messageLiteral, ModuleBase* module, VAddressMap& references)
{
   mssg_t message = ByteCodeUtil::resolveMessage(messageLiteral, module, true);

   return createMessage(module, message, references);
}

mssg_t JITLinker :: parseMessageNameLiteral(ustr_t messageLiteral, ModuleBase* module, VAddressMap& references)
{
   mssg_t message = ByteCodeUtil::resolveMessageName(messageLiteral, module, true);

   return createMessage(module, message, references);
}

Pair<mssg_t, addr_t> JITLinker :: parseExtMessageLiteral(ustr_t messageLiteral, ModuleBase* module, VAddressMap& references)
{
   Pair<mssg_t, addr_t> retVal = {};

   IdentifierString messageName(messageLiteral);
   IdentifierString extensionReferenceName;

   size_t index = messageLiteral.find('<');
   assert(index != NOTFOUND_POS);
   size_t endIndex = messageLiteral.findSub(index, '>');

   extensionReferenceName.copy(messageLiteral + index + 1, endIndex - index - 1);
   messageName.cut(index, endIndex - index + 1);

   //vextAddress = resolve(*extensionReferenceName, mskVMTRef, false);

   retVal.value1 = createMessage(module, ByteCodeUtil::resolveMessage(*messageName, module, true), references);
   // HOTFIX : extension message should be a function one
   retVal.value1 |= FUNCTION_MESSAGE;

   addr_t vmtExtVAddress = resolve(*extensionReferenceName, mskVMTRef, false);
   retVal.value2 = getVMTMethodAddress(vmtExtVAddress, retVal.value1);;

   return retVal;
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
      case mskLongLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.longLiteralClass;
         size = 8;
         structMode = true;
         break;
      case mskRealLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.realLiteralClass;
         size = 8;
         structMode = true;
         break;
      case mskLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.literalClass;
         size = value.length_pos() + 1;
         structMode = true;
         break;
      case mskWideLiteralRef:
      {
         WideMessage tmp(value);
         vmtReferenceInfo.referenceName = _constantSettings.wideLiteralClass;
         size = (tmp.length_pos() + 1) << 1;
         structMode = true;
         break;
      }
      case mskCharacterRef:
         vmtReferenceInfo.referenceName = _constantSettings.characterClass;
         size = 4;
         structMode = true;
         break;
      case mskMssgLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.messageClass;
         size = 4;
         structMode = true;
         break;
      case mskMssgNameLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.messageNameClass;
         size = 4;
         structMode = true;
         break;
      case mskExtMssgLiteralRef:
         vmtReferenceInfo.referenceName = _constantSettings.extMessageClass;
         size = _compiler->getExtMessageSize();
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

   VAddressMap messageReferences({});

   // allocate object header
   _compiler->allocateHeader(writer, vmtVAddress, size, structMode, _virtualMode);
   vaddress = calculateVAddress(writer, mskRDataRef);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   switch (sectionMask) {
      case mskIntLiteralRef:
         _compiler->writeInt32(writer, StrConvertor::toUInt(value, 16));
         break;
      case mskLongLiteralRef:
         _compiler->writeInt64(writer, StrConvertor::toLong(value, 16));
         break;
      case mskRealLiteralRef:
         _compiler->writeFloat64(writer, StrConvertor::toDouble(value));
         break;
      case mskCharacterRef:
         _compiler->writeChar32(writer, value);
         break;
      case mskMssgLiteralRef:
         _compiler->writeMessage(writer, parseMessageLiteral(value, referenceInfo.module, messageReferences));
         break;
      case mskMssgNameLiteralRef:
         _compiler->writeMessage(writer, parseMessageNameLiteral(value, referenceInfo.module, messageReferences));
         break;
      case mskExtMssgLiteralRef:
      {
         pos_t position = writer.position();
         _compiler->allocateBody(writer, size);
         writer.seek(position);

         auto extensionInfo = parseExtMessageLiteral(value, referenceInfo.module, messageReferences);

         _compiler->writeExtMessage(writer, extensionInfo, _virtualMode);
         break;
      }
      case mskLiteralRef:
         _compiler->writeLiteral(writer, value);
         break;
      case mskWideLiteralRef:
      {
         WideMessage tmp(value);

         _compiler->writeWideLiteral(writer, *tmp);
         break;
      }
      default:
         break;
   }

   // load message references
   fixReferences(messageReferences, nullptr);

   return vaddress;
}

addr_t JITLinker :: resolveStaticVariable(ReferenceInfo referenceInfo, ref_t sectionMask)
{
   // get target image & resolve virtual address
   MemoryBase* image = _imageProvider->getTargetSection(mskStatDataRef);
   MemoryWriter writer(image);

   addr_t vaddress = calculateVAddress(writer, mskStatDataRef);
   _compiler->writeVariable(writer);

   _mapper->mapReference(referenceInfo, vaddress, sectionMask);

   return vaddress;
}

void JITLinker :: copyMetaList(ModuleInfo info, ModuleInfoList& output)
{
   auto sectionInfo = _loader->getSection({ info.module, info.module->resolveReference(info.reference) }, mskTypeListRef, 0, true);
   if (!sectionInfo.module)
      return;

   MemoryReader bcReader(sectionInfo.section);

   while (!bcReader.eof()) {
      ref_t symbolRef = bcReader.getRef();

      output.add({ info.module, symbolRef & ~mskAnyRef });
   }
}

void JITLinker :: prepare(JITCompilerBase* compiler)
{
   _compiler = compiler;

   _withDebugInfo = _compiler->isWithDebugInfo();

   VAddressMap references(VAddressInfo(0, nullptr, 0, 0));
   JITLinkerReferenceHelper helper(this, nullptr, &references);

   // --- attribute table ---
   // allocate the place for section size place-holder
   MemoryBase* aSection = _imageProvider->getADataSection();
   MemoryBase::writeDWord(aSection, 0, 0);

   // --- message tables ---
   // should start with empty entry
   createAction(nullptr, 0u, 0u);
   // dispatch message should be the next one - to make sure it is always
   // the first entry in the class VMT
   resolveWeakAction(DISPATCH_MESSAGE);
   // protected default constructor message should be the second
   // NOTE : protected constructor action can be used only for default constructor due to current implementation
   // (we have to guarantee that default constructor is always the second one)
   resolveWeakAction(CONSTRUCTOR_MESSAGE2);
   // constructor message should be the third
   resolveWeakAction(CONSTRUCTOR_MESSAGE);

   // prepare jit compiler
   _compiler->prepare(_loader, _imageProvider, &helper, nullptr, _jitSettings);

   // fix not loaded references
   fixReferences(references, _imageProvider->getTextSection());
}

void JITLinker :: complete(JITCompilerBase* compiler, ustr_t superClass)
{
   if (!superClass.empty()) {
      // set voidobj
      addr_t superAddr = resolve(superClass, mskVMTRef, true);
      compiler->updateVoidObject(_imageProvider->getRDataSection(), superAddr, _virtualMode);
   }

   // fix message body references
   MemoryBase* mbSection = _imageProvider->getMBDataSection();
   VAddressMap mbReferences({});
   _mapper->forEachLazyReference<VAddressMap*>(&mbReferences, [](VAddressMap* mbReferences, LazyReferenceInfo info)
   {
      if (info.mask == mskMessageBodyRef) {
         mbReferences->add(info.position, { info.reference, info.module, info.addressMask, 0 });
      }
   });

   // fix static table size
   compiler->updateEnvironment(
      _imageProvider->getRDataSection(),
      compiler->getStaticCounter(_imageProvider->getStatSection(), true),
      _virtualMode);

   fixReferences(mbReferences, mbSection);

   // fix attribute image - specify the attribute size
   MemoryBase* aSection = _imageProvider->getADataSection();
   MemoryWriter aWriter(aSection);
   aWriter.align(8, 0);
   aWriter.seek(0);
   aWriter.writePos(aSection->length());
}

addr_t JITLinker :: resolve(ustr_t referenceName, ref_t sectionMask, bool silentMode)
{
   switch (sectionMask) {
      case mskLiteralRef:
      case mskIntLiteralRef:
      case mskCharacterRef:
      case mskMssgLiteralRef:
      case mskLongLiteralRef:
      case mskRealLiteralRef:
         return resolve({ nullptr, referenceName }, sectionMask, silentMode);
      default:
      {
         ReferenceInfo referenceInfo = _loader->retrieveReferenceInfo(referenceName, _forwardResolver);

         return resolve(referenceInfo, sectionMask, silentMode);
      }
   }
}

addr_t JITLinker :: resolve(ReferenceInfo referenceInfo, ref_t sectionMask, bool silentMode)
{
   addr_t address = _mapper->resolveReference(referenceInfo, sectionMask);
   if (address == INVALID_ADDR) {
      switch (sectionMask) {
         case mskSymbolRef:
         case mskProcedureRef:
            address = resolveBytecodeSection(referenceInfo, sectionMask, 
               _loader->getSection(referenceInfo, sectionMask, mskMetaSymbolInfoRef, silentMode));
            break;
         case mskVMTRef:
            address = resolveVMTSection(referenceInfo, 
               _loader->getClassSections(referenceInfo, mskVMTRef, mskClassRef, silentMode));
            break;
         case mskStaticVariable:
            address = resolveStaticVariable(referenceInfo, sectionMask);
            break;
         case mskTypeListRef:
            address = resolveMetaSection(referenceInfo, sectionMask, 
               _loader->getSection(referenceInfo, sectionMask, 0, silentMode));
            break;
         case mskIntLiteralRef:
         case mskLongLiteralRef:
         case mskRealLiteralRef:
         case mskLiteralRef:
         case mskWideLiteralRef:
         case mskCharacterRef:
         case mskMssgLiteralRef:
         case mskExtMssgLiteralRef:
         case mskMssgNameLiteralRef:
            address = resolveConstant(referenceInfo, sectionMask);
            break;
         case mskConstant:
            address = resolveConstantDump(referenceInfo, sectionMask, false);
            break;
         case mskConstArray:
            address = resolveConstantArray(referenceInfo, sectionMask, silentMode);
            break;
         case mskPSTRRef:
            address = resolveRawConstant(referenceInfo);
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

addr_t JITLinker :: resolveTape(ustr_t referenceName, MemoryBase* tape)
{
   return resolveConstantDump({ referenceName }, { tape }, mskConstant);
}

addr_t JITLinker :: resolveTemporalByteCode(MemoryDump& tapeSymbol, ModuleBase* module)
{
   VAddressMap references(VAddressInfo(0, nullptr, 0, 0));
   JITLinkerReferenceHelper helper(this, module, &references);

   MemoryReader reader(&tapeSymbol);

   MemoryBase* image = _imageProvider->getTargetSection(mskCodeRef);

   // map dynamic code
   MemoryWriter writer(image);
   addr_t vaddress = calculateVAddress(writer, mskCodeRef);

   _compiler->compileProcedure(&helper, reader, writer, nullptr);

   fixReferences(references, image);

   return vaddress;
}

addr_t JITLinker :: resolveTLSSection(JITCompilerBase* compiler)
{
   JITLinkerReferenceHelper helper(this, nullptr, nullptr);

   MemoryWriter tlsWriter(_imageProvider->getTLSSection());
   compiler->allocateThreadContent(&tlsWriter);

   MemoryBase* image = _imageProvider->getTargetSection(mskDataRef);
   MemoryWriter writer(image);

   addr_t address = compiler->allocateTLSIndex(&helper, writer);

   return address;
}

void JITLinker :: loadPreloaded(ustr_t preloadedSection)
{
   ModuleInfoList list({});
   ModuleInfoList symbolList({});

   IdentifierString nameToResolve(META_PREFIX, preloadedSection);

   // load preloaded symbols
   _loader->loadDistributedSymbols(*nameToResolve, list);
   for (auto it = list.start(); !it.eof(); ++it) {
      copyMetaList(*it, symbolList);
   }

   // save preloaded symbols as auto symbols
   for (auto it = symbolList.start(); !it.eof(); ++it) {
      auto info = *it;

      _mapper->addLazyReference({ mskAutoSymbolRef, INVALID_POS, info.module, info.reference, 0 });
   }
}
