//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class declarations.
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef filesH
#define filesH 1

namespace _ELENA_
{

#define LOCAL_PATH_LENGTH FILENAME_MAX

// --- Path ---

class Path : public String<_path_t, LOCAL_PATH_LENGTH>
{
public:
   static bool create(const _path_t* root, const _path_t* path);

   static bool isRelative(const wchar16_t* path, size_t length);
   static bool isRelative(const char* path, size_t length);

   static bool checkExtension(const _path_t*  path, const _path_t* extension)
   {
      int namepos = StringHelper::findLast(path, PATH_SEPARATOR) + 1;

      int pos = StringHelper::findLast(path + namepos, '.');
      if (pos != -1) {
         return StringHelper::compare(path + namepos + pos + 1, extension);
      }
      else return emptystr(extension);
   }

   void combine(const wchar16_t* path, size_t length)
   {
      if (length > 0) {
         if (isRelative(path, length)) {
            size_t strLength = getlength(_string);

            if(strLength > 0 && _string[strLength - 1] != PATH_SEPARATOR)
               append((wchar16_t)PATH_SEPARATOR);

            append(path, length);
         }
         else copy(path, length);
      }
   }
   void combine(const wchar16_t* path)
   {
      combine(path, getlength(path));
   }
   void combine(const char* path, size_t length)
   {
      if (length > 0) {
         if (isRelative(path, length)) {
            size_t strLength = getlength(_string);

            if(strLength > 0 && _string[strLength - 1] != PATH_SEPARATOR)
               append(PATH_SEPARATOR);

            append(path, length);
         }
         else copy(path, length);
      }
   }
   void combine(const char* path)
   {
      combine(path, getlength(path));
   }

   void copyPath(const wchar16_t* path)
   {
      int pos = StringHelper::findLast(path, PATH_SEPARATOR);
      if (pos > 0) {
         copy(path, pos);
         _string[pos] = 0;
      }
      else clear();
   }

   void copyPath(const char* path)
   {
      int pos = StringHelper::findLast(path, PATH_SEPARATOR);
      if (pos > 0) {
         copy(path, pos);
         _string[pos] = 0;
      }
      else clear();
   }

   void copySubPath(const wchar16_t* path)
   {
      int pos = StringHelper::find(path, PATH_SEPARATOR);
      if (pos > 0) {
         copy(path + pos + 1);
      }
      else clear();
   }

   void appendExtension(const _path_t* extension)
   {
      if(!emptystr(extension)) {
         append('.');
         append(extension);
      }
   }

   void nameToPath(const wchar16_t* name, const _path_t* extension)
   {
      while (true) {
         int pos = StringHelper::find(name, '\'');
         if (pos != -1) {
            combine(name, pos);
            name += pos + 1;
         }
         else {
            combine(name);
            break;
         }
      }
      appendExtension(extension);
   }

   void changeExtension(const _path_t* extension)
   {
      const _path_t* path = _string;
      int namepos = findLast(PATH_SEPARATOR) + 1;
      int index = StringHelper::findLast(path + namepos, '.');
      if (index >= 0) {
         _string[(size_t)index + namepos] = 0;
      }
      append('.');
      append(extension);
   }

   _path_t* clone()
   {
      return StringHelper::clone(_string);
   }

   Path()
   {
   }
   Path(const char* filePath)
   {
      copy(filePath);
   }
   Path(const wchar16_t* filePath)
   {
      copy(filePath);
   }
   Path(const wchar16_t* rootPath, const wchar16_t* filePath)
   {
      copy(rootPath);
      combine(filePath);
   }
   Path(const char* rootPath, const char* filePath)
   {
      copy(rootPath);
      combine(filePath);
   }
   Path(const wchar16_t* rootPath, const wchar16_t* path, const wchar16_t* filePath)
   {
      copy(rootPath);
      combine(path);
      combine(filePath);
   }
   Path(const wchar16_t* rootPath, size_t length)
   {
      copy(rootPath, length);
      _string[length] = 0;
   }
};

// --- FileNameTemplate ---

class FileName : public String<_path_t, LOCAL_PATH_LENGTH>
{
public:
   void copyName(const char* path)
   {
      int index = StringHelper::findLast(path, PATH_SEPARATOR) + 1;
      int dotpos = StringHelper::findLast(path, '.', getlength(path));

      copy(path + index, dotpos - index);
      _string[dotpos - index] = 0;
   }

   void copyName(const wchar16_t* path)
   {
      int index = StringHelper::findLast(path, PATH_SEPARATOR) + 1;
      int dotpos = StringHelper::findLast(path, '.', getlength(path));

      copy(path + index, dotpos - index);
      _string[dotpos - index] = 0;
   }

   FileName(const wchar16_t* path)
   {
      copyName(path);
   }
   FileName(const char* path)
   {
      copyName(path);
   }
   FileName()
   {
   }
};

// --- File class ---

class File
{
   FILE* _file;
   int   _encoding;

   void detectEncoding();

public:
   bool isOpened() const { return (_file != NULL); }

   int getEncoding() const { return _encoding; }

   bool Eof();
   long Position() const;
   long Length();

   bool seek(long position);

   bool write(const void* s, size_t length);

   bool read(void* s, size_t length);

   bool writeLiteral(const wchar16_t* s, size_t length);
   bool writeLiteral(const char* s, size_t length);
   bool writeNewLine();

   bool readLiteral(wchar16_t* s, size_t length, size_t& wasread);
   bool readLiteral(char* s, size_t length, size_t& wasread);

   bool readLine(wchar16_t* s, size_t length);
   bool readLine(char* s, size_t length);

   void rewind();

#ifdef _WIN32
   File(const wchar_t* path, const wchar_t* mode, int encoding, bool autoDetect);
#endif
   File(const char* path, const char* mode, int encoding, bool autoDetect);
   ~File();
};

// --- FileReader class ---

class FileReader : public StreamReader
{
   File _file;

public:
   bool isOpened() const { return _file.isOpened(); }

   virtual bool Eof() { return _file.Eof(); }

   int getEncoding() const { return _file.getEncoding(); }

   virtual size_t Position() { return _file.Position(); }
   virtual size_t Length() { return _file.Length(); }

   virtual bool seek(size_t position) { return _file.seek(position); }

   virtual bool read(void* s, size_t length);

   bool readText(wchar16_t* s, size_t length, size_t& wasread)
   {
      return _file.readLiteral(s, length, wasread);
   }

   bool readText(char* s, size_t length, size_t& wasread)
   {
      return _file.readLiteral(s, length, wasread);
   }

   virtual const wchar16_t* getWideLiteral() { return NULL; }
   virtual const char* getLiteral() { return NULL; }

   FileReader(const _path_t* path, int encoding, bool autoDetect);
   FileReader(const _path_t* path, const _path_t* mode, int encoding, bool autoDetect);
};

// --- FileWriter class ---

class FileWriter : public StreamWriter
{
   File _file;

public:
   int getEncoding() const { return _file.getEncoding(); }

   virtual size_t Position() const { return _file.Position(); }
   virtual size_t Length()   { return _file.Length(); }

   virtual bool isOpened() { return _file.isOpened(); }

   virtual bool write(const void* s, size_t length);

   bool writeText(const wchar16_t* s, size_t length)
   {
      return _file.writeLiteral(s, length);
   }

   bool writeText(const char* s, size_t length)
   {
      return _file.writeLiteral(s, length);
   }

   virtual void align(int alignment);

   FileWriter(const _path_t* path, int encoding, bool autoDetect);
};

// --- TextFileReader class ---

class TextFileReader : public TextReader
{
   File _file;

public:
   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool read(wchar16_t* s, size_t length);
   virtual bool read(char* s, size_t length);

   TextFileReader(const _path_t* path, int encoding, bool autoDetect);
};

// --- TextFileWriter class ---

class TextFileWriter : public TextWriter
{
   File _file;

public:
   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool write(const wchar16_t* s, size_t length);
   virtual bool write(const char* s, size_t length);

   virtual bool writeNewLine();

   TextFileWriter(const _path_t* path, int encoding, bool autoDetect);
};

} // _ELENA_

#endif // filesH
