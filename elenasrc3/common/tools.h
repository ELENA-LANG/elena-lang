//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains the common ELENA Project routine functions
//
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TOOLS_H
#define TOOLS_H

#ifdef __GNUG__

#include <ctype.h>

#endif // __GNUG__

#if !defined(_max)
#define _max(a, b)       ((a) > (b) ? (a) : (b))
#endif

#if !defined(_min)
#define _min(a, b)       ((a) < (b) ? (a) : (b))
#endif

namespace elena_lang
{

#ifdef __GNUG__
inline size_t wcslen(const unsigned short* s)
{
   const unsigned short* p = s;

   while (*p) p++;

   return (size_t)(p - s);
}
#endif

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

// --- miscellaneous string routines ---

#ifdef _MSC_VER

inline static size_t __fastcall getlength(const wchar_t* s)
{
   return (s == nullptr) ? 0 : wcslen(s);
}

inline static pos_t __fastcall getlength_pos(const wchar_t* s)
{
   return (s == nullptr) ? 0 : (pos_t)wcslen(s);
}

inline static int __fastcall getlength_int(const wchar_t* s)
{
   return (s == nullptr) ? 0 : (int)wcslen(s);
}

#elif __GNUG__

inline size_t getlength(const unsigned short* s)
{
   return (s == nullptr) ? 0 : wcslen(s);
}

inline pos_t getlength_pos(const unsigned short* s)
{
   return (s == nullptr) ? 0 : (pos_t)wcslen(s);
}

inline int getlength_int(const unsigned short* s)
{
   return (s == nullptr) ? 0 : (int)wcslen(s);
}

inline static bool emptystr(const unsigned short* s)
{
   return (s == nullptr || s[0] == 0);
}

#endif

inline static bool emptystr(const char* s)
{
   return (s == nullptr || s[0] == 0);
}

inline static bool emptystr(const wchar_t* s)
{
   return (s == nullptr || s[0] == 0);
}

inline size_t getlength(const char* s)
{
   return (s == nullptr) ? 0 : strlen(s);
}

inline pos_t getlength_pos(const char* s)
{
   return (s == nullptr) ? 0 : (pos_t)strlen(s);
}

inline int getlength_int(const char* s)
{
   return (s == nullptr) ? 0 : (int)strlen(s);
}

// --- calcTabShift ---

inline int calcTabShift(int col, int tabSize)
{
   int nextCol = (col / tabSize * tabSize) + tabSize;

   return nextCol - col;
}

// --- miscellaneous routines ---

inline bool test(const int number, const int mask)
{
   return ((number & mask) == mask);
}
inline bool test(const unsigned int number, const unsigned int mask)
{
   return ((number & mask) == mask);
}

inline bool testany(const int number, const int mask)
{
   return ((number & mask) != 0);
}

inline bool testanyLong(long long number, long long mask)
{
   return ((number & mask) != 0LL);
}

inline bool test64(uint64_t number, uint64_t mask)
{
   return ((number & mask) == mask);
}

// --- _abs ---
inline int _abs(int x)
{
   return (x ^ (x >> 31)) - (x >> 31);
}


} // _ELENA_

#endif // toolsH
