//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitcompiler.h"
#include "core.h"

#pragma warning(disable : 4100)

using namespace _ELENA_;

inline void insertVMTEntry(VMTEntry* entries, size_t count, pos_t index)
{
   for (pos_t i = count ; i > index ; i--) {
      entries[i] = entries[i-1];
   }
}

inline void insertVMTXEntry(VMTXEntry* entries, size_t count, pos_t index)
{
   for (pos_t i = count; i > index; i--) {
      entries[i] = entries[i - 1];
   }
}

// --- _JITCompiler ---

void _JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter)
{
   compileProcedure(helper, reader, codeWriter);
}

// --- JITCompiler32 ---

void JITCompiler32 :: allocateMetaInfo(_Module* messages)
{
   messages->mapSection(messages->mapReference(MATTRIBUTE_TABLE + getlength(META_MODULE)) | mskRDataRef, false);
   messages->mapSection(messages->mapReference(MESSAGE_TABLE + getlength(META_MODULE)) | mskRDataRef, false)->writeBytes(0, 0, 8); // write dummy place holder
   messages->mapSection(messages->mapReference(MESSAGEBODY_TABLE + getlength(META_MODULE)) | mskRDataRef, false)->writeBytes(0, 0, 4); // write dummy place holder
}

void JITCompiler32 :: compileInt32(MemoryWriter* writer, int integer)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800004);

   // object body
   writer->writeDWord(integer);
}

void JITCompiler32 :: compileMessage(MemoryWriter* writer, mssg_t mssg)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800004);

   // object body
   writer->writeDWord(mssg);
}

void JITCompiler32 :: compileAction(MemoryWriter* writer, ref_t action)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800004);

   // object body
   writer->writeDWord(action);
}

void JITCompiler32 :: compileInt64(MemoryWriter* writer, long long integer)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800008);

   // object body
   writer->write(&integer, 8u);
}

void JITCompiler32 :: compileMssgExtension(MemoryWriter* writer, mssg_t mssg, ref_t ref, int refOffset)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800008);

   // object body
   writer->writeDWord(mssg);
   writer->writeRef(ref, refOffset);
}

void JITCompiler32 :: compileMssgExtension(MemoryWriter* writer, mssg_t mssg, uintptr_t addr)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800008);

   // object body
   writer->writeDWord(mssg);
   writer->writeDWord(addr);
}

void JITCompiler32 :: compileReal64(MemoryWriter* writer, double number)
{
   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800008);

   // object body
   writer->write(&number, 8u);
}

void JITCompiler32 :: compileLiteral(MemoryWriter* writer, const char* value)
{
   size_t length = getlength(value) + 1;

   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800000 | length);

   // object body
   writer->writeLiteral(value, length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileWideLiteral(MemoryWriter* writer, const wide_c* value)
{
   size_t length = (getlength(value) + 1) << 1;

   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800000 | length);

   // object body
   writer->writeLiteral(value, length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileChar32(MemoryWriter* writer, const char* value)
{
   size_t len = 1;
   unic_c ch = 0;
   Convertor::copy(&ch, value, getlength(value), len);

   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800004);

   // object body
   writer->writeDWord(ch);
}

void JITCompiler32 :: compileBinary(MemoryWriter* writer, _Memory* binary)
{
   size_t length = binary->Length();

   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(0x800000 | length);

   // object body
   writer->write(binary->get(0), length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileCollection(MemoryWriter* writer, _Memory* binary)
{
   size_t length = binary->Length();

   writer->seek(writer->Position() - elObjectOffset32);

   // object header
   writer->writeDWord(0);
   writer->writeDWord(length);

   // object body
   writer->write(binary->get(0), length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileMAttribute(MemoryWriter& writer, int category, ident_t fullName, lvaddr_t address, bool virtualMode)
{
   writer.writeDWord(category);
   writer.writeDWord(getlength(fullName) + 9);
   writer.writeLiteral(fullName);

   if (!virtualMode) {
      writer.writeDWord(address);
   }
   else writer.writeRef((ref_t)address, 0);
}

lvaddr_t JITCompiler32 :: findClassPtr(void* refVMT)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)refVMT - elVMTClassOffset32);

   return header->parentRef;
}

ref_t JITCompiler32 :: findFlags(void* refVMT)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)refVMT - elVMTClassOffset32);

   return header->flags;
}

size_t JITCompiler32 :: findLength(void* refVMT)
{
   VMTHeader* header = (VMTHeader*)((uintptr_t)refVMT - elVMTClassOffset32);

   return header->count;
}

lvaddr_t JITCompiler32 :: findMethodAddress(void* refVMT, mssg_t message, size_t count)
{
   VMTEntry* entries = (VMTEntry*)refVMT;

   // search for the message entry
   size_t i = 0;
   while (i < count && entries[i].message != message) {
      i++;
   }

   // return the method address
   // if the vmt entry was not resolved, SEND_MESSAGE routine should be used (the first method entry)
   return (i < count) ? entries[i].address : entries[0].address;
}

pos_t JITCompiler32 :: findMethodIndex(void* refVMT, mssg_t message, size_t count)
{
   VMTEntry* entries = (VMTEntry*)refVMT;

   // search for the message entry
   size_t i = 0;
   while (i < count && entries[i].message != message) {
      i++;
   }

   // return the method index
   // if the vmt entry was not resolved, SEND_MESSAGE index should be used (the first method entry)
   return (i < count) ? i : 0;
}

pos_t JITCompiler32 :: allocateConstant(MemoryWriter& writer, size_t objectOffset)
{
   writer.writeBytes(0, objectOffset);

   alignCode(&writer, VA_ALIGNMENT, false);

   return writer.Position() - elPageVMTOffset32;
}

void JITCompiler32 :: allocateVariable(MemoryWriter& writer)
{
   writer.writeDWord(0);
}

void JITCompiler32 :: allocateArray(MemoryWriter& writer, size_t count)
{
   writer.writeBytes(0, count * 4);
}

void JITCompiler32 :: allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, pos_t staticSize)
{
   // create VMT static table
   vmtWriter.writeBytes(0, staticSize << 2);

   alignCode(&vmtWriter, VA_ALIGNMENT, false);

   // create VMT header:
   VMTHeader header;
   //   dummy parent reference
   header.parentRef = 0;
   header.flags = flags;
   //   dummy class reference
   header.classRef = 0;
   header.count = vmtLength;

   vmtWriter.write(&header, sizeof(VMTHeader));

   int position = vmtWriter.Position();

   size_t vmtSize = 0;
   if (test(flags, elStandartVMT)) {
      // + VMT length
      vmtSize = vmtLength * sizeof(VMTEntry);
   }

   vmtWriter.writeBytes(0, vmtSize);

   vmtWriter.seek(position);
}

pos_t JITCompiler32 :: copyParentVMT(void* parentVMT, VMTEntry* entries)
{
   if (parentVMT != NULL) {
      // get the parent vmt size
      VMTHeader* header = (VMTHeader*)((uintptr_t)parentVMT - elVMTClassOffset32);

      // get the parent entry array
      VMTEntry* parentEntries = (VMTEntry*)parentVMT;

      // copy parent VMT
      for(pos_t i = 0 ; i < header->count ; i++) {
         entries[i] = parentEntries[i];
      }

      return header->count;
   }
   else return 0;
}

void JITCompiler32 :: addVMTEntry(mssg_t message, lvaddr_t codePosition, VMTEntry* entries, pos_t& entryCount)
{
   size_t index = 0;

   // find the message entry
   while (index < entryCount && (entries[index].message < message))
      index++;

   if(index < entryCount) {
      if (entries[index].message != message) {
         insertVMTEntry(entries, entryCount, index);
         entryCount++;
      }
   }
   else entryCount++;

   entries[index].message = message;
   entries[index].address = codePosition;
}

void JITCompiler32 :: fixVMT(MemoryWriter& vmtWriter, lvaddr_t classClassVAddress, lvaddr_t parentVAddress, pos_t count,
   bool virtualMode, bool abstractMode)
{
   _Memory* image = vmtWriter.Memory();

   // update class package reference if available
   pos_t position = vmtWriter.Position();
   if (parentVAddress != 0) {
      vmtWriter.seek(position - 0x10);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)parentVAddress, 0);
      }
      else vmtWriter.writeDWord((pos_t)parentVAddress);

      vmtWriter.seek(position);
   }

   // update class vmt reference if available
   if (classClassVAddress != 0) {
      vmtWriter.seek(vmtWriter.Position() - elPageVMTOffset32);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)classClassVAddress, 0);
      }
      else vmtWriter.writeDWord((pos_t)classClassVAddress);

      vmtWriter.seek(position);
   }

   // if in virtual mode mark method addresses as reference
   if (virtualMode) {
      pos_t entryPosition = vmtWriter.Position();
      if (abstractMode) {
         // HOTFIX : skip abstract methods in abstract mode
         for (size_t i = 0; i < count; i++) {
            if ((*image)[entryPosition + 4])
               image->addReference(mskCodeRef, entryPosition + 4);

            entryPosition += 8;
         }
      }
      else {
         for (size_t i = 0; i < count; i++) {
            image->addReference(mskCodeRef, entryPosition + 4);

            entryPosition += 8;
         }

      }
   }
}

ref_t JITCompiler32 :: allocateActionEntry(MemoryWriter& mdataWriter, MemoryWriter& bodyWriter, ident_t actionName,
   ref_t weakActionRef, ref_t signature)
{
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
      mdataWriter.writeRef(mskMessageTableRef | bodyWriter.Position(), 0);

      bodyWriter.writeLiteral(actionName, getlength(actionName) + 1);
      bodyWriter.align(4, 0);
   }

   return actionRef;
}

void JITCompiler32 :: allocateSignatureEntry(MemoryWriter& writer, ref_t typeClassRef)
{
   if (typeClassRef) {
      writer.writeRef(typeClassRef | mskVMTRef, 0);
   }
   else writer.writeDWord(0);
}

void JITCompiler32 :: generateProgramStart(MemoryDump& tape)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeDWord(0);            // write size place holder
}

void JITCompiler32 :: generateProgramEnd(MemoryDump& tape)
{
   MemoryWriter ecodes(&tape);

   tape[0] = ecodes.Position() - 4;
}

// --- JITCompiler64 ---

void JITCompiler64 :: allocateMetaInfo(_Module* messages)
{
   messages->mapSection(messages->mapReference(MATTRIBUTE_TABLE + getlength(META_MODULE)) | mskRDataRef, false);
   messages->mapSection(messages->mapReference(MESSAGE_TABLE + getlength(META_MODULE)) | mskRDataRef, false)->writeBytes(0, 0, 16); // write dummy place holder
   messages->mapSection(messages->mapReference(MESSAGEBODY_TABLE + getlength(META_MODULE)) | mskRDataRef, false)->writeBytes(0, 0, 8); // write dummy place holder
}

void JITCompiler64 :: compileInt32(MemoryWriter* writer, int integer)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000004u);

   // object body
   writer->writeDWord(integer);
}

void JITCompiler64 :: compileMessage(MemoryWriter* writer, mssg_t mssg)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000008u);

   // object body
   writer->writeQWord(toMessage64(mssg));
}

void JITCompiler64 :: compileAction(MemoryWriter* writer, ref_t integer)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000008u);

   // object body
   writer->writeQWord(integer);
}

void JITCompiler64 :: compileInt64(MemoryWriter* writer, long long integer)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000008u);

   // object body
   writer->write(&integer, 8);
}

void JITCompiler64 :: compileMssgExtension(MemoryWriter* writer, mssg_t mssg, ref_t ref, int refOffset)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000010u);

   // object body
   writer->writeQWord(toMessage64(mssg));
   writer->writeRef(ref, refOffset);
   writer->writeDWord(0);
}

void JITCompiler64 ::compileMssgExtension(MemoryWriter* writer, mssg_t mssg, uintptr_t high)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000010u);

   // object body
   writer->writeQWord(toMessage64(mssg));
   writer->writeQWord(high);
}

void JITCompiler64 :: compileReal64(MemoryWriter* writer, double number)
{
   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000008u);

   // object body
   writer->write(&number, 8);
}

void JITCompiler64 :: compileLiteral(MemoryWriter* writer, const char* value)
{
   unsigned int length = getlength(value) + 1;

   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000000u | length);

   // object body
   writer->writeLiteral(value, length);
   writer->align(8, 0);
}

void JITCompiler64 :: compileWideLiteral(MemoryWriter* writer, const wide_c* value)
{
   unsigned int length = (getlength(value) + 1) << 1;

   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000000u | length);

   // object body
   writer->writeLiteral(value, length);
   writer->align(8, 0);
}

void JITCompiler64 :: compileChar32(MemoryWriter* writer, const char* value)
{
   size_t len = 1;
   unic_c ch = 0;
   Convertor::copy(&ch, value, getlength(value), len);

   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000004u);

   // object body
   writer->writeDWord(ch);
}

void JITCompiler64 :: compileBinary(MemoryWriter* writer, _Memory* binary)
{
   unsigned int length = binary->Length();

   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(0x40000000u | length);

   // object body
   writer->write(binary->get(0), length);
   writer->align(8, 0);
}

void JITCompiler64 :: compileCollection(MemoryWriter* writer, _Memory* binary)
{
   pos_t length = binary->Length();

   writer->seek(writer->Position() - elObjectOffset64);

   // object header
   writer->writeQWord(0);
   writer->writeDWord(0);
   writer->writeDWord(length << 1);

   // object body
   pos_t index = 0;
   while (index < length) {
      writer->writeQWord((*binary)[index]);
      index += 4;
   }
   writer->align(8, 0);
}

void JITCompiler64 :: compileMAttribute(MemoryWriter& writer, int category, ident_t fullName, lvaddr_t address, bool virtualMode)
{
   writer.writeDWord(category);
   writer.writeDWord(getlength(fullName) + 9);
   writer.writeLiteral(fullName);

   if (virtualMode) {
      writer.writeRef((ref_t)address, 0);
      writer.writeDWord(0);
   }
   else writer.writeQWord(address);
}

ref_t JITCompiler64 :: findFlags(void* refVMT)
{
   VMTXHeader* header = (VMTXHeader*)((uintptr_t)refVMT - elVMTClassOffset64);

   return (ref_t)header->flags;
}

size_t JITCompiler64 :: findLength(void* refVMT)
{
   VMTXHeader* header = (VMTXHeader*)((uintptr_t)refVMT - elVMTClassOffset64);

   return (size_t)header->count;
}

lvaddr_t JITCompiler64 :: findClassPtr(void* refVMT)
{
   VMTXHeader* header = (VMTXHeader*)((uintptr_t)refVMT - elVMTClassOffset64);

   return (lvaddr_t)header->parentRef;
}

lvaddr_t JITCompiler64 :: findMethodAddress(void* refVMT, mssg_t message, size_t count)
{
   return findMethodAddressX(refVMT, toMessage64(message), count);
}

lvaddr_t JITCompiler64 :: findMethodAddressX(void* refVMT, mssg64_t messageID, size_t count)
{
   VMTXEntry* entries = (VMTXEntry*)refVMT;

   // search for the message entry
   size_t i = 0;
   while (i < count && entries[i].message != messageID) {
      i++;
   }

   // return the method address
   // if the vmt entry was not resolved, SEND_MESSAGE routine should be used (the first method entry)
   return (i < count) ? (lvaddr_t)entries[i].address : (lvaddr_t)entries[0].address;
}

pos_t JITCompiler64 :: findMethodIndex(void* refVMT, mssg_t message, size_t count)
{
   return findMethodIndexX(refVMT, toMessage64(message), count);
}

pos_t JITCompiler64 :: findMethodIndexX(void* refVMT, mssg64_t messageID, size_t count)
{
   VMTXEntry* entries = (VMTXEntry*)refVMT;

   // search for the message entry
   size_t i = 0;
   while (i < count && entries[i].message != messageID) {
      i++;
   }

   // return the method index
   // if the vmt entry was not resolved, SEND_MESSAGE index should be used (the first method entry)
   return (i < count) ? i : 0;
}

pos_t JITCompiler64 :: allocateConstant(MemoryWriter& writer, size_t objectOffset)
{
   writer.writeBytes(0, objectOffset);

   alignCode(&writer, VA_ALIGNMENT, false);

   return writer.Position() - elObjectOffset64;
}

void JITCompiler64 :: allocateVariable(MemoryWriter& writer)
{
   writer.writeQWord(0);
}

void JITCompiler64 :: allocateArray(MemoryWriter& writer, size_t count)
{
   writer.writeBytes(0, count * 8);
}

void JITCompiler64 :: allocateVMT(MemoryWriter& vmtWriter, pos_t flags, pos_t vmtLength, pos_t staticSize)
{
   // create VMT static table
   vmtWriter.writeBytes(0, staticSize << 3);

   alignCode(&vmtWriter, VA_ALIGNMENT, false);

   // create VMT header:
   VMTXHeader header;
   //   dummy parent reference
   header.parentRef = 0;
   header.flags = flags;
   //   dummy class reference
   header.classRef = 0;
   header.count = vmtLength;

   vmtWriter.write(&header, sizeof(VMTXHeader));

   ////   vmt flags
   //vmtWriter.writeQWord((flags &  ~elStandartVMT) | elExtendedVMT);

   int position = vmtWriter.Position();

   size_t vmtSize = 0;
   if (test(flags, elStandartVMT)) {
      // + VMT length
      vmtSize = vmtLength * sizeof(VMTXEntry);
   }

   vmtWriter.writeBytes(0, vmtSize);

   vmtWriter.seek(position);
}

pos_t JITCompiler64 :: copyParentVMT(void* parentVMT, VMTEntry* entries)
{
   //HOTFIX : 64bit compiler supports only VMTX
   return copyParentVMTX(parentVMT, (VMTXEntry*)entries);
}

pos_t JITCompiler64 :: copyParentVMTX(void* parentVMT, VMTXEntry* entries)
{
   if (parentVMT != NULL) {
      VMTXHeader* header = (VMTXHeader*)((uintptr_t)parentVMT - elVMTClassOffset64);

      // get the parent entry array
      VMTXEntry* parentEntries = (VMTXEntry*)parentVMT;

      // copy parent VMT
      for (pos_t i = 0; i < header->count; i++) {
         entries[i] = parentEntries[i];
      }

      return (pos_t)header->count;
   }
   else return 0;
}

void JITCompiler64 :: addVMTEntry(mssg_t message, lvaddr_t codePosition, VMTEntry* entries, pos_t& entryCount)
{
   // HOTFIX : 64bit compiler supports only VMTX
   addVMTXEntry(toMessage64(message), codePosition, (VMTXEntry*)entries, entryCount);
}

void JITCompiler64 :: addVMTXEntry(mssg64_t message, lvaddr_t codePosition, VMTXEntry* entries, pos_t& entryCount)
{
   size_t index = 0;

   // find the message entry
   while (index < entryCount && (entries[index].message < message))
      index++;

   if (index < entryCount) {
      if (entries[index].message != message) {
         insertVMTXEntry(entries, entryCount, index);
         entryCount++;
      }
   }
   else entryCount++;

   entries[index].message = message;
   entries[index].address = codePosition;
}

void JITCompiler64 :: fixVMT(MemoryWriter& vmtWriter, lvaddr_t classClassVAddress, lvaddr_t parentVAddress,
   pos_t count, bool virtualMode, bool abstractMode)
{
   _Memory* image = vmtWriter.Memory();

   // update class package reference if available
   if (parentVAddress != NULL) {
      int position = vmtWriter.Position();
      vmtWriter.seek(position - elVMTClassOffset64);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)parentVAddress, 0);
      }
      else vmtWriter.writeQWord(parentVAddress);

      vmtWriter.seek(position);
   }

   // update class vmt reference if available
   if (classClassVAddress != NULL) {
      int position = vmtWriter.Position();
      vmtWriter.seek(vmtWriter.Position() - elPageVMTOffset64);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)classClassVAddress, 0);
      }
      else vmtWriter.writeQWord(classClassVAddress);

      vmtWriter.seek(position);
   }

   // if in virtual mode mark method addresses as reference
   if (virtualMode) {
      pos_t entryPosition = vmtWriter.Position();

      if (abstractMode) {
         // HOTFIX : skip abstract methods in abstract mode
         for (size_t i = 0; i < count; i++) {
            if ((*image)[entryPosition + 8])
               image->addReference(mskCodeRef, entryPosition + 8);

            entryPosition += 16;
         }
      }
      else {
         for (size_t i = 0; i < count; i++) {
            image->addReference(mskCodeRef, entryPosition + 8);

            entryPosition += 16;
         }
      }
   }
}

ref_t JITCompiler64 :: allocateActionEntry(MemoryWriter& mdataWriter, MemoryWriter& bodyWriter, ident_t actionName,
   ref_t weakActionRef, ref_t signature)
{
   ref_t actionRef = mdataWriter.Position() / 16;
   // weak action ref for strong one or the same ref
   if (weakActionRef) {
      mdataWriter.writeQWord(weakActionRef);
   }
   else mdataWriter.writeQWord(0);

   // signature or action name for weak message
   if (signature) {
      mdataWriter.writeRef(mskMessageTableRef | signature, 0);
      mdataWriter.writeDWord(0);
   }
   else {
      mdataWriter.writeRef(mskMessageTableRef | bodyWriter.Position(), 0);
      mdataWriter.writeDWord(0);

      bodyWriter.writeLiteral(actionName, getlength(actionName) + 1);
      bodyWriter.align(8, 0);
   }

   return actionRef;
}

void JITCompiler64 :: allocateSignatureEntry(MemoryWriter& writer, ref_t typeClassRef)
{
   if (typeClassRef) {
      writer.writeRef(typeClassRef | mskVMTRef, 0);
      writer.writeDWord(0);
   }
   else writer.writeQWord(0);
}

void JITCompiler64 :: generateProgramStart(MemoryDump& tape)
{
   MemoryWriter ecodes(&tape);

   ecodes.writeDWord(0);            // write size place holder
}

void JITCompiler64 :: generateProgramEnd(MemoryDump& tape)
{
   MemoryWriter ecodes(&tape);

   tape[0] = ecodes.Position() - 4;
}
