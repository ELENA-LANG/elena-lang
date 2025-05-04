//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class declarations.
//
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef FILES_H
#define FILES_H

namespace elena_lang
{

   #if (defined(_WIN32) || defined(__WIN32__))

   // --- FileEncoding ---
   enum class FileEncoding { Ansi = 0, Raw = -1, UTF8 = -2, UTF16 = -3, UTF32 = -4 };

   // --- modes ---
   constexpr auto FileWBPlusMode = L"wb+";
   constexpr auto FileRBMode = L"rb";

   #else

   enum class FileEncoding { UTF8 = 0, Raw = -1, UTF16 = -2, UTF32 = -3 };

   // --- modes ---
   constexpr auto FileWBPlusMode = "wb+";
   constexpr auto FileRBMode = "rb";

   #endif

   // --- File ---

   class File
   {
      FILE* _file;

   public:
      bool isOpen() const { return _file != nullptr; }

      bool eof() const;

      FileEncoding detectEncoding(FileEncoding defaultEncoding);

      long length() const;
      long position() const;

      bool seek(long position);

      bool write(const void* s, const size_t length);
      bool read(void* s, const size_t length);

      bool readLine(char* s, const size_t length);

      bool readText(char* s, FileEncoding encoding, size_t size, size_t& wasRead);
      bool readText(wide_c* s, FileEncoding encoding, size_t size, size_t& wasRead);

      bool writeText(const wide_c* s, FileEncoding encoding, size_t size);
      bool writeText(const char* s, FileEncoding encoding, size_t size);

      void rewind();

      File(path_t path, path_t mode);
      ~File();
   };

   // --- FileReader ---

   class FileReader : public StreamReader
   {
      File         _file;
      FileEncoding _encoding;

   public:
      FileEncoding encoding() const { return _encoding; }

      bool eof() override { return _file.eof(); }

      bool isOpen() const { return _file.isOpen(); }

      pos_t length() const override { return _file.length(); }

      pos_t position() const override { return _file.position(); }

      bool seek(pos_t position) override { return _file.seek(position); }

      bool read(void* s, pos_t length) override;

      bool readText(char* s, size_t size, size_t& wasRead)
      {
         return _file.readText(s, _encoding, size, wasRead);
      }
      bool readText(wide_c* s, size_t size, size_t& wasRead)
      {
         return _file.readText(s, _encoding, size, wasRead);
      }

      FileReader(path_t path, path_t mode, FileEncoding encoding, bool withBOM);
   };

   // --- FileWriter ---

   class FileWriter : public StreamWriter
   {
      File         _file;
      FileEncoding _encoding;

   public:
      bool isOpen() const override { return _file.isOpen(); }

      pos_t length() const override { return _file.length(); }

      pos_t position() const override { return _file.position(); }

      bool write(const void* s, pos_t length) override;

      void align(unsigned int alignment);

      bool writeText(char* s, size_t size)
      {
         return _file.writeText(s, _encoding, size);
      }
      bool writeText(wide_c* s, size_t size)
      {
         return _file.writeText(s, _encoding, size);
      }

      FileWriter(path_t path, FileEncoding encoding, bool withBOM);
   };

   // --- TextFileReader ---
   class TextFileReader : public TextReader<char>
   {
      File         _file;
      FileEncoding _encoding;

   public:
      bool isOpen() const override { return _file.isOpen(); }

      pos_t position() const override { return _file.position(); }

      bool read(char* s, pos_t length) override;

      void reset() override;

      TextFileReader(path_t path, FileEncoding encoding, bool withBOM);
   };

   // --- TextFileWriter ---
   class TextFileWriter : public TextWriter<char>
   {
      File         _file;
      FileEncoding _encoding;

   public:
      bool isOpen() const override { return _file.isOpen(); }

      pos_t position() const override { return _file.position(); }

      bool write(const char* s, pos_t length) override;
      bool writeNewLine() override;

      TextFileWriter(path_t path, FileEncoding encoding, bool withBOM);
   };

}

#endif // FILES_H