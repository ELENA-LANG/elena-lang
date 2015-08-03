//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class declarations.
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef filesH
#define filesH 1

namespace _ELENA_
{

#define LOCAL_PATH_LENGTH FILENAME_MAX

// --- Path ---

class Path : public String<path_c, LOCAL_PATH_LENGTH>
{
public:
   static bool create(path_t root, path_t path);

   static bool isRelative(path_t path, size_t length);

   static bool comparePaths(path_t s1, path_t s2, size_t length);

   static void loadPath(Path& dest, ident_t sour)
   {
      size_t length = LOCAL_PATH_LENGTH;
      StringHelper::copy(dest, sour, getlength(sour), length);
      dest[length] = 0;
   }

   static void loadPath(Path& dest, ident_t sour, size_t sour_length)
   {
      size_t length = LOCAL_PATH_LENGTH;
      StringHelper::copy(dest, sour, sour_length, length);
      dest[length] = 0;
   }

   static void combinePath(Path& dest, ident_t sour)
   {
      Path subPath;
      loadPath(subPath, sour);

      dest.combine(subPath);
   }

   static void combinePath(Path& dest, ident_t sour, size_t length)
   {
      Path subPath;
      loadPath(subPath, sour, length);

      dest.combine(subPath, length);
   }

   static void appendPath(Path& dest, ident_t sour)
   {
      Path subPath;
      loadPath(subPath, sour);

      dest.append(subPath);
   }

   static void loadSubPath(Path& dest, ident_t sour)
   {
      Path subPath;
      loadPath(subPath, sour);

      dest.copySubPath(subPath);
   }

   static void savePath(path_t sour, ident_c* dest, size_t length)
   {
      StringHelper::copy(dest, sour, getlength(sour), length);
      dest[length] = 0;
   }

   static bool checkExtension(path_t path, path_t extension)
   {
      int namepos = StringHelper::findLast(path, PATH_SEPARATOR) + 1;

      int pos = StringHelper::findLast(path + namepos, '.');
      if (pos != -1) {
         return StringHelper::compare(path + namepos + pos + 1, extension);
      }
      else return emptystr(extension);
   }

#ifdef _WIN32
   static bool checkExtension(const char* path, const char* extension)
   {
      int namepos = StringHelper::findLast(path, PATH_SEPARATOR) + 1;

      int pos = StringHelper::findLast(path + namepos, '.');
      if (pos != -1) {
         return StringHelper::compare(path + namepos + pos + 1, extension);
      }
      else return emptystr(extension);
   }

   void changeExtension(const char* s)
   {
      Path ext;
      Path::loadPath(ext, s);

      changeExtension(ext);
   }

   static void combinePath(Path& dest, path_t sour)
   {
      Path subPath(sour);

      dest.combine(subPath);
   }

#endif

   static bool checkExtension(path_t path)
   {
      int index = StringHelper::findLast(path, PATH_SEPARATOR) + 1;
      int dotpos = StringHelper::findLast(path + index, '.', -1   );

      return dotpos != -1;
   }

   void combine(path_t path, size_t length)
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
   void combine(path_t path)
   {
      combine(path, getlength(path));
   }

   void copySubPath(path_t path)
   {
      int pos = StringHelper::findLast(path, PATH_SEPARATOR);
      if (pos > 0) {
         copy(path, pos);
         _string[pos] = 0;
      }
      else clear();
   }

   void appendExtension(path_t extension)
   {
      if(!emptystr(extension)) {
         append('.');
         append(extension);
      }
   }

   void nameToPath(ident_t name, path_t extension)
   {
      path_c buf[LOCAL_PATH_LENGTH];
      size_t bufLen;
      size_t maxLen = LOCAL_PATH_LENGTH;

      bool stopped = false;
      while (!stopped) {
         int pos = StringHelper::find(name, '\'');
         if (pos == -1) {
            pos = getlength(name);
            stopped = true;
         }

         bufLen = maxLen;
         StringHelper::copy(buf, name, (size_t)pos, bufLen);
         maxLen -= bufLen;

         combine(buf, bufLen);
         name += pos + 1;
      }
      appendExtension(extension);
   }

   void changeExtension(path_t extension)
   {
      path_t path = _string;
      int namepos = findLast(PATH_SEPARATOR) + 1;
      int index = StringHelper::findLast(path + namepos, '.');
      if (index >= 0) {
         _string[(size_t)index + namepos] = 0;
      }
      append('.');
      append(extension);
   }

   Path()
   {
   }
   Path(path_t filePath)
   {
      size_t length = LOCAL_PATH_LENGTH;
      StringHelper::copy(_string, filePath, getlength(filePath), length);

      _string[length] = 0;
   }
   Path(path_t filePath, size_t pathLength)
   {
      size_t length = LOCAL_PATH_LENGTH;
      StringHelper::copy(_string, filePath, pathLength, length);

      _string[length] = 0;
   }
};

// --- FileNameTemplate ---

class FileName : public String<path_c, LOCAL_PATH_LENGTH>
{
public:
   static void load(FileName& dest, const char* path)
   {
      int index = StringHelper::findLast(path, PATH_SEPARATOR) + 1;
      int dotpos = StringHelper::findLast(path, '.', getlength(path));

      size_t length = LOCAL_PATH_LENGTH;
      StringHelper::copy(dest, path + index, dotpos - index, length);
      dest[length] = 0;
   }

   void copyName(path_t path)
   {
      int index = StringHelper::findLast(path, PATH_SEPARATOR) + 1;
      int dotpos = StringHelper::findLast(path, '.', getlength(path));

      copy(path + index, dotpos - index);
      _string[dotpos - index] = 0;
   }

   void copyExtension(path_t path)
   {
      int dotpos = StringHelper::findLast(path, '.', getlength(path));

      copy(path + dotpos + 1);
   }

   FileName(path_t path)
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

   bool writeLiteral(const wide_c* s, size_t length);
   bool writeLiteral(const char* s, size_t length);
   bool writeNewLine();

   bool readLiteral(wide_c* s, size_t length, size_t& wasread);
   bool readLiteral(char* s, size_t length, size_t& wasread);

   bool readLine(ident_c* s, size_t length);

   void rewind();

   File(path_t path, path_t mode, int encoding, bool withBOM);
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

   bool readText(wide_c* s, size_t length, size_t& wasread)
   {
      return _file.readLiteral(s, length, wasread);
   }

   bool readText(char* s, size_t length, size_t& wasread)
   {
      return _file.readLiteral(s, length, wasread);
   }

   virtual const char* getLiteral(const char* def) { return def; }
   virtual const wide_c* getLiteral(const wide_c* def) { return def; }

   FileReader(path_t path, int encoding, bool withBOM);
   FileReader(path_t path, path_t mode, int encoding, bool withBOM);
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

   bool writeText(const wide_c* s, size_t length)
   {
      return _file.writeLiteral(s, length);
   }

   bool writeText(const char* s, size_t length)
   {
      return _file.writeLiteral(s, length);
   }

   virtual void align(int alignment);

   FileWriter(path_t path, int encoding, bool withBOM);
};

// --- TextFileReader class ---

class TextFileReader : public TextReader
{
   File _file;

public:
   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool read(ident_c* s, size_t length);

   TextFileReader(path_t path, int encoding, bool withBOM);
};

// --- TextFileWriter class ---

class TextFileWriter : public TextWriter
{
   File _file;

public:
   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool write(const wide_c* s, size_t length);
   virtual bool write(const char* s, size_t length);

   virtual bool writeNewLine();

   TextFileWriter(path_t path, int encoding, bool withBOM);
};

} // _ELENA_

#endif // filesH
