//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains the declaration of ELENA Engine Data Memory dump
//		classes.
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef DumpH
#define DumpH 1

namespace _ELENA_
{

// --- MemoryDump class ---

class MemoryDump : public _Memory
{
protected:
   char*  _buffer;
   size_t _total;
   size_t _used;

   void resize(size_t size);

public:
   size_t getFreeSpace() const { return _total - _used; }

   virtual size_t Length() const { return _used; }

   virtual void* get(size_t position) const;

   void reserve(size_t size);

   virtual bool read(size_t position, void* s, size_t length);

   virtual void load(StreamReader* reader, size_t length);

   virtual bool write(size_t position, const void* s, size_t length);

   virtual bool writeBytes(size_t position, char value, size_t length);

   bool writeDWord(size_t position, int value)
   {
      return write(position, (void*)&value, 4);
   }
   bool writeWord(size_t position, short value)
   {
      return write(position, (void*)&value, 2);
   }
   bool writeByte(size_t position, char value)
   {
      return write(position, (void*)&value, 1);
   }

   bool writeLiteral(size_t position, const char* s)
   {
      if (!emptystr(s)) {
         return write(position, s, strlen(s) + 1);
      }
      else return writeWord(position, 0);
   }

   virtual void insert(size_t position, const void* s, size_t length);

   virtual void clear() { _used = 0; }

   virtual void trim(size_t size)
   {
      _used = size;
   }

   char* extract()
   {
      char* buffer = _buffer;

      _buffer = NULL;
      _used = _total = 0;

      return buffer;
   }

   MemoryDump();
   MemoryDump(size_t capacity);
   MemoryDump(MemoryDump& copy);
   virtual ~MemoryDump() { freestr(_buffer); }
};

// --- ByteArray ---

class ByteArray : public _Memory
{
   char*  _bytes;
   size_t _length;

public:
   virtual size_t Length() const { return _length; }

   virtual void* get(size_t position) const;

   virtual bool read(size_t position, void* s, size_t length);

   virtual bool write(size_t position, const void* s, size_t length);

   virtual void insert(size_t position, const void* s, size_t length);

   virtual bool writeBytes(size_t position, char value, size_t length);

   virtual void* getReferences() { return NULL; }

   virtual void trim(size_t)
   {
   }

   ByteArray(void* bytes, size_t length)
   {
      _bytes = (char*)bytes;
      _length = length;
   }
};

class LineReader : public TextReader
{
//   TextReader* _reader;
//
//public:
//   virtual bool read(wchar16_t* s, size_t length)
//   {
//      const wchar16_t* start = s;
//      while (length > 0) {
//         if(!_reader->read(s, 1))
//            break;
//
//         if (s[0] == '\n') {
//            s[1] = 0;
//            return true;
//         }
//         length--;
//         s++;
//      }
//      s[0] = 0;
//
//      return s != start;
//   }
//
//   virtual bool read(char* s, size_t length)
//   {
//      while (length > 0) {
//         if(!_reader->read(s, 1))
//            return false;
//
//         if (s[0] == '\n') {
//            s[1] = 0;
//            return true;
//         }
//         length--;
//         s++;
//      }
//      s[0] = 0;
//
//      return true;
//   }
//
//   LineReader(TextReader* reader)
//   {
//      _reader = reader;
//   }
};

} // _ELENA_

#endif  // DumpH
