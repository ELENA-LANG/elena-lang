//----------------------- ----------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains String classes implementations
//
//                                              (C)2005-2019, by Alexei Rakov
//                                              (C)1994-2004, Unicode, Inc.
//---------------------------------------------------------------------------

///* ---------------------------------------------------------------------
//     Conversions between UTF-16, and UTF-8.
//     Author: Mark E. Davis, 1994.
//     Rev History: Rick McGowan, fixes & updates May 2001.
//     Sept 2001: fixed const & error conditions per
//         mods suggested by S. Parent & A. Lillich.
//     June 2002: Tim Dodd added detection and handling of incomplete
//         source sequences, enhanced error detection, added casts
//         to eliminate compiler warnings.
//     July 2003: slight mods to back out aggressive FFFE detection.
//     Jan 2004: updated switches in from-UTF8 conversions.
//     Oct 2004: updated to use UNI_MAX_LEGAL_UTF32 in UTF-32 conversions.
//------------------------------------------------------------------------ */

#include "common.h"
// --------------------------------------------------------------------------
#include "altstrings.h"
#include <math.h>

#pragma warning(disable : 4996)

using namespace _ELENA_;

// --- Unicode coversion routines ---

#define UNI_MAX_BMP           (unsigned int)0x0000FFFF
#define UNI_REPLACEMENT_CHAR  (unsigned int)0x0000FFFD
#define UNI_MAX_UTF16         (unsigned int)0x0010FFFF
#define UNI_SUR_HIGH_START    (unsigned int)0xD800
#define UNI_SUR_HIGH_END      (unsigned int)0xDBFF
#define UNI_SUR_LOW_START     (unsigned int)0xDC00
#define UNI_SUR_LOW_END       (unsigned int)0xDFFF
#define UNI_MAX_LEGAL_UTF32   (unsigned int)0x0010FFFF

static const int halfShift  = 10; /* used for shifting by 10 bits */

static const unsigned int halfBase = 0x0010000UL;
static const unsigned int halfMask = 0x3FFUL;

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
static const unsigned char trailingBytesForUTF8[256] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
   2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
};

/*
* Magic values subtracted from a buffer value during UTF8 conversion.
* This table contains as many values as there might be trailing bytes
* in a UTF-8 sequence.
*/
static const unsigned int offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL,
                                                   0x03C82080UL, 0xFA082080UL, 0x82082080UL };
/*
* Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
* into the first byte, depending on how many bytes follow.  There are
* as many entries in this table as there are UTF-8 sequence types.
* (I.e., one byte sequence, two byte... etc.). Remember that sequencs
* for *legal* UTF-8 will be 4 or fewer bytes total.
*/
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *  length = trailingBytesForUTF8[*source]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns false.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */
static bool isLegalUTF8(const unsigned char* source, size_t length)
{
   unsigned char a;
   const unsigned char* srcptr = source + length;

   switch (length) {
      default:
         return false;
      /* Everything else falls through when "true"... */
      case 4:
         if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
      case 3:
         if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
      case 2:
         if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
         switch (*source) {
            /* no fall-through in this inner switch */
            case 0xE0: if (a < 0xA0) return false; break;
            case 0xED: if (a > 0x9F) return false; break;
            case 0xF0: if (a < 0x90) return false; break;
            case 0xF4: if (a > 0x8F) return false; break;
            default:   if (a < 0x80) return false;
         }
      case 1:
         if (*source >= 0x80 && *source < 0xC2) return false;
   }
   if (*source > 0xF4) return false;

   return true;
}

bool Convertor :: copy(char* dest, const char* sour, size_t sourLength, size_t& destLength)
{
   if(sourLength <= destLength) {
      memcpy(dest, sour, sourLength);
      destLength = sourLength;

      return true;
   }
   else return false;
}

bool Convertor :: copy(wide_c* dest, const wide_c* sour, size_t sourLength, size_t& destLength)
{
   if (sourLength <= destLength) {
      memcpy(dest, sour, sourLength << 1);
      destLength = sourLength;

      return true;
   }
   else return false;
}

bool Convertor :: copy(wide_c* dest, const char* sour, size_t sourLength, size_t& destLength)
{
   bool result = true;

   const unsigned char* s = (const unsigned char*)sour;
   const unsigned char* end = s + sourLength;

   wide_c* d = dest;
   wide_c* d_end = dest + destLength;

   while (end > s) {
      unsigned int ch = 0;
      unsigned short extraBytesToRead = trailingBytesForUTF8[*s];

      if (extraBytesToRead >= end - s) {
         result = false;
         break;
      }
      /* Do this check whether lenient or strict */
      if (!isLegalUTF8(s, extraBytesToRead+1)) {
         result = false;
         break;
      }
      /*
      * The cases all fall through. See "Note A" below.
      */
      switch (extraBytesToRead) {
         case 5: ch += *s++; ch <<= 6; /* remember, illegal UTF-8 */
         case 4: ch += *s++; ch <<= 6; /* remember, illegal UTF-8 */
         case 3: ch += *s++; ch <<= 6;
         case 2: ch += *s++; ch <<= 6;
         case 1: ch += *s++; ch <<= 6;
         case 0: ch += *s++;
      }
      ch -= offsetsFromUTF8[extraBytesToRead];

      if (d >= d_end) {
         s -= (extraBytesToRead + 1); /* Back up source pointer! */
         result = false;
         break;
      }
      if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
         /* UTF-16 surrogate values are illegal in UTF-32 */
         if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
            *d++ = UNI_REPLACEMENT_CHAR;
         }
         else {
            *d++ = (wide_c)ch; /* normal case */
         }
      }
      else if (ch > UNI_MAX_UTF16) {
         *d++ = UNI_REPLACEMENT_CHAR;
      }
      else {
         /* target is a character in range 0xFFFF - 0x10FFFF. */
         if (d + 1 >= d_end) {
            s -= (extraBytesToRead + 1); /* Back up source pointer! */
            result = false;
            break;
         }
         ch -= halfBase;
         *d++ = (wide_c)((ch >> halfShift) + UNI_SUR_HIGH_START);
         *d++ = (wide_c)((ch & halfMask) + UNI_SUR_LOW_START);
      }
   }
   destLength = d - dest;

   return result;
}

bool Convertor :: copy(char* dest, const wide_c* sour, size_t sourLength, size_t& destLength)
{
   bool result = true;

   const wide_c* s = sour;
   const wide_c* end = s + sourLength;

   char* d = dest;
   const char* d_end = d + destLength;

   while (s < end) {
      unsigned short bytesToWrite = 0;
      const unsigned int byteMask = 0xBF;
      const unsigned int byteMark = 0x80;
      const wide_c* oldSource = s; /* In case we have to back up because of target overflow. */
      unsigned int ch = *s++;
      /* If we have a surrogate pair, convert to UTF32 first. */
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
         /* If the 16 bits following the high surrogate are in the source buffer... */
         if (s < end) {
            unsigned int ch2 = *s;
            /* If it's a low surrogate, convert to UTF32. */
            if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
               ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
               ++s;
            }
         }
         else { /* We don't have the 16 bits following the high surrogate. */
            --s; /* return to the high surrogate */
            result = false;
            break;
         }
      }
      /* Figure out how many bytes the result will require */
      if (ch < (unsigned int)0x80) {
         bytesToWrite = 1;
      }
      else if (ch < (unsigned int)0x800) {
         bytesToWrite = 2;
      }
      else if (ch < (unsigned int)0x10000) {
         bytesToWrite = 3;
      }
      else if (ch < (unsigned int)0x110000) {
         bytesToWrite = 4;
      }
      else {
         bytesToWrite = 3;
         ch = UNI_REPLACEMENT_CHAR;
      }

      d += bytesToWrite;
      if (d > d_end) {
         s = oldSource; /* Back up source pointer! */
         d -= bytesToWrite;
         result = false;
         break;
      }
      switch (bytesToWrite)
      { /* note: everything falls through. */
         case 4:
            *--d = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 3:
            *--d = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 2:
            *--d = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 1:
            *--d =  (char)(ch | firstByteMark[bytesToWrite]);
      }
      d += bytesToWrite;
   }
   destLength = d - dest;

   return result;
}

bool Convertor :: copy(char* dest, const unic_c* sour, size_t sourLength, size_t& destLength)
{
   bool result = true;

   const unsigned int* s = sour;
   const unsigned int* end = s + sourLength;

   char* d = dest;
   const char* d_end = d + destLength;

   while (s < end) {
      unsigned int ch;
      unsigned short bytesToWrite = 0;
      const unsigned int byteMask = 0xBF;
      const unsigned int byteMark = 0x80;
      ch = *s++;
      /*
      * Figure out how many bytes the result will require. Turn any
      * illegally large UTF32 things (> Plane 17) into replacement chars.
      */
      if (ch < (unsigned int)0x80) {         bytesToWrite = 1; }
      else if (ch < (unsigned int)0x800) {   bytesToWrite = 2; }
      else if (ch < (unsigned int)0x10000) { bytesToWrite = 3; }
      else if (ch <= UNI_MAX_LEGAL_UTF32) {  bytesToWrite = 4; }
      else {                                 bytesToWrite = 3;
                                             ch = UNI_REPLACEMENT_CHAR;
                                             result = false;
      }

      d += bytesToWrite;
      if (d > d_end) {
         --s; /* Back up source pointer! */
         d -= bytesToWrite;
         result = false;
         break;
      }
      switch (bytesToWrite) { /* note: everything falls through. */
         case 4: *--d = (char)((ch | byteMark) & byteMask); ch >>= 6;
         case 3: *--d = (char)((ch | byteMark) & byteMask); ch >>= 6;
         case 2: *--d = (char)((ch | byteMark) & byteMask); ch >>= 6;
         case 1: *--d = (char)(ch | firstByteMark[bytesToWrite]);
      }
      d += bytesToWrite;
   }
   destLength = d - dest;

   return result;
}

bool Convertor :: copy(unic_c* dest, const char* sour, size_t sourLength, size_t& destLength)
{
   bool result = true;

   const unsigned char* s = (const unsigned char*)sour;
   const unsigned char* end = s + sourLength;

   unsigned int* d = dest;
   const unsigned int* d_end = d + destLength;

   while (s < end) {
      unsigned int ch = 0;
      unsigned short extraBytesToRead = trailingBytesForUTF8[*s];
      if (extraBytesToRead >= end - s) {
         result = false;
         *d++ = UNI_REPLACEMENT_CHAR;
         break;
      }
      if (d >= d_end) {
         result = false;
         break;
      }
      /* Do this check whether lenient or strict */
      if (!isLegalUTF8((unsigned char*)s, extraBytesToRead + 1)) {
         result = false;
         break;
      }
      /*
      * The cases all fall through. See "Note A" below.
      */
      switch (extraBytesToRead) {
         case 5: ch += *s++; ch <<= 6;
         case 4: ch += *s++; ch <<= 6;
         case 3: ch += *s++; ch <<= 6;
         case 2: ch += *s++; ch <<= 6;
         case 1: ch += *s++; ch <<= 6;
         case 0: ch += *s++;
      }
      ch -= offsetsFromUTF8[extraBytesToRead];
      if (ch <= UNI_MAX_LEGAL_UTF32) {
         /*
         * UTF-16 surrogate values are illegal in UTF-32, and anything
         * over Plane 17 (> 0x10FFFF) is illegal.
         */
         if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
            * d++ = UNI_REPLACEMENT_CHAR;
         }
         else {
            *d++ = ch;
         }
      }
      else { /* i.e., ch > UNI_MAX_LEGAL_UTF32 */
         result = false;
         *d++ = UNI_REPLACEMENT_CHAR;
      }
   }
   destLength = d - dest;

   return result;
}

char* StrFactory :: clone(const char* s)
{
   return emptystr(s) ? NULL : _strdup(s);
}

char* StrFactory :: allocate(size_t size, const char* value)
{
   char* s = (char*)malloc(size);

   if (!emptystr(value))
      memcpy(s, value, size);

   return s;
}

void StrHelper :: move(char* s1, const char* s2, size_t length)
{
   memmove(s1, s2, length);
}

char* StrFactory :: reallocate(char* s, size_t size)
{
   return (char*)realloc(s, size);
}

size_t StrHelper::findChar(const char* s, char ch, size_t length, size_t defValue)
{
   const char* p = (const char*)memchr(s, ch, length);
   if (p == NULL) {
      return defValue;
   }
   else return p - s;
}

size_t find(const char* s, const char* subs, size_t defValue)
{
   const char* p = strstr(s, subs);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

size_t __find(const char* s, char c, size_t defValue)
{
   const char* p = strchr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

size_t __findLast(const char* s, char c, size_t defValue)
{
   const char* p = strrchr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

size_t findSubStr(const char* s, char c, size_t length, size_t defValue)
{
   for (size_t i = 0; i < length; i++) {
      if (s[i] == c)
         return i;
   }

   return defValue;
}

size_t findLastSubStr(const char* s, char c, size_t length, size_t defValue)
{
   size_t last = defValue;
   for (size_t i = 0; i < length; i++) {
      if (s[i] == c) {
         last = i;
      }
   }

   return last;
}

void append(char* dest, const char* sour, size_t length)
{
   strncat(dest, sour, length);
}

bool compare(const char* s1, const char* s2)
{
   if (s1 && s2) return (strcmp(s1, s2)==0);
   else return (s1 == s2);
}

bool compare(const char* s1, const char* s2, size_t n)
{
   if (s1 && s2) return (strncmp(s1, s2, n)==0);
   else return (n > 0);
}

//bool StringHelper :: greater(const char* s1, const char* s2, size_t n)
//{
//   if (s1 && s2) return (strncmp(s1, s2, n) > 0);
//   else return (n > 0);
//}

bool greater(const char* s1, const char* s2)
{
   if (s1 && s2) return (strcmp(s1, s2) > 0);
   else return (s1 == s2);
}

char* StrHelper :: lower(char* s)
{
   return _strlwr(s);
}

char StrHelper :: lower(char ch)
{
   char s[2];
   s[0] = ch;
   s[1] = 0;

   _strlwr(s);
   return s[0];
}

char* StrHelper :: upper(char* s)
{
   return _strupr(s);
}

char* clone(const char* s)
{
   return emptystr(s) ? NULL : _strdup(s);
}

char* clone(const char* s, size_t length)
{
   return emptystr(s) ? NULL : StrFactory::allocate(length, s);
}

char* Convertor :: intToStr(int n, char* s, int radix)
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

   return s;
}

int Convertor :: strToInt(const char* s)
{
   return atoi(s);
}

long strToLong(const char* s, int radix)
{
   return strtol(s, NULL, radix);
}

long strToULong(const char* s, int radix)
{
   return strtoul(s, NULL, radix);
}

long long strToLongLong(const char* s, int radix)
{
   long long number = 0;

   bool negative = false;
   if (s[0] == '-') {
      negative = true;
      s++;
   }

   char dump[10];
   size_t length = getlength(s);
   while (length > 9) {
      memcpy(dump, (char*)s, 9);
      dump[9] = 0;

      long long temp = strToLong(dump, radix);
      for (size_t i = 0; i < (length - 9); i++) {
         temp *= radix;
      }
      number += temp;

      length -= 9;
      s += 9;
   }
   memcpy(dump, s, length);
   dump[length] = 0;
   long long temp = strToLong(dump, radix);
   number += temp;

   if (negative)
      number = -number;

   return number;
}

//char* StringHelper::ulongToStr(unsigned long n, char* s, int radix)
//{
//   int  rem = 0;
//   int  pos = 0;
//   int start = 0;
//
//   do
//   {
//      rem = n % radix;
//      n /= radix;
//      switch (rem) {
//      case 10:
//         s[pos++] = 'a';
//         break;
//      case 11:
//         s[pos++] = 'b';
//         break;
//      case 12:
//         s[pos++] = 'c';
//         break;
//      case 13:
//         s[pos++] = 'd';
//         break;
//      case 14:
//         s[pos++] = 'e';
//         break;
//      case 15:
//         s[pos++] = 'f';
//         break;
//      default:
//         if (rem < 10) {
//            s[pos++] = (char)(rem + 0x30);
//         }
//      }
//   } while (n != 0);
//
//   s[pos] = 0;
//   pos--;
//   while (start < pos) {
//      char tmp = s[start];
//      s[start++] = s[pos];
//      s[pos--] = tmp;
//   }
//
//   return s;
//}

char* Convertor :: doubleToStr(double value, int digit, char* s)
{
   // !!HOTFIX : to recognize nan
   if (value != value) {
      StrHelper::append(s, "nan", 4);
   }
   else if (isinf(value)) {
      if (value == -INFINITY) {
         StrHelper::append(s, "-inf", 4);
      }
      else StrHelper::append(s, "+inf", 4);
   }
   else _gcvt(value, digit, s);

   return s;
}

double strToDouble(const char* s)
{
   // !!HOTFIX : to recognize nan
   if (strcmp(s, "nan") == 0) {
      return NAN;
   }
   else if (strcmp(s, "-inf") == 0) {
      return -INFINITY;
   }
   else if (strcmp(s, "+inf") == 0) {
      return INFINITY;
   }
   else return atof(s);
}

//void StringHelper :: trim(char* s, char ch)
//{
//   size_t length = getlength(s);
//   while (length > 0 && s[length - 1] == ch) {
//      s[length - 1] = 0;
//      length = getlength(s);
//   }
//}

#ifdef _MSC_VER

wchar_t* StrFactory :: clone(const wchar_t* s)
{
   return _wcsdup(s);
}

wchar_t* clone(const wchar_t* s, size_t length)
{
   return emptystr(s) ? NULL : StrFactory::allocate(length, s);
}

wchar_t* StrFactory :: allocate(size_t size, const wchar_t* value)
{
   wchar_t* s = (wchar_t*)malloc(size << 1);

   if (value)
      wcsncpy(s, value, size);

   return s;
}

wchar_t* StrFactory :: reallocate(wchar_t* s, size_t size)
{
   return (wchar_t*)realloc(s, size << 1);
}

void StrHelper :: move(wchar_t* s1, const wchar_t* s2, size_t length)
{
   memmove(s1, s2, length << 1);
}

void append(wchar_t* dest, const wchar_t* sour, size_t length)
{
   wcsncat(dest, sour, length);
}

bool compare(const wchar_t* s1, const wchar_t* s2)
{
   if (s1 && s2) return (wcscmp(s1, s2)==0);
   else return (s1 == s2);
}

bool compare(const wchar_t* s1, const wchar_t* s2, size_t n)
{
   if (s1 && s2) return (wcsncmp(s1, s2, n)==0);
   else return (n > 0);
}

//bool StringHelper :: greater(const wchar_t* s1, const wchar_t* s2, size_t n)
//{
//   if (s1 && s2) return (wcsncmp(s1, s2, n) > 0);
//   else return (n > 0);
//}

bool greater(const wchar_t* s1, const wchar_t* s2)
{
   if (s1 && s2) return (wcscmp(s1, s2) > 0);
   else return (s1 == s2);
}

size_t __find(const wchar_t* s, wchar_t c, size_t defValue)
{
   const wchar_t* p = wcschr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

size_t __findLast(const wchar_t* s, wchar_t c, size_t defValue)
{
   const wchar_t* p = wcsrchr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

wchar_t* StrHelper :: lower(wchar_t* s)
{
   return _wcslwr(s);
}

wchar_t StrHelper :: lower(wchar_t ch)
{
   wchar_t s[2];
   s[0] = ch;
   s[1] = 0;

   _wcslwr(s);
   return s[0];
}

wchar_t* StrHelper :: upper(wchar_t* s)
{
   return _wcsupr(s);
}

wchar_t* clone(const wchar_t* s)
{
   return _wcsdup(s);
}

int strToInt(const wchar_t* s)
{
   return _wtoi(s);
}

long strToLong(const wchar_t* s, int radix)
{
   return wcstoul(s, NULL, radix);
}

//double StringHelper :: strToDouble(const wchar_t* s)
//{
//   return wcstod(s, NULL);
//}
//
//wchar_t* StringHelper :: intToStr(int n, wchar_t* s, int radix)
//{
//   return _itow(n, s, radix);
//}

//wchar_t* ulongToStr(unsigned long n, wchar_t* s, int radix)
//{
//   return _ultow(n, s, radix);
//}

char* Convertor :: longlongToStr(long long n, char* s, int radix)
{
   return _i64toa(n, s, radix);
}

wchar_t* Convertor :: longlongToStr(long long n, wchar_t* s, int radix)
{
   return _i64tow(n, s, radix);
}

//long long StringHelper :: strToLongLong(const wchar_t* s, int radix)
//{
//   long long number = 0;
//
//   wchar_t dump[10];
//   int length = getlength(s);
//   while (length > 9) {
//      wcsncpy(dump, (wchar_t*)s, 9);
//      dump[9] = 0;
//
//      long long temp = strToLong(dump, radix);
//      for (int i = 0 ; i < (length - 9) ; i++) {
//         temp *= radix;
//      }
//      number += temp;
//
//      length -= 9;
//      s += 9;
//   }
//   wcsncpy(dump, s, length);
//   dump[length] = 0;
//   long long temp = strToLong(dump, radix);
//   number += temp;
//
//   return number;
//}

size_t findSubStr(const wchar_t* s, wchar_t c, size_t length, size_t defValue)
{
   for (size_t i = 0; i < length; i++) {
      if (s[i] == c)
         return i;
   }

   return defValue;
}

wchar_t* Convertor :: doubleToStr(double value, int digit, wchar_t* s)
{
   char tmp[25];
   gcvt(value, digit, tmp);

   for (size_t i = 0; i <= getlength(tmp); i++) {
      s[i] = tmp[i];
   }

   return s;
}

#elif _LINUX

unsigned short* StrFactory :: allocate(size_t size, const unsigned short* value)
{
   unsigned short* s = (unsigned short*)malloc(size << 1);
   if (value)
      memcpy(s, value, size << 1);

   return s;
}

unsigned short* StrFactory :: reallocate(unsigned short* s, size_t size)
{
   return (unsigned short*)realloc(s, size << 1);
}

void StrHelper :: move(unsigned short* s1, const unsigned short* s2, size_t length)
{
   memmove(s1, s2, length << 1);
}

void append(unsigned short* dest, const unsigned short* sour, size_t length)
{
   unsigned short* p = dest + getlength(dest);
   for(size_t i = 0 ; i < length ; i++)
      p[i] = sour[i];

   p[length] = 0;
}

bool compare(const unsigned short* s1, const unsigned short* s2)
{
   if (s1 && s2) {
      while (*s1 || *s2) {
         if (*s1++ != *s2++)
            return false;
      }
      return true;
   }
   else return (s1 == s2);
}

bool compare(const unsigned short* s1, const unsigned short* s2, size_t n)
{
   if (s1 && s2) {
      while (n > 0) {
         if (*s1++ != *s2++)
            return false;

         n--;
      }
      return true;
   }
   else return (n > 0);
}

bool greater(const unsigned short* s1, const unsigned short* s2, size_t n)
{
   if (s1 && s2) {
      while (*s1 && *s1 == *s2) {
         s1++;
         s2++;
      }
      return *s1 > *s2;
   }
   else return (n > 0);
}

bool greater(const unsigned short* s1, const unsigned short* s2)
{
   return greater(s1, s2, getlength(s1) + 1);
}

size_t __find(const unsigned short* s, unsigned short c, size_t defValue)
{
   const unsigned short* p = s;

   while(*p) {
      if (*p == c)
         return p - s;

      p++;
   }

   return defValue;
}

size_t __findLast(const unsigned short* s, unsigned short c, size_t defValue)
{
   const unsigned short* p = s + getlength(s);

   while(p != s) {
      if (*p == c)
         return p - s;

      p--;
   }

   return defValue;
}

unsigned short* clone(const unsigned short* s)
{
   int length = getlength(s);

   return emptystr(s) ? NULL : StrFactory::allocate(length, s);
}

unsigned short* clone(const unsigned short* s, size_t length)
{
   return emptystr(s) ? NULL : StrFactory::allocate(length, s);
}

int strToInt(const unsigned short* s)
{
   int n = 0;
   bool neg = false;

   //!! temporal
   if (*s == '-') {
      s++;
      neg = true;
   }
   while (*s) {
      n *= 10;

      unsigned short c = *s;
      if (c >= '0' && c <= '9') {
         n += (c - '0');

         s++;
      }
      else return 0;
   }
   if (neg)
      n *= -1;

   return n;
}

long strToLong(const unsigned short* s, int radix)
{
   int n = 0;
   bool neg = false;

   //!! temporal
   if (*s == '-') {
      s++;
      neg = true;
   }
   while (*s) {
      n *= radix;

      unsigned short c = *s;
      if(c >= '0' && c <= '9') {
         n += (c - '0');
      }
      else if(c >= 'A' && c <= 'F') {
         n += (c - 'A');
         n += 0x0A;
      }
      else if(c >= 'a' && c <= 'f') {
         n += (c - 'a');
         n += 0x0A;
      }
      else return 0;

      s++;
   }
   if (neg)
      n *= -1;

   return n;
}

//long long StringHelper :: strToLongLong(const unsigned short* s, int radix)
//{
//   long long number = 0;
//
//   unsigned short dump[10];
//   size_t length = getlength(s);
//   while (length > 9) {
//      size_t len = 9;
//      copy(dump, (unsigned short*)s, len, len);
//      dump[len] = 0;
//
//      long long temp = strToLong(dump, radix);
//      for (size_t i = 0 ; i < (length - 9) ; i++) {
//         temp *= radix;
//      }
//      number += temp;
//
//      length -= 9;
//      s += 9;
//   }
//   copy(dump, s, length, length);
//   dump[length] = 0;
//   long long temp = strToLong(dump, radix);
//   number += temp;
//
//   return number;
//}
//
//double StringHelper :: strToDouble(const unsigned short* s)
//{
//   // !! temporal solution
//   char tmp[31];
//   size_t len = getlength(s);
//   copy(tmp, s, len, len);
//   tmp[len] = 0;
//
//   return atof(tmp);
//}
//
//unsigned short* StringHelper :: intToStr(int n, unsigned short* s, int radix)
//{
//   int  rem = 0;
//   int  pos = 0;
//   do
//   {
//      rem = n % radix;
//      n /= radix;
//      switch(rem) {
//         case 10:
//            s[pos++] = 'a';
//            break;
//         case 11:
//            s[pos++] = 'b';
//            break;
//         case 12:
//            s[pos++] = 'c';
//            break;
//         case 13:
//            s[pos++] = 'd';
//            break;
//         case 14:
//            s[pos++] = 'e';
//            break;
//         case 15:
//            s[pos++] = 'f';
//            break;
//         default:
//            if (rem < 10) {
//               s[pos++] = (rem + 0x30);
//            }
//      }
//   }
//   while( n != 0 );
//
//   s[pos] = 0;
//
//   return s;
//}

char* Convertor :: longlongToStr(long long n, char* s, int radix)
{
   int  rem = 0;
   int  pos = 0;
   do
   {
      rem = n % radix;
      n /= radix;
      switch(rem) {
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
               s[pos++] = (rem + 0x30);
            }
      }
   }
   while( n != 0 );

   s[pos] = 0;

   return s;
}

size_t findSubStr(const unsigned short* s, unsigned short c, size_t length, size_t defValue)
{
   for (size_t i = 0; i < length; i++) {
      if (s[i] == c)
         return i;
   }

   return defValue;
}

unsigned short* StrHelper :: lower(unsigned short* s)
{
   while (*s) {
      *s = tolower(*s); // !! temporal: currently only ascii symbols are handled
      s++;
   }
   return s;
}

unsigned short* StrHelper :: upper(unsigned short* s)
{
   while (*s) {
      *s = toupper((unsigned char) *s);
      s++;
   }
   return s;
}


unsigned short StrHelper :: lower(unsigned short c)
{
   return tolower(c); // !! temporal: currently only ascii symbols are handled
}

#endif

//int StringHelper :: find(const wchar_t* s, const wchar_t* subs, int defValue)
//{
//   const wchar_t* p = wcsstr(s, subs);
//   if (p==NULL) {
//      return defValue;
//   }
//   else return p - s;
//}

//void StringHelper :: trim(wchar_t* s, wchar_t ch)
//{
//   size_t length = getlength(s);
//   while (length > 0 && s[length - 1] == ch) {
//      s[length - 1] = 0;
//      length = getlength(s);
//   }
//}

//#else
//
//char* StringHelper :: allocateText(size_t size)
//{
//   return allocate(size);
//}
//
//void StringHelper :: copy(unsigned short* dest, const unsigned short* sour, size_t length)
//{
//   memcpy(dest, sour, length << 1);
//}
//
//int StringHelper :: find(const unsigned short* s, const unsigned short* subs, int defValue)
//{
//   int l = getlength(s);
//   int ls = getlength(subs);
//
//   for(int i = 0 ; i < l - ls ; i++) {
//      if (s[i]==subs[0]) {
//         bool match = true;
//         for(int j = 1 ; j < ls ; j++) {
//            if (s[i+j] != subs[j]) {
//               match = false;
//               break;
//            }
//         }
//         if (match)
//            return i;
//      }
//   }
//
//   return defValue;
//}

//unsigned short* StringHelper :: w_clone(const char* s)
//{
//   size_t length = strlen(s) + 1;
//   unsigned short* dup = allocate((unsigned short*)NULL, length);
//   copy(dup, s, length);
//
//   return dup;
//}
//
//char* StringHelper :: clone(const unsigned short* s)
//{
//   size_t length = getlength(s) + 1;
//   char* dup = allocate((char*)NULL, length);
//   copy(dup, s, length);
//
//   return dup;
//}

//void StringHelper :: trim(unsigned short* s, unsigned short ch)
//{
//   size_t length = getlength(s);
//   while (length > 0 && s[length - 1] == ch) {
//      s[length - 1] = 0;
//      length = getlength(s);
//   }
//}

//unsigned short* StringHelper :: doubleToStr(double value, int digit, unsigned short* s)
//{
//   //!! temporal
//   char temp[20];
//
//   sprintf(temp, "%lf", value);
//   int len = strlen(temp);
//   for (int i = 0 ; i <= len ; i++) {
//      s[0] = temp[0];
//   }
//
//   return s;
//}
//
//#endif
//
//void StringHelper :: copy(char* dest, const char* sour, int length)
//{
//   strncpy(dest, sour, length);
//}

//void StringHelper :: move(char* s1, const char* s2, size_t length)
//{
//   memmove(s1, s2, length);
//}

// --- ident_t ---

bool ident_t :: copyTo(char* dest, size_t length, size_t& destLength)
{
   return Convertor::copy(dest, _string, length, destLength);
}

bool ident_t :: copyTo(wide_c* dest, size_t length, size_t& destLength)
{
   return Convertor::copy(dest, _string, length, destLength);
}

int ident_t :: toInt()
{
   return Convertor::strToInt(_string);
}

int ident_t :: toInt(size_t index)
{
   return Convertor::strToInt(_string + index);
}

long ident_t :: toLong(int radix)
{
   return strToLong(_string, radix);
}

long ident_t :: toLong(int radix, size_t index)
{
   return strToLong(_string + index, radix);
}

long ident_t :: toULong(int radix, size_t index)
{
   return strToULong(_string + index, radix);
}

long long ident_t :: toLongLong(int radix, size_t index)
{
   return strToLongLong(_string + index, radix);
}

double ident_t :: toDouble(size_t index)
{
   return strToDouble(_string + index);
}

size_t ident_t :: find(const char* s, size_t defValue)
{
   return ::find(_string, s, defValue);
}

size_t ident_t :: findSubStr(size_t index, const char* s, size_t defValue)
{
   size_t retVal = ::find(_string + index, s, NOTFOUND_POS);

   return retVal == NOTFOUND_POS ? defValue : retVal + index;
}

size_t ident_t :: find(char c, size_t defValue)
{
   return __find(_string, c, defValue);
}

size_t ident_t :: find(size_t index, char ch, size_t defValue)
{
   for (size_t i = index; i < getlength(_string); i++) {
      if (_string[i] == ch)
         return i;
   }

   return defValue;
}

size_t ident_t :: findLast(char c, size_t defValue)
{
   return __findLast(_string, c, defValue);
}

size_t ident_t :: findLast(size_t index, char ch, size_t defValue)
{
   size_t i = getlength(_string);
   while (i >= index) {
      if (_string[i] == ch)
         return i;

      i--;
   }

   return defValue;
}

size_t ident_t :: findSubStr(size_t index, char c, size_t length, size_t defValue)
{
   size_t pos = ::findSubStr(_string + index, c, length, NOTFOUND_POS);
   if (pos != NOTFOUND_POS) {
      return index + pos;
   }
   else return defValue;
}

size_t ident_t :: findLastSubStr(size_t index, char c, size_t length, size_t defValue)
{
   size_t pos = ::findLastSubStr(_string + index, c, length, NOTFOUND_POS);
   if (pos != NOTFOUND_POS) {
      return index + pos;
   }
   else return defValue;
}

char* ident_t :: clone()
{
   return ::clone(_string);
}

char* ident_t :: clone(size_t index)
{
   return ::clone(_string + index);
}

char* ident_t::clone(size_t index, size_t length)
{
   char* dup = ::clone(_string + index, length + 1);

   dup[length] = 0;

   return dup;
}

bool ident_t :: compare(const char* s) const
{
   return ::compare(_string, s);
}

bool ident_t::greater(const char* s)
{
   return ::greater(_string, s);
}

bool ident_t::compare(const char* s, size_t length) const
{
   return ::compare(_string, s, length);
}

bool ident_t::compare(const char* s, size_t index, size_t length)
{
   return ::compare(_string + index, s, length);
}

bool ident_t :: endsWith(const char* s)
{
   size_t slen = getlength(s);
   size_t len = getlength(_string);

   return ::compare(_string + len - slen, s, slen);
}

bool ident_t::startsWith(const char* s)
{
   size_t slen = getlength(s);
   return ::compare(_string, s, slen);
}

// --- wide_t ---

bool wide_t :: copyTo(char* dest, size_t length, size_t& destLength)
{
   return Convertor::copy(dest, _string, length, destLength);
}

bool wide_t :: copyTo(wide_c* dest, size_t length, size_t& destLength)
{
   return Convertor::copy(dest, _string, length, destLength);
}

int wide_t :: toInt()
{
   return strToInt(_string);
}

int wide_t :: toInt(size_t index)
{
   return strToInt(_string + index);
}

long wide_t :: toLong(int radix)
{
   return strToLong(_string, radix);
}

long wide_t :: toLong(int radix, size_t index)
{
   return strToLong(_string + index, radix);
}

size_t wide_t :: find(wide_c c, size_t defValue)
{
   return __find(_string, c, defValue);
}

size_t wide_t :: find(size_t index, wide_c ch, size_t defValue)
{
   for (size_t i = index; i < getlength(_string); i++) {
      if (_string[i] == ch)
         return i;
   }

   return defValue;
}

size_t wide_t :: findLast(wide_c c, size_t defValue)
{
   return __findLast(_string, c, defValue);
}

size_t wide_t :: findLast(size_t index, wide_c ch, size_t defValue)
{
   size_t i = getlength(_string);
   while (i >= index) {
      if (_string[i] == ch)
         return i;

      /*/ HOTFIX : stop at the 0*/
      if (!i) break;

      i--;
   }

   return defValue;
}

size_t wide_t :: findSubStr(size_t index, wide_c c, size_t length, size_t defValue)
{
   return ::findSubStr(_string + index, c, length, defValue);
}

wide_c* wide_t :: clone()
{
   return ::clone(_string);
}

wide_c* wide_t :: clone(size_t index)
{
   return ::clone(_string + index);
}

wide_c* wide_t :: clone(size_t index, size_t length)
{
   wide_c* dup = ::clone(_string + index, length + 1);

   dup[length] = 0;

   return dup;
}

bool wide_t :: compare(const wide_c* s) const
{
   return ::compare(_string, s);
}

bool wide_t :: greater(const wide_c* s)
{
   return ::greater(_string, s);
}

bool wide_t::compare(const wide_c* s, size_t length) const
{
   return ::compare(_string, s, length);
}

// --- String conversion routines ---

void StrHelper :: append(char* dest, const char* sour, size_t length)
{
   ::append(dest, sour, length);
}

void StrHelper :: append(wide_c* dest, const wide_c* sour, size_t length)
{
   ::append(dest, sour, length);
}

void StrHelper :: insert(char* s, size_t pos, size_t len, const char* subs)
{
   size_t totalLen = getlength(s);

   for (size_t i = totalLen; i > pos; i--) {
      s[i + len - 1] = s[i - 1];
   }

   memmove(s + pos, subs, len);

   s[totalLen + len] = 0;
}
