//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT linker class implementation.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitlinker.h"

using namespace _ELENA_;

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

   return _owner->_loader->getSectionInfo(referenceName, reference & mskAnyRef);
}

SectionInfo JITLinker::ReferenceHelper :: getCoreSection(ref_t reference)
{
   return _owner->_loader->getCoreSectionInfo(reference, 0);
}

ref_t JITLinker::ReferenceHelper :: resolveMessage(ref_t reference, _Module* module)
{
   if (!module)
      module = _module;

   return _owner->resolveMessage(module, reference);
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

//void ReferenceLoader::ReferenceHelper :: writeMethodReference(SectionWriter& writer, size_t tapeDisp)
//{
//   _relocations->add(tapeDisp, writer.Position());
//   writer.writeRef(mskCodeRef, 0);
//}

void JITLinker::ReferenceHelper :: writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module)
{
   ref_t mask = reference & mskAnyRef;
   ref_t refID = reference & ~mskAnyRef;

   //// check if it is a constant, resolve it immediately
   //if (mask == mskLinkerConstant) {
   //   writer.writeDWord(getLinkerConstant(refID));
   //   return;
   //}

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
   void* vaddress = _owner->_loader->resolveReference(
      _owner->_loader->retrieveReference(module, refID, mask), mask);

   if (vaddress != LOADER_NOTLOADED) {
      resolveReference(writer.Memory(), position, (ref_t)vaddress, mask, _owner->_virtualMode);
   }
   // or resolve later
   else _references->add(position, RefInfo(reference, module));
}

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

ref_t JITLinker :: resolveMessage(_Module* module, ref_t message)
{
   ref_t verbId = 0;
   ref_t signRef = 0;
   int paramCount = 0;
   decodeMessage(message, signRef, verbId, paramCount);

   // if it is generic message
   if (signRef == 0) {
      return message | MESSAGE_MASK;
   }

   // otherwise signature and custom verb should be imported
   if (signRef != 0) {
      ident_t subject = module->resolveSubject(signRef);
      signRef = (ref_t)_loader->resolveReference(subject, 0);
   }
   return encodeMessage(signRef, verbId, paramCount) | MESSAGE_MASK;
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
         void* refVAddress = resolve(_loader->retrieveReference(current.module, currentRef, mskVMTRef), mskVMTRef, false);

         // message id should be replaced with an appropriate method address
         size_t offset = it.key();
         size_t messageID = (*image)[offset];

         (*image)[offset] = getVMTMethodAddress(refVAddress, messageID);
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
      // otherwise
      else {   
         void* refVAddress = resolve(_loader->retrieveReference(current.module, currentRef, currentMask), currentMask, false);

         resolveReference(image, it.key(), (ref_t)refVAddress, currentMask, _virtualMode);
      }
      it++;
   }
}

void* JITLinker :: getVMTAddress(_Module* module, ref_t reference, References& references)
{
   if (reference != 0) {
      ident_t referenceName = _loader->retrieveReference(module, reference, mskVMTRef);

      void* vaddress = _loader->resolveReference(referenceName, mskVMTRef);

      if (vaddress==LOADER_NOTLOADED) {
         // create VMT table without resolving references to prevent circular reference
         vaddress = createBytecodeVMTSection(referenceName, mskVMTRef, _loader->getClassSectionInfo(referenceName, mskClassRef, mskVMTRef, false), references);

         if (vaddress == LOADER_NOTLOADED)
            throw JITUnresolvedException(referenceName);
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

void* JITLinker :: resolveNativeSection(ident_t reference, int mask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == NULL)
      return LOADER_NOTLOADED;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mask);
   MemoryWriter writer(image);

   void* vaddress = calculateVAddress(&writer, mask & mskImageMask);
   size_t position = writer.Position();

   _loader->mapReference(reference, vaddress, mask);

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
      else if (currentMask == 0) {
         (*image)[*it + position] = resolveMessage(sectionInfo.module, currentRef);
      }
      else {
         void* refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

         resolveReference(image, *it + position, (ref_t)refVAddress, currentMask, _virtualMode);
      }
      it++;
   }
   return vaddress;
}

void* JITLinker :: resolveNativeVariable(ident_t reference, int mask)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskDataRef);
   MemoryWriter writer(image);

   void* vaddress = calculateVAddress(&writer, mskDataRef, 4);

   _compiler->allocateVariable(writer);

   _loader->mapReference(reference, vaddress, mask);

   return vaddress;
}

void* JITLinker :: resolveBytecodeSection(ident_t reference, int mask, SectionInfo sectionInfo)
{
   if (sectionInfo.section == NULL)
      return LOADER_NOTLOADED;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mask);
   MemoryWriter writer(image);

   void* vaddress = calculateVAddress(&writer, mask & mskImageMask);

   _loader->mapReference(reference, vaddress, mask);

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
         createNativeSymbolDebugInfo(reference, vaddress, sizePtr);

      _compiler->compileSymbol(refHelper, reader, writer);
   }

   if (_withDebugInfo)
      endNativeDebugInfo(sizePtr);

   // fix not loaded references
   fixReferences(references, image);

   return vaddress;
}

void* JITLinker :: createBytecodeVMTSection(ident_t reference, int mask, ClassSectionInfo sectionInfo, References& references)
{
   if (sectionInfo.codeSection == NULL || sectionInfo.vmtSection == NULL)
      return LOADER_NOTLOADED;

   ReferenceHelper refHelper(this, sectionInfo.module, &references);

   // VMT just in time compilation
   MemoryReader vmtReader(sectionInfo.vmtSection);
   // read tape record size
   size_t size = vmtReader.getDWord();

   // read class reference
   ref_t classClassRef = vmtReader.getDWord();

   // read VMT header
   ClassHeader header;
   vmtReader.read((void*)&header, sizeof(ClassHeader));

   // get target image & resolve virtual address
   _Memory* vmtImage = _loader->getTargetSection(mask);
   _Memory* codeImage = _loader->getTargetSection(mskClassRef);

   MemoryWriter vmtWriter(vmtImage);

   // allocate space and make VTM offset
   _compiler->allocateVMT(vmtWriter, header.flags, header.count);

   void* vaddress = calculateVAddress(&vmtWriter, mask & mskImageMask);

   _loader->mapReference(reference, vaddress, mask);

   // if it is a standard VMT
   if (test(header.flags, elStandartVMT)) {
      size_t position = vmtWriter.Position();

      // load parent class
      void* parentVMT = getVMTReference(sectionInfo.module, header.parentRef, references);
      size_t count = _compiler->copyParentVMT(parentVMT, (VMTEntry*)vmtImage->get(position));

      // create native debug info header if debug info enabled
      size_t sizePtr = (size_t)-1;
      if (_withDebugInfo)
         createNativeClassDebugInfo(reference, vaddress, sizePtr);

      // read and compile VMT entries
      MemoryWriter   codeWriter(codeImage);
      MemoryReader   codeReader(sectionInfo.codeSection);

      size_t          methodPosition;
      VMTEntry        entry;

      size -= sizeof(ClassHeader) + 4;
      while (size > 0) {
         vmtReader.read((void*)&entry, sizeof(VMTEntry));
   
         codeReader.seek(entry.address);
         methodPosition = loadMethod(refHelper, codeReader, codeWriter);
         
         _compiler->addVMTEntry(refHelper, entry.message, methodPosition, (VMTEntry*)vmtImage->get(position), count);

         size -= sizeof(VMTEntry);
      }
      if (_withDebugInfo)
         endNativeDebugInfo(sizePtr);

      //if (count != header.count)
      //   count = 0;

      // load class class
      void* classClassVAddress = getVMTAddress(sectionInfo.module, classClassRef, references);

      // fix VMT
      _compiler->fixVMT(vmtWriter, classClassVAddress, count, _virtualMode);
   }

   return vaddress;
}

void* JITLinker :: resolveBytecodeVMTSection(ident_t reference, int mask, ClassSectionInfo sectionInfo)
{
   References      references(RefInfo(0, NULL));

   // create VMT
   void* vaddress = createBytecodeVMTSection(reference, mask, sectionInfo, references);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection(mskClassRef));

   return vaddress;
}

void* JITLinker :: resolveConstant(ident_t reference, int mask)
{
   bool constantValue = true;
   ident_t value = NULL;
   ident_t vmtReference = reference;
   if (mask == mskLiteralRef) {
      value = reference;
      vmtReference = _loader->getLiteralClass();
   }
   else if (mask == mskCharRef) {
      value = reference;
      vmtReference = _loader->getCharacterClass();
   }
   else if (mask == mskInt32Ref) {
      value = reference;
      vmtReference = _loader->getIntegerClass();
   }
   else if (mask == mskInt64Ref) {
      value = reference;
      vmtReference = _loader->getLongClass();
   }
   else if (mask == mskRealRef) {
      value = reference;
      vmtReference = _loader->getRealClass();
   }
   else constantValue = false;

   // get constant VMT reference
   void* vmtVAddress = resolve(vmtReference, mskVMTRef, true);

   // HOTFIX: if the constant is referred by iself it could be already resolved
   void* vaddress = _loader->resolveReference(reference, mask);
   if (vaddress != LOADER_NOTLOADED)
      return vaddress;

   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   int vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(reference, vaddress, mask);

   size_t position = writer.Position();
   if (mask == mskLiteralRef) {
      _compiler->compileLiteral(&writer, value);
   }
   else if (mask == mskCharRef) {
      _compiler->compileChar32(&writer, value);
   }
   else if (mask == mskInt32Ref) {
      _compiler->compileInt32(&writer, StringHelper::strToULong(value, 16));
   }
   else if (mask == mskInt64Ref) {
      // a constant starts with a special mark to tell apart from integer constant, so it should be skipped before converting to the number
      _compiler->compileInt64(&writer, StringHelper::strToLongLong(value + 1, 10));
   }
   else if (mask == mskRealRef) {
      _compiler->compileReal64(&writer, StringHelper::strToDouble(value));
   }
   else if (vmtVAddress == LOADER_NOTLOADED) {
      // check if it built-in constants
      if (StringHelper::compare(reference, PACKAGE_KEY)) {
         _compiler->compileLiteral(&writer, _loader->getNamespace());

         vmtVAddress = resolve(_loader->getLiteralClass(), mskVMTRef, true);
      }
      else {
         // resolve constant value
         SectionInfo sectionInfo = _loader->getSectionInfo(reference, mskRDataRef);
         _compiler->compileBinary(&writer, sectionInfo.section);

         // resolve section references
         _ELENA_::RelocationMap::Iterator it(sectionInfo.section->getReferences());
         ref_t currentMask = 0;
         ref_t currentRef = 0;
         while (!it.Eof()) {
            currentMask = it.key() & mskAnyRef;
            currentRef = it.key() & ~mskAnyRef;

            void* refVAddress = resolve(_loader->retrieveReference(sectionInfo.module, currentRef, currentMask), currentMask, false);

            if (*it == -4) {
               // resolve the constant vmt reference
               vmtVAddress = refVAddress;
            }
            else resolveReference(image, *it + position, (ref_t)refVAddress, currentMask, _virtualMode);

            it++;
         }
      }
      constantValue = true;
   }

   if (vmtVAddress == LOADER_NOTLOADED)
      throw JITUnresolvedException(reference);

   // check if the class could be constant one
   if (!constantValue) {
      // read VMT flags
      size_t flags = getVMTFlags(vmtVAddress);

      if (!test(flags, elStateless))
         throw JITConstantExpectedException(reference);
   }

   // fix object VMT reference
   resolveReference(image, vmtPosition, (ref_t)vmtVAddress, mskVMTRef, _virtualMode);

   return vaddress;
}

void* JITLinker :: resolveStaticVariable(ident_t reference, int mask)
{
   // get target image & resolve virtual address
   MemoryWriter writer(_loader->getTargetSection(mask));

   size_t vaddress = (_virtualMode ? writer.Position() | mask : (size_t)writer.Address());

   _compiler->allocateVariable(writer);

   _statLength++;

   _loader->mapReference(reference, (void*)vaddress, mask);

   return (void*)vaddress;
}

//void* JITLinker :: resolveDump(const wchar16_t*  reference, int size, int mask)
//{
//   // get target image & resolve virtual address
//   MemoryWriter writer(_loader->getTargetSection(mask));
//
//   size_t vaddress = _virtualMode ? (writer.Position() | mask) : (size_t)writer.Address();
//
//   _compiler->allocateArray(writer, size);
//
//   _loader->mapReference(reference, (void*)vaddress, mask);
//
//   return (void*)vaddress;
//}

ref_t JITLinker :: parseMessage(ident_t reference)
{
   // message constant: nverb&signature

   int verbId = 0;
   int signatureId = 0;

   // read the param counter
   int count = reference[0] - '0';
   
   // skip the param counter
   reference++;

   int index = StringHelper::find(reference, '&');
   //HOTFIX: for generic GET message we have to ignore ampresand
   if (reference[index + 1] == 0)
      index = -1;

   if (index != -1) {
      //HOTFIX: for GET message we have &&, so the second ampersand should be used
      if (reference[index + 1] == 0 || reference[index + 1]=='&')
         index++;
      
      IdentifierString verb(reference, index);
      ident_t signature = reference + index + 1;

      // if it is a predefined verb
      if (verb[0] == '#') {
         verbId = verb[1] - 0x20;
      }

      // resolve signature
      signatureId = (int)_loader->resolveReference(signature, 0);
   }
   else {
      // if it is a predefined verb
      if (reference[0] == '#') {
         verbId = reference[1] - 0x20;
      }
      else signatureId = (int)_loader->resolveReference(reference, 0);
   }

   return MESSAGE_MASK | encodeMessage(signatureId, verbId, count);
}

void* JITLinker :: resolveMessage(ident_t reference, ident_t vmt)
{
   // get target image & resolve virtual address
   _Memory* image = _loader->getTargetSection(mskRDataRef);
   MemoryWriter writer(image);

   // allocate object header
   int vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));

   void* vaddress = calculateVAddress(&writer, mskRDataRef);

   _loader->mapReference(reference, vaddress, mskMessage);

   _compiler->compileInt32(&writer, parseMessage(reference));

   // get constant VMT reference
   void* vmtVAddress = resolve(vmt, mskVMTRef, false);

   // fix object VMT reference
   resolveReference(image, vmtPosition, (ref_t)vmtVAddress, mskVMTRef, _virtualMode);

   return vaddress;
}

//void* JITLinker :: resolveLoader(const wchar16_t*  reference)
//{
//   // resolve symbol reference
//   void* symbolVAddress = resolve(reference, mskSymbolRef, /*true*/false);
//   
//   // resolve vmt
//   void* vmtVAddress = resolve(ConstantIdentifier(SYMBOL_CLASS), mskVMTRef, false);
//
//   // get target image & resolve virtual address
//   _Memory* image = _loader->getTargetSection(mskRDataRef);
//   MemoryWriter writer(image);
//
//   // allocate object header
//   int vmtPosition = _compiler->allocateConstant(writer, _loader->getLinkerConstant(lnObjectSize));
//
//   bool valueConstant = false;
//   void* vaddress = calculateVAddress(&writer, mskRDataRef);
//
//   _loader->mapReference(reference, vaddress, mskSymbolLoaderRef);
//
//   size_t valuePosition = writer.Position();
//   _compiler->allocateVariable(writer);
//
//   // fix object VMT reference
//   resolveReference(image, vmtPosition, (ref_t)vmtVAddress, mskVMTRef, _virtualMode);
//   resolveReference(image, valuePosition, (ref_t)symbolVAddress, mskSymbolRef, _virtualMode);
//
//   return vaddress;
//}

//void* JITLinker :: resolveThreadSafeVariable(const TCHAR*  reference, int mask)
//{
//   // get target image & resolve virtual address
//   MemoryWriter writer(_loader->getTargetSection(mskTLSRef));
//
//   size_t vaddress = (_virtualMode ? writer.Position() : (size_t)writer.Address()) | mskTLSRef;
//
//   writer.writeDWord(0);
//
//   _loader->mapReference(reference, (void*)vaddress, mskTLSRef);
//
//   return (void*)vaddress;
//}

void JITLinker :: createNativeDebugInfo(ident_t reference, void* param, size_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);
   writer.writeLiteral(reference);

   sizePtr = writer.Position();
   writer.writeDWord(0); // size place holder

   writer.writeDWord((size_t)param);
}

void JITLinker :: createNativeSymbolDebugInfo(ident_t reference, void* address, size_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);
   // start with # to distinguish the symbol debug info from the class one
   writer.writeChar('#');
   writer.writeLiteral(reference);

   sizePtr = writer.Position();
   writer.writeDWord(0); // size place holder

   // save symbol entry

   // save VMT address
   if (!_virtualMode || address == NULL) {
      writer.writeDWord((size_t)address);
   }
   else writer.writeRef((ref_t)address, 0);
}

void JITLinker :: createNativeClassDebugInfo(ident_t reference, void* vaddress, size_t& sizePtr)
{
   _Memory* debug = _loader->getTargetDebugSection();

   MemoryWriter writer(debug);
   writer.writeLiteral(reference);

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

   if (_withDebugInfo) {
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

// NOTE: reference should not be a forward one, otherwise there may be code duplication
void* JITLinker :: resolve(ident_t reference, int mask, bool silentMode)
{
   void* vaddress = _loader->resolveReference(reference, mask);
   if (vaddress==LOADER_NOTLOADED) {
      switch (mask) {
         case mskSymbolRef:
//         case mskClassRef:
            vaddress = resolveBytecodeSection(reference, mask, _loader->getSectionInfo(reference, mask));
            break;
         case mskInternalRef:
         case mskInternalRelRef:
            vaddress = resolveBytecodeSection(reference, mask & ~mskRelCodeRef, _loader->getSectionInfo(reference, 0));
            break;
         case mskSymbolRelRef:
//         case mskClassRelRef:
            vaddress = resolveBytecodeSection(reference, mask & ~mskRelCodeRef, _loader->getSectionInfo(reference, mask & ~mskRelCodeRef));
            break;
         case mskVMTRef:
            vaddress = resolveBytecodeVMTSection(reference, mask, _loader->getClassSectionInfo(reference, mskClassRef, mskVMTRef, silentMode));
            break;
         case mskNativeCodeRef:
         case mskNativeDataRef:
         case mskNativeRDataRef:
            vaddress = resolveNativeSection(reference, mask, _loader->getSectionInfo(reference, mask));
            break;
         case mskNativeRelCodeRef:
            vaddress = resolveNativeSection(reference, mskNativeCodeRef, _loader->getSectionInfo(reference, mskNativeCodeRef));
            break;
         case mskConstantRef:
         case mskLiteralRef:
         case mskCharRef:
         case mskInt32Ref:
         case mskRealRef:
         case mskInt64Ref:
            vaddress = resolveConstant(reference, mask);
            break;
         case mskStatSymbolRef:
            vaddress = resolveStaticVariable(reference, mskStatRef);
            break;
         case mskMessage:
            vaddress = resolveMessage(reference, _loader->getMessageClass());
            break;
         case mskSignature:
            vaddress = resolveMessage(reference, _loader->getSignatureClass());
            break;
         case mskVerb:
            vaddress = resolveMessage(reference, _loader->getVerbClass());
            break;
//         //case mskSymbolLoaderRef:
//         //   vaddress = resolveLoader(reference);
//         //   break;
         case mskNativeVariable:
         case mskLockVariable:
            vaddress = resolveNativeVariable(reference, mask);
            break;
      }
   }
   if (!silentMode && vaddress == LOADER_NOTLOADED)
      throw JITUnresolvedException(reference);

   return vaddress;
}

void JITLinker :: prepareCompiler()
{
   References      references(RefInfo(0, NULL));
   ReferenceHelper helper(this, NULL, &references);

   _compiler->prepareCore(helper, _loader);

   // fix not loaded references
   fixReferences(references, _loader->getTargetSection((ref_t)mskCodeRef));
}
