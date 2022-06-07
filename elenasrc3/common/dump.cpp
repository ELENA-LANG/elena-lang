//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the implementation of ELENA Engine Data Section
//		classes.
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "common.h"
//---------------------------------------------------------------------------
#include "dump.h"

using namespace elena_lang;

constexpr auto SECTION_PAGE_SIZE = 0x0040;            // the section page size, should be aligned to power of two

// --- MemoryDump ---

MemoryDump :: MemoryDump()
{
   _used = 0;
   _total = SECTION_PAGE_SIZE;

   _buffer = realloc(nullptr, SECTION_PAGE_SIZE);
}

MemoryDump :: MemoryDump(pos_t capacity)
{
   _used = 0;
   _total = capacity;

   _buffer = realloc(nullptr, capacity);
}

MemoryDump :: MemoryDump(const MemoryDump& copy)
{
   _buffer = realloc(nullptr, copy._total);
   if (_buffer) {
      memcpy(_buffer, copy._buffer, copy._used);
      _used = copy._used;
      _total = copy._total;
   }
   else {
      _used = _total = 0;
   }
}

void MemoryDump :: reserve(pos_t size)
{
   if (size > _total) {
      const pos_t newSize = align(size, SECTION_PAGE_SIZE);     
      void* newBuffer = realloc(_buffer, newSize);
      if (newBuffer) {
         _buffer = newBuffer;
         _total = newSize;
      }
   }
}

void MemoryDump :: resize(pos_t size)
{
   if (size > _used) {
      _used = size;

      reserve(size);
   }
}

void* MemoryDump :: get(pos_t position) const
{
   if (position < _used) {
      return (char*)_buffer + position;
   }
   else return nullptr;
}

bool MemoryDump :: write(pos_t position, const void* s, pos_t length)
{
   if (position <= _used) {
      resize(position + length);

      memcpy(static_cast<char*>(_buffer) + position, s, length);

      return true;
   }
   else return false;

}

void MemoryDump :: insert(pos_t position, const void* s, pos_t length)
{
   if (position <= _used) {
      resize(_used + length);

      memmove(get(position + length), get(position), _used - position - length);
      if (s != nullptr)
         memcpy(get(position), s, length);
   }
}

bool MemoryDump :: read(pos_t position, void* s, pos_t length)
{
   if (position < _used && _used >= position + length) {
      memcpy(s, static_cast<char*>(_buffer) + position, length);

      return true;
   }
   else return false;
}

void MemoryDump :: load(StreamReader& reader, pos_t length)
{
   reserve(_used + length);

   reader.read(static_cast<char*>(_buffer) + _used, length);

   _used += length;
}
