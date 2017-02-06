//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class implementation.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitcompiler.h"

#pragma warning(disable : 4100)

using namespace _ELENA_;

// --- ELENA Class constants ---
const int elVMTCountOffset32      = 0x000C;           // a VMT size offset
const int elVMTCountOffset64      = 0x0018;           // a VMTX size offset

inline void insertVMTEntry(VMTEntry* entries, int count, int index)
{
   for (int i = count ; i > index ; i--) {
      entries[i] = entries[i-1];
   }
}

inline void insertVMTXEntry(VMTXEntry* entries, int count, int index)
{
   for (int i = count; i > index; i--) {
      entries[i] = entries[i - 1];
   }
}

// --- _JITCompiler ---

void _JITCompiler :: compileSymbol(_ReferenceHelper& helper, MemoryReader& reader, MemoryWriter& codeWriter)
{
   compileProcedure(helper, reader, codeWriter);
}

// --- JITCompiler32 ---

void JITCompiler32 :: compileInt32(MemoryWriter* writer, int integer)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800004);
   writer->writeDWord(0);

   // object body
   writer->writeDWord(integer);
}

void JITCompiler32 :: compileInt64(MemoryWriter* writer, long long integer)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800008);
   writer->writeDWord(0);

   // object body
   writer->write(&integer, 8u);
}

void JITCompiler32 :: compileInt64(MemoryWriter* writer, int low, ref_t ref, int refOffset)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800008);
   writer->writeDWord(0);

   // object body
   writer->writeDWord(low);
   writer->writeRef(ref, refOffset);
}

void JITCompiler32::compileInt64(MemoryWriter* writer, int low, int high)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800008);
   writer->writeDWord(0);

   // object body
   writer->writeDWord(low);
   writer->writeDWord(high);
}

void JITCompiler32 :: compileReal64(MemoryWriter* writer, double number)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800008);
   writer->writeDWord(0);

   // object body
   writer->write(&number, 8u);
}

void JITCompiler32 :: compileLiteral(MemoryWriter* writer, const char* value)
{
   size_t length = getlength(value) + 1;

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800000 | length);
   writer->writeDWord(0);

   // object body
   writer->writeLiteral(value, length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileWideLiteral(MemoryWriter* writer, const wide_c* value)
{
   size_t length = (getlength(value) + 1) << 1;

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800000 | length);
   writer->writeDWord(0);

   // object body
   writer->writeLiteral(value, length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileChar32(MemoryWriter* writer, const char* value)
{
   size_t len = 1;
   unic_c ch = 0;
   Convertor::copy(&ch, value, getlength(value), len);

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800004);
   writer->writeDWord(0);

   // object body
   writer->writeDWord(ch);
}

void JITCompiler32 :: compileBinary(MemoryWriter* writer, _Memory* binary)
{
   size_t length = binary->Length();

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(0x800000 | length);
   writer->writeDWord(0);

   // object body
   writer->write(binary->get(0), length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileCollection(MemoryWriter* writer, _Memory* binary)
{
   size_t length = binary->Length();

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(length);
   writer->writeDWord(0);

   // object body
   writer->write(binary->get(0), length);
   writer->align(4, 0);
}

size_t JITCompiler32 :: findFlags(void* refVMT)
{
   return *(int*)((ref_t)refVMT - 0x08);  // !! explicit constant
}

size_t JITCompiler32 :: findLength(void* refVMT)
{
   int count = *(int*)((int)refVMT - elVMTCountOffset32);
   return count;
}

pos_t JITCompiler32 :: findMethodAddress(void* refVMT, size_t message, size_t count)
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

int JITCompiler32 :: findMethodIndex(void* refVMT, ref_t message, size_t count)
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

int JITCompiler32 :: allocateConstant(MemoryWriter& writer, size_t objectOffset)
{
   writer.writeBytes(0, objectOffset);

   alignCode(&writer, VA_ALIGNMENT, false);

   return writer.Position() - 4;
}

void JITCompiler32 :: allocateVariable(MemoryWriter& writer)
{
   writer.writeDWord(0);
}

void JITCompiler32 :: allocateArray(MemoryWriter& writer, size_t count)
{
   writer.writeBytes(0, count * 4);
}

void JITCompiler32 :: allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength)
{
   alignCode(&vmtWriter, VA_ALIGNMENT, false);   

   // create VMT header:
   //   dummy package reference
   vmtWriter.writeDWord(0);

   //   vmt length
   vmtWriter.writeDWord(vmtLength);

   //   vmt flags
   vmtWriter.writeDWord(flags);

   //   dummy class reference
   vmtWriter.writeDWord(0);

   int position = vmtWriter.Position();

   size_t vmtSize = 0;
   if (test(flags, elStandartVMT)) {
      // + VMT length
      vmtSize = vmtLength * sizeof(VMTEntry);
   }

   vmtWriter.writeBytes(0, vmtSize);

   vmtWriter.seek(position);
}

int JITCompiler32 :: copyParentVMT(void* parentVMT, VMTEntry* entries)
{
   if (parentVMT != NULL) {
      // get the parent vmt size
      int count = *(int*)((int)parentVMT - elVMTCountOffset32);

      // get the parent entry array
      VMTEntry* parentEntries = (VMTEntry*)parentVMT;

      // copy parent VMT
      for(int i = 0 ; i < count ; i++) {
         entries[i] = parentEntries[i];
      }

      return count;
   }
   else return 0;
}

void JITCompiler32 :: addVMTEntry(ref_t message, size_t codePosition, VMTEntry* entries, size_t& entryCount)
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

void JITCompiler32 :: fixVMT(MemoryWriter& vmtWriter, pos_t classClassVAddress, pos_t packageVAddress, int count, bool virtualMode)
{
   _Memory* image = vmtWriter.Memory();   

   // update class package reference if available
   if (packageVAddress != NULL) {
      int position = vmtWriter.Position();
      vmtWriter.seek(position - 0x10);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)packageVAddress, 0);
      }
      else vmtWriter.writeDWord((int)packageVAddress);

      vmtWriter.seek(position);
   }

   // update class vmt reference if available
   if (classClassVAddress != NULL) {
      vmtWriter.seek(vmtWriter.Position() - 4);

      if (virtualMode) {                                  
         vmtWriter.writeRef((ref_t)classClassVAddress, 0);
      }
      else vmtWriter.writeDWord((int)classClassVAddress);
   }

   // if in virtual mode mark method addresses as reference
   if (virtualMode) {
      ref_t entryPosition = vmtWriter.Position();
      for (int i = 0 ; i < count ; i++) {
         image->addReference(mskCodeRef, entryPosition + 4);
      
         entryPosition += 8;
      }
   }
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

void JITCompiler64 :: compileInt32(MemoryWriter* writer, int integer)
{
   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000004u);
   writer->writeQWord(0);

   // object body
   writer->writeQWord(integer);
}

void JITCompiler64 :: compileInt64(MemoryWriter* writer, long long integer)
{
   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000008u);
   writer->writeQWord(0);

   // object body
   writer->write(&integer, 8);
}

void JITCompiler64 :: compileInt64(MemoryWriter* writer, int low, ref_t ref, int refOffset)
{
   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000008u);
   writer->writeQWord(0);

   // object body
   writer->writeDWord(low);
   writer->writeRef(ref, refOffset);
}

void JITCompiler64 :: compileInt64(MemoryWriter* writer, int low, int high)
{
   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000008u);
   writer->writeQWord(0);

   // object body
   writer->writeDWord(low);
   writer->writeDWord(high);
}

void JITCompiler64 :: compileReal64(MemoryWriter* writer, double number)
{
   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000008u);
   writer->writeQWord(0);

   // object body
   writer->write(&number, 8);
}

void JITCompiler64 :: compileLiteral(MemoryWriter* writer, const char* value)
{
   unsigned int length = getlength(value) + 1;

   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000000u | length);
   writer->writeQWord(0);

   // object body
   writer->writeLiteral(value, length);
   writer->align(8, 0);
}

void JITCompiler64 :: compileWideLiteral(MemoryWriter* writer, const wide_c* value)
{
   unsigned int length = (getlength(value) + 1) << 1;

   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000000u | length);
   writer->writeQWord(0);

   // object body
   writer->writeLiteral(value, length);
   writer->align(8, 0);
}

void JITCompiler64 :: compileChar32(MemoryWriter* writer, const char* value)
{
   size_t len = 1;
   unic_c ch = 0;
   Convertor::copy(&ch, value, getlength(value), len);

   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000004u);
   writer->writeQWord(0);

   // object body
   writer->writeQWord(ch);
}

void JITCompiler64 :: compileBinary(MemoryWriter* writer, _Memory* binary)
{
   unsigned int length = binary->Length();

   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(0x80000000u | length);
   writer->writeQWord(0);

   // object body
   writer->write(binary->get(0), length);
   writer->align(8, 0);
}

void JITCompiler64 :: compileCollection(MemoryWriter* writer, _Memory* binary)
{
   unsigned int length = binary->Length();

   writer->seek(writer->Position() - 0x10);

   // object header
   writer->writeQWord(length);
   writer->writeQWord(0);

   // object body
   writer->write(binary->get(0), length);
   writer->align(8, 0);
}

size_t JITCompiler64 :: findFlags(void* refVMT)
{
   return *(int*)((ref_t)refVMT - 0x10);  // !! explicit constant
}

size_t JITCompiler64 :: findLength(void* refVMT)
{
   ref64_t count = *(int*)((int)refVMT - elVMTCountOffset64);
   return (size_t)count;
}

pos_t JITCompiler64 :: findMethodAddress(void* refVMT, size_t message, size_t count)
{
   ref64_t address = findMethodAddressX(refVMT, toMessage64(message), count);

   if (address < 10000000000000000ull) {
      return (pos_t)address;
   }
   else throw InternalError("Addresses bigger than 4GB are not supported");
}

ref64_t JITCompiler64 :: findMethodAddressX(void* refVMT, ref64_t messageID, size_t count)
{
   VMTXEntry* entries = (VMTXEntry*)refVMT;

   // search for the message entry
   size_t i = 0;
   while (i < count && entries[i].message != messageID) {
      i++;
   }

   // return the method address
   // if the vmt entry was not resolved, SEND_MESSAGE routine should be used (the first method entry)
   return (i < count) ? entries[i].address : entries[0].address;
}

int JITCompiler64::findMethodIndex(void* refVMT, size_t message, size_t count)
{
   return findMethodIndexX(refVMT, toMessage64(message), count);
}

int JITCompiler64::findMethodIndexX(void* refVMT, ref64_t messageID, size_t count)
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

int JITCompiler64 :: allocateConstant(MemoryWriter& writer, size_t objectOffset)
{
   writer.writeBytes(0, objectOffset);

   alignCode(&writer, VA_ALIGNMENT, false);

   return writer.Position() - 8;
}

void JITCompiler64 :: allocateVariable(MemoryWriter& writer)
{
   writer.writeQWord(0);
}

void JITCompiler64 :: allocateArray(MemoryWriter& writer, size_t count)
{
   writer.writeBytes(0, count * 8);
}

void JITCompiler64 :: allocateVMT(MemoryWriter& vmtWriter, size_t flags, size_t vmtLength)
{
   alignCode(&vmtWriter, VA_ALIGNMENT, false);

   // create VMT header:
   //   dummy package reference
   vmtWriter.writeQWord(0);

   //   vmt length
   vmtWriter.writeQWord(vmtLength);

   //   vmt flags
   vmtWriter.writeQWord((flags &  ~elStandartVMT) | elExtendedVMT);

   //   dummy class reference
   vmtWriter.writeQWord(0);

   int position = vmtWriter.Position();

   size_t vmtSize = 0;
   if (test(flags, elStandartVMT)) {
      // + VMT length
      vmtSize = vmtLength * sizeof(VMTXEntry);
   }
   
   vmtWriter.writeBytes(0, vmtSize);

   vmtWriter.seek(position);
}

int JITCompiler64 :: copyParentVMT(void* parentVMT, VMTEntry* entries)
{
   //HOTFIX : 64bit compiler supports only VMTX
   return copyParentVMTX(parentVMT, (VMTXEntry*)entries);
}

int JITCompiler64 :: copyParentVMTX(void* parentVMT, VMTXEntry* entries)
{
   if (parentVMT != NULL) {
      // get the parent vmt size
      int count = *(int*)((int)parentVMT - elVMTCountOffset64);

      // get the parent entry array
      VMTXEntry* parentEntries = (VMTXEntry*)parentVMT;

      // copy parent VMT
      for (int i = 0; i < count; i++) {
         entries[i] = parentEntries[i];
      }

      return count;
   }
   else return 0;
}

void JITCompiler64 :: addVMTEntry(ref_t message, size_t codePosition, VMTEntry* entries, size_t& entryCount)
{
   // HOTFIX : 64bit compiler supports only VMTX
   addVMTXEntry(toMessage64(message), codePosition, (VMTXEntry*)entries, entryCount);
}

void JITCompiler64 :: addVMTXEntry(ref64_t message, size_t codePosition, VMTXEntry* entries, size_t& entryCount)
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

void JITCompiler64 :: fixVMT(MemoryWriter& vmtWriter, pos_t classClassVAddress, pos_t packageVAddress, int count, bool virtualMode)
{
   _Memory* image = vmtWriter.Memory();

   // update class package reference if available
   if (packageVAddress != NULL) {
      int position = vmtWriter.Position();
      vmtWriter.seek(position - 0x20);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)packageVAddress, 0);
      }
      else vmtWriter.writeDWord(packageVAddress);

      vmtWriter.seek(position);
   }

   // update class vmt reference if available
   if (classClassVAddress != NULL) {
      vmtWriter.seek(vmtWriter.Position() - 8);

      if (virtualMode) {
         vmtWriter.writeRef((ref_t)classClassVAddress, 0);
      }
      else vmtWriter.writeDWord(classClassVAddress);
   }

   // if in virtual mode mark method addresses as reference
   if (virtualMode) {
      ref_t entryPosition = vmtWriter.Position();
      for (int i = 0; i < count; i++) {
         image->addReference(mskCodeRef, entryPosition + 8);

         entryPosition += 16;
      }
   }
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
