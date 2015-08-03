//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler Engine
//
//		This file contains ELENA JIT compiler class implementation.
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "jitcompiler.h"

using namespace _ELENA_;

// --- ELENA Class constants ---
const int elVMTCountOffset32      = 0x000C;           // a VMT size offset
const int elVMTOffset32           = 0x0010;           // a VMT header size , NOTE - it should be aligned to VA_ALIGNMENT

inline void insertVMTEntry(VMTEntry* entries, int count, int index)
{
   for (int i = count ; i > index ; i--) {
      entries[i] = entries[i-1];
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
   writer->writeDWord(-4);
   writer->writeDWord(0);

   // object body
   writer->writeDWord(integer);
}

void JITCompiler32 :: compileInt64(MemoryWriter* writer, long long integer)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(-4);
   writer->writeDWord(0);

   // object body
   writer->write(&integer, 8);
}

void JITCompiler32 :: compileReal64(MemoryWriter* writer, double number)
{
   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(-4);
   writer->writeDWord(0);

   // object body
   writer->write(&number, 8);
}

void JITCompiler32 :: compileLiteral(MemoryWriter* writer, const char* value)
{
   int length = getlength(value) + 1;

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(-length);
   writer->writeDWord(0);

   // object body
   writer->writeLiteral(value, length);
   writer->align(4, 0);
}

void JITCompiler32 :: compileChar32(MemoryWriter* writer, const char* value)
{
   size_t len = 1;
   unic_c ch = 0;
   StringHelper::copy(&ch, value, getlength(value), len);

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(-4);
   writer->writeDWord(0);

   // object body
   writer->writeDWord(ch);
}

void JITCompiler32 :: compileBinary(MemoryWriter* writer, _Memory* binary)
{
   size_t length = binary->Length();

   writer->seek(writer->Position() - 8);

   // object header
   writer->writeDWord(-(int)length);
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

int JITCompiler32 :: findMethodAddress(void* refVMT, size_t message, size_t count)
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

   // return the method address
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
   //   type
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

void JITCompiler32 :: addVMTEntry(_ReferenceHelper& helper, ref_t message, size_t codePosition, VMTEntry* entries, size_t& entryCount)
{
   size_t index = 0;

   // if message has subject it should be resolved
   //if (message > MESSAGE_MASK) {
      message = helper.resolveMessage(message);
   //}

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

void JITCompiler32 :: fixVMT(MemoryWriter& vmtWriter, void* classClassVAddress, int count, bool virtualMode)
{
   _Memory* image = vmtWriter.Memory();   

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
   