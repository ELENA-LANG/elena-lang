//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class declarations.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef filesH
#define filesH 1

namespace _ELENA_
{

#define LOCAL_PATH_LENGTH FILENAME_MAX

// --- path_t ---

#ifdef _MSC_VER

typedef wide_t path_t;

#elif _LINUX

typedef ident_t path_t;

#endif

// --- Path ---

class Path
{
   String<path_c, LOCAL_PATH_LENGTH>  _path;

public:
   static bool ifExist(path_t path);

   static bool comparePaths(path_t s1, path_t s2, size_t length);

   static bool checkExtension(path_t path, path_t extension);

   operator const path_c*() const { return _path; }

   path_c& operator[](size_t index)
   {
      return *(_path + index);
   }

   path_c& operator[](int index)
   {
      return *(_path + index);
   }

   path_t str() const { return path_t(_path); }
   const path_c* c_str() const { return _path; }

   static bool isRelative(path_t path, size_t length);

   static bool create(path_t root, path_t path);

   void nameToPath(ident_t name, path_t extension)
   {
      path_c buf[LOCAL_PATH_LENGTH];
      size_t bufLen;
      size_t maxLen = LOCAL_PATH_LENGTH;

      bool stopped = false;
      bool rootNs = true;
      while (!stopped) {
         size_t pos = name.find('\'');
         if (pos == NOTFOUND_POS) {
            pos = getlength(name);
            stopped = true;
         }

         bufLen = maxLen;
         name.copyTo(buf, (size_t)pos, bufLen);
         maxLen -= bufLen;

         if (rootNs) {
            combine(path_t(buf), bufLen);
            rootNs = false;
         }
         else appendSubName(path_t(buf), bufLen);

         name += pos + 1;
      }
      appendExtension(extension);
   }

   void changeExtension(path_t extension)
   {
      path_t path(_path.c_str());
      size_t namepos = path.findLast(PATH_SEPARATOR) + 1;
      size_t index = path.findLast(namepos, '.', NOTFOUND_POS);
      if (index != NOTFOUND_POS) {
         _path[index] = 0;
      }
      _path.append('.');
      _path.append(extension);
   }

   bool isEmpty()
   {
      return getlength(_path) == 0;
   }

#ifdef _MSC_VER
   static bool checkExtension(path_t path, const char* extension)
   {
      Path ext(extension);

      return checkExtension(path, ext.c_str());
   }

   static bool checkExtension(const char* path, const char* extension)
   {
      ident_t s = path;
      size_t len = getlength(s);

      size_t namepos = s.findLast(PATH_SEPARATOR) + 1;

      size_t pos = s.findLastSubStr(namepos, '.', len - namepos, NOTFOUND_POS);
      if (pos != NOTFOUND_POS) {
         return ident_t(path + pos + 1).compare(extension);
      }
      else return emptystr(extension);
   }

   void copy(const char* path)
   {
      size_t len = LOCAL_PATH_LENGTH;

      wchar_t temp[LOCAL_PATH_LENGTH];
      Convertor::copy(temp, path, getlength(path), len);
      temp[len] = 0;

      _path.copy(temp);
   }

   void copy(const char* path, size_t path_len)
   {
      size_t len = LOCAL_PATH_LENGTH;

      wchar_t temp[LOCAL_PATH_LENGTH];
      Convertor::copy(temp, path, path_len, len);
      temp[len] = 0;

      _path.copy(temp);
   }

   void combine(path_t path, size_t length)
   {
      if (length > 0) {
         if (isRelative(path, length)) {
            size_t strLength = getlength(_path);

            if(strLength > 0 && _path[strLength - 1] != PATH_SEPARATOR)
               _path.append(PATH_SEPARATOR);

            _path.append(path, length);
         }
         else _path.copy(path, length);
      }
   }
   void combine(path_t path)
   {
      combine(path, getlength(path));
   }

   void combine(ident_t path)
   {
      size_t len = getlength(path);

      wchar_t temp[LOCAL_PATH_LENGTH];
      path.copyTo(temp, len);
      temp[len] = 0;

      combine(temp);
   }
   void combine(ident_t path, size_t len)
   {
      wchar_t temp[LOCAL_PATH_LENGTH];
      path.copyTo(temp, len);
      temp[len] = 0;

      combine(temp);
   }

   void copySubPath(ident_t path)
   {
      size_t len = getlength(path);

      wchar_t temp[LOCAL_PATH_LENGTH];
      path.copyTo(temp, len);
      temp[len] = 0;

      copySubPath(temp);
   }

   void changeExtension(const char* s)
   {
      Path ext(s);

      changeExtension(ext.c_str());
   }

   void append(const char* s)
   {
      Path temp(s);

      append(temp.c_str());
   }

   void append(wide_t s)
   {
      _path.append(s);
   }

#elif _LINUX

   void combine(const char* path, size_t length)
   {
      if (length > 0) {
         if (isRelative(path, length)) {
            size_t strLength = getlength(_path);

            if(strLength > 0 && _path[strLength - 1] != PATH_SEPARATOR)
               _path.append(PATH_SEPARATOR);

            _path.append(path, length);
         }
         else _path.copy(path, length);
      }
   }
   void combine(const char* path)
   {
      combine(path, getlength(path));
   }

   void append(const char* s)
   {
      _path.append(s);
   }

   void copySubPath(const char* path)
   {
      copySubPath(path_t(path));
   }

   void copy(const char* path, size_t path_len)
   {
      _path.copy(path, path_len);
   }

#endif

   static bool checkExtension(path_t path)
   {
      size_t index = path.findLast(PATH_SEPARATOR) + 1;
      size_t dotpos = path.findLast(index, '.', NOTFOUND_POS);

      return dotpos != NOTFOUND_POS;
   }

   path_c* clone()
   {
      return StrFactory::clone(_path);
   }

   void copy(path_t path)
   {
      _path.copy(path);
   }

   void copySubPath(path_t path)
   {
      size_t pos =  path.findLast(PATH_SEPARATOR);
      if (pos != NOTFOUND_POS) {
         _path.copy(path, pos);
         _path[pos] = 0;
      }
      else _path.clear();
   }

   void append(wide_c c)
   {
      _path.append(c);
   }

   void appendInt(int n)
   {
      _path.appendInt(n);
   }

   void appendExtension(path_t extension)
   {
      if(!emptystr(extension)) {
         _path.append('.');
         _path.append(extension);
      }
   }

   void appendSubName(path_t name, size_t len)
   {
      if (!emptystr(name)) {
         _path.append('.');
         _path.append(name, len);
      }
   }

   void lower()
   {
      _path.lower();
   }

   void clear()
   {
      _path.clear();
   }

   Path()
   {

   }

   Path(const path_c* rootPath)
      : _path(rootPath)
   {
   }

   Path(path_t rootPath)
      : _path(rootPath)
   {
   }

   Path(path_t rootPath, size_t length)
      : _path(rootPath, length)
   {
   }

#ifdef _MSC_VER
   Path(path_t rootPath, ident_t subPath)
      : _path(rootPath)
   {
      combine(subPath);
   }

   Path(path_t rootPath, path_t subPath)
	   : _path(rootPath)
   {
	   combine(subPath);
   }

   Path(ident_t path)
   {
      size_t length = LOCAL_PATH_LENGTH;
      path.copyTo(_path, getlength(path), length);
      _path[length] = 0;
   }

   Path(ident_t path, ident_t subPath)
   {
      size_t length = LOCAL_PATH_LENGTH;
      path.copyTo(_path, getlength(path), length);
      _path[length] = 0;

      combine(subPath);
   }
#elif _LINUX
   Path(const char* rootPath, const char* subPath)
      : _path(rootPath)
   {
      combine(subPath);
   }
#endif
};

// --- FileNameTemplate ---

class FileName
{
   String<path_c, LOCAL_PATH_LENGTH>  _path;

public:
   operator const path_c*() const { return path_t(_path); }

   path_t c_str() { return path_t(_path); }

   void copyName(path_t path)
   {
      size_t index = path.findLast(PATH_SEPARATOR) + 1;
      size_t dotpos = path.findLast('.', getlength(path));

      _path.copy(path + index, dotpos - index);
   }

   void copyExtension(path_t path)
   {
      size_t len = getlength(path);
      size_t dotpos = path.findLast('.', len);

      _path.copy(path + dotpos + 1);
   }

   void appendExtension(path_t path)
   {
      size_t len = getlength(path);
      size_t dotpos = path.findLast('.', len);

      _path.append('.');
      _path.append(path + dotpos + 1);
   }

   bool isEmpty() const
   {
      return getlength(_path) == 0;
   }

   void clear()
   {
      _path.clear();
   }

   FileName(const path_c* path)
   {
      copyName(path);
   }

#ifdef _MSC_VER
   FileName(const char* pathStr)
      : FileName(pathStr, false)
   {
   }

   FileName(const char* pathStr, bool withExtension)
   {
      Path path(pathStr);

      copyName(path.c_str());
      if (withExtension) {
         appendExtension(path.c_str());
      }
   }
#elif _LINUX
   FileName(const char* pathStr, bool withExtension)
   {
      copyName(pathStr);
      if (withExtension) {
         appendExtension(pathStr);
      }
   }

#endif

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

   bool readLine(char* s, int length);

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

   virtual pos_t Position() { return _file.Position(); }
   virtual pos_t Length() { return _file.Length(); }

   virtual bool seek(pos_t position) { return _file.seek(position); }
   virtual bool seek(pos64_t position)
   {
      if (position < INT_MAX) {
         return _file.seek((long)position);
      }
      else return false;
   }

   virtual bool read(void* s, pos_t length);

   bool readText(wide_c* s, pos_t length, pos_t& wasread)
   {
#if defined(_WIN64) || defined(__LP64__)
      size_t longLength = length;
      size_t longWasRead = wasread;

      bool retVal = _file.readLiteral(s, longLength, longWasRead);

      wasread = (pos_t)longWasRead;

      return retVal;
#else
      return _file.readLiteral(s, length, wasread);
#endif
   }

   bool readText(char* s, pos_t length, pos_t& wasread)
   {
#if defined(_WIN64) || defined(__LP64__)
      size_t longLength = length;
      size_t longWasRead = wasread;

      bool retVal = _file.readLiteral(s, longLength, longWasRead);

      wasread = (pos_t)longWasRead;

      return retVal;
#else
      return _file.readLiteral(s, length, wasread);
#endif
   }

   bool readText(char* s, pos64_t length, pos64_t& wasread)
   {
#if defined(_WIN64)
      return _file.readLiteral(s, length, wasread);
#elif defined(__LP64__)
      size_t tmp_wasread = 0;
      bool retVal = _file.readLiteral(s, length, tmp_wasread);
      
      wasread = tmp_wasread;

      return retVal;
#else
      size_t longLength = (size_t)length;
      size_t longWasRead = (size_t)wasread;

      bool retVal = _file.readLiteral(s, longLength, longWasRead);

      wasread = (pos64_t)longWasRead;

      return retVal;
#endif
   }
   bool readText(wide_c* s, pos64_t length, pos64_t& wasread)
   {
#if defined(_WIN64)
      return _file.readLiteral(s, length, wasread);
#elif defined(__LP64__)
      size_t tmp_wasread = 0;
      bool retVal = _file.readLiteral(s, length, tmp_wasread);

      wasread = tmp_wasread;

      return retVal;
#else
      size_t longLength = (size_t)length;
      size_t longWasRead = (size_t)wasread;

      bool retVal = _file.readLiteral(s, longLength, longWasRead);

      wasread = (pos64_t)longWasRead;

      return retVal;
#endif
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

   virtual pos_t Position() const { return _file.Position(); }
   virtual pos_t Length()   { return _file.Length(); }

   virtual bool isOpened() { return _file.isOpened(); }

   virtual bool write(const void* s, pos_t length);
   virtual bool writeLong(const void* s, pos64_t length)
   {
      if (length < INT_MAX) {
         return write(s, (pos_t)length);
      }
      else return false;
   }

   bool writeText(const wide_c* s, pos_t length)
   {
      return _file.writeLiteral(s, length);
   }

   bool writeText(const char* s, pos_t length)
   {
      return _file.writeLiteral(s, length);
   }

   bool writeText(const wide_c* s, pos64_t length)
   {
#if defined(_WIN64) || defined(__LP64__)
      return _file.writeLiteral(s, length);
#else
      if (length < INT_MAX) {
         return _file.writeLiteral(s, (size_t)length);
      }
      else return false;
#endif
   }

   bool writeText(const char* s, pos64_t length)
   {
#if defined(_WIN64) || defined(__LP64__)
      return _file.writeLiteral(s, length);
#else
      if (length < INT_MAX) {
         return _file.writeLiteral(s, (size_t)length);
      }
      else return false;
#endif
   }

   virtual void align(int alignment);

   FileWriter(path_t path, int encoding, bool withBOM);
};

// --- TextFileReader class ---

class TextFileReader : public TextReader
{
   File _file;

public:
   virtual void reset()
   {
      _file.rewind();
   }

   virtual pos_t Position() { return _file.Position(); }

   virtual bool seek(pos_t position) { return _file.seek(position); }

   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool read(char* s, pos_t length);

   TextFileReader(path_t path, int encoding, bool withBOM);
};

// --- TextFileWriter class ---

class TextFileWriter : public TextWriter
{
   File _file;

public:
   virtual pos_t Position() const { return _file.Position(); }

   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool write(const wide_c* s, pos_t length);
   virtual bool write(const char* s, pos_t length);
   virtual bool write(const wide_c* s, pos64_t length)
   {
      if (length < INT_MAX) {
         return write(s, (pos_t)length);
      }
      else return false;
   }
   virtual bool write(const char* s, pos64_t length)
   {
      if (length < INT_MAX) {
         return write(s, (pos_t)length);
      }
      else return false;
   }

   virtual bool writeNewLine();

   TextFileWriter(path_t path, int encoding, bool withBOM);
};

} // _ELENA_

#endif // filesH
