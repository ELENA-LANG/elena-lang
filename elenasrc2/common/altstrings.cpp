//----------------------- ----------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains String classes implementations
//
//                                              (C)2005-2012, by Alexei Rakov
//                                              (C)2001-2004, Unicode, Inc.
//---------------------------------------------------------------------------

#include "common.h"
// --------------------------------------------------------------------------
#include "altstrings.h"

using namespace _ELENA_;

#include <ctype.h>

// --- Unicode coversion routines ---

#define UNI_MAX_BMP           (unsigned int)0x0000FFFF
#define UNI_REPLACEMENT_CHAR  (unsigned int)0x0000FFFD
#define UNI_MAX_UTF16         (unsigned int)0x0010FFFF
#define UNI_SUR_HIGH_START    (unsigned int)0xD800
#define UNI_SUR_HIGH_END      (unsigned int)0xDBFF
#define UNI_SUR_LOW_START     (unsigned int)0xDC00
#define UNI_SUR_LOW_END       (unsigned int)0xDFFF

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
static const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
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
static const char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

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
static bool isLegalUTF8(const unsigned char* source, int length)
{
   unsigned char a;
   const unsigned char* srcptr = source+length;
   switch (length) {
      default:
         return false;
      /* Everything else falls through when "true"... */
      case 4:
         if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
            return false;
      case 3:
         if ((a = (*--srcptr)) < 0x80 || a > 0xBF)
            return false;
      case 2:
         if ((a = (*--srcptr)) > 0xBF)
            return false;

         switch (*source) {
            /* no fall-through in this inner switch */
            case 0xE0:
               if (a < 0xA0)
                  return false;
               break;
            case 0xED:
               if (a > 0x9F)
                  return false;
               break;
            case 0xF0:
               if (a < 0x90)
                  return false;
               break;
            case 0xF4:
               if (a > 0x8F)
                  return false;
               break;
            default:
               if (a < 0x80)
                  return false;
         }
      case 1:
         if (*source >= 0x80 && *source < 0xC2)
            return false;
   }
   if (*source > 0xF4)
      return false;

   return true;
}

// --- StringHelper ---

#ifdef _WIN32

wchar_t* StringHelper :: w_allocate(size_t size)
{
   return (wchar_t*)malloc(size << 1);
}

wchar_t* StringHelper :: w_reallocate(wchar_t* s, size_t size)
{
   return (wchar_t*)realloc(s, size << 1);
}

wchar_t* StringHelper :: clone(const wchar_t* s)
{
   return wcsdup(s);
}

int StringHelper :: find(const wchar_t* s, wchar_t c, int defValue)
{
   const wchar_t* p = wcschr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

int StringHelper :: find(const wchar_t* s, const wchar_t* subs, int defValue)
{
   const wchar_t* p = wcsstr(s, subs);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

int StringHelper :: findLast(const wchar_t* s, wchar_t c, int defValue)
{
   const wchar_t* p = wcsrchr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

bool StringHelper :: compare(const wchar_t* s1, const wchar_t* s2)
{
   if (s1 && s2) return (wcscmp(s1, s2)==0);
   else return (s1 == s2);
}

bool StringHelper :: compare(const wchar_t* s1, const wchar_t* s2, size_t n)
{
   if (s1 && s2) return (wcsncmp(s1, s2, n)==0);
   else return (n > 0);
}

bool StringHelper :: greater(const wchar_t* s1, const wchar_t* s2, size_t n)
{
   if (s1 && s2) return (wcsncmp(s1, s2, n) > 0);
   else return (n > 0);
}

bool StringHelper :: greater(const wchar_t* s1, const wchar_t* s2)
{
   if (s1 && s2) return (wcscmp(s1, s2) > 0);
   else return (s1 == s2);
}

void StringHelper :: copy(wchar_t* dest, const wchar_t* sour, size_t length)
{
   wcsncpy(dest, sour, length);
}

void StringHelper :: append(wchar_t* dest, const wchar_t* sour, size_t length)
{
   wcsncat(dest, sour, length);
}

void StringHelper :: insert(wchar_t* s, int pos, const wchar_t* subs)
{
   size_t len = getlength(subs);

   //s[getlength(s) + len + 1] = 0;
   for (int i = getlength(s) ; i >= pos ; i--) {
      s[i+len] = s[i];
   }
   wcsncpy(s + pos, subs, len);
}

void StringHelper :: move(wchar_t* s1, const wchar_t* s2, size_t length)
{
   memmove(s1, s2, length << 1);
}

wchar_t* StringHelper :: lower(wchar_t* s)
{
   return wcslwr(s);
}

wchar_t StringHelper :: lower(wchar_t ch)
{
   wchar_t s[2];
   s[0] = ch;
   s[1] = 0;

   wcslwr(s);
   return s[0];
}

wchar_t* StringHelper :: upper(wchar_t* s)
{
   return wcsupr(s);
}

void StringHelper :: trim(wchar_t* s, wchar_t ch)
{
   size_t length = getlength(s);
   while (length > 0 && s[length - 1] == ch) {
      s[length - 1] = 0;
      length = getlength(s);
   }
}

int StringHelper :: strToInt(const wchar_t* s)
{
   return _wtoi(s);
}

long StringHelper :: strToLong(const wchar_t* s, int radix)
{
   return wcstoul(s, NULL, radix);
}

wchar_t* StringHelper :: intToStr(int n, wchar_t* s, int radix)
{
   return _itow(n, s, radix);
}

wchar_t* StringHelper :: longlongToStr(long long n, wchar_t* s, int radix)
{
   return _i64tow(n, s, radix);
}

wchar_t* StringHelper :: doubleToStr(double value, int digit, wchar_t* s)
{
   char temp[20];

   _gcvt(value, digit, temp);

   size_t length = strlen(temp);
   copy(s, temp, length);
   s[length] = 0;

   return s;
}

long long StringHelper :: strToLongLong(const wchar_t* s, int radix)
{
   long long number = 0;

   wchar_t dump[10];
   int length = getlength(s);
   while (length > 9) {
      wcsncpy(dump, (wchar_t*)s, 9);
      dump[9] = 0;

      long long temp = strToLong(dump, radix);
      for (int i = 0 ; i < (length - 9) ; i++) {
         temp *= radix;
      }
      number += temp;

      length -= 9;
      s += 9;
   }
   wcsncpy(dump, s, length);
   dump[length] = 0;
   long long temp = strToLong(dump, radix);
   number += temp;

   return number;
}

double StringHelper :: strToDouble(const wchar_t* s)
{
   return wcstod(s, NULL);
}

bool StringHelper :: copy(wchar_t* dest, const char* sour, size_t& length)
{
   bool result = true;

   wchar_t* start = dest;
   const unsigned char* s = (const unsigned char*)sour;
   const unsigned char* end = s + length;
   while (end > s) {
      unsigned int ch = 0;
      unsigned short extraBytesToRead = trailingBytesForUTF8[*s];
      if (extraBytesToRead >= length) {
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
         case 5:
            ch += *s++;
            ch <<= 6; /* remember, illegal UTF-8 */
         case 4:
            ch += *s++;
            ch <<= 6; /* remember, illegal UTF-8 */
         case 3:
            ch += *s++;
            ch <<= 6;
         case 2:
            ch += *s++;
            ch <<= 6;
         case 1:
            ch += *s++;
            ch <<= 6;
         case 0:
            ch += *s++;
      }
      ch -= offsetsFromUTF8[extraBytesToRead];

      if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
         /* UTF-16 surrogate values are illegal in UTF-32 */
         if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
            *dest++ = UNI_REPLACEMENT_CHAR;
         }
         else {
            *dest++ = (unsigned short)ch; /* normal case */
         }
      }
      else if (ch > UNI_MAX_UTF16) {
         *dest++ = UNI_REPLACEMENT_CHAR;
      }
      else {
         /* target is a character in range 0xFFFF - 0x10FFFF. */
         ch -= halfBase;
         *dest++ = (unsigned short)((ch >> halfShift) + UNI_SUR_HIGH_START);
         *dest++ = (unsigned short)((ch & halfMask) + UNI_SUR_LOW_START);
      }
   }
   length = dest - start;
   return result;
}

bool StringHelper :: copy(char* dest, const wchar_t* sour, size_t& length)
{
   bool result = true;
   char* s = dest;
   const wchar_t* end = sour + length;
   while (sour < end) {
      unsigned short bytesToWrite = 0;
      const unsigned int byteMask = 0xBF;
      const unsigned int byteMark = 0x80;
      const unsigned short* oldSource = (unsigned short*)sour; /* In case we have to back up because of target overflow. */
      unsigned int ch = *sour++;
      /* If we have a surrogate pair, convert to UTF32 first. */
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
         /* If the 16 bits following the high surrogate are in the source buffer... */
         if (sour < end) {
            unsigned int ch2 = *sour;
            /* If it's a low surrogate, convert to UTF32. */
            if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
               ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
               ++sour;
            }
         }
         else { /* We don't have the 16 bits following the high surrogate. */
            --sour; /* return to the high surrogate */
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

      dest += bytesToWrite;
      switch (bytesToWrite)
      { /* note: everything falls through. */
         case 4:
            *--dest = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 3:
            *--dest = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 2:
            *--dest = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 1:
            *--dest =  (char)(ch | firstByteMark[bytesToWrite]);
      }
      dest += bytesToWrite;
   }

   length = dest - s;

   return result;
}

bool StringHelper :: append(wchar_t* dest, const char* sour, size_t length)
{
   size_t current_length = getlength(dest);
   if (copy(dest + current_length, sour, length)) {
      dest[current_length + length] = 0;

      return true;
   }
   else return false;
}

bool StringHelper :: append(char* dest, const wchar_t* sour, size_t length)
{
   size_t current_length = getlength(dest);
   if (copy(dest + current_length, sour, length)) {
      dest[current_length + length] = 0;

      return true;
   }
   else return false;
}

#else

unsigned short* StringHelper :: w_allocate(size_t size)
{
   return (unsigned short*)malloc(size << 1);
}

unsigned short* StringHelper :: w_reallocate(unsigned short* s, size_t size)
{
   return (unsigned short*)realloc(s, size << 1);
}

bool StringHelper :: compare(const unsigned short* s1, const unsigned short* s2)
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

bool StringHelper :: compare(const unsigned short* s1, const unsigned short* s2, size_t n)
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

bool StringHelper :: greater(const unsigned short* s1, const unsigned short* s2, size_t n)
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

bool StringHelper :: greater(const unsigned short* s1, const unsigned short* s2)
{
   return greater(s1, s2, getlength(s1) + 1);
}

int StringHelper :: find(const unsigned short* s, unsigned short c, int defValue)
{
   const unsigned short* p = s;

   while(*p) {
      if (*p == c)
         return p - s;

      p++;
   }

   return defValue;
}

int StringHelper :: findLast(const unsigned short* s, unsigned short c, int defValue)
{
   const unsigned short* p = s + getlength(s);

   while(p > s) {
      if (*p == c)
         return p - s;

      p--;
   }

   return defValue;
}

void StringHelper :: copy(unsigned short* dest, const unsigned short* sour, size_t length)
{
   memcpy(dest, sour, length << 1);
}

void StringHelper :: append(unsigned short* dest, const unsigned short* sour, size_t length)
{
   unsigned short* p = dest + getlength(dest);
   for(int i = 0 ; i <= length ; i++)
      *p++ = *sour++;
}

bool StringHelper :: append(unsigned short* dest, const char* sour, size_t length)
{
   size_t current_length = getlength(dest);
   if (copy(dest + current_length, sour, length)) {
      dest[current_length + length] = 0;

      return true;
   }
   else return false;
}

bool StringHelper :: append(char* dest, const unsigned short* sour, int length)
{
   size_t current_length = getlength(dest);
   if (copy(dest + current_length, sour, length)) {
      dest[current_length + length] = 0;

      return true;
   }
   else return false;
}

bool StringHelper :: copy(unsigned short* dest, const char* sour, size_t& length)
{
   bool result = true;

   unsigned short* start = dest;
   const unsigned char* s = sour;
   const unsigned char* end = s + length;
   while (end > s) {
      unsigned int ch = 0;
      unsigned short extraBytesToRead = trailingBytesForUTF8[*s];
      if (extraBytesToRead >= count) {
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
         case 5:
            ch += *s++;
            ch <<= 6; /* remember, illegal UTF-8 */
         case 4:
            ch += *s++;
            ch <<= 6; /* remember, illegal UTF-8 */
         case 3:
            ch += *s++;
            ch <<= 6;
         case 2:
            ch += *s++;
            ch <<= 6;
         case 1:
            ch += *s++;
            ch <<= 6;
         case 0:
            ch += *s++;
      }
      ch -= offsetsFromUTF8[extraBytesToRead];

      if (ch <= UNI_MAX_BMP) { /* Target is a character <= 0xFFFF */
         /* UTF-16 surrogate values are illegal in UTF-32 */
         if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
            *dest++ = UNI_REPLACEMENT_CHAR;
         }
         else {
            *dest++ = (unsigned short)ch; /* normal case */
         }
      }
      else if (ch > UNI_MAX_UTF16) {
         *dest++ = UNI_REPLACEMENT_CHAR;
      }
      else {
         /* target is a character in range 0xFFFF - 0x10FFFF. */
         ch -= halfBase;
         *dest++ = (unsigned short)((ch >> halfShift) + UNI_SUR_HIGH_START);
         *dest++ = (unsigned short)((ch & halfMask) + UNI_SUR_LOW_START);
      }
   }
   length = dest - start;

   return result;
}

bool StringHelper :: copy(char* dest, const unsigned short* sour, size_t& length)
{
   bool result = true;
   const unsigned short* end = sour + length;
   while (sour < end) {
      unsigned short bytesToWrite = 0;
      const unsigned int byteMask = 0xBF;
      const unsigned int byteMark = 0x80;
      const unsigned short* oldSource = sour; /* In case we have to back up because of target overflow. */
      unsigned int ch = *sour++;
      /* If we have a surrogate pair, convert to UTF32 first. */
      if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_HIGH_END) {
         /* If the 16 bits following the high surrogate are in the source buffer... */
         if (sour < end) {
            unsigned int ch2 = *sour;
            /* If it's a low surrogate, convert to UTF32. */
            if (ch2 >= UNI_SUR_LOW_START && ch2 <= UNI_SUR_LOW_END) {
               ch = ((ch - UNI_SUR_HIGH_START) << halfShift) + (ch2 - UNI_SUR_LOW_START) + halfBase;
               ++sour;
            }
         }
         else { /* We don't have the 16 bits following the high surrogate. */
            --sour; /* return to the high surrogate */
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

      dest += bytesToWrite;
      switch (bytesToWrite)
      { /* note: everything falls through. */
         case 4:
            *--dest = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 3:
            *--dest = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 2:
            *--dest = (char)((ch | byteMark) & byteMask);
            ch >>= 6;
         case 1:
            *--dest =  (char)(ch | firstByteMark[bytesToWrite]);
      }
      dest += bytesToWrite;
   }

   length = dest - s;

   return result;
}

void StringHelper :: move(unsigned short* s1, const unsigned short* s2, size_t length)
{
   memmove(s1, s2, length << 1);
}

unsigned short* StringHelper :: w_clone(const unsigned short* s)
{
   size_t length = getlength(s) + 1;
   unsigned short* dup = allocate((unsigned short*)NULL, length);
   copy(dup, s, length);

   return dup;
}

unsigned short* StringHelper :: w_clone(const char* s)
{
   size_t length = strlen(s) + 1;
   unsigned short* dup = allocate((unsigned short*)NULL, length);
   copy(dup, s, length);

   return dup;
}

char* StringHelper :: clone(const unsigned short* s)
{
   size_t length = getlength(s) + 1;
   char* dup = allocate((char*)NULL, length);
   copy(dup, s, length);

   return dup;
}

unsigned short* StringHelper :: lower(unsigned short* s)
{
   while (*s) {
      *s = tolower(*s); // !! temporal: currently only ascii symbols are handled
      s++;
   }
   return s;
}

unsigned short StringHelper :: lower(unsigned short c)
{
   return tolower(c); // !! temporal: currently only ascii symbols are handled
}

unsigned short* StringHelper :: upper(unsigned short* s)
{
   while (*s) {
      *s = toupper(*s); // !! temporal: currently only ascii symbols are handled
      s++;
   }
   return s;
}

void StringHelper :: trim(unsigned short* s, unsigned short ch)
{
   size_t length = getlength(s);
   while (length > 0 && s[length - 1] == ch) {
      s[length - 1] = 0;
      length = getlength(s);
   }
}

int StringHelper :: strToInt(const unsigned short* s)
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
      if(c >= '0' && c <= '9') {
         n += (c - '0');
      }
      else return 0;
   }
   if (neg)
      n *= -1;

   return n;
}

unsigned short* intToStr(int n, unsigned short* s, int radix)
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

unsigned short* StringHelper :: doubleToStr(double value, int digit, unsigned short* s)
{
   //!! temporal
   char temp[20];

   sprintf(temp, "%lf", value);
   int len = strlen(temp);
   for (int i = 0 ; i <= len ; i++) {
      s[0] = temp[0];
   }

   return s;
}

unsigned short* StringHelper :: longlongToStr(long long n, unsigned short* s, int radix)
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

#endif

char* StringHelper :: allocate(size_t size)
{
   return (char*)malloc(size);
}

char* StringHelper :: reallocate(char* s, size_t size)
{
   return (char*)realloc(s, size);
}

char* StringHelper :: clone(const char* s)
{
   return strdup(s);
}

void StringHelper :: move(char* s1, const char* s2, size_t length)
{
   memmove(s1, s2, length);
}

bool StringHelper :: compare(const char* s1, const char* s2)
{
   if (s1 && s2) return (strcmp(s1, s2)==0);
   else return (s1 == s2);
}

bool StringHelper :: compare(const char* s1, const char* s2, size_t n)
{
   if (s1 && s2) return (strncmp(s1, s2, n)==0);
   else return (n > 0);
}

bool StringHelper :: greater(const char* s1, const char* s2, size_t n)
{
   if (s1 && s2) return (strncmp(s1, s2, n) > 0);
   else return (n > 0);
}

bool StringHelper :: greater(const char* s1, const char* s2)
{
   if (s1 && s2) return (strcmp(s1, s2) > 0);
   else return (s1 == s2);
}

int StringHelper :: find(const char* s, char c, int defValue)
{
   const char* p = strchr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

int StringHelper :: findLast(const char* s, char c, int defValue)
{
   const char* p = strrchr(s, c);
   if (p==NULL) {
      return defValue;
   }
   else return p - s;
}

void StringHelper :: copy(char* dest, const char* sour, int length)
{
   strncpy(dest, sour, length);
}

void StringHelper :: append(char* dest, const char* sour, int length)
{
   strncat(dest, sour, length);
}

#ifdef _WIN32

char* StringHelper :: lower(char* s)
{
   return strlwr(s);
}

char* StringHelper :: upper(char* s)
{
   return strupr(s);
}

#else

char* StringHelper :: lower(char* s)
{
   while (*s) {
      *s = tolower((unsigned char) *s);
      s++;
   }
   return s;
}

char* StringHelper :: upper(char* s)
{
   while (*s) {
      *s = toupper((unsigned char) *s);
      s++;
   }
   return s;
}

#endif

void StringHelper :: trim(char* s, char ch)
{
   size_t length = getlength(s);
   while (length > 0 && s[length - 1] == ch) {
      s[length - 1] = 0;
      length = getlength(s);
   }
}

int StringHelper :: strToInt(const char* s)
{
   return atoi(s);
}

char* StringHelper :: intToStr(int n, char* s, int radix)
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
   pos--;
   while (start < pos) {
      char tmp = s[start];
      s[start++] = s[pos];
      s[pos--] = tmp;
   }

   return s;
}
