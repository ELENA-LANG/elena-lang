//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class declarations.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef filesH
#define filesH 1

namespace _ELENA_
{

#define LOCAL_PATH_LENGTH FILENAME_MAX

// --- path_t ---

class path_t
{
   const path_c* _path;

public:
   operator const path_c*() const { return _path; }

   path_t& operator +=(int offset)
   {
      _path += offset;

      return *this;
   }

   path_c* clone() const
   {
      return StrFactory::clone(_path);
   }

   int findLast(path_c ch, int defValue = -1)
   {
      int index = getlength(_path) - 1;
      while (index > 0) {
         if (_path[index] == ch)
            return index;

         index--;
      }

      return defValue;
   }
   int findLast(int index, path_c ch, int defValue)
   {
      int i = getlength(_path);
      while (i >= index) {
         if (_path[i] == ch)
            return i;

         i--;
      }

      return defValue;
   }

   int find(path_c ch)
   {
      for (size_t i = 0; i < getlength(_path); i++) {
         if (_path[i] == ch)
            return i;
      }

      return -1;
   }

   int find(int index, path_c ch)
   {
      for (size_t i = index; i < getlength(_path); i++) {
         if (_path[i] == ch)
            return i;
      }

      return -1;
   }

   void copyTo(char* buffer, size_t& length) const
   {
      Convertor::copy(buffer, _path, getlength(_path), length);
   }

   void copyTo(char* buffer, size_t length, size_t& destLength) const
   {
      Convertor::copy(buffer, _path, length, destLength);
   }

#ifdef _WIN32

   bool compare(const wchar_t* s, int length)
   {
      return wide_t(_path).compare(s, length);
   }

#else

   bool compare(const char* s, int length)
   {
      return ident_t(_path).compare(s, length);
   }

   bool compare(const char* s)
   {
      return ident_t(_path).compare(s, getlength(s));
   }

#endif

   path_t()
   {
      _path = NULL;
   }
   path_t(const path_c* value)
   {
      _path = value;
   }
};

// --- Path ---

class Path
{
   String<path_c, LOCAL_PATH_LENGTH>  _path;

public:
   static bool comparePaths(path_t s1, path_t s2, size_t length);

   static bool checkExtension(path_t path, path_t extension);

   operator const path_c*() const { return _path; }

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
      while (!stopped) {
         int pos = name.find('\'');
         if (pos == -1) {
            pos = getlength(name);
            stopped = true;
         }

         bufLen = maxLen;
         name.copyTo(buf, (size_t)pos, bufLen);
         maxLen -= bufLen;

         combine(path_t(buf), bufLen);
         name += pos + 1;
      }
      appendExtension(extension);
   }

   void changeExtension(path_t extension)
   {
      path_t path(_path.str());
      int namepos = path.findLast(PATH_SEPARATOR) + 1;
      int index = path.findLast(namepos, '.', -1);
      if (index >= 0) {
         _path[index] = 0;
      }
      _path.append('.');
      _path.append(extension);
   }

   bool isEmpty()
   {
      return getlength(_path) == 0;
   }

#ifdef _WIN32
   static bool checkExtension(path_t path, const char* extension)
   {
      Path ext(extension);

      return checkExtension(path, ext.c_str());
   }

   static bool checkExtension(const char* path, const char* extension)
   {
      ident_t s = path;
      int len = getlength(s);

      int namepos = s.findLast(PATH_SEPARATOR) + 1;

      int pos = s.findSubStr(namepos, '.', len - namepos, -1);
      if (pos != -1) {
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

   void append(wide_t s)
   {
      _path.append(s);
   }

#else

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


#endif

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
      int pos =  path.findLast(PATH_SEPARATOR);
      if (pos > 0) {
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

#ifdef _WIN32
   Path(path_t rootPath, ident_t subPath)
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
#else
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

   path_t str() { return path_t(_path); }

   void copyName(path_t path)
   {
      int index = path.findLast(PATH_SEPARATOR) + 1;
      int dotpos = path.findLast('.', getlength(path));

      _path.copy(path + index, dotpos - index);
   }

   void copyExtension(path_t path)
   {
      int len = getlength(path);
      int dotpos = path.findLast('.', len);

      _path.copy(path + dotpos + 1);
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

#ifdef _WIN32
   FileName(const char* pathStr)
   {
      Path path(pathStr);

      copyName(path.c_str());
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

   bool readLine(char* s, size_t length);

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
   virtual void reset()
   {
      _file.rewind();
   }

   virtual size_t Position() { return _file.Position(); }

   virtual bool seek(size_t position) { return _file.seek(position); }

   int getEncoding() const { return _file.getEncoding(); }

   bool isOpened() const { return _file.isOpened(); }

   virtual bool read(char* s, size_t length);

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
