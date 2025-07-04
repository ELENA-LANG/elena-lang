//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine Path class declarations.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PATHS_H
#define PATHS_H

namespace elena_lang
{

   // --- path_t ---
#if (defined(_WIN32) || defined(__WIN32__))

   typedef wstr_t path_t;

#elif defined(__unix__) || defined(__MACH__)

   typedef ustr_t path_t;

#endif

   class PathString;

   // --- PathUtil ---
   class PathUtil
   {
   public:
      static bool ifExist(path_t path);

      static bool compare(path_t path1, path_t path);
      static bool compare(path_t s1, path_t s2, size_t length);

      static bool removeFile(path_t path);

      static bool recreatePath(/*path_t root, */path_t path);

      static bool isRelative(path_t path, size_t length);

      static bool checkExtension(path_t path, path_t extension);
#if (defined(_WIN32) || defined(__WIN32__))
      static bool checkExtension(path_t path, ustr_t extension);
#endif

      static void combineCanonicalized(PathString& target, path_t subpath);

      static void makeCorrectExePath(PathString& target);
   };

   // --- PathString ---
   class PathString : public String<path_c, FILENAME_MAX>
   {
   public:
      path_t operator*() const { return path_t(_string); }

      void copy(path_t path)
      {
         String::copy(path);
      }
      void copy(path_t path, size_t length)
      {
         String::copy(path, length);
      }
      void copySubPath(path_t path, bool includeSeparator)
      {
         size_t pos = path.findLast(PATH_SEPARATOR);
         if (pos != NOTFOUND_POS) {
            if (includeSeparator)
               pos++;

            copy(path, pos);
         }
         else clear();
      }

      void combine(path_t path)
      {
         combine(path, path.length());
      }
      void combine(path_t path, size_t length)
      {
         if (length > 0) {
            if (PathUtil::isRelative(path, length)) {
               size_t strLength = getlength(_string);

               if (strLength > 0 && _string[strLength - 1] != PATH_SEPARATOR)
                  append(PATH_SEPARATOR);

               append(path, length);
            }
            else copy(path, length);
         }
      }

      void appendSubName(path_t name, size_t len)
      {
         if (!emptystr(name)) {
            append('.');
            append(name, len);
         }
      }

      void appendExtension(path_t extension)
      {
         append('.');
         append(extension, extension.length());
      }

      void changeExtension(path_t extension)
      {
         path_t p(_string);

         size_t namepos = p.findLast(PATH_SEPARATOR) + 1;
         size_t index = p.findLastSub(namepos, '.', NOTFOUND_POS);
         if (index != NOTFOUND_POS) {
            _string[index] = 0;
         }
         append('.');
         append(extension, extension.length());
      }

      PathString() = default;

      PathString(path_t path)
      {
         copy(path);
      }
      PathString(path_t root, path_t subPath)
      {
         copy(root);
         combine(subPath);
      }
#if (defined(_WIN32) || defined(__WIN32__))
      bool append(const path_c* s, size_t length)
      {
         return String::append(s, length);
      }
      bool append(const path_c* s)
      {
         return String::append(s);
      }
      bool append(path_c c)
      {
         return String::append(c);
      }
      void append(ustr_t s)
      {
         PathString tmp(s);

         append(*tmp);
      }

      void changeExtension(ustr_t extension)
      {
         PathString ext(extension);

         changeExtension(*ext);
      }

      void appendExtension(ustr_t extension)
      {
         PathString ext(extension);

         appendExtension(*ext);
      }

      void copy(ustr_t path)
      {
         size_t destLen = FILENAME_MAX;
         StrConvertor::copy(_string, path.str(), path.length(), destLen);

         _string[destLen] = 0;
      }
      void copy(ustr_t path, size_t length)
      {
         size_t destLen = FILENAME_MAX;
         StrConvertor::copy(_string, path.str(), length, destLen);

         _string[destLen] = 0;
      }
      void combine(ustr_t path)
      {
         PathString pathStr(path);
         combine(*pathStr);
      }
      void combine(ustr_t path, size_t index)
      {
         PathString pathStr(path, index);
         combine(*pathStr);
      }

      PathString(ustr_t path)
      {
         copy(path);
      }
      PathString(ustr_t path, size_t index)
      {
         copy(path, index);
      }
      PathString(path_t root, ustr_t path)
      {
         copy(root);

         PathString subPath(path);
         combine(*subPath);
      }
#endif
   };

   // --- FileNameString ---
   class FileNameString : public String<path_c, FILENAME_MAX>
   {
   public:
      path_t operator*() const { return path_t(_string); }

      void copyName(path_t path)
      {
         size_t index = path.findLast(PATH_SEPARATOR) + 1;
         size_t dotIndex = path.findLastSub(index, '.', path.length());

         copy(path.str() + index, dotIndex - index);
      }
      void copyExtension(path_t path)
      {
         size_t index = path.findLast(PATH_SEPARATOR) + 1;
         size_t dotIndex = path.findLastSub(index, '.', path.length());

         append('.');
         append(path.str() + dotIndex + 1);
      }

      FileNameString(path_t path, bool withExtension = false)
      {
         copyName(path);
         if (withExtension)
            copyExtension(path);
      }
   };

   inline void freepath(path_t path)
   {
      path.free();
   }

}

#endif
