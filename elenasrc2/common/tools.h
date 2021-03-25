//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common ELENA Project routine functions
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef toolsH
#define toolsH 1

#if !defined(max)
#define max(a, b)       ((a) > (b) ? (a) : (b))
#endif

#if !defined(min)
#define min(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifdef _LINUX

#include <ctype.h>

#define _strdup strdup
#define _gcvt gcvt

namespace _ELENA_
{

inline size_t wcslen(const unsigned short* s)
{
   const unsigned short* p = s;

   while (*p) p++;

   return (size_t)(p - s);
}

inline char* _strlwr(char* str)
{
   char* it = str;
   while (*it != 0) { *it = tolower(*it); ++it; }

   return str;
}

inline char* _strupr(char* str)
{
   char* it = str;
   while (*it != 0) { *it = toupper(*it); ++it; }

   return str;
}

////int _itow(int val, wchar_t* s, int radix)
////{
////    int len = 0;
////
////    if (val < 0) {
////        *s++ = '-';
////        val = 0 - val;
////
////        len++;
////    }
////
////    wchar_t* current = s;
////    unsigned int digit;
////    do {
////        digit = val % radix;
////        val /= radix;
////
////        *current++ = digit + '0';
////
////    } while (val > 0);
////
////    len += (int)(current - s);
////
////    *current-- = 0;
////
////    //swap
////    do {
////        wchar_t temp = *current;
////        *current = *s;
////        *s = temp;
////
////        --current;
////        ++s;
////
////    } while (s < current);
////
////    return len;
////}

inline bool emptystr(const unsigned short* s)
{
   return (s == NULL || s[0]==0);
}

inline size_t getlength(const unsigned short* s)
{
   return (s==NULL) ? 0 : wcslen(s);
}

#elif _MSC_VER

namespace _ELENA_
{

inline bool __fastcall emptystr(const wchar_t* s)
{
   return (s == NULL || s[0]==0);
}

inline static pos_t __fastcall getlength(const wchar_t* s)
{
   return (s==NULL) ? 0 : (pos_t)wcslen(s);
}

#endif

// --- miscellaneous string routines ---

inline static bool emptystr(const char* s)
{
   return (s == NULL || s[0] == 0);
}

inline pos_t getlength(const char* s)
{
   return (s == NULL) ? 0 : (pos_t)strlen(s);
}

// --- resource freeing routines ---

template <class T> void freeobj(T obj)
{
   if (obj != nullptr) {
      delete obj;
   }
}

inline void freestr(wchar_t* s)
{
   if (s != nullptr) {
      free(s);
   }
}

inline void freestr(char* s)
{
   if (s != nullptr) {
      free(s);
   }
}

inline void freestr(unsigned short* s)
{
   if (s != nullptr) {
      free(s);
   }
}

// --- alignment routines ---

inline unsigned int align(unsigned int number, const unsigned int alignment)
{
   if (number & (alignment - 1)) {
      return (number & ~(alignment - 1)) + alignment;
   }
   else return number & ~(alignment - 1);
}

inline size_t alignSize(size_t number, size_t alignment)
{
   if (number & (alignment - 1)) {
      return (number & ~(alignment - 1)) + alignment;
   }
   else return number & ~(alignment - 1);
}

// --- miscellaneous routines ---

inline bool ifAny(int target, int value1, int value2)
{
   return target == value1 || target == value2;
}

inline bool test(int number, int mask)
{
   return ((number & mask) == mask);
}

inline bool test(int number, int mask, int value)
{
   return ((number & mask) == value);
}

inline bool testLong(long long number, long long mask)
{
   return ((number & mask) == mask);
}

inline bool testLong(long long number, long long mask, long long value)
{
   return ((number & mask) == value);
}

inline bool testany(int number, int mask)
{
   return ((number & mask) != 0);
}

inline bool testanyLong(long long number, long long mask)
{
   return ((number & mask) != 0LL);
}

inline bool isbetween(size_t starting, size_t len , size_t value)
{
   return (starting < value && value < starting + len);
}

//inline bool isNumeric(ident_t s, int length)
//{
//   for (int i = 0 ; i < length ; i++)
//   {
//      if (s[i] < '0' || s[i] > '9')
//         return false;
//   }
//
//   return true;
//}

// --- calcTabShift ---

inline int calcTabShift(int col, int tabSize)
{
   int nextCol = (col / tabSize * tabSize) + tabSize;

   return nextCol - col;
}

// --- __abs ---

inline int __abs(int x)
{
   return (x ^ (x>>31)) - (x>>31);
}

} // _ELENA_

#endif // toolsH
