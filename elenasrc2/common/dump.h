//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains the declaration of ELENA Engine Data Memory dump
//		classes.
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef DumpH
#define DumpH 1

namespace _ELENA_
{

// --- MemoryDump class ---

class MemoryDump : public _Memory
{
protected:
   char* _buffer;
   pos_t _total;
   pos_t _used;

   void resize(pos_t size);

public:
   pos_t getFreeSpace() const { return _total - _used; }

   virtual pos_t Length() const { return _used; }

   virtual void* get(pos_t position) const;

   //virtual void* getLong(pos64_t position) const
   //{
   //   if (position < INT_MAX) {
   //      return get((pos_t)position);
   //   }
   //   else return nullptr;
   //}

   void reserve(pos_t size);

   virtual bool read(pos_t position, void* s, pos_t length);

   virtual bool readLong(pos64_t position, void* s, pos64_t length)
   {
      if (position < INT_MAX && length < INT_MAX) {
         return read((pos_t)position, s, (pos_t)length);
      }
      else return false;
   }

   virtual void load(StreamReader* reader, pos_t length);

   virtual bool write(pos_t position, const void* s, pos_t length);
//   bool write(pos_t position, const void* s, pos64_t length)
//   {
//      if (length < INT_MAX) {
//         return write(position, s, (pos_t)length);
//      }
//      else return false;
//   }

   virtual bool writeBytes(pos_t position, char value, pos_t length);

   bool writeSize(pos_t position, size_t value)
   {
      return write(position, (void*)&value, sizeof(size_t));
   }
   bool writeQWord(pos_t position, long long value)
   {
      return write(position, (void*)&value, 8u);
   }
   bool writeDWord(pos_t position, int value)
   {
      return write(position, (void*)&value, 4u);
   }
   bool writeWord(pos_t position, short value)
   {
      return write(position, (void*)&value, 2u);
   }
   bool writeByte(pos_t position, char value)
   {
      return write(position, (void*)&value, 1u);
   }

   bool writeLiteral(pos_t position, const char* s)
   {
      if (!emptystr(s)) {
         return write(position, s, getlength(s) + 1);
      }
      else return writeWord(position, 0);
   }

   virtual void insert(pos_t position, const void* s, pos_t length);

   virtual void clear() { _used = 0; }

   virtual void trim(pos_t size)
   {
      _used = size;
   }
   virtual void trimLong(pos64_t size)
   {
      if (size < INT_MAX) {
         trim((pos_t)size);
      }
   }

//   char* extract()
//   {
//      char* buffer = _buffer;
//
//      _buffer = NULL;
//      _used = _total = 0;
//
//      return buffer;
//   }

   MemoryDump();
   MemoryDump(pos_t capacity);
   MemoryDump(MemoryDump& copy);
   virtual ~MemoryDump() { freestr(_buffer); }
};

// --- ByteArray ---

class ByteArray : public _Memory
{
   char* _bytes;
   pos_t _length;

public:
   virtual pos_t Length() const { return _length; }

   virtual void* get(pos_t position) const;

   //virtual void* getLong(pos64_t position) const
   //{
   //   if (position < INT_MAX)
   //   {
   //      return get((pos_t)position);
   //   }
   //   else return NULL;
   //}

   virtual bool read(pos_t position, void* s, pos_t length);

   virtual bool readLong(pos64_t position, void* s, pos64_t length)
   {
      if (position < INT_MAX && length < INT_MAX) {
         return read((pos_t)position, s, (pos_t)length);
      }
      else return false;
   }

   virtual bool write(pos_t position, const void* s, pos_t length);

   virtual void insert(pos_t position, const void* s, pos_t length);

   virtual bool writeBytes(pos_t position, char value, pos_t length);

   virtual void* getReferences() { return NULL; }

   virtual void trim(pos_t)
   {
   }
   virtual void trimLong(pos64_t)
   {
   }

   ByteArray(void* bytes, pos_t length)
   {
      _bytes = (char*)bytes;
      _length = length;
   }
};

//class LineReader : public TextReader
//{
////   TextReader* _reader;
////
////public:
////   virtual bool read(wchar16_t* s, size_t length)
////   {
////      const wchar16_t* start = s;
////      while (length > 0) {
////         if(!_reader->read(s, 1))
////            break;
////
////         if (s[0] == '\n') {
////            s[1] = 0;
////            return true;
////         }
////         length--;
////         s++;
////      }
////      s[0] = 0;
////
////      return s != start;
////   }
////
////   virtual bool read(char* s, size_t length)
////   {
////      while (length > 0) {
////         if(!_reader->read(s, 1))
////            return false;
////
////         if (s[0] == '\n') {
////            s[1] = 0;
////            return true;
////         }
////         length--;
////         s++;
////      }
////      s[0] = 0;
////
////      return true;
////   }
////
////   LineReader(TextReader* reader)
////   {
////      _reader = reader;
////   }
//};

} // _ELENA_

#endif  // DumpH
