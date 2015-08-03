//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//      This header contains the declaration of abstract stream reader
//      and writer classes
//                                              (C)2005-2015, by Alexei Rakov
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
   int& operator[](size_t position) const
   {
      return *(int*)get(position);
   }

   virtual size_t Length() const = 0;

   virtual void* get(size_t position) const = 0;

   virtual bool read(size_t position, void* s, size_t length) = 0;

   virtual bool write(size_t position, const void* s, size_t length) = 0;

   virtual void insert(size_t position, const void* s, size_t length) = 0;

   virtual bool writeBytes(size_t position, char value, size_t length) = 0;

   virtual bool addReference(ref_t, size_t)
   {
      return false;
   }

   virtual void* getReferences() { return NULL; }

   virtual void trim(size_t position) = 0;

   virtual ~_Memory() {}
};

// --- StreamReader interface ---

class StreamReader
{
public:
   virtual bool Eof() = 0;
   virtual size_t Position() = 0;

   virtual bool seek(size_t position) = 0;

   virtual bool read(void* s, size_t length) = 0;

   virtual const char* getLiteral(const char* def) = 0;
   virtual const wide_c* getLiteral(const wide_c* def) = 0;

   int getDWord()
   {
      int value = 0;
      readDWord(value);

      return value;
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

   bool readDWord(size_t& dword)
   {
      return read((void*)&dword, 4);
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

      ident_c ch = 0;
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

   virtual size_t Position() const = 0;

   virtual bool write(const void* s, size_t length) = 0;

   bool writeLiteral(const char* s)
   {
      return writeLiteral(s, getlength(s) + 1);
   }

   bool writeLiteral(const char* s, size_t length)
   {
      return write((void*)s, length);
   }

   bool writeLiteral(const wide_c* s, size_t length)
   {
      return write((void*)s, length << 1);
   }

   bool writeLiteral(const wide_c* s)
   {
      return writeLiteral(s, getlength(s) + 1);
   }

   bool writeChar(char ch)
   {
      return writeLiteral(&ch, 1);
   }

   bool writeChar(wide_c ch)
   {
      return writeLiteral(&ch, 1);
   }

   void writeDWord(int dword)
   {
      write(&dword, 4);
   }

   void writeWord(unsigned short word)
   {
      write(&word, 2);
   }

   bool writeByte(unsigned char ch)
   {
      return write((void*)&ch, 1);
   }

   virtual bool writeBytes(unsigned char ch, size_t count)
   {
      while (count > 0) {
         if (!write((char*)&ch, 1))
            return false;
         count--;
      }
      return true;
   }

   void read(StreamReader* reader, size_t length)
   {
      char    buffer[BLOCK_SIZE];
      size_t  blockLen = BLOCK_SIZE;
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
   virtual bool read(ident_c* s, size_t length) = 0;

   template<class String, class T> bool readLine(String& s, T* buffer, size_t size = BLOCK_SIZE)
   {
      s.clear();

      while (read(buffer, size)) {
         s.append(buffer);

         if (buffer[getlength(buffer) - 1] == '\n')
            return true;
      }
      return (getlength(s) != 0);
   }
   virtual ~TextReader() {}
};

// --- TextWriter ---

class TextWriter
{
public:
   virtual bool writeNewLine() = 0;
   virtual bool write(const wide_c* s, size_t length) = 0;
   virtual bool write(const char* s, size_t length) = 0;

   virtual bool writeChar(char ch)
   {
      return write(&ch, 1);
   }

   virtual bool writeChar (wide_c ch)
   {
      return write(&ch, 1);
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

   virtual bool fillText(const wide_c* s, size_t length, size_t count)
   {
      while (count > 0) {
         if (!write(s, length))
            return false;
         count--;
      }
      return true;
   }

   virtual bool fillText(const char* s, size_t length, size_t count)
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
   CHAR*   _text;
   size_t  _offset;
   size_t  _size;

public:
   size_t Position() const { return _offset; }

   void reset()
   {
      _offset = 0;
   }

   virtual bool writeNewLine()
   {
      return false; // !!
   }

   virtual bool write(const wide_c* s, size_t length)
   {
      size_t lenToWrite = _size - _offset;

      if (StringHelper::copy(_text + _offset, s, length, lenToWrite)) {
         _offset += lenToWrite;

         return true;
      }
      else return false;
   }

   virtual bool write(const char* s, size_t length)
   {
      size_t lenToWrite = _size - _offset;

      if (StringHelper::copy(_text + _offset, s, length, lenToWrite)) {
         _offset += lenToWrite;

         return true;
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

   LiteralWriter(CHAR* text, int size)
   {
      _text = text;
      _offset = 0;
      _size = size;
   }
   LiteralWriter(CHAR* text, int size, int offset)
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
   size_t      _offset;
   size_t      _length;

public:
   virtual bool read(char* s, size_t length)
   {
      if (_offset + length > _length) {
         length = _length - _offset;
      }

      if (length > 0) {
         StringHelper::copy(s, _text + _offset, length, length);
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
   LiteralTextReader(const CHAR* text, int offset)
   {
      _text = text;
      _offset = offset;
      _length = getlength(text);
   }
};

typedef LiteralTextReader<ident_c> IdentifierTextReader;

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

// --- MemoryReader ---

class MemoryReader : public StreamReader
{
protected:
   _Memory* _memory;
   size_t   _position;

public:
   virtual bool Eof() { return _position >= _memory->Length(); }

   virtual size_t Position() { return _position; }

   void* Address() const { return _memory->get(_position); }

   virtual bool seek(size_t position)
   {
      if (position <= _memory->Length()) {
         _position = position;

         return true;
      }
      else return false;
   }

   virtual bool read(void* s, size_t length)
   {
      if (_memory->read(_position, s, length)) {
         _position += length;

         return true;
      }
      else return false;
   }

//   virtual bool read2(void* s, size_t length)
//   {
//      return read(s, length << 1);
//   }
//
//   virtual bool read4(void* s, size_t length)
//   {
//      return read(s, length << 4);
//   }

   virtual bool read(void* s, size_t length, size_t& wasread)
   {
      if (read(s, length)) {
         wasread = length;
      }
      else wasread = 0;

      return (wasread > 0);
   }

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

      _position += (getlength(s) + 1);

      return s;
   }

   virtual const wide_c* getLiteral(const wide_c*)
   {
      const wide_c* s = (const wide_c*)_memory->get(_position);

      _position += ((getlength(s) + 1) << 1);

      return s;
   }

   MemoryReader(_Memory* memory)
   {
      _memory = memory;
      _position = 0;
   }
   MemoryReader(_Memory* memory, size_t position)
   {
      _memory = memory;
      _position = position;
   }
};

// --- MemoryWriter ---

class MemoryWriter : public StreamWriter
{
protected:
   _Memory* _memory;
   size_t   _position;

public:
   _Memory* Memory() { return _memory; }

   void* Address() const { return _memory->get(_position); }

   virtual bool isOpened() { return (_memory != NULL); }

   virtual size_t Position() const { return _position; }

   virtual bool write(const void* s, size_t length)
   {
      if (_memory->write(_position, s, length)) {
         _position += length;

         return true;
      }
      else return false;
   }

   bool writeBytes(unsigned char ch, size_t count)
   {
      if (_memory->writeBytes(_position, ch, count)) {
         _position += count;

         return true;
      }
      else return false;
   }

   virtual bool seek(size_t position)
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

   void align(size_t alignment, unsigned char c)
   {
      size_t aligned = _ELENA_::align(_position, alignment);

      writeBytes(c, aligned - _position);
   }

   void insertDWord(size_t position, int value)
   {
      _memory->insert(position, (char*)&value, 4);

      if (position <= _position)
         _position += 4;
   }

   void insertWord(size_t position, short value)
   {
      _memory->insert(position, (char*)&value, 2);

      if (position <= _position)
         _position += 2;
   }

   void insertByte(size_t position, char value)
   {
      _memory->insert(position, &value, 1);

      if (position <= _position)
         _position += 1;
   }

   bool writeRef(ref_t reference, size_t value)
   {
      if (_memory->addReference(reference, _position)) {
         writeDWord(value);

         return true;
      }
      else return false;
   }

   MemoryWriter(_Memory* memory)
   {
      _memory = memory;
      _position = _memory ? _memory->Length() : 0;
   }
   MemoryWriter(_Memory* memory, size_t position)
   {
      _memory = memory;
      _position = position;
   }
};

} // _ELENA_

#endif // streamsH
