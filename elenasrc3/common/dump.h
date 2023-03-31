//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains the declaration of ELENA Engine Data Memory dump
//		classes.
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DUMP_H
#define DUMP_H

namespace elena_lang
{
   // --- MemoryDump ---

   class MemoryDump : public MemoryBase
   {
   protected:
      pos_t _total;
      pos_t _used;
      void* _buffer;

      void resize(pos_t size);

   public:
      void reserve(pos_t size);

      pos_t length() const override { return _used; }

      pos_t freeSpace() const { return _total - _used; }

      bool write(pos_t position, const void* s, pos_t length) override;

      bool read(pos_t position, void* s, pos_t length) const override;

      void* get(pos_t position) const override;

      ref_t getRef(pos_t position)
      {
         ref_t retVal = 0;
         read(position, &retVal, sizeof(ref_t));

         return retVal;
      }

      pos_t getPos(pos_t position) const
      {
         pos_t retVal = 0;
         read(position, &retVal, sizeof(pos_t));

         return retVal;
      }

      void writePos(pos_t position, pos_t value)
      {
         write(position, &value, sizeof(pos_t));
      }

      void writeUInt(pos_t position, unsigned int value)
      {
         write(position, &value, sizeof(unsigned int));
      }
      void writeInt(pos_t position, int value)
      {
         write(position, &value, sizeof(int));
      }

      bool writeBytes(pos_t position, char value, pos_t length)
      {
         if (position <= _used && length > 0) {
            resize(position + length);

            memset(static_cast<char*>(_buffer) + position, value, length);

            return true;
         }
         else return false;
      }

      bool insert(pos_t position, const void* s, pos_t length) override;

      void load(StreamReader& reader, pos_t length);

      void clear()
      {
         _used = 0;
      }

      void trim(pos_t position)
      {
         if (position < _used) {
            _used = position;
         }
      }
      void trimLong(size_t position)
      {
         if (position < (size_t)_used) {
            _used = (pos_t)position;
         }
      }

      MemoryDump();
      MemoryDump(pos_t capacity);
      MemoryDump(const MemoryDump& copy);
      virtual ~MemoryDump()
      {
         freestr((char*)_buffer);
      }
   };

   // --- ByteArray ---
   class ByteArray : public MemoryBase
   {
      char* _bytes;
      pos_t _length;

   public:
      pos_t length() const override
      {
         return _length;
      }

      void* get(pos_t position) const override;

      bool write(pos_t position, const void* s, pos_t length) override;

      bool read(pos_t position, void* s, pos_t length) const override;

      bool insert(pos_t position, const void* s, pos_t length) override;

      void trim(pos_t position) override;

      ByteArray(void* bytes, pos_t length)
      {
         _bytes = (char*)bytes;
         _length = length;
      }
   };

}

#endif // DUMP_H