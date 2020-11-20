//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains String classes declarations
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef altstringsH
#define altstringsH

#define NOTFOUND_POS (size_t)-1

namespace _ELENA_
{

class Convertor
{
public:
   static bool copy(char* dest, const char* sour, size_t sourLength, size_t& destLength);
   static bool copy(char* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
   static bool copy(wide_c* dest, const char* sour, size_t sourLength, size_t& destLength);
   static bool copy(wide_c* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
   static bool copy(char* dest, const unic_c* sour, size_t sourLength, size_t& destLength);
   static bool copy(unic_c* dest, const char* sour, size_t sourLength, size_t& destLength);

   static char* doubleToStr(double value, int digit, char* s);
   static wide_c* doubleToStr(double value, int digit, wide_c* s);

   static char* longlongToStr(long long n, char* s, int radix);
   static wide_c* longlongToStr(long long n, wide_c* s, int radix);

   static char* intToStr(int n, char* s, int radix);
   static int strToInt(const char* s);
};

class StrFactory
{
public:
   static char* allocate(size_t size, const char* value);
   static char* reallocate(char* s, size_t size);
   static wide_c* allocate(size_t size, const wide_c* value);
   static wide_c* reallocate(wide_c* s, size_t size);

   static char* clone(const char* s);
   static wide_c* clone(const wide_c* s);
};

class StrHelper
{
public:
   static void move(char* s1, const char* s2, size_t length);
   static void move(wide_c* s1, const wide_c* s2, size_t length);
   static void append(char* dest, const char* sour, size_t length);
   static void append(wide_c* dest, const wide_c* sour, size_t length);
   static void insert(char* s, size_t pos, size_t length, const char* subs);

   static char* lower(char* s);
   static wide_c* lower(wide_c* s);
   static char lower(char s);
   static wide_c lower(wide_c s);
   static char* upper(char* s);
   static wide_c* upper(wide_c* s);

   static size_t findChar(const char* s, char ch, size_t length, size_t defaultValue = NOTFOUND_POS);
};

class ident_t
{
private:
   const char* _string;

public:
   operator const char*() const { return _string; }

   const char* c_str() const { return _string; }

   ident_t& operator +=(size_t offset)
   {
      _string += offset;

      return *this;
   }

   bool empty() const { return emptystr(_string); }

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

   size_t find(const char* s, size_t defValue = NOTFOUND_POS);
   size_t find(char c, size_t defValue = NOTFOUND_POS);
   size_t find(size_t index, char ch, size_t defValue);
   size_t findLast(char c, size_t defValue = NOTFOUND_POS);
   size_t findLast(size_t index, char c, size_t defValue);

   size_t findSubStr(size_t index, char c, size_t length, size_t defValue);
   size_t findSubStr(size_t index, const char* s, size_t defValue = NOTFOUND_POS);

   size_t findLastSubStr(size_t index, char c, size_t length, size_t defValue);

   int toInt();
   int toInt(size_t index);

   long toLong(int radix);
   long toLong(int radix, size_t index);
   long toULong(int radix, size_t index = 0);
   long long toLongLong(int radix, size_t index = 0);

   double toDouble(size_t index = 0);

   char* clone();
   char* clone(size_t index);
   char* clone(size_t index, size_t length);

   bool compare(const char* s) const;
   bool compare(const char* s, size_t length) const;
   bool compare(const char* s, size_t start, size_t length);

   bool greater(const char* s);

   bool endsWith(const char* s);
   bool startsWith(const char* s);

   ident_t()
   {
      _string = nullptr;
   }
   ident_t(const char* s)
   {
      _string = s;
   }
};

class wide_t
{
private:
   const wide_c* _string;

public:
   operator const wide_c*() const { return _string; }

   //wide_t& operator +=(int offset)
   //{
   //   _string += offset;

   //   return *this;
   //}

   wide_t& operator +=(size_t offset)
   {
      _string += offset;

      return *this;
   }

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

   size_t find(wide_c c, size_t defValue = -1);
   size_t find(size_t index, wide_c ch, size_t defValue);

   size_t findLast(wide_c c, size_t defValue = -1);
   size_t findLast(size_t index, wide_c c, size_t defValue);

   size_t findSubStr(size_t index, wide_c c, size_t length, size_t defValue);

   int toInt();
   int toInt(size_t index);

   long toLong(int radix);
   long toLong(int radix, size_t index);

   wide_c* clone();
   wide_c* clone(size_t index);
   wide_c* clone(size_t index, size_t length);

   bool compare(const wide_c* s) const;
   bool compare(const wide_c* s, size_t length) const;

   bool greater(const wide_c* s);

   wide_t()
   {
      _string = nullptr;
   }
   wide_t(const wide_c* s)
   {
      _string = s;
   }
};

// --- String ---

template <class T> class BaseString
{
protected:
   static void intToStr(int n, T* s, int radix)
   {
      int  rem = 0;
      int  pos = 0;
      int start = 0;
      if (n < 0) {
         start++;
         n = -n;
         s[pos++] = '-';
      }

      do
      {
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
               s[pos++] = (char)(rem + 0x30);
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
   }
   static void uintToStr(size_t n, T* s, int radix)
   {
      int  rem = 0;
      int  pos = 0;
      int start = 0;

      do
      {
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
               s[pos++] = (char)(rem + 0x30);
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
   }
};

template <class T, size_t size> class String : BaseString<T>
{
protected:
   T _string[size + 1];

public:
   operator const T*() const { return _string; }

   operator T*() { return _string; }

   const T* c_str() const { return _string; }

//   T& operator[](size_t index)
//   {
//      return *(_string + index);
//   }
//
//   T& operator[](int index)
//   {
//      return *(_string + index);
//   }

   void replaceAll(T oldCh, T newCh, size_t index)
   {
      for (size_t i = index ; i < getlength(_string) ; i++) {
         if (_string[i] == oldCh)
            _string[i] = newCh;
      }
   }

   void copy(const T* s, size_t length)
   {
      Convertor::copy(_string, s, length, length);
      _string[length] = 0;
   }
   void copy(const T* s)
   {
      size_t length = getlength(s);
      if (length == 0)
         _string[0] = 0;
      else if (length < size) {
         copy(s, length);
      }
   }

   void insert(const T* s, size_t index)
   {
      StrHelper::insert(_string, index, getlength(s), s);
   }

   void insert(const T* s, size_t index, int length)
   {
      StrHelper::insert(_string, index, length, s);
   }

   void append(const T* s, size_t length)
   {
      size_t newLength = getlength(_string) + length;
      if (newLength < size) {
         StrHelper::append(_string, s, length);
         _string[newLength] = 0;
      }
   }
   void append(const T* s)
   {
      size_t length = getlength(s);
      if (length > 0)
         append(s, length);
   }
   void append(T c)
   {
      append(&c, 1);
   }

   void appendInt(int n)
   {
      size_t pos = getlength(_string);

      BaseString<T>::intToStr(n, _string + pos, 10);
   }

   void copyInt(int n)
   {
      BaseString<T>::intToStr(n, _string, 10);
   }

   void copyHex(int n)
   {
      BaseString<T>::uintToStr(n, _string, 16);
   }

   void appendHex(int n)
   {
      size_t pos = getlength(_string);

      BaseString<T>::uintToStr(n, _string + pos, 16);

      StrHelper::upper(_string + pos);
   }

   void appendDouble(double n)
   {
      size_t pos = getlength(_string);

      Convertor::doubleToStr(n, 10, _string + pos);
      if (_string[getlength(_string) - 1]=='.')
         append("0");
   }

   void appendHex64(long long n)
   {
      size_t pos = getlength(_string);

      Convertor::longlongToStr(n, _string + pos, 16);
      StrHelper::upper(_string + pos);
   }

   void appendInt64(long long n)
   {
      size_t pos = getlength(_string);

      Convertor::longlongToStr(n, _string + pos, 10);
      StrHelper::upper(_string + pos);
   }

   void appendSize(size_t n)
   {
#ifdef _WIN64
      appendHex64(n);
#else
      appendHex(n);
#endif
   }

   void lower()
   {
      StrHelper::lower(_string);
   }

   void upper()
   {
      StrHelper::upper(_string);
   }

   void trim(T ch)
   {
      size_t length = getlength(_string);
      while (length > 0 && _string[length - 1] == ch) {
         _string[length - 1] = 0;
         length = getlength(_string);
      }
   }

   void clear()
   {
      _string[0] = 0;
   }

   void truncate(size_t pos)
   {
      _string[pos] = 0;
   }

   size_t Length() const
   {
      return getlength(_string);
   }

   void cut(size_t index, size_t length)
   {
      StrHelper::move(_string + index, _string + index + length, getlength(_string) - index - length + 1);
   }

   String()
   {
      _string[0] = 0;
   }
   String(const T* s)
   {
      copy(s);
   }
   String(const T* s1, const T* s2)
   {
      copy(s1);
      append(s2);
   }
   String(const T* s1, const T* s2, const T* s3)
   {
      copy(s1);
      append(s2);
      append(s3);
   }
   String(const T* s1, const T* s2, const T* s3, const T* s4)
   {
      copy(s1);
      append(s2);
      append(s3);
      append(s4);
   }
   String(const T* s, size_t length)
   {
      copy(s, length);
      _string[length] = 0;
   }
   String(const T* s, size_t index, size_t length)
   {
      copy(s + index, length);
      _string[length] = 0;
   }
};

// --- DynamicString ---

template <class T, size_t pageSize = 0x20> class DynamicString : BaseString<T>
{
protected:
   T*     _string;
   size_t _size;

   void assignOrCopy(const T* value, T* &ptr, size_t& size)
   {
      ptr = (T*)value;
      size = 0;
   }

   void create(size_t size)
   {
      if (_size == 0) {
         _size = alignSize(size, pageSize);
         _string = StrFactory::allocate(_size, (const T*)NULL);
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
   operator const T*() const { return _string; }

   operator T*() { return _string; }

   const char* str() const { return _string; }

   T& operator[](size_t index)
   {
      return *(_string + index);
   }

   T& operator[](int index)
   {
      return *(_string + index);
   }

//   bool isEmpty() const { return emptystr(_string); }

   void copy(const T* s, size_t length)
   {
      if (_size <= length) {
         create(length + 1);
      }

      Convertor::copy(_string, s, length, length);
      _string[length] = 0;
   }
   void copy(const T* s)
   {
      size_t length = getlength(s);
      if (length == 0) {
         _string[0] = 0;
      }
      else copy(s, length);
   }

   void append(const T* s, size_t length)
   {
      size_t newLength = getlength(_string) + length;
      if (newLength >= _size)
         create(newLength + 1);

      StrHelper::append(_string, s, length);
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

   void appendInt(int n)
   {
      int pos = getlength(_string);

      intToStr(n, _string + pos, 10);
   }

   void copyInt(int n)
   {
      intToStr(n, _string, 10);
   }

   void insert(const T* s, size_t index)
   {
      if (!emptystr(s)) {
         size_t slen = getlength(s);
         size_t len = getlength(_string);
         if (slen + len >= _size) {
            create(slen + len + 1);
         }

         StrHelper::insert(_string, index, slen, s);
      }
   }

   void cut(size_t index, size_t length)
   {
      StrHelper::move(_string + index, _string + index + length, getlength(_string) - index - length + 1);
   }

   void lower()
   {
      StrHelper::lower(_string);
   }

   void upper()
   {
      StrHelper::upper(_string);
   }

   void trim(T ch)
   {
      size_t length = getlength(_string);
      while (length > 0 && _string[length - 1] == ch) {
         _string[length - 1] = 0;
         length = getlength(_string);
      }
   }

   void truncate(size_t pos)
   {
      if (_string != NULL)
         _string[pos] = 0;
   }

   char* cut()
   {
      char* value = _string;

      _string = NULL;
      _size = 0;

      return value;
   }

//   T* clone()
//   {
//      return StringHelper::clone(_string);
//   }
//
//   T* clone(int pos)
//   {
//      return StringHelper::clone(_string + pos);
//   }
//
//   int toInt()
//   {
//      return StringHelper::strToInt(_string);
//   }

   void clear()
   {
      if (_string)
         _string[0] = 0;
   }

   size_t Length() const
   {
      return getlength(_string);
   }

   DynamicString()
   {
      _size = 0;
      _string = NULL;
   }
   DynamicString(const T* value)
   {
      assignOrCopy(value, _string, _size);
   }
   DynamicString(const T* value, size_t index, size_t length)
   {
      _size = 0;
      _string = NULL;

      copy(value + index, length);
   }


   ~DynamicString() { if (_size > 0) freestr(_string); }
};

// --- HOTFIX : internal conversion routines ---

inline void copystr(char* d, const char* s)
{
   size_t len = strlen(s);
   ((ident_t)s).copyTo(d, len, len);
   d[len] = 0;
}

} // _ELENA_

#endif // altstringH
