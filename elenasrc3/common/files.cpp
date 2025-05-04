//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class implementations.
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "common.h"
// -------------------------------------------------------
#include "files.h"
#include "errno.h"

using namespace elena_lang;

#if (defined(_WIN32) || defined(__WIN32__))

#define file_open _wfopen

#define NEW_LINE "\r\n"

#include <windows.h>
#include <direct.h>

#else

#define NEW_LINE "\n"

#define file_open fopen

#endif

constexpr auto TEMP_SIZE = 0x100;

// --- File ---

File :: File(path_t path, path_t mode)
{
   _file = file_open(path, mode);
}

bool File :: seek(long position)
{
   return fseek(_file, position, SEEK_SET) == 0;
}

bool File :: write(const void* s, const size_t length)
{
   return fwrite(s, 1, length, _file) > 0;
}

bool File :: read(void* s, const size_t length)
{
   return (fread(s, 1, length, _file) > 0);
}

bool File ::  readLine(char* s, const size_t length)
{
   return fgets(s, (int)length, _file) != nullptr;
}

bool File :: readText(char* s, FileEncoding encoding, size_t length, size_t& wasRead)
{
   switch (encoding) {
      case FileEncoding::UTF16:
      {
         wide_c temp[TEMP_SIZE];
         size_t count, destCount;
         while (length != 0) {
            count = (length > TEMP_SIZE) ? TEMP_SIZE : length;

            count = fread(temp, 2, count, _file);

            StrConvertor::copy(s, temp, count, destCount);
            wasRead += count;

            length -= count;
            s += destCount;
         }
         return wasRead != 0;
      }
#if (defined(_WIN32) || defined(__WIN32__))
      case FileEncoding::Ansi:
#endif
      case FileEncoding::UTF8:
         wasRead = fread((char*)s, 1, length, _file);
         return (wasRead > 0);
      default:
         return false;
   }
}

bool File :: readText(wide_c* s, FileEncoding encoding, size_t length, size_t& wasRead)
{
   switch (encoding) {
      case FileEncoding::UTF16:
         wasRead = fread((char*)s, 2, length, _file);
         return (wasRead > 0);
      case FileEncoding::UTF8:
      {
         wasRead = 0;

         char temp[TEMP_SIZE];
         size_t count;
         while (length > 0) {
            size_t converted = length;
            count = (length > 0x100) ? 0x100 : length;
            length -= count;

            count = fread(temp, 1, count, _file);

            StrConvertor::copy(s, temp, count, converted);
            wasRead += converted;
            s += converted;
         }
         return (wasRead > 0);
      }
#if (defined(_WIN32) || defined(__WIN32__))
      case FileEncoding::Ansi:
      default:
      {
         wasRead = 0;

         char temp[TEMP_SIZE];
         int count;
         while (length > 0) {
            count = (length > 0x100) ? 0x100 : (int)length;

            wasRead += fread(temp, 1, count, _file);

            MultiByteToWideChar((int)encoding, MB_PRECOMPOSED, temp, count, s, count);

            length -= count;
            s += count;
         }
         return (wasRead > 0);
      }
#else
      default:
         return false;
#endif
   }
}

bool File :: writeText(const wide_c* s, FileEncoding encoding, size_t length)
{
   switch (encoding) {
      case FileEncoding::UTF16:
         return fwrite((const char*)s, 2, length, _file) == length;
      case FileEncoding::UTF8:
      {
         char temp[TEMP_SIZE * 4];
         size_t count;
         while (length > 0) {
            count = (length > TEMP_SIZE) ? TEMP_SIZE : length;

            size_t utfCount = TEMP_SIZE * 4;
            wstr_t(s).copyTo(temp, count, utfCount);

            if (fwrite(temp, 1, utfCount, _file) <= 0)
               return false;

            length -= count;
            s += count;
         }
         return true;
      }
   #if (defined(_WIN32) || defined(__WIN32__))
      case FileEncoding::Ansi:
      default:
      {
         char temp[TEMP_SIZE];
         int count;
         while (length > 0) {
            count = (length > TEMP_SIZE) ? TEMP_SIZE : (int)length;

            BOOL withError = 0;
            WideCharToMultiByte((UINT)encoding, WC_COMPOSITECHECK, s, count, 
               temp, count, "?", &withError);

            if (fwrite(temp, 1, count, _file) <= 0)
               return false;

            length -= count;
            s += count;
         }
         return true;
      }
   #else
      default:
         return false;
   #endif
   }
}

bool File :: writeText(const char* s, FileEncoding encoding, size_t length)
{
   switch (encoding) {
      case FileEncoding::UTF8:
         return (fwrite(s, 1, length, _file) == length);
      case FileEncoding::UTF16:
      {
         wide_c temp[TEMP_SIZE * 4];
         size_t count;
         while (length > 0) {
            count = (length > TEMP_SIZE) ? TEMP_SIZE : length;

            size_t utf16Count = TEMP_SIZE * 4;
            ustr_t(s).copyTo(temp, count, utf16Count);

            if (fwrite(temp, 2, utf16Count, _file) <= 0)
               return false;

            length -= count;
            s += count;
         }
         return true;
      }
   #if (defined(_WIN32) || defined(__WIN32__))
      case FileEncoding::Ansi:
      default:
      {
         wide_c temp[TEMP_SIZE];
         int count;
         while (length > 0) {
            count = (length > TEMP_SIZE) ? TEMP_SIZE : (int)length;

            count = MultiByteToWideChar((UINT)encoding, 0, s, (int)length, temp, TEMP_SIZE);

            if (fwrite(temp, 2, count, _file) <= 0)
               return false;

            length -= count;
            s += count;
         }
         return true;
      }
   #else
      default:
         return false;
   #endif
   }
}

bool File :: eof() const
{
   if (_file) {
      long pos = ftell(_file);
      fgetc(_file);
      bool eof = (feof(_file) != 0);
      fseek(_file, pos, SEEK_SET);

      return eof;
   }
   else return true;

}

FileEncoding File :: detectEncoding(FileEncoding defaultEncoding)
{
   FileEncoding encoding = defaultEncoding;

   unsigned short signature = 0;
   size_t read = fread(&signature, 1, 2, _file);
   if (read == 2 && signature == 0xFEFF) {
      encoding = FileEncoding::UTF16;
   }
   else if (signature == 0xBBEF) {
      unsigned char ch;
      read = fread(&ch, 1, 1, _file);
      if (read == 1 && ch == 0xBF) {
         encoding = FileEncoding::UTF8;
      }
      else rewind();
   }
   else rewind();

   return encoding;
}

long File :: length() const
{
   const long position = ftell(_file);
   fseek(_file, 0, SEEK_END);

   const long length = ftell(_file);
   fseek(_file, position, SEEK_SET);

   return length;
}

long File :: position() const
{
   return ftell(_file);
}

void File :: rewind()
{
   ::rewind(_file);
}

File::~File()
{
   if (_file != nullptr) {
      fclose(_file);
   }
}

// --- FileWriter ---

FileWriter :: FileWriter(path_t path, FileEncoding encoding, bool withBOM)
   : _file(path, FileWBPlusMode)
{
   _encoding = encoding;

   if (withBOM && _file.isOpen() && encoding == FileEncoding::UTF16) {
      unsigned short signature = 0xFEFF;
      _file.write(&signature, 2);
   }
}

bool FileWriter::write(const void* s, pos_t length)
{
   return _file.write(s, length);
}

void FileWriter::align(unsigned int alignment)
{
   unsigned int len = ::align(_file.position(), alignment) - _file.position();

   writeBytes('\0', len);
}

// --- FileReader ---

FileReader :: FileReader(path_t path, path_t mode, FileEncoding encoding, bool withBOM)
   : _file(path, mode)
{
   if (_file.isOpen() && withBOM) {
      _encoding = _file.detectEncoding(encoding);
   }
   else _encoding = encoding;
}

bool FileReader :: read(void* s, pos_t length)
{
   return _file.read(s, length);
}

// --- TextFileReader ---

TextFileReader :: TextFileReader(path_t path, FileEncoding encoding, bool withBOM)
   : _file(path, FileRBMode)
{
   if (_file.isOpen() && withBOM) {
      _encoding = _file.detectEncoding(encoding);
   }
   else _encoding = encoding;
}

bool TextFileReader :: read(char* s, pos_t length)
{
   switch (_encoding) {
      case FileEncoding::UTF8:
#if (defined(_WIN32) || defined(__WIN32__))
      case FileEncoding::Ansi:
#endif
         return _file.readLine(s, length);
      default:
         return false;
   }
}

void TextFileReader :: reset()
{
   _file.rewind();
}

// --- TextFileWriter ---

TextFileWriter :: TextFileWriter(path_t path, FileEncoding encoding, bool withBOM)
   : _file(path, FileWBPlusMode)
{
   _encoding = encoding;
   if (withBOM) {
      if (encoding == FileEncoding::UTF16 && _file.isOpen()) {
         unsigned short signature = 0xFEFF;
         _file.write((void*)&signature, 2);
      }
      else if (encoding == FileEncoding::UTF8 && _file.isOpen()) {
         int signature = 0xBFBBEF;
         _file.write((void*)&signature, 3);
      }
   }
}

bool TextFileWriter :: write(const char* s, pos_t length)
{
   return _file.writeText(s, _encoding, length);
}

bool TextFileWriter :: writeNewLine()
{
   return write(NEW_LINE, getlength_pos(NEW_LINE));
}
