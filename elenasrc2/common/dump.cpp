//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the implementation of ELENA Engine Data Section
//		classes.
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "common.h"
//---------------------------------------------------------------------------
#include "dump.h"

using namespace _ELENA_;

// --- Constant definition ---
#define SECTION_PAGE_SIZE           0x0040            // the section page size, should be aligned to power of two

// --- MemoryDump ---

MemoryDump :: MemoryDump()
{
   _used = 0;
   _total = SECTION_PAGE_SIZE;

   _buffer = (char*)malloc(SECTION_PAGE_SIZE);
}

MemoryDump :: MemoryDump(size_t capacity)
{
   _used = 0;
   _total = capacity;

   _buffer = (capacity > 0) ? (char*)malloc(capacity) : NULL;
}

MemoryDump :: MemoryDump(MemoryDump& copy)
{
   _used = copy._used;
   _total = copy._total;

   _buffer = (char*)malloc(_total);
   memcpy(_buffer, copy._buffer, _total);
}

void* MemoryDump :: get(size_t position) const
{
   if (position < _used) {
      return _buffer + position;
   }
   else return NULL;
}

void MemoryDump :: reserve(size_t size)
{
   if (size > _total) {
      _total = align(size, SECTION_PAGE_SIZE);

      _buffer = (char*)realloc(_buffer, _total);
   }
}

void MemoryDump :: resize(size_t size)
{
   if (size > _used) {
      _used = size;

      reserve(size);
   }
}

bool MemoryDump :: write(size_t position, const void* s, size_t length)
{
   if (position <= _used) {
      resize(position + length);

      memcpy(_buffer + position, s, length);

      return true;
   }
   else return false;
}

bool MemoryDump :: writeBytes(size_t position, char value, size_t length)
{
   if (position <= _used && length > 0) {
      resize(position + length);

      memset(_buffer + position, value, length);

      return true;
   }
   else return false;
}

void MemoryDump :: insert(size_t position, const void* s, size_t length)
{
   if (position <= _used) {
      resize(_used + length);

      memmove(_buffer + position + length, _buffer + position, _used - position - length);
      memcpy(_buffer + position, s, length);
   }
}

bool MemoryDump :: read(size_t position, void* s, size_t length)
{
   if (position < _used && _used >= position + length) {
      memcpy(s, _buffer + position, length);

      return true;
   }
   else return false;
}

void MemoryDump :: load(StreamReader* reader, size_t length)
{
   reserve(_used + length);

   reader->read(_buffer + _used, length);

   _used += length;
}

// --- ByteArray ---

void* ByteArray :: get(size_t position) const
{
   return _bytes + position;
}

bool ByteArray :: read(size_t position, void* s, size_t length)
{
   memcpy(s, _bytes + position, length);

   return true;
}

bool ByteArray :: write(size_t position, const void* s, size_t length)
{
   memcpy(_bytes + position, s, length);

   return true;
}

void ByteArray :: insert(size_t position, const void* s, size_t length)
{
   memmove(_bytes + position + length, _bytes + position, _length - position - length);
   memcpy(_bytes + position, s, length);
}

bool ByteArray :: writeBytes(size_t position, char value, size_t length)
{
   memset(_bytes + position, value, length);

   return true;
}
