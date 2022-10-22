//----------------------- ----------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains String classes implementations
//
//                                             (C)2021-2022, by Aleksey Rakov
//                                             (C)1994-2004, Unicode, Inc.
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
#include "ustring.h"

using namespace elena_lang;

#pragma warning(disable:4996)

// --- Unicode coversion routines ---

static const int halfShift = 10; /* used for shifting by 10 bits */

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

// --- StrConvertor ---

bool StrConvertor :: copy(wide_c* dest, const char* sour, size_t sourLength, size_t& destLength)
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
      if (!isLegalUTF8(s, extraBytesToRead + 1)) {
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

bool StrConvertor::copy(char* dest, const wide_c* sour, size_t sourLength, size_t& destLength)
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
         *--d = (char)(ch | firstByteMark[bytesToWrite]);
      }
      d += bytesToWrite;
   }
   destLength = d - dest;

   return result;
}

bool StrConvertor :: copy(char* dest, const char* sour, size_t sourLength, size_t& destLength)
{
   if (sourLength <= destLength) {
      memcpy(dest, sour, sourLength);
      destLength = sourLength;

      return true;
   }
   else return false;
}

bool StrConvertor :: copy(wide_c* dest, const wide_c* sour, size_t sourLength, size_t& destLength)
{
   if (sourLength <= destLength) {
      memcpy(dest, sour, sourLength << 1);
      destLength = sourLength;

      return true;
   }
   else return false;
}

bool StrConvertor :: copy(char* dest, const unic_c* sour, size_t sourLength, size_t& destLength)
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
      if (ch < (unsigned int)0x80) { bytesToWrite = 1; }
      else if (ch < (unsigned int)0x800) { bytesToWrite = 2; }
      else if (ch < (unsigned int)0x10000) { bytesToWrite = 3; }
      else if (ch <= UNI_MAX_LEGAL_UTF32) { bytesToWrite = 4; }
      else {
         bytesToWrite = 3;
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

bool StrConvertor :: copy(unic_c* dest, const char* sour, size_t sourLength, size_t& destLength)
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
            *d++ = UNI_REPLACEMENT_CHAR;
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

int StrConvertor :: toInt(const char* s, int radix)
{
   return strtol(s, nullptr, radix);
}

int StrConvertor :: toInt(const wide_c* s, int radix)
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

unsigned int StrConvertor :: toUInt(const char* s, int radix)
{
   return strtoul(s, nullptr, radix);
}

// --- internal functions ---

bool inline util_compare(const char* s1, const char* s2)
{
   if (s1 && s2) return (strcmp(s1, s2) == 0);
   else return (s1 == s2);
}

bool inline util_compare(const char* s1, const char* s2, size_t length)
{
   if (s1 && s2) return (strncmp(s1, s2, length) == 0);
   else return (s1 == s2);
}

inline size_t util_find(const char* s, char ch, size_t length, size_t defValue)
{
   const char* p = (const char*)memchr(s, ch, length);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

inline size_t util_find_str(const char* s, const char* subs, size_t defValue)
{
   const char* p = (const char*)strstr(s, subs);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

inline size_t util_find_str(const char* s, size_t index, const char* subs, size_t defValue)
{
   const char* p = (const char*)strstr(s + index, subs);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

size_t inline util_find_last(const char* s, char c, size_t defValue)
{
   const char* p = strrchr(s, c);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

void append(char* dest, const char* sour, size_t length)
{
   strncat(dest, sour, length);
}

char* util_clone(const char* s, size_t length)
{
   if (!emptystr(s)) {
      char* copy = StrFactory::allocate(length + 1, s);
      copy[length] = 0;

      return copy;
   }
   else return nullptr;
}

#ifdef _MSC_VER

char* util_clone(const char* s)
{
   return emptystr(s) ? nullptr : _strdup(s);
}

wchar_t* util_clone(const wchar_t* s)
{
   return _wcsdup(s);
}

wchar_t* util_clone(const wchar_t* s, size_t length)
{
   if (!emptystr(s)) {
      wchar_t* copy = StrFactory::allocate(length + 1, s);
      copy[length] = 0;

      return copy;
   }
   else return nullptr;
}

void append(wchar_t* dest, const wchar_t* sour, size_t length)
{
   wcsncat(dest, sour, length);
}

inline bool util_compare(const wchar_t* s1, const wchar_t* s2)
{
   if (s1 && s2) return (wcscmp(s1, s2) == 0);
   else return (s1 == s2);
}

inline bool util_compare(const wchar_t* s1, const wchar_t* s2, size_t length)
{
   if (s1 && s2) return (wcsncmp(s1, s2, length) == 0);
   else return (s1 == s2);
}

size_t inline util_find(const wchar_t* s, wchar_t ch, size_t length, size_t defValue)
{
   const wchar_t* p = wcschr(s, ch);
   if (p == NULL) {
      return defValue;
   }
   else return p - s;
}

inline size_t util_find_last(const wchar_t* s, wchar_t c, size_t defValue)
{
   const wchar_t* p = wcsrchr(s, c);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

inline size_t util_find_str(const wchar_t* s, const wchar_t* subs, size_t defValue)
{
   const wchar_t* p = wcsstr(s, subs);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

inline size_t util_find_str(const wchar_t* s, size_t index, const wchar_t* subs, size_t defValue)
{
   const wchar_t* p = wcsstr(s + index, subs);
   if (p == nullptr) {
      return defValue;
   }
   else return p - s;
}

inline char* util_lower(char* s)
{
   return _strlwr(s);
}

inline char util_lower(char ch)
{
   char s[2];
   s[0] = ch;
   s[1] = 0;

   _strlwr(s);
   return s[0];
}

inline wchar_t* util_lower(wchar_t* s)
{
   return _wcslwr(s);
}

inline wchar_t util_lower(wchar_t ch)
{
   wchar_t s[2];
   s[0] = ch;
   s[1] = 0;

   _wcslwr(s);
   return s[0];
}

#else

char* util_clone(const char* s)
{
   size_t length = getlength(s);

   return emptystr(s) ? nullptr : StrFactory::allocate(length + 1, s);
}

unsigned short* util_clone(const unsigned short* s, size_t length)
{
   if (!emptystr(s))
   {
      unsigned short* copy = StrFactory::allocate(length + 1, s);
      copy[length] = 0;

      return copy;
   }
   else return nullptr;
}

unsigned short* util_clone(const unsigned short* s)
{
   return util_clone(s, getlength(s));
}

void append(unsigned short* dest, const unsigned short* sour, size_t length)
{
   unsigned short* p = dest + getlength(dest);
   for (size_t i = 0; i < length; i++)
      p[i] = sour[i];

   p[length] = 0;
}

inline bool util_compare(const unsigned short* s1, const unsigned short* s2)
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

inline bool util_compare(const unsigned short* s1, const unsigned short* s2, size_t length)
{
   if (s1 && s2) {
      while (length > 0 && (*s1 || *s2)) {
         if (*s1++ != *s2++)
            return false;

         length--;
      }
      return true;
   }
   else return (s1 == s2);
}

inline size_t util_find(const unsigned short* s, unsigned short c, size_t length, size_t defValue)
{
   for (size_t i = 0; i < length; i++) {
      if (s[i] == c)
         return i;
   }

   return defValue;
}

inline size_t util_find(const unsigned short* s, unsigned short c, size_t defValue)
{
   const unsigned short* p = s;

   while (*p) {
      if (*p == c)
         return p - s;

      p++;
   }

   return defValue;
}

inline size_t util_find_last(const unsigned short* s, unsigned short c, size_t defValue)
{
   const unsigned short* p = s + getlength(s);

   while (p != s) {
      if (*p == c)
         return p - s;

      p--;
   }

   return defValue;
}

inline size_t util_find_str(const unsigned short* s, const unsigned short* subs, size_t defValue)
{
   size_t length = getlength(s);
   size_t sub_len = getlength(subs);

   for (size_t i = 0; i < length; i++) {
      bool found = true;
      for (size_t j = 0; j < sub_len; j++) {
         if (s[i + j] != subs[j]) {
            found = false;
            break;
         }            
      }

      if (found)
         return i;
   }

   return defValue;
}

inline size_t util_find_str(const unsigned short* s, size_t index, const unsigned short* subs, size_t defValue)
{
   size_t length = getlength(s);
   size_t sub_len = getlength(subs);

   for (size_t i = index; i < length; i++) {
      bool found = true;
      for (size_t j = 0; j < sub_len; j++) {
         if (s[i + j] != subs[j]) {
            found = false;
            break;
         }
      }

      if (found)
         return i;
   }

   return defValue;
}

inline char* util_lower(char* s)
{
   while (*s) {
      *s = tolower(*s); // !! temporal: currently only ascii symbols are handled

      s++;
   }
   return s;
}

inline char util_lower(char ch)
{
   return tolower(ch);
}

inline unsigned short* util_lower(unsigned short* s)
{
   while (*s) {
      *s = tolower(*s); // !! temporal: currently only ascii symbols are handled
      s++;
   }
   return s;
}

inline wchar_t util_lower(unsigned short ch)
{
   return tolower(ch); // !! temporal: currently only ascii symbols are handled
}

#endif

// --- StrUtil ---

char* StrUtil :: clone(const char* s)
{
   return util_clone(s);
}

wide_c* StrUtil :: clone(const wide_c* s)
{
   return util_clone(s);
}

void StrUtil :: move(char* s1, const char* s2, size_t length)
{
   memmove(s1, s2, length);
}

void StrUtil :: append(char* dest, const char* sour, size_t length)
{
   ::append(dest, sour, length);
}

void StrUtil :: append(wide_c* dest, const wide_c* sour, size_t length)
{
   ::append(dest, sour, length);
}

void StrUtil :: insert(char* s, size_t pos, size_t length, const char* subs)
{
   size_t totalLen = getlength(s);

   for (size_t i = totalLen; i > pos; i--) {
      s[i + length - 1] = s[i - 1];
   }

   memmove(s + pos, subs, length);

   s[totalLen + length] = 0;
}

size_t StrUtil :: findChar(const char* s, char ch, size_t length, size_t defaultValue)
{
   return util_find(s, ch, length, defaultValue);
}

char* StrUtil::lower(char* s)
{
   return util_lower(s);
}

char StrUtil::lower(char ch)
{
   return util_lower(ch);
}

wide_c* StrUtil::lower(wide_c* s)
{
   return util_lower(s);
}

wide_c StrUtil::lower(wide_c ch)
{
   return util_lower(ch);
}

// --- StrFactory ---

char* StrFactory::allocate(size_t size, const char* value)
{
   char* s = (char*)malloc(size);
#ifdef _DEBUG
   if (!s)
      return nullptr;
#endif

   if (!emptystr(value)) {
      memcpy(s, value, size);
   }
   else s[0] = 0;

   return s;
}

char* StrFactory :: reallocate(char* s, size_t size)
{
   return (char*)realloc(s, size);
}

#ifdef _MSC_VER

void StrUtil :: move(wchar_t* s1, const wchar_t* s2, size_t length)
{
   memmove(s1, s2, length << 1);
}

wide_c* StrFactory::allocate(size_t size, const wide_c* value)
{
   wchar_t* s = (wchar_t*)malloc(size << 1);

#ifdef _DEBUG
   if (!s)
      return nullptr;
#endif

   if (value) {
      wcsncpy(s, value, size);
   }
   else s[0] = 0;

   return s;
}

wchar_t* StrFactory::reallocate(wchar_t* s, size_t size)
{
   return (wchar_t*)realloc(s, size << 1);
}

#else

void StrUtil :: move(unsigned short* s1, const unsigned short* s2, size_t length)
{
   memmove(s1, s2, length << 1);
}

unsigned short* StrFactory::allocate(size_t size, const unsigned short* value)
{
   unsigned short* s = (unsigned short*)malloc(size << 1);
   if (value)
      memcpy(s, value, size << 1);

   return s;
}

unsigned short* StrFactory::reallocate(unsigned short* s, size_t size)
{
   return (unsigned short*)realloc(s, size << 1);
}

#endif

// --- ustr_t ---

bool ustr_t :: compare(const char* s) const
{
   return util_compare(_string, s);
}

bool ustr_t::compare(const char* s, size_t length) const
{
   return util_compare(_string, s, length);
}

bool ustr_t::compareSub(const char* s, size_t index, size_t length) const
{
   return util_compare(_string + index, s, length);
}

bool ustr_t::startsWith(const char* s)
{
   size_t slen = getlength(s);
   return compareSub(s, 0, slen);
}

bool ustr_t :: endsWith(const char* s)
{
   size_t slen = getlength(s);
   size_t len = getlength(_string);

   return compareSub(s, len - slen, slen);
}

size_t ustr_t::find(char c, size_t defValue)
{
   return util_find(_string, c, getlength(_string), defValue);
}

size_t ustr_t::findSub(size_t index, char c, size_t defValue)
{
   return util_find(_string + index, c, getlength(_string), defValue - index) + index;
}

size_t ustr_t::findSub(size_t index, char c, size_t length, size_t defValue)
{
   return util_find(_string + index, c, length, defValue - index) + index;
}

size_t ustr_t :: findLast(char c, size_t defValue)
{
   return util_find_last(_string, c, defValue);
}

size_t ustr_t::findLastSub(size_t index, char c, size_t defValue)
{
   return util_find_last(_string + index, c, defValue - index) + index;
}

size_t ustr_t::findStr(const char* subs, size_t defValue)
{
   return util_find_str(_string, subs, defValue);
}

size_t ustr_t::findSubStr(size_t index, const char* subs, size_t length, size_t defValue)
{
   return util_find_str(_string, index, subs, defValue);
}

char* ustr_t :: clone()
{
   return _string ? ::util_clone(_string) : nullptr;
}

char* ustr_t :: clone(size_t index, size_t length)
{
   return ::util_clone(_string + index, length);
}

bool ustr_t :: copyTo(char* dest, size_t length, size_t& destLength)
{
   return StrConvertor::copy(dest, _string, length, destLength);
}

bool ustr_t::copyTo(wide_c* dest, size_t length, size_t& destLength)
{
   return StrConvertor::copy(dest, _string, length, destLength);
}

// --- wstr_t ---

bool wstr_t :: compare(const wide_c* s) const
{
   return util_compare(_string, s);
}

bool wstr_t::compareSub(const wide_c* s, size_t index, size_t length) const
{
   return util_compare(_string + index, s, length);
}

bool wstr_t::endsWith(const wide_c* s)
{
   size_t slen = getlength(s);
   size_t len = getlength(_string);

   return compareSub(s, len - slen, slen);
}

bool wstr_t :: startsWith(const wide_c* s)
{
   size_t slen = getlength(s);
   return compareSub(s, 0, slen);
}

size_t wstr_t::find(wide_c c, size_t defValue)
{
   return util_find(_string, c, getlength(_string), defValue);
}

size_t wstr_t::findSub(size_t index, char c, size_t defValue)
{
   return util_find(_string + index, c, getlength(_string), defValue - index) + index;
}

size_t wstr_t::findSub(size_t index, char c, size_t length, size_t defValue)
{
   return util_find(_string + index, c, length, defValue - index) + index;
}

size_t wstr_t::findStr(const wide_c* subs, size_t defValue)
{
   return util_find_str(_string, subs, defValue);
}

size_t wstr_t::findSubStr(size_t index, const wide_c* subs, size_t length, size_t defValue)
{
   return util_find_str(_string, index, subs, defValue);
}

size_t wstr_t :: findLast(wide_c c, size_t defValue)
{
   return util_find_last(_string, c, defValue);
}

size_t wstr_t :: findLastSub(size_t index, char c, size_t defValue)
{
   return util_find_last(_string + index, c, defValue - index) + index;
}

wide_c* wstr_t :: clone()
{
   return _string ? ::util_clone(_string) : nullptr;
}

wide_c* wstr_t :: clone(size_t index, size_t length)
{
   return ::util_clone(_string + index, length);
}

bool wstr_t::copyTo(char* dest, size_t length, size_t& destLength)
{
   return StrConvertor::copy(dest, _string, length, destLength);
}

bool wstr_t::copyTo(wide_c* dest, size_t length, size_t& destLength)
{
   return StrConvertor::copy(dest, _string, length, destLength);
}
