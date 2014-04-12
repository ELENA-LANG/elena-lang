//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains String classes declarations
//
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef altstringsH
#define altstringsH

namespace _ELENA_
{

inline bool emptystr(const wchar16_t* s)
{
   return (s == NULL || s[0]==0);
}

inline bool emptystr(const char* s)
{
   return (s == NULL || s[0]==0);
}

inline static size_t getlength(const wchar_t* s)
{
   return (s==NULL) ? 0 : wcslen(s);
}

inline static size_t getlength(const unsigned short* s)
{
   const unsigned short* p = s;

   while (*p++);

   return (size_t)(p - s) >> 1;
}

inline static size_t getlength(const char* s)
{
   return (s==NULL) ? 0 : strlen(s);
}

// --- StringHelper ---

class StringHelper
{
public:
   static char* allocate(size_t size);
   static char* reallocate(char* s, size_t size);

   static wchar16_t* w_allocate(size_t size);
   static wchar16_t* w_reallocate(wchar_t* s, size_t size);

   static tchar_t* allocateText(size_t size);

   static void copy(char* dest, const char* sour, int length);
   static bool copy(wchar16_t* dest, const char* sour, size_t& length);
   static void copy(wchar16_t* dest, const wchar16_t* sour, size_t length);
   static bool copy(char* dest, const wchar16_t* sour, size_t& length);

   static wchar16_t* clone(const wchar16_t* s);
   static char* clone(const char* s);

   static void move(wchar16_t* s1, const wchar16_t* s2, size_t length);

   static void insert(wchar16_t* s, int pos, const wchar16_t* subs);

   static void append(char* dest, const char* sour, int length);
   static void append(wchar16_t* dest, const wchar16_t* sour, size_t length);
   static bool append(wchar16_t* dest, const char* sour, size_t length);
   static bool append(char* dest, const wchar16_t* sour, size_t length);

   static wchar16_t  lower(wchar16_t s);
   static wchar16_t* lower(wchar16_t* s);
   static char* lower(char* s);
   static wchar16_t* upper(wchar16_t* s);
   static char* upper(char* s);

   static void trim(char* s, char c);

   static bool compare(const wchar16_t* s1, const wchar16_t* s2);
   static bool compare(const wchar16_t* s1, const wchar16_t* s2, size_t length);
   static bool compare(const char* s1, const char* s2);
   static bool compare(const char* s1, const char* s2, size_t length);

   static bool greater(const wchar16_t* s1, const wchar16_t* s2);
   static bool greater(const wchar16_t* s1, const wchar16_t* s2, size_t length);
   static bool greater(const char* s1, const char* s2);
   static bool greater(const char* s1, const char* s2, size_t length);

   static int find(const wchar16_t* s, const wchar16_t* subs, int defValue = -1);
   static int find(const char* s, char c, int defValue = -1);
   static int find(const wchar16_t* s, wchar_t c, int defValue = -1);
   static int findLast(const wchar16_t* s, wchar16_t c, int defValue = -1);
   static int findLast(const char* s, char c, int defValue = -1);

   static int strToInt(const wchar16_t* s);
   static int strToInt(const char* s);

   static wchar16_t* intToStr(int n, wchar16_t* s, int radix);
   static char* intToStr(int n, char* s, int radix);

   static long strToLong(const wchar16_t* s, int radix);
   static long strToLong(const char* s, int radix);

   static long long strToLongLong(const wchar16_t* s, int radix);

   static double strToDouble(const wchar16_t* s);

//#ifdef _WIN32
//   static void trim(wchar_t* s, wchar_t c);

   static wchar_t* longlongToStr(long long n, wchar16_t* s, int radix);
   static wchar_t* doubleToStr(double value, int digit, wchar16_t* s);

//#else
//
//   static unsigned short* w_allocate(size_t size);
//   static unsigned short* w_reallocate(const unsigned short* s, size_t size);
//
//   static int find(const unsigned short* s, unsigned short c, int defValue = -1);
//
//   static void copy(unsigned short* dest, const unsigned short* sour, int length);
//   static bool copy(unsigned short* dest, const char* sour, size_t& length);
//   static bool copy(char* dest, const unsigned short* sour, size_t& length);
//
//   static void append(unsigned short* dest, const unsigned short* sour, int length);
//   static bool append(unsigned short* dest, const char* sour, int length);
//   static bool append(char* dest, const unsigned short* sour, int length);
//
//   static void move(unsigned short* s1, const unsigned short* s2, size_t length);
//
//   static unsigned short* clone(const unsigned short* s);
//   static char* clone(const char* s);
//
//   static unsigned short* lower(unsigned short* s);
//   static unsigned short lower(unsigned short s);
//   static unsigned short* upper(unsigned short* s);
//
////   static void append(unsigned short* dest, const unsigned short* sour, int length);
////
////   static bool convertMultiByteToWide(const char* sour, unsigned short* dest, size_t count);
//
//   static void trim(unsigned short* s, unsigned short c);
//
//   static long strToLong(const unsigned short* s, int radix);
//
//   static unsigned short* intToStr(int n, unsigned short* s, int radix);
//   static unsigned short* longlongToStr(long long n, unsigned short* s, int radix);
//   static unsigned short* doubleToStr(double value, int digit, unsigned short* s);
//
//#endif
   static wchar16_t* cloneLowered(const wchar16_t* s)
   {
      return lower(clone(s));
   }

//   static void move(char* s1, const char* s2, size_t length);
};

// --- String ---

template <class T, size_t size> class String
{
protected:
   T _string[size + 1];

public:
   operator const T*() const { return _string; }

   T& operator[](size_t index)
   {
      return *(_string + index);
   }

   T& operator[](int index)
   {
      return *(_string + index);
   }

   bool isEmpty() const { return emptystr(_string); }

   int findLast(T ch)
   {
      return StringHelper::findLast(_string, ch, -1);
   }

   int find(T ch)
   {
      return StringHelper::find(_string, ch, -1);
   }

   bool compare(const T* s)
   {
      return StringHelper::compare(_string, s);
   }

//   void replaceAll(T oldCh, T newCh, int index)
//   {
//      for (size_t i = index ; i < getlength(_string) ; i++) {
//         if (_string[i] == oldCh)
//            _string[i] = newCh;
//      }
//   }

   void copy(const wchar16_t* s, size_t length)
   {
      StringHelper::copy(_string, s, length);
      _string[length] = 0;
   }
   void copy(const wchar16_t* s)
   {
      size_t length = getlength(s);
      if (length == 0)
         _string[0] = 0;
      else if (length < size) {
         copy(s, length);
      }
   }
   void copy(const char* s, size_t length)
   {
      StringHelper::copy(_string, s, length);
      _string[length] = 0;
   }
   void copy(const char* s)
   {
      size_t length = getlength(s);
      if (length == 0)
         _string[0] = 0;
      else if (length < size) {
         copy(s, length);
      }
   }

   void append(const wchar16_t* s, size_t length)
   {
      size_t newLength = getlength(_string) + length + 1;
      if (newLength > size)
         length -= (newLength - size);

      StringHelper::append(_string, s, length);
   }
   void append(const wchar16_t* s)
   {
      size_t length = getlength(s);
      if (length > 0)
         append(s, length);
   }
   void append(const char* s, size_t length)
   {
      size_t newLength = getlength(_string) + length + 1;
      if (newLength > size)
         length -= (newLength - size);

      StringHelper::append(_string, s, length);
   }
   void append(const char* s)
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

      StringHelper::intToStr(n, _string + pos, 10);
   }

   void appendHex(int n)
   {
      int pos = getlength(_string);

      StringHelper::intToStr(n, _string + pos, 16);
      StringHelper::upper(_string + pos);
   }

   void appendLong(long n)
   {
      int pos = getlength(_string);

      StringHelper::intToStr(n, _string + pos, 10);
   }

   void appendDouble(double n)
   {
      int pos = getlength(_string);

      StringHelper::doubleToStr(n, 8, _string + pos);
      if (_string[getlength(_string) - 1]=='.')
         append("0");
   }

   void appendHex64(long long n)
   {
      int pos = getlength(_string);

      StringHelper::longlongToStr(n, _string + pos, 16);
      wcsupr(_string + pos);
   }

   void appendInt64(long long n)
   {
      int pos = getlength(_string);

      StringHelper::longlongToStr(n, _string + pos, 10);
      wcsupr(_string + pos);
   }

   void lower()
   {
      StringHelper::lower(_string);
   }

   void upper()
   {
      StringHelper::upper(_string);
   }

   void trim(T ch)
   {
      StringHelper::trim(_string, ch);
   }

//   static char* clone(const char* s)
//   {
//      return StringHelper::clone(s);
//   }

   T* clone()
   {
      return StringHelper::clone(_string);
   }

   T* clone(int pos)
   {
      return StringHelper::clone(_string + pos);
   }

   int toInt()
   {
      return StringHelper::strToInt(_string);
   }
   int toInt(int defaultValue)
   {
      return emptystr(_string) ? defaultValue : toInt();
   }

   long toLong(int radix)
   {
      return StringHelper::strToLong(_string, radix);
   }

//   void convertAnsi(char* s, size_t length)
//   {
//      String::convertAnsiToWide(s, _string, length);
//      _string[length] = 0;
//   }

   void clear()
   {
      _string[0] = 0;
   }

   size_t Length()
   {
      return getlength(_string);
   }

   String()
   {
      _string[0] = 0;
   }
   String(const wchar16_t* s)
   {
      copy(s);
   }
   String(const char* s)
   {
      copy(s);
   }
   String(const wchar16_t* s1, const wchar16_t* s2)
   {
      copy(s1);
      append(s2);
   }
   String(const char* s1, const wchar16_t* s2)
   {
      copy(s1);
      append(s2);
   }
   String(const wchar16_t* s1, const wchar16_t* s2, const wchar16_t* s3)
   {
      copy(s1);
      append(s2);
      append(s3);
   }
   String(const wchar16_t* s1, const wchar16_t* s2, const wchar16_t* s3, const wchar16_t* s4)
   {
      copy(s1);
      append(s2);
      append(s3);
      append(s4);
   }
   String(const wchar16_t* s, size_t length)
   {
      copy(s, length);
      _string[length] = 0;
   }
   String(const char* s, size_t length)
   {
      copy(s, length);
      _string[length] = 0;
   }
};

// --- DynamicString ---

template <class T, size_t pageSize = 0x20> class DynamicString
{
protected:
   T*     _string;
   size_t _size;

//   void assignOrCopy(const wchar16_t* value, wchar16_t* &ptr, size_t& size)
//   {
//      ptr = (wchar16_t*)value;
//      size = 0;
//   }

   void create(wchar16_t*, size_t size)
   {
      if (_size == 0) {
         _size = align(size, pageSize);
         _string = StringHelper::w_allocate(_size);
         _string[0] = 0;
      }
      else {
         int length = getlength(_string);

         _size = align(size, pageSize);
         _string = StringHelper::w_reallocate(_string, _size);

         _string[length] = 0;
      }
   }

   void create(char*, size_t size)
   {
      if (_size == 0) {
         _size = align(size, pageSize);
         _string = StringHelper::allocate(_size);
         _string[0] = 0;
      }
      else {
         int length = getlength(_string);

         _size = align(size, pageSize);
         _string = StringHelper::reallocate(_string, _size);

         _string[length] = 0;
      }
   }

public:
   operator const T*() const { return _string; }

   T& operator[](size_t index)
   {
      return *(_string + index);
   }

   T& operator[](int index)
   {
      return *(_string + index);
   }

   bool isEmpty() const { return emptystr(_string); }

   int findLast(T ch)
   {
      return StringHelper::findLast(_string, ch, -1);
   }

   int find(T ch)
   {
      return StringHelper::find(_string, ch, -1);
   }

   int find(int index, T ch)
   {
      return StringHelper::find(_string + index, ch, -1);
   }

   bool compare(const wchar16_t* s)
   {
      return StringHelper::compare(_string, s);
   }

   void copy(const wchar16_t* s, size_t length)
   {
      if (_size <= length) {
         create(_string, length + 1);
      }
      StringHelper::copy(_string, s, length);
      _string[length] = 0;
   }
   void copy(const wchar16_t* s)
   {
      size_t length = getlength(s);
      if (length == 0) {
         _string[0] = 0;
      }
      else copy(s, length);
   }
   void copy(const char* s, size_t length)
   {
      if (_size <= length) {
         create(_string, length + 1);
      }
      StringHelper::copy(_string, s, length);
      _string[length] = 0;
   }
   void copy(const char* s)
   {
      size_t length = getlength(s);
      if (length == 0) {
         _string[0] = 0;
      }
      else copy(s, length);
   }

   void append(const wchar16_t* s, size_t length)
   {
      size_t newLength = getlength(_string) + length + 1;
      if (newLength > _size)
         create(_string, newLength);

      StringHelper::append(_string, s, length);
   }
   void append(const wchar16_t* s)
   {
      size_t length = getlength(s);
      if (length > 0) {
         append(s, length);
      }
   }
   void append(const char* s, size_t length)
   {
      size_t newLength = getlength(_string) + length + 1;
      if (newLength > _size)
         create(_string, newLength);

      StringHelper::append(_string, s, length);
   }
   void append(const char* s)
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

   void lower()
   {
      StringHelper::lower(_string);
   }

   void trim(T ch)
   {
      StringHelper::trim(_string, ch);
   }

   T* clone()
   {
      return clone(_string);
   }

   T* clone(int pos)
   {
      return clone(_string + pos);
   }

   int toInt()
   {
      return StringHelper::strToInt(_string);
   }

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
   DynamicString(const char* value)
   {
      assignOrCopy(value, _string, _size);
   }

   DynamicString(const wchar16_t* value)
   {
      assignOrCopy(value, _string, _size);
   }

   ~DynamicString() { if (_size > 0) freestr(_string); }
};

// --- conversion routines ---

inline void copystr(wchar16_t* d, const char* s)
{
   size_t len = strlen(s);
   StringHelper::copy(d, s, len);
   d[len] = 0;
}

} // _ELENA_

#endif // altstringH
