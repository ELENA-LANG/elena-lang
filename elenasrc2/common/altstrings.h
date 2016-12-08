//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains String classes declarations
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef altstringsH
#define altstringsH

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
   static void insert(char* s, int pos, int length, const char* subs);

   static char* lower(char* s);
   static wide_c* lower(wide_c* s);
   static char lower(char s);
   static wide_c lower(wide_c s);
   static char* upper(char* s);
   static wide_c* upper(wide_c* s);
};

class ident_t
{
private:
   const char* _string;

public:
   operator const char*() const { return _string; }

   const char* c_str() const { return _string; }

   ident_t& operator +=(int offset)
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

   int find(const char* s, int defValue = -1);
   int find(char c, int defValue = -1);
   int find(int index, char ch, int defValue);
   int findLast(char c, int defValue = -1);
   int findLast(int index, char c, int defValue);

   int findSubStr(int index, char c, size_t length, int defValue);
   int findSubStr(int index, const char* s, int defValue = -1);

   int toInt();
   int toInt(int index);

   long toLong(int radix);
   long toLong(int radix, int index);
   long toULong(int radix, int index = 0);
   long long toULongLong(int radix, int index = 0);

   double toDouble(int index = 0);

   char* clone();
   char* clone(int index);
   char* clone(int index, int length);

   bool compare(const char* s) const;
   bool compare(const char* s, size_t length) const;
   bool compare(const char* s, int start, size_t length);

   bool greater(const char* s);

   bool endsWith(const char* s);
   bool startsWith(const char* s);

   ident_t()
   {
      _string = NULL;
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

   wide_t& operator +=(int offset)
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

   int find(wide_c c, int defValue = -1);
   int find(int index, wide_c ch, int defValue);

   int findLast(wide_c c, int defValue = -1);
   int findLast(int index, wide_c c, int defValue);

   int findSubStr(int index, wide_c c, size_t length, int defValue);

   int toInt();
   int toInt(int index);

   long toLong(int radix);
   long toLong(int radix, int index);

   wide_c* clone();
   wide_c* clone(int index);

   bool compare(const wide_c* s) const;
   bool compare(const wide_c* s, size_t length) const;

   bool greater(const wide_c* s);

   wide_t()
   {
      _string = NULL;
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

   const T* str() const { return _string; }

   T& operator[](size_t index)
   {
      return *(_string + index);
   }

   T& operator[](int index)
   {
      return *(_string + index);
   }

//   void replaceAll(T oldCh, T newCh, int index)
//   {
//      for (size_t i = index ; i < getlength(_string) ; i++) {
//         if (_string[i] == oldCh)
//            _string[i] = newCh;
//      }
//   }

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
      int pos = getlength(_string);

      BaseString<T>::intToStr(n, _string + pos, 10);
   }

   void copyInt(int n)
   {
      BaseString<T>::intToStr(n, _string, 10);
   }

   void appendHex(int n)
   {
      int pos = getlength(_string);

      BaseString<T>::uintToStr(n, _string + pos, 16);

      StrHelper::upper(_string + pos);
   }

   void appendDouble(double n)
   {
      int pos = getlength(_string);

      Convertor::doubleToStr(n, 8, _string + pos);
      if (_string[getlength(_string) - 1]=='.')
         append("0");
   }

   void appendHex64(long long n)
   {
      int pos = getlength(_string);

      Convertor::longlongToStr(n, _string + pos, 16);
      StrHelper::upper(_string + pos);
   }

   void appendInt64(long long n)
   {
      int pos = getlength(_string);

      Convertor::longlongToStr(n, _string + pos, 10);
      StrHelper::upper(_string + pos);
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
};

// --- DynamicString ---

template <class T, size_t pageSize = 0x20> class DynamicString : BaseString<T>
{
protected:
   T*     _string;
   size_t _size;

   void assignOrCopy(const T* value, T* &ptr, size_t& size)
   {
      ptr = value;
      size = 0;
   }

   void create(T*, size_t size)
   {
      if (_size == 0) {
         _size = align(size, pageSize);
         _string = StrFactory::allocate(_size, (const T*)NULL);
         _string[0] = 0;
      }
      else {
         int length = getlength(_string);

         _size = align(size, pageSize);
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
         create(_string, length + 1);
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
      size_t newLength = getlength(_string) + length + 1;
      if (newLength > _size)
         create(_string, newLength);

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
      size_t slen = getlength(s);
      size_t len = getlength(_string);
      if (slen + len > _size) {
         create(_string, slen + len + 1);
      }

      StrHelper::insert(_string, index, slen, s);
   }

   void cut(size_t index, int length)
   {
      StrHelper::move(_string + index, _string + index + length, length);
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

   size_t Length()
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
