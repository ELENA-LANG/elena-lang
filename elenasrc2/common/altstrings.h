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

class ident_t
{
private:
   const char* _string;

public:
   operator const char*() const { return _string; }

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
   int findLast(char c, int defValue = -1);

   int findSubStr(int index, char c, size_t length, int defValue);

   int toInt();
   int toInt(int index);

   long toLong(int radix);
   long toLong(int radix, int index);
   long toULong(int radix, int index = 0);
   long long toULongLong(int radix, int index = 0);

   double toDouble(int index = 0);

   char* clone();
   char* clone(int index);

   bool compare(const char* s) const;
   bool compare(const char* s, size_t length) const;

   bool greater(const char* s);

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

   int find(wide_c c, int defValue = -1);
   int findLast(wide_c c, int defValue = -1);

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
         char tmp = s[start];
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
      __copy(_string, s, length, length);
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

//   void insert(const T* s, size_t index)
//   {
//      StringHelper::insert(_string, index, s);
//   }

   void append(const T* s, size_t length)
   {
      size_t newLength = getlength(_string) + length;
      if (newLength < size) {
         __append(_string, s, length);
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

      intToStr(n, _string + pos, 10);
   }

   void copyInt(int n)
   {
      intToStr(n, _string, 10);
   }

   void appendHex(int n)
   {
      int pos = getlength(_string);

      intToStr(n, _string + pos, 16);

      __upper(_string + pos);
   }

//   void appendDouble(double n)
//   {
//      int pos = getlength(_string);
//
//      StringHelper::doubleToStr(n, 8, _string + pos);
//      if (_string[getlength(_string) - 1]=='.')
//         append("0");
//   }
//
//   void appendHex64(long long n)
//   {
//      int pos = getlength(_string);
//
//      StringHelper::longlongToStr(n, _string + pos, 16);
//      StringHelper::upper(_string + pos);
//   }
//
//   void appendInt64(long long n)
//   {
//      int pos = getlength(_string);
//
//      StringHelper::longlongToStr(n, _string + pos, 10);
//      StringHelper::upper(_string + pos);
//   }

   void lower()
   {
      __lower(_string);
   }

//   void upper()
//   {
//      StringHelper::upper(_string);
//   }

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

//   void assignOrCopy(const T* value, T* &ptr, size_t& size)
//   {
//      ptr = value;
//      size = 0;
//   }

   void create(T*, size_t size)
   {
      if (_size == 0) {
         _size = align(size, pageSize);
         _string = __allocate(_size, (const T*)NULL);
         _string[0] = 0;
      }
      else {
         int length = getlength(_string);

         _size = align(size, pageSize);
         _string = __reallocate(_string, _size);

         _string[length] = 0;
      }
   }

public:
   operator const T*() const { return _string; }

   operator T*() { return _string; }

   T& operator[](size_t index)
   {
      return *(_string + index);
   }

   T& operator[](int index)
   {
      return *(_string + index);
   }

//   bool isEmpty() const { return emptystr(_string); }
//
//   int findLast(T ch)
//   {
//      return StringHelper::findLast(_string, ch, -1);
//   }
//
//   int find(T ch)
//   {
//      return StringHelper::find(_string, ch, -1);
//   }
//
//   int find(int index, T ch)
//   {
//      return StringHelper::find(_string + index, ch, -1);
//   }
//
//   bool compare(const T* s)
//   {
//      return StringHelper::compare(_string, s);
//   }

   void copy(const T* s, size_t length)
   {
      if (_size <= length) {
         create(_string, length + 1);
      }
      __copy(_string, s, length, length);
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

      __append(_string, s, length);
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

//   void lower()
//   {
//      StringHelper::lower(_string);
//   }
//
//   void trim(T ch)
//   {
//      StringHelper::trim(_string, ch);
//   }
//
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
//
//   void clear()
//   {
//      if (_string)
//         _string[0] = 0;
//   }
//
//   size_t Length()
//   {
//      return getlength(_string);
//   }

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

void __copy(char* dest, const char* sour, size_t sourLength, size_t& destLength);
void __copy(char* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
void __copy(wide_c* dest, const char* sour, size_t sourLength, size_t& destLength);
void __copy(wide_c* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
void __copy(char* dest, const unic_c* sour, size_t sourLength, size_t& destLength);
void __copy(unic_c* dest, const char* sour, size_t sourLength, size_t& destLength);
void __append(char* dest, const char* sour, size_t length);
void __append(wide_c* dest, const wide_c* sour, size_t length);

char* __allocate(size_t size, const char* value);
char* __reallocate(char* s, size_t size);
wchar_t* __allocate(size_t size, const wchar_t* value);
wchar_t* __reallocate(wchar_t* s, size_t size);

char* __lower(char* s);
wchar_t* __lower(wide_c* s);
char* __upper(char* s);
wchar_t* __upper(wide_c* s);

inline void copystr(char* d, const char* s)
{
   size_t len = strlen(s);
   ((ident_t)s).copyTo(d, len, len);
   d[len] = 0;
}

} // _ELENA_

#endif // altstringH
