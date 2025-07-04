//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//      This header contains the declaration of abstract stream reader
//      and writer classes
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef STREAMS_H
#define STREAMS_H

namespace elena_lang
{

   // --- Constant definition ---
   constexpr auto BLOCK_SIZE = 0x0200;            // the temporal exchange buffer size

   // --- MemoryBase ---

   class MemoryBase
   {
   public:
      char& operator[](pos_t position) const
      {
         return *(char*)get(position);
      }

      static bool writeDWord(MemoryBase* target, pos_t position, unsigned int value)
      {
         return target->write(position, &value, sizeof(value));
      }

      static unsigned int getDWord(MemoryBase* source, pos_t position)
      {
         unsigned int value = 0;
         source->read(position, &value, sizeof(value));

         return value;
      }
      static unsigned long long getQWord(MemoryBase* source, pos_t position)
      {
         unsigned long long value = 0;
         source->read(position, &value, sizeof(value));

         return value;
      }

      static void maskDWord(MemoryBase* source, pos_t position, ref_t mask)
      {
         unsigned int value = getDWord(source, position);

         writeDWord(source, position, value | mask);
      }

      static bool writeQWord(MemoryBase* target, pos_t position, unsigned long long value)
      {
         return target->write(position, &value, sizeof(value));
      }

      virtual pos_t length() const = 0;

      virtual void* get(pos_t position) const = 0;

      virtual bool write(pos_t position, const void* s, pos_t length) = 0;

      virtual bool insert(pos_t position, const void* s, pos_t length) = 0;

      virtual bool read(pos_t position, void* s, pos_t length) const = 0;

      virtual bool addReference(ref_t, pos_t)
      {
         return false;
      }

      virtual void* getReferences() const
      {
         return nullptr;
      }

      virtual void trim(pos_t position) = 0;

      virtual ~MemoryBase() = default;
   };


   // --- StreamReader ---

   class StreamReader
   {
   public:
      virtual bool eof() = 0;
      virtual pos_t length() const = 0;

      virtual pos_t position() const = 0;

      virtual bool seek(pos_t position) = 0;

      virtual bool read(void* s, pos_t length) = 0;

      bool readChar(char& retVal)
      {
         return read(&retVal, 1);
      }
      bool readUChar(unsigned char& retVal)
      {
         return read(&retVal, 1);
      }
      bool readDWord(unsigned int& retVal)
      {
         return read(&retVal, 4);
      }
      bool readPos(pos_t& retVal)
      {
         return read(&retVal, sizeof(pos_t));
      }
      bool readRef(ref_t& retVal)
      {
         return read(&retVal, sizeof(ref_t));
      }
      bool readBool(bool& retVal)
      {
         return read(&retVal, sizeof(bool));
      }
      bool readRef64(ref64_t& retVal)
      {
         return read(&retVal, sizeof(ref64_t));
      }

      char getChar()
      {
         char retVal = 0;
         if (readChar(retVal)) {
            return retVal;
         }
         else return 0;
      }
      unsigned char getUChar()
      {
         unsigned char retVal = 0;
         if (readUChar(retVal)) {
            return retVal;
         }
         else return 0;
      }
      unsigned int getDWord()
      {
         unsigned int retVal = 0;
         if (readDWord(retVal)) {
            return retVal;
         }
         else return 0u;
      }
      pos_t getPos()
      {
         pos_t retVal = 0;
         if (readPos(retVal)) {
            return retVal;
         }
         else return 0u;
      }
      ref_t getRef()
      {
         ref_t retVal = 0;
         if (readRef(retVal)) {
            return retVal;
         }
         else return 0u;
      }
      ref64_t getRef64()
      {
         ref64_t retVal = 0;
         if (readRef64(retVal)) {
            return retVal;
         }
         else return 0ull;
      }
      bool getBool()
      {
         bool retVal = false;
         if (readBool(retVal)) {
            return retVal;
         }
         else return false;
      }

      template<class T, size_t size> bool readString(String<T, size>& s)
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

      template<class T, size_t size> bool appendString(String<T, size>& s)
      {
         char ch = 0;
         while (readChar(ch)) {
            if (ch != 0) {
               s.append(ch);
            }
            else return true;
         }
         return false;
      }

      virtual ~StreamReader() = default;
   };

   // --- TextReader ---
   template<class T> class TextReader
   {
   public:
      virtual bool isOpen() const = 0;

      virtual pos_t position() const = 0;

      virtual void reset() = 0;

      virtual bool read(char* s, pos_t length) = 0;

      template<class String> bool readAll(String& s, T* buffer, pos_t size = BLOCK_SIZE)
      {
         s.clear();

         while (read(buffer, size)) {
            s.append(buffer);
         }
         return (s.length() != 0);
      }

      virtual ~TextReader() = default;
   };

   // --- TextWriter ---
   template<class T> class TextWriter
   {
   public:
      virtual bool isOpen() const = 0;

      virtual pos_t position() const = 0;

      virtual bool write(const T* s, pos_t length) = 0;
      virtual bool writeNewLine() = 0;

      bool writeChar(T ch)
      {
         return write(&ch, 1);
      }

      bool writeText(const T* s)
      {
         return write(s, getlength_pos(s));
      }

      virtual bool writeTextLine(const T* s)
      {
         if (writeText(s)) {
            return writeNewLine();
         }
         else return false;
      }

      bool fillText(const T* s, pos_t length, pos_t count)
      {
         while (count > 0) {
            if (!write(s, length))
               return false;

            count--;
         }

         return true;
      }

      virtual ~TextWriter() = default;
   };

   // --- StreamWriter

   class StreamWriter
   {
   public:
      virtual bool isOpen() const = 0;

      virtual pos_t position() const = 0;
      virtual pos_t length() const = 0;

      virtual bool write(const void* s, pos_t length) = 0;

      bool writeInt(int value)
      {
         return write(&value, sizeof(value));
      }
      bool writeQWord(unsigned long long value)
      {
         return write(&value, 8);
      }
      bool writeDWord(unsigned int value)
      {
         return write(&value, 4);
      }
      bool writeWord(unsigned short value)
      {
         return write(&value, 2);
      }
      bool writeShort(short value)
      {
         return write(&value, 2);
      }
      bool writeByte(unsigned char value)
      {
         return write(&value, 1);
      }
      bool writeChar(char value)
      {
         return write(&value, 1);
      }
      bool writeRef(ref_t value)
      {
         return write(&value, sizeof(ref_t));
      }
      bool writePos(pos_t value)
      {
         return write(&value, sizeof(pos_t));
      }
      bool writeBool(bool value)
      {
         return write(&value, sizeof(bool));
      }
      bool writeRef64(ref64_t value)
      {
         return write(&value, sizeof(ref64_t));
      }
      bool writeSize(size_t value)
      {
         return write(&value, sizeof(size_t));
      }
      bool writeDouble(double value)
      {
         return write(&value, sizeof(double));
      }

      virtual bool writeBytes(unsigned char ch, pos_t count)
      {
         while (count > 0) {
            if (!write(&ch, 1))
               return false;
            count--;
         }
         return true;
      }

      bool writeWideString(wstr_t s, pos_t length)
      {
         return write(s, length << 1);
      }
      bool writeWideString(wstr_t s)
      {
         return writeWideString(s, s.length_pos() + 1);
      }

      bool writeString(ustr_t s, pos_t length)
      {
         return write(s, length);
      }
      bool writeString(ustr_t s)
      {
         return write(s, s.length_pos() + 1);
      }

      void copyFrom(StreamReader* reader, pos_t length)
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

      virtual ~StreamWriter() = default;
   };

   // --- MemoryReader ---

   class MemoryReader : public StreamReader
   {
   protected:
      MemoryBase* _memory;
      pos_t       _position;

   public:
      MemoryBase* Memory()
      {
         return _memory;
      }

      bool unassigned() const
      {
         return _memory == nullptr;
      }

      void* address() const
      {
         return _memory->get(_position);
      }

      bool eof() override
      {
         return _position >= _memory->length();
      }

      pos_t length() const override
      {
         return _memory->length();
      }

      pos_t position() const override
      {
         return _position;
      }

      bool seek(pos_t position) override
      {
         if (position <= _memory->length()) {
            _position = position;

            return true;
         }
         else return false;
      }

      bool read(void* s, pos_t length) override
      {
         if (_memory->read(_position, s, length)) {
            _position += length;

            return true;
         }
         else return false;
      }

      ustr_t getString(ustr_t)
      {
         const char* s = (const char*)_memory->get(_position);

         seek(_position + getlength_pos(s) + 1);

         return s;
      }

      MemoryReader(MemoryBase* memory)
      {
         _memory = memory;
         _position = 0;
      }
      MemoryReader(MemoryBase* memory, pos_t position)
      {
         _memory = memory;
         _position = position;
      }
      virtual ~MemoryReader() = default;
   };

   // --- MemoryWriter ---
   class MemoryWriter : public StreamWriter
   {
   protected:
      MemoryBase* _memory;
      pos_t       _position;

   public:
      MemoryBase* Memory()
      {
         return _memory;
      }

      bool isOpen() const override
      {
         return _memory != nullptr;
      }

      pos_t position() const override { return _position; }

      void* address() const { return _memory->get(_position); }

      pos_t length() const override { return _position; }

      bool seek(pos_t position)
      {
         if (position <= _memory->length()) {
            _position = position;

            return true;
         }
         else return false;
      }

      void seekEOF()
      {
         _position = _memory->length();
      }

      bool write(const void* s, pos_t length) override
      {
         if (_memory->write(_position, s, length)) {
            _position += length;

            return true;
         }
         else return false;
      }

      void maskDWord(int mask)
      {
         int value = *(int*)_memory->get(_position);

         writeDWord(value | mask);
      }

      bool writeDReference(ref_t reference, pos_t value)
      {
         if (_memory->addReference(reference, _position)) {
            return writeDWord(value);
         }
         else return false;
      }
      bool writeQReference(ref_t reference, pos_t value)
      {
         if (_memory->addReference(reference, _position)) {
            return writeQWord(value);
         }
         else return false;
      }

      void insertByte(pos_t position, char value)
      {
         _memory->insert(position, &value, 1);

         if (position <= _position)
            _position += 1;
      }
      void insertDWord(pos_t position, int value)
      {
         _memory->insert(position, &value, 4);

         if (position <= _position)
            _position += 4;
      }
      void insert(pos_t position, void* value, pos_t length)
      {
         _memory->insert(position, value, length);

         if (position <= _position)
            _position += length;
      }

      void align(pos_t alignment, unsigned char c)
      {
         pos_t aligned = elena_lang::align(_position, alignment);

         writeBytes(c, aligned - _position);
      }

      MemoryWriter(MemoryBase* memory)
      {
         _memory = memory;
         _position = _memory ? _memory->length() : 0;
      }
      MemoryWriter(MemoryBase* memory, pos_t position)
      {
         _memory = memory;
         _position = position;
      }
   };

   // --- StringTextWriter ---
   template<class T, size_t Size> class StringTextWriter : public TextWriter<T>
   {
      T*    _text;
      pos_t _offset;

   public:
      bool isOpen() const override { return _text != nullptr; }

      pos_t position() const override { return _offset; }

      bool seek(pos_t position)
      {
         _offset = position;

         return true;
      }

      bool writeNewLine() override
      {
         T ch = '\n';

         return write(&ch, 1);
      }

      bool write(const T* s, pos_t length)
      {
         size_t lenToWrite = Size - _offset;
         if (StrConvertor::copy(_text + _offset, s, length, lenToWrite)) {
            seek(_offset + (pos_t)lenToWrite);

            return true;
         }
         else return false;
      }

      void reset()
      {
         _offset = 0;
      }

      StringTextWriter(T* text)
      {
         _text = text;
         _offset = 0;
      }
   };

   // --- StringTextReader ---
   template<class T> class StringTextReader : public TextReader<T>
   {
      const T* _text;
      pos_t    _offset;
      pos_t    _length;

   public:
      bool isOpen() const override { return _text != nullptr; }

      pos_t position() const override { return _offset; }

      bool read(char* s, pos_t length) override
      {
         if (_offset + length > _length) {
            length = _length - _offset;
         }
         if (length != 0) {
            size_t eol = StrUtil::findChar(_text + _offset, '\n', length);
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
            StrConvertor::copy(s, _text + _offset, tmpLength, tmpLength);
            length = (pos_t)tmpLength;

            s[length] = 0;

            _offset += length;

            return true;
         }
         else return false;
      }

      void reset() override
      {
         _offset = 0;
      }

      StringTextReader(const T* string)
      {
         _text = string;
         _offset = 0;
         _length = getlength_pos(string);
      }
   };


   // --- DumpReader ---

   class DumpReader : public StreamReader
   {
      void* _dump;
      pos_t  _offset;
      pos_t  _length;

   public:
      bool eof() override { return _offset >= _length; }

      pos_t position() const override { return _offset; }

      pos_t length() const override { return _length; }

      bool seek(pos_t position) override
      {
         _offset = position;

         return true;
      }

      virtual const char* getLiteral(const char*)
      {
         const char* s = (char*)_dump + _offset;

         seek(_offset + getlength_pos(s) + 1);

         return s;

      }
      virtual const wide_c* getLiteral(const wide_c*)
      {
         const wide_c* s = (const wide_c*)((char*)_dump + _offset);

         seek(_offset + (getlength_pos(s) << 1) + 1);

         return s;
      }

      bool read(void* s, pos_t length) override
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

      ~DumpReader() override = default;
   };

   typedef StringTextReader<char> IdentifierTextReader;

}

#endif // STREAMS_H
