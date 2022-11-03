//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains UTF8 String classes declarations
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef USTRING_H
#define USTRING_H

namespace elena_lang
{
   #define UNI_MAX_BMP           (unsigned int)0x0000FFFF
   #define UNI_REPLACEMENT_CHAR  (unsigned int)0x0000FFFD
   #define UNI_MAX_UTF16         (unsigned int)0x0010FFFF
   #define UNI_SUR_HIGH_START    (unsigned int)0xD800
   #define UNI_SUR_HIGH_END      (unsigned int)0xDBFF
   #define UNI_SUR_LOW_START     (unsigned int)0xDC00
   #define UNI_SUR_LOW_END       (unsigned int)0xDFFF
   #define UNI_MAX_LEGAL_UTF32   (unsigned int)0x0010FFFF

   constexpr auto NOTFOUND_POS = (size_t)-1;

   // --- StrConvertor ---
   class StrConvertor
   {
   public:
      static bool copy(char* dest, const char* sour, size_t sourLength, size_t& destLength);
      static bool copy(char* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
      static bool copy(wide_c* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
      static bool copy(wide_c* dest, const char* sour, size_t sourLength, size_t& destLength);

      static bool copy(char* dest, const unic_c* sour, size_t sourLength, size_t& destLength);
      static bool copy(unic_c* dest, const char* sour, size_t sourLength, size_t& destLength);

      static int toInt(const char* s, int radix);
      static int toInt(const wide_c* s, int radix);

      static unsigned int toUInt(const char* s, int radix);
   };

   // --- StrUtil ---
   class StrUtil
   {
   public:
      static char* clone(const char* s);
      static wide_c* clone(const wide_c* s);

      static void move(char* s1, const char* s2, size_t length);
      static void move(wide_c* s1, const wide_c* s2, size_t length);

      static void append(char* dest, const char* sour, size_t length);
      static void append(wide_c* dest, const wide_c* sour, size_t length);

      static void insert(char* s, size_t pos, size_t length, const char* subs);

      static char* lower(char* s);
      static wide_c* lower(wide_c* s);
      static char lower(char s);
      static wide_c lower(wide_c s);

      static size_t findChar(const char* s, char ch, size_t length, size_t defaultValue = NOTFOUND_POS);
   };

   // --- StrFactory ---
   class StrFactory
   {
   public:
      static char* allocate(size_t size, const char* value);
      static char* reallocate(char* s, size_t size);

      static wide_c* allocate(size_t size, const wide_c* value);
      static wide_c* reallocate(wide_c* s, size_t size);
   };

   // --- ustr_t ---

   class ustr_t
   {
   private:
      const char* _string;

   public:
      operator const char* () const { return _string; }

      ustr_t& operator +=(size_t offset)
      {
         _string += offset;

         return *this;
      }

      bool operator ==(const char* val) const
      {
         return compare(val);
      }
      bool operator ==(ustr_t val) const
      {
         return compare(val);
      }
      bool operator !=(ustr_t val) const
      {
         return !compare(val);
      }

      const char* str() const { return _string; }

      size_t length() const { return getlength(_string); }
      pos_t length_pos() const { return getlength_pos(_string); }
      int length_int() const { return getlength_int(_string); }

      bool empty() const { return getlength(_string) == 0;  }

      bool compare(const char* s) const;
      bool compare(const char* s, size_t length) const;
      bool compareSub(const char* s, size_t index, size_t length) const;

      bool greater(const char* s) const;

      bool startsWith(const char* s);
      bool endsWith(const char* s);

      size_t find(char c, size_t defValue = NOTFOUND_POS);
      size_t findSub(size_t index, char c, size_t defValue = NOTFOUND_POS);
      size_t findSub(size_t index, char c, size_t length, size_t defValue);

      size_t findLast(char c, size_t defValue = NOTFOUND_POS);
      size_t findLastSub(size_t index,  char c, size_t defValue = NOTFOUND_POS);

      size_t findStr(const char* subs, size_t defValue = NOTFOUND_POS);
      size_t findSubStr(size_t index, const char* s, size_t length, size_t defValue = NOTFOUND_POS);

      char* clone();
      char* clone(size_t index, size_t length);

      bool copyTo(char* dest, size_t length, size_t& destLength);
      bool copyTo(wide_c* dest, size_t length, size_t& destLength);
      bool copyTo(char* dest, size_t& destLength)
      {
         return copyTo(dest, getlength(_string), destLength);
      }
      bool copyTo(wide_c* dest, size_t& destLength)
      {
         return copyTo(dest, getlength(_string), destLength);
      }

      void free()
      {
         freestr((char*)_string);
         _string = nullptr;
      }

      ustr_t()
      {
         _string = nullptr;
      }
      ustr_t(const char* s)
      {
         _string = s;
      }
   };

   // --- wstr_t ---

   class wstr_t
   {
   private:
      const wide_c* _string;

   public:
      operator const wide_c* () const { return _string; }

      wstr_t& operator +=(size_t offset)
      {
         _string += offset;

         return *this;
      }
      bool operator ==(wstr_t key) const
      {
         return compare(key);
      }
      bool operator !=(wstr_t key) const
      {
         return !compare(key);
      }

      const wide_c* str() const { return _string; }

      size_t length() const { return getlength(_string); }
      pos_t length_pos() const { return getlength_pos(_string); }
      int length_int() const { return getlength_int(_string); }

      bool empty() const { return getlength(_string) == 0; }

      bool compare(const wide_c* s) const;
      bool compareSub(const wide_c* s, size_t index, size_t length) const;

      bool startsWith(const wide_c* s);
      bool endsWith(const wide_c* s);

      size_t find(wide_c c, size_t defValue = NOTFOUND_POS);
      size_t findSub(size_t index, char c, size_t defValue = NOTFOUND_POS);
      size_t findSub(size_t index, char c, size_t length, size_t defValue);

      size_t findStr(const wide_c* subs, size_t defValue = NOTFOUND_POS);
      size_t findSubStr(size_t index, const wide_c* s, size_t length, size_t defValue = NOTFOUND_POS);

      size_t findLast(wide_c c, size_t defValue = NOTFOUND_POS);
      size_t findLastSub(size_t index, char c, size_t defValue = NOTFOUND_POS);

      wide_c* clone();
      wide_c* clone(size_t index, size_t length);

      bool copyTo(char* dest, size_t length, size_t& destLength);
      bool copyTo(wide_c* dest, size_t length, size_t& destLength);
      bool copyTo(char* dest, size_t& destLength)
      {
         return copyTo(dest, getlength(_string), destLength);
      }
      bool copyTo(wide_c* dest, size_t& destLength)
      {
         return copyTo(dest, getlength(_string), destLength);
      }

      void free()
      {
         freestr((wide_c*)_string);
         _string = nullptr;
      }

      wstr_t()
      {
         _string = nullptr;
      }
      wstr_t(const wide_c* s)
      {
         _string = s;
      }
   };

   // --- String ---

   template <class T, size_t size> class String
   {
   protected:
      T _string[size + 1];

   public:
      static bool intToStr(int n, T* s, int radix, size_t maxLength)
      {
         int    rem = 0;
         size_t pos = 0;
         size_t start = 0;
         if (n < 0) {
            start++;
            n = -n;
            s[pos++] = '-';
         }

         do
         {
            if (pos >= maxLength)
               return false;

            rem = (unsigned int)n % radix;
            n /= radix;
            switch (rem) {
            case 10:
               s[pos++] = 'a';
               break;
            case 11:
               s[pos++] = 'b';
               break;
            case 12:
               s[pos++] = 'c';
               break;
            case 13:
               s[pos++] = 'd';
               break;
            case 14:
               s[pos++] = 'e';
               break;
            case 15:
               s[pos++] = 'f';
               break;
            default:
               if (rem < 10) {
                  s[pos++] = (T)(rem + 0x30);
               }
            }
         } while (n != 0);

         s[pos] = 0;
         pos--;
         while (start < pos) {
            T tmp = s[start];
            s[start++] = s[pos];
            s[pos--] = tmp;
         }

         return true;
      }
      static bool uintToStr(unsigned int n, T* s, int radix, size_t maxLength)
      {
         int    rem = 0;
         size_t pos = 0;
         size_t start = 0;

         do
         {
            if (pos >= maxLength)
               return false;

            rem = (unsigned int)n % radix;
            n /= radix;
            switch (rem) {
            case 10:
               s[pos++] = 'a';
               break;
            case 11:
               s[pos++] = 'b';
               break;
            case 12:
               s[pos++] = 'c';
               break;
            case 13:
               s[pos++] = 'd';
               break;
            case 14:
               s[pos++] = 'e';
               break;
            case 15:
               s[pos++] = 'f';
               break;
            default:
               if (rem < 10) {
                  s[pos++] = (T)(rem + 0x30);
               }
            }
         } while (n != 0);

         s[pos] = 0;
         pos--;
         while (start < pos) {
            T tmp = s[start];
            s[start++] = s[pos];
            s[pos--] = tmp;
         }

         return true;
      }

      T& operator[](size_t index)
      {
         return *(_string + index);
      }

      const T* str() const { return _string; }

      size_t length() const { return getlength(_string); }

      pos_t length_pos() const { return getlength_pos(_string); }

      bool empty() const { return getlength(_string) == 0; }

      bool copy(const T* s, size_t length)
      {
         if (StrConvertor::copy(_string, s, length, length)) {
            _string[length] = 0;

            return true;
         }
         else return false;
      }
      bool append(const T* s, size_t length)
      {
         size_t newLength = getlength(_string) + length;
         if (newLength < size) {
            StrUtil::append(_string, s, length);
            _string[newLength] = 0;

            return true;
         }
         else return false;
      }
      bool append(const T* s)
      {
         return append(s, getlength(s));
      }
      bool append(T c)
      {
         return append(&c, 1);
      }

      bool appendInt(int value, int radix = 10)
      {
         size_t pos = getlength(_string);

         return intToStr(value, _string + pos, radix, size - pos);
      }

      bool appendUInt(int value, int radix = 10)
      {
         size_t pos = getlength(_string);

         return uintToStr(value, _string + pos, radix, size - pos);
      }

      bool copy(const T* s)
      {
         const size_t length = getlength(s);
         if (length == 0) {
            _string[0] = 0;

            return true;
         }
         else if (length < size) {
            return copy(s, length);
         }
         else return false;
      }

      void insert(const T* s, size_t index)
      {
         StrUtil::insert(_string, index, getlength(s), s);
      }

      void cut(size_t index, size_t length)
      {
         StrUtil::move(_string + index, _string + index + length, getlength(_string) - index - length + 1);
      }

      void replaceAll(T oldCh, T newCh, size_t index)
      {
         for (size_t i = index; i < getlength(_string); i++) {
            if (_string[i] == oldCh)
               _string[i] = newCh;
         }
      }

      void lower()
      {
         StrUtil::lower(_string);
      }

      void trim(T ch)
      {
         size_t length = getlength(_string);
         while (length != 0 && _string[length - 1] == ch) {
            _string[length - 1] = 0;
            --length;
         }
      }

      void truncate(size_t pos)
      {
         _string[pos] = 0;
      }

      void clear()
      {
         _string[0] = 0;
      }

      int toInt(int radix = 10) const
      {
         return StrConvertor::toInt(_string, radix);
      }

      int toUInt(int radix = 10) const
      {
         return StrConvertor::toUInt(_string, radix);
      }

      String()
      {
         _string[0] = 0;
      }
      String(const T* s)
      {
         copy(s);
      }
      String(const T* s, size_t length)
      {
         copy(s, length);
      }
   };

   // --- DynamicString ---
   template <class T, size_t pageSize = 0x40> class DynamicString
   {
   protected:
      T*     _string;
      size_t _size;

      void create(size_t size)
      {
         if (_size == 0) {
            _size = alignSize(size, pageSize);
            _string = StrFactory::allocate(_size, (const T*)nullptr);
            _string[0] = 0;
         }
         else {
            size_t length = getlength(_string);

            _size = alignSize(size, pageSize);
            _string = StrFactory::reallocate(_string, _size);

            _string[length] = 0;
         }
      }

   public:
      T& operator[](size_t index)
      {
         return *(_string + index);
      }

      const T* str() const { return _string; }

      bool empty() const { return getlength(_string) == 0; }

      size_t length() const { return getlength(_string); }

      pos_t length_pos() const { return getlength_pos(_string); }

      void copy(const T* s, size_t length)
      {
         if (_size <= length) {
            create(length + 1);
         }

         StrConvertor::copy(_string, s, length, length);
         _string[length] = 0;
      }
      void copy(const T* s)
      {
         copy(s, getlength(s));
      }

      void append(const T* s, size_t length)
      {
         size_t newLength = getlength(_string) + length;
         if (_size <= newLength)
            create(newLength + 1);

         StrUtil::append(_string, s, length);
         _string[newLength] = 0;
      }
      void append(const T* s)
      {
         size_t length = getlength(s);
         if (length > 0) {
            append(s, length);
         }
      }
      void append(T c)
      {
         append(&c, 1);
      }

      void insert(const T* s, size_t index)
      {
         StrUtil::insert(_string, index, getlength(s), s);
      }

      int toInt(int radix = 10) const
      {
         return StrConvertor::toInt(_string, radix);
      }

      void clear()
      {
         if (_string)
            _string[0] = 0;
      }

      DynamicString()
      {
         _size = 0;
         _string = nullptr;
      }
   };

}

#endif // USTRING_H
