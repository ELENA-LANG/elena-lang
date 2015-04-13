//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This header contains String classes declarations
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef altstringsH
#define altstringsH

namespace _ELENA_
{

// --- StringHelper ---

class StringHelper
{
public:
   static char* allocate(size_t size, const char* value);
   static wide_c* allocate(size_t size, const wide_c* value);
   static char* reallocate(char* s, size_t size);
   static wide_c* reallocate(wide_c* s, size_t size);

   static bool copy(char* dest, const char* sour, size_t sourLength, size_t& destLength);
   static bool copy(wide_c* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
   static bool copy(wide_c* dest, const char* sour, size_t sourLength, size_t& destLength);
   static bool copy(char* dest, const wide_c* sour, size_t sourLength, size_t& destLength);
   static bool copy(char* dest, const unic_c* sour, size_t sourLength, size_t& destLength);
   static bool copy(unic_c* dest, const char* sour, size_t sourLength, size_t& destLength);

   static wide_c* clone(const wide_c* s);
   static char* clone(const char* s);

   static void move(char* s1, const char* s2, size_t length);
   static void move(wide_c* s1, const wide_c* s2, size_t length);

   static void insert(ident_c* s, int pos, ident_t subs);

   static void append(char* dest, const char* sour, int length);
   static void append(wide_c* dest, const wide_c* sour, size_t length);
//   static bool append(wchar16_t* dest, const char* sour, size_t length);
//   static bool append(char* dest, const wchar16_t* sour, size_t length);

   static wide_c lower(wide_c s);
   static wide_c* lower(wide_c* s);
   static char* lower(char* s);
   static char lower(char s);

   static wide_c* upper(wide_c* s);
   static char* upper(char* s);

   static void trim(char* s, char c);

   static bool compare(const wide_c* s1, const wide_c* s2);
   static bool compare(const wide_c* s1, const wide_c* s2, size_t length);
   static bool compare(const char* s1, const char* s2);
   static bool compare(const char* s1, const char* s2, size_t length);

   static bool greater(const wide_c* s1, const wide_c* s2);
   static bool greater(const wide_c* s1, const wide_c* s2, size_t length);
   static bool greater(const char* s1, const char* s2);
   static bool greater(const char* s1, const char* s2, size_t length);

   static int find(const char* s, const char* subs, int defValue = -1);
   static int find(const char* s, char c, int defValue = -1);
   static int findSubStr(const char* s, char c, size_t length, int defValue = -1);
   static int find(const wide_c* s, wide_c c, int defValue = -1);
   static int findLast(const wide_c* s, wide_c c, int defValue = -1);
   static int findLast(const char* s, char c, int defValue = -1);

   static int strToInt(const wide_c* s);
   static int strToInt(const char* s);

   static wide_c* intToStr(int n, wide_c* s, int radix);
   static char* intToStr(int n, char* s, int radix);
   static wide_c* ulongToStr(unsigned long n, wide_c* s, int radix);
   static char* ulongToStr(unsigned long n, char* s, int radix);

   static long strToLong(const wide_c* s, int radix);
   static long strToLong(const char* s, int radix);
   static long strToULong(const char* s, int radix);

   static long long strToLongLong(const char* s, int radix);
   static long long strToLongLong(const wide_c* s, int radix);

   static double strToDouble(const char* s);
   static double strToDouble(const wide_c* s);

   static ident_c* longlongToStr(long long n, ident_c* s, int radix);
   static ident_c* doubleToStr(double value, int digit, ident_c* s);
};

// --- String ---

template <class T, size_t size> class String
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

   void copy(const T* s, size_t length)
   {
      StringHelper::copy(_string, s, length, length);
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

   void append(const T* s, size_t length)
   {
      size_t newLength = getlength(_string) + length;
      if (newLength < size) {
         StringHelper::append(_string, s, length);
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

      StringHelper::intToStr(n, _string + pos, 10);
   }

   void appendHex(int n)
   {
      int pos = getlength(_string);

      StringHelper::ulongToStr(n, _string + pos, 16);
      StringHelper::upper(_string + pos);
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
      StringHelper::upper(_string + pos);
   }

   void appendInt64(long long n)
   {
      int pos = getlength(_string);

      StringHelper::longlongToStr(n, _string + pos, 10);
      StringHelper::upper(_string + pos);
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

   long toULong(int radix)
   {
      return StringHelper::strToULong(_string, radix);
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

template <class T, size_t pageSize = 0x20> class DynamicString
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
         _string = StringHelper::allocate(_size, (const T*)NULL);
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

   bool compare(const T* s)
   {
      return StringHelper::compare(_string, s);
   }

   void copy(const T* s, size_t length)
   {
      if (_size <= length) {
         create(_string, length + 1);
      }
      StringHelper::copy(_string, s, length);
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

      StringHelper::append(_string, s, length);
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

// --- conversion routines ---

inline void copystr(char* d, const char* s)
{
   size_t len = strlen(s);
   StringHelper::copy(d, s, len, len);
   d[len] = 0;
}

} // _ELENA_

#endif // altstringH
