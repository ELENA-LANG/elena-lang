//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//      This header contains the declaration of abstract stream reader
//      and writer classes
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef streamsH
#define streamsH 1

namespace _ELENA_
{

// --- Constant definition ---
#define BLOCK_SIZE                  0x0200            // the temporal exchange buffer size

// --- _Memory interface ---

class _Memory
{
public:
   int& operator[](pos_t position) const
   {
      return *(int*)get(position);
   }

   virtual pos_t Length() const = 0;

   //virtual pos64_t LongLength() const = 0;

   virtual void* get(pos_t position) const = 0;

   //virtual void* getLong(pos64_t position) const = 0;

   virtual bool read(pos_t position, void* s, pos_t length) = 0;

   virtual bool readLong(pos64_t position, void* s, pos64_t length) = 0;

   virtual bool write(pos_t position, const void* s, pos_t length) = 0;

   //virtual bool write(pos64_t position, const void* s, pos64_t length) = 0;

   virtual void insert(pos_t position, const void* s, pos_t length) = 0;

   virtual bool writeBytes(pos_t position, char value, pos_t length) = 0;

   virtual bool addReference(ref_t, pos_t)
   {
      return false;
   }

   virtual void* getReferences() { return nullptr; }

   virtual void trim(pos_t position) = 0;
   virtual void trimLong(pos64_t position) = 0;

//#ifdef _WIN64
//
//   void* get_st(size_t position)
//   {
//      return getLong(position);
//   }
//
//#else
//
//   void* get_st(size_t position)
//   {
//      return get(position);
//   }
//
//#endif

   virtual ~_Memory() {}
};

// --- StreamReader interface ---

class StreamReader
{
public:
   virtual bool Eof() = 0;
   virtual pos_t Position() = 0;

   virtual bool seek(pos_t position) = 0;
//   virtual bool seek(pos64_t position) = 0;

   virtual bool read(void* s, pos_t length) = 0;

   virtual const char* getLiteral(const char* def) = 0;
   virtual const wide_c* getLiteral(const wide_c* def) = 0;

   lvaddr_t getVAddress()
   {
      lvaddr_t value = 0;
      read(&value, sizeof(lvaddr_t));

      return value;
   }

   uintptr_t getIntPtr()
   {
      uintptr_t value = 0;
      read(&value, sizeof(uintptr_t));

      return value;
   }

   unsigned long long getQWord()
   {
      unsigned long long value = 0;
      readQWord(value);

      return value;
   }

   unsigned int getDWord()
   {
      unsigned int value = 0;
      readDWord(value);

      return value;
   }

   unsigned int getDWord(int defValue)
   {
      unsigned int value = 0;
      if (readDWord(value)) {
         return value;
      }
      else return defValue;
   }

   unsigned char getByte()
   {
      unsigned char value = 0;
      readByte(value);

      return value;
   }

   bool readDWord(int& dword)
   {
      return read((void*)&dword, 4);
   }

   bool readQWord(unsigned long long& qword)
   {
      return read((void*)&qword, 8);
   }

   bool readDWord(unsigned int& dword)
   {
      return read((void*)&dword, 4);
   }

   bool readSize(size_t& size)
   {
      return read((void*)&size, sizeof(size_t));
   }

   bool readByte(unsigned char& ch)
   {
      return read((void*)&ch, 1);
   }

   bool readChar(char& ch)
   {
      return read(&ch, 1);
   }

   template<class String> bool readString(String& s)
   {
      s.clear();

      char ch = 0;
      while (readChar(ch)) {
         if (ch != 0) {
            s.append(ch);
         }
         else return true;
      }
      return false;
   }

   virtual ~StreamReader() {}
};

// --- StreamWriter interface ---

class StreamWriter
{
public:
   virtual bool isOpened() = 0;

   virtual pos_t Position() const = 0;

   virtual bool write(const void* s, pos_t length) = 0;
//   virtual bool writeLong(const void* s, pos64_t length) = 0;
//
//#ifdef _WIN64
//
//   bool writeLiteral(const char* s)
//   {
//      return writeLiteral(s, getlength(s) + 1);
//   }
//
//   bool writeLiteral(const char* s, pos64_t length)
//   {
//      return writeLong((void*)s, length);
//   }
//
//   bool writeLiteral(const wide_c* s, pos64_t length)
//   {
//      return writeLong((void*)s, length << 1);
//   }
//
//   bool writeLiteral(const wide_c* s)
//   {
//      return writeLiteral(s, getlength(s) + 1);
//   }
//
//   bool write_st(void* s, pos64_t length)
//   {
//      return writeLong(s, length);
//   }
//
//#else

   bool writeLiteral(const char* s)
   {
      return writeLiteral(s, getlength(s) + 1);
   }

   bool writeLiteral(const char* s, pos_t length)
   {
      return write((void*)s, length);
   }

   bool writeLiteral(const wide_c* s, pos_t length)
   {
      return write((void*)s, length << 1);
   }

   bool writeLiteral(const wide_c* s)
   {
      return writeLiteral(s, getlength(s) + 1);
   }

//   bool write_st(void* s, pos_t length)
//   {
//      return write(s, length);
//   }
//
//#endif

   bool writeChar(char ch)
   {
      return writeLiteral(&ch, 1u);
   }

   bool writeChar(wide_c ch)
   {
      return writeLiteral(&ch, 1u);
   }

   void writeDWord(unsigned int dword)
   {
      write(&dword, 4);
   }

   void writeSize(size_t size)
   {
      write(&size, sizeof(size_t));
   }

   void writeQWord(unsigned long long qword)
   {
      write(&qword, 8);
   }

   void writeWord(unsigned short word)
   {
      write(&word, 2);
   }

   bool writeByte(unsigned char ch)
   {
      return write((void*)&ch, 1);
   }
   bool writeByte(int ch)
   {
      return write((void*)&ch, 1);
   }

   void writeVAddress(lvaddr_t addr)
   {
      write(&addr, sizeof(lvaddr_t));
   }
   void writeIntPtr(uintptr_t addr)
   {
      write(&addr, sizeof(uintptr_t));
   }

   virtual bool writeBytes(unsigned char ch, pos_t count)
   {
      while (count > 0) {
         if (!write((char*)&ch, 1))
            return false;
         count--;
      }
      return true;
   }

   void read(StreamReader* reader, pos_t length)
   {
      char   buffer[BLOCK_SIZE];
      pos_t  blockLen = BLOCK_SIZE;
      while (length > 0) {
         if (blockLen >= length)
            blockLen = length;

         reader->read(buffer, blockLen);
         write(buffer, blockLen);

         length -= blockLen;
      }
   }

   virtual ~StreamWriter() {}
};

// --- TextReader ---

class TextReader
{
public:
   virtual pos_t Position() = 0;

   virtual bool seek(pos_t position) = 0;

   virtual void reset() = 0;

   virtual bool read(char* s, pos_t length) = 0;

   template<class String, class T> bool readLine(String& s, T* buffer, pos_t size = BLOCK_SIZE)
   {
      s.clear();

      while (read(buffer, size)) {
         s.append(buffer);

         if (buffer[getlength(buffer) - 1] == '\n')
            return true;
      }
      return (getlength(s) != 0);
   }

   template<class String, class T> bool readAll(String& s, T* buffer, pos_t size = BLOCK_SIZE)
   {
      s.clear();

      while (read(buffer, size)) {
         s.append(buffer);
      }
      return (getlength(s) != 0);
   }

   virtual ~TextReader() {}
};

// --- TextWriter ---

class TextWriter
{
public:
   virtual pos_t Position() const = 0;

   virtual bool writeNewLine() = 0;
   virtual bool write(const wide_c* s, pos_t length) = 0;
   virtual bool write(const char* s, pos_t length) = 0;
   //virtual bool write(const wide_c* s, pos64_t length) = 0;
   //virtual bool write(const char* s, pos64_t length) = 0;

   virtual bool writeChar(char ch)
   {
      return write(&ch, 1u);
   }

   virtual bool writeChar (wide_c ch)
   {
      return write(&ch, 1u);
   }

   virtual bool writeLiteral(const wide_c* s)
   {
      return write(s, getlength(s));
   }
   virtual bool writeLiteral(const char* s)
   {
      return write(s, getlength(s));
   }

   virtual bool writeLiteralNewLine(const wide_c* s)
   {
      if (writeLiteral(s)) {
         return writeNewLine();
      }
      else return false;
   }

   virtual bool writeLiteralNewLine(const char* s)
   {
      if (writeLiteral(s)) {
         return writeNewLine();
      }
      else return true;
   }

//   virtual bool fillChar(char ch, size_t count)
//   {
//      while (count > 0) {
//         if (!write(&ch, 1))
//            return false;
//         count--;
//      }
//      return true;
//   }
//
//   virtual bool fillWideChar(wchar16_t ch, size_t count)
//   {
//      while (count > 0) {
//         if (!write(&ch, 1))
//            return false;
//         count--;
//      }
//      return true;
//   }

   virtual bool fillText(const wide_c* s, pos_t length, pos_t count)
   {
      while (count > 0) {
         if (!write(s, length))
            return false;
         count--;
      }
      return true;
   }

   virtual bool fillText(const char* s, pos_t length, pos_t count)
   {
      while (count > 0) {
         if (!write(s, length))
            return false;
         count--;
      }
      return true;
   }

   virtual ~TextWriter() {}
};

// --- LiteralWriter ---

template<class CHAR> class LiteralWriter : public TextWriter
{
   CHAR* _text;
   pos_t _offset;
   pos_t _size;

public:
   virtual pos_t Position() const { return _offset; }

   void reset()
   {
      _offset = 0;
   }

   virtual bool writeNewLine()
   {
      return false; // !!
   }

   bool seek(pos_t position)
   {
      _offset = position;

      return true;
   }

   //bool seek(pos64_t position)
   //{
   //   if (position < INT_MAX) {
   //      return seek((pos_t)position);
   //   }
   //   else return false;
   //}

   virtual bool write(const wide_c* s, pos_t length)
   {
      size_t lenToWrite = _size - _offset;

      if (Convertor::copy(_text + _offset, s, length, lenToWrite)) {
         seek(_offset + lenToWrite);

         return true;
      }
      else return false;
   }

   virtual bool write(const char* s, pos_t length)
   {
      size_t lenToWrite = _size - _offset;

      if (Convertor::copy(_text + _offset, s, length, lenToWrite)) {
         seek(_offset + lenToWrite);

         return true;
      }
      else return false;
   }

   virtual bool write(const wide_c* s, pos64_t length)
   {
      if (length < INT_MAX) {
         return write(s, (pos_t)length);
      }
      else return false;
   }

   virtual bool write(const char* s, pos64_t length)
   {
      if (length < INT_MAX) {
         return write(s, (pos_t)length);
      }
      else return false;
   }

   bool isOpened() { return _text != NULL; }

//   bool writeByte(unsigned char ch)
//   {
//      return false;
//   }
//
//   virtual bool write(const void* s, size_t length)
//   {
//      size_t size = length >> 1;
//      if (_offset + size <= _size) {
//         void* p = _text + _offset;
//         memcpy(p, s, length);
//
//         _offset += size;
//
//         return true;
//      }
//      else return false;
//   }

   LiteralWriter(CHAR* text, pos_t size)
   {
      _text = text;
      _offset = 0;
      _size = size;
   }
   LiteralWriter(CHAR* text, pos_t size, pos_t offset)
   {
      _text = text;
      _offset = offset;
      _size = size;
   }

   virtual ~LiteralWriter() {}
};

// --- LiteralTextReader ---

template<class CHAR> class LiteralTextReader : public TextReader
{
   const CHAR* _text;
   pos_t       _offset;
   pos_t       _length;

public:
   virtual pos_t Position() { return _offset; }

   virtual bool seek(pos_t position)
   {
      if (position < _length) {
         _offset = position;

         return true;
      }
      else return false;
   }

   virtual void reset()
   {
      _offset = 0;
   }

   virtual bool read(char* s, pos_t length)
   {
      if (_offset + length > _length) {
         length = _length - _offset;
      }
      if (length > 0) {
         size_t eol = StrHelper::findChar(_text + _offset, '\n', length);
         if (eol == NOTFOUND_POS) {
            if (_length - _offset > length) {
               // if we are not lucky - try to find a whitespace
               eol = _offset + length - 2;

               while (eol != 0 && _text[eol] != ' ')
                  eol--;

               length = (pos_t)eol - _offset + 1;
            }
         }
         else length = (pos_t)eol + 1;

         size_t tmpLength = length;
         Convertor::copy(s, _text + _offset, tmpLength, tmpLength);
         length = (pos_t)tmpLength;

         s[length] = 0;

         _offset += length;

         return true;
      }
      else return false;
   }

   LiteralTextReader(const CHAR* text)
   {
      _text = text;
      _offset = 0;
      _length = getlength(text);
   }
   LiteralTextReader(const CHAR* text, pos_t offset)
   {
      _text = text;
      _offset = offset;
      _length = getlength(text);
   }
};

typedef LiteralTextReader<char> IdentifierTextReader;

//// --- LiteralReader ---
//
//class LiteralReader : public StreamReader
//{
//   const wchar_t* _text;
//   size_t       _offset;
//
//public:
//   virtual bool Eof() { return getlength(_text) == _offset; }
//
//   virtual size_t Position() { return _offset; }
//
//   virtual bool seek(size_t position)
//   {
//      _offset = position;
//
//      return true;
//   }
//
//   virtual const wchar_t* getLiteral()
//   {
//      const wchar_t* s = _text + _offset;
//
//      _offset += getlength(s) + 1;
//
//      return s;
//   }
//
//   virtual bool read(void* s, size_t length)
//   {
//      return false;
//   }
//
////   bool readLiteral(char*, size_t)
////   {
////      return false;
////   }
////
////   int readDWord()
////   {
////      int dword = 0;
////      void* p = (void*)(_text + _offset);
////      memcpy(&dword, p, 4);
////      _offset += 2;
////
////      return dword;
////   }
//
//   LiteralReader(const wchar_t* text)
//   {
//      _text = text;
//	  _offset = 0;
//   }
//   LiteralReader(const wchar_t* text, int offset)
//   {
//      _text = text;
//	  _offset = offset;
//   }
//
//   virtual ~LiteralReader() {}
//};

// --- DumpReader ---

class DumpReader : public StreamReader
{
   void*  _dump;
   pos_t  _offset;
   pos_t  _length;

public:
   virtual bool Eof() { return _offset >= _length; }

   virtual pos_t Position() { return _offset; }

   virtual bool seek(pos_t position)
   {
      _offset = position;

      return true;
   }

   //virtual bool seek(pos64_t position)
   //{
   //   if (position < INT_MAX) {
   //      return seek((pos_t)position);
   //   }
   //   else return false;
   //}

   virtual const char* getLiteral(const char*)
   {
      const char* s = (char*)_dump + _offset;

      seek(_offset + getlength(s) + 1);

      return s;

   }
   virtual const wide_c* getLiteral(const wide_c*)
   {
      const wide_c* s = (const wide_c*)((char*)_dump + _offset);

      seek(_offset + (getlength(s) << 1) + 1);

      return s;
   }

   virtual bool read(void* s, pos_t length)
   {
      if (_offset + length <= _length) {
         memcpy(s, (char*)_dump + _offset, length);

         _offset += length;

         return true;
      }
      else return false;

   }

   void setSize(pos_t length)
   {
      _length = length;
   }

   DumpReader(void* dump, pos_t length)
   {
      _dump = dump;
	  _offset = 0;
     _length = length;
   }
   DumpReader(void* dump, pos_t length, int offset)
   {
      _dump = dump;
      _offset = 0;
      _length = length;
      _offset = offset;
   }

   virtual ~DumpReader() {}
};

// --- MemoryReader ---

class MemoryReader : public StreamReader
{
protected:
   _Memory* _memory;
   pos_t    _position;

public:
   virtual bool Eof() { return _position >= _memory->Length(); }

   virtual pos_t Position() { return _position; }

   void* Address() const { return _memory->get(_position); }

   virtual bool seek(pos_t position)
   {
      if (position <= _memory->Length()) {
         _position = position;

         return true;
      }
      else return false;
   }
//   virtual bool seek(pos64_t position)
//   {
//      if (position < INT_MAX) {
//         return seek((pos_t)position);
//      }
//      else return false;
//   }

   virtual bool read(void* s, pos_t length)
   {
      if (_memory->read(_position, s, length)) {
         _position += length;

         return true;
      }
      else return false;
   }

////   virtual bool read2(void* s, size_t length)
////   {
////      return read(s, length << 1);
////   }
////
//////   virtual bool read4(void* s, size_t length)
//////   {
//////      return read(s, length << 4);
//////   }
////
////   virtual bool read(void* s, size_t length, size_t& wasread)
////   {
////      if (read(s, length)) {
////         wasread = length;
////      }
////      else wasread = 0;
////
////      return (wasread > 0);
////   }

//   virtual bool read2(void* s, size_t length, size_t& wasread)
//   {
//      return read(s, length << 1, wasread);
//   }
//
//   virtual bool read4(void* s, size_t length, size_t& wasread)
//   {
//      return read(s, length << 4, wasread);
//   }

   virtual const char* getLiteral(const char*)
   {
      const char* s = (const char*)_memory->get(_position);

      seek(_position + getlength(s) + 1);

      return s;
   }

   virtual const wide_c* getLiteral(const wide_c*)
   {
      const wide_c* s = (const wide_c*)_memory->get(_position);

      seek(_position + ((getlength(s) + 1) << 1));

      return s;
   }

   MemoryReader(_Memory* memory)
   {
      _memory = memory;
      _position = 0;
   }
   MemoryReader(_Memory* memory, pos_t position)
   {
      _memory = memory;
      _position = position;
   }
   //MemoryReader(_Memory* memory, pos64_t position)
   //{
   //   _memory = memory;
   //   seek(position);
   //}
};

// --- MemoryWriter ---

class MemoryWriter : public StreamWriter
{
protected:
   _Memory* _memory;
   pos_t    _position;

public:
   _Memory* Memory() { return _memory; }

   void* Address() const { return _memory->get(_position); }

   virtual bool isOpened() { return (_memory != nullptr); }

   virtual pos_t Position() const { return _position; }

   virtual bool write(const void* s, pos_t length)
   {
      if (_memory->write(_position, s, length)) {
         _position += length;

         return true;
      }
      else return false;
   }
//   virtual bool write(const void* s, pos64_t length)
//   {
//      if (length < INT_MAX) {
//         return write(s, (pos_t)length);
//      }
//      else return false;
//   }

   bool writePtr(uintptr_t ptr)
   {
      return write(&ptr, sizeof(uintptr_t));
   }

   bool writeBytes(unsigned char ch, pos_t count)
   {
      if (_memory->writeBytes(_position, ch, count)) {
         _position += count;

         return true;
      }
      else return false;
   }

   virtual bool seek(pos_t position)
   {
      if (position <= _memory->Length()) {
         _position = position;

         return true;
      }
      else return false;
   }

   void seekEOF()
   {
      _position = _memory->Length();
   }

   void align(pos_t alignment, unsigned char c)
   {
      pos_t aligned = _ELENA_::align(_position, alignment);

      writeBytes(c, aligned - _position);
   }

   void insertDWord(pos_t position, int value)
   {
      _memory->insert(position, (char*)&value, 4);

      if (position <= _position)
         _position += 4;
   }

   void insertWord(pos_t position, short value)
   {
      _memory->insert(position, (char*)&value, 2);

      if (position <= _position)
         _position += 2;
   }

   void insertByte(pos_t position, char value)
   {
      _memory->insert(position, &value, 1);

      if (position <= _position)
         _position += 1;
   }
   void insert(pos_t position, void* value, pos_t length)
   {
      _memory->insert(position, value, length);

      if (position <= _position)
         _position += length;
   }

   bool writeRef(ref_t reference, pos_t value)
   {
      if (_memory->addReference(reference, _position)) {
         writeDWord(value);

         return true;
      }
      else return false;
   }
//   bool writeRef64(ref_t reference, pos64_t value)
//   {
//      if (_memory->addReference(reference, _position)) {
//         writeQWord(value);
//
//         return true;
//      }
//      else return false;
//   }

   MemoryWriter(_Memory* memory)
   {
      _memory = memory;
      _position = _memory ? _memory->Length() : 0;
   }
   MemoryWriter(_Memory* memory, pos_t position)
   {
      _memory = memory;
      _position = position;
   }
};

} // _ELENA_

#endif // streamsH
