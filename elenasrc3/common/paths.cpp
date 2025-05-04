//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class implementations.
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "common.h"
// -------------------------------------------------------
#include "paths.h"

using namespace elena_lang;

#if (defined(_WIN32) || defined(__WIN32__))

#include <direct.h>
#include <io.h>

inline int checkDir(const wchar_t* name, int mode)
{
   return _waccess(name, mode);
}

inline int createDir(const wchar_t* path)
{
   return _wmkdir(path);
}

bool PathUtil :: recreatePath(/*path_t root, */path_t path)
{
   PathString dirPath;
   dirPath.copySubPath(path, false);

   if (!dirPath.empty()) {
      if (checkDir(dirPath.str(), 0) != 0) {
         return createDir(dirPath.str()) == 0;
      }
   }
   return true;

   //if (checkDir(dirPath.str(), 0) != 0) {
   //   if (!dirPath.empty() && !(*dirPath).compare(root)) {
   //      recreatePath(root, dirPath.str());
   //   }
   //   return createDir(dirPath.str()) == 0;
   //}
   //else return false;
}

bool PathUtil ::removeFile(path_t path)
{
   return _wremove(path.str()) != 0;
}

bool PathUtil :: isRelative(path_t path, size_t length)
{
   if (path[0] != PATH_SEPARATOR) {
      for (size_t i = 0; i < length - 1; i++) {
         if (path[i] == ':' && path[i + 1] == PATH_SEPARATOR) {
            return false;
         }
      }
      return true;
   }
   else return false;
}

bool PathUtil :: checkExtension(path_t path, ustr_t extension)
{
   PathString wExtension(extension);
   return PathUtil::checkExtension(path, *wExtension);
}

void PathUtil :: makeCorrectExePath(PathString& target)
{
   if (!PathUtil::checkExtension(*target, "exe")) {
      target.appendExtension("exe");
   }
}

#elif defined(__unix__)

#include <unistd.h>
#include <sys/stat.h>

bool PathUtil :: isRelative(path_t path, size_t length)
{
   // !! temporal
   if (path[0] != PATH_SEPARATOR) {
      return true;
   }
   else return false;
}

inline int checkDir(const char* name, int mode)
{
   return access(name, mode);
}

inline void createDir(const char* path)
{
   mkdir(path, S_IRWXO | S_IRWXG);
}

bool PathUtil::recreatePath(path_t path)
{
   PathString dirPath;
   dirPath.copySubPath(path, false);

   if (!dirPath.empty()) {
      if (checkDir(dirPath.str(), 0) != 0) {
         createDir(dirPath.str());

         return checkDir(dirPath.str(), 0) == 0;
      }
   }
   return true;
}

bool PathUtil :: removeFile(path_t path)
{
   return ::remove(path.str()) != 0;;
}

void PathUtil::makeCorrectExePath(PathString& target)
{
}

#endif


bool PathUtil :: compare(path_t s1, path_t s2, size_t length)
{
   for (size_t i = 0; i < length; i++) {
      path_c ch1 = StrUtil::lower(s1[i]);
      path_c ch2 = StrUtil::lower(s2[i]);

      if (ch1 != ch2)
         return false;
   }
   return true;
}

bool PathUtil :: compare(path_t path1, path_t path2)
{
   size_t l1 = path1.length();
   size_t l2 = path2.length();

   if (l1 == l2) {
      for (size_t i = 0; i < l1; i++) {
         path_c ch1 = StrUtil::lower(path1[i]);
         path_c ch2 = StrUtil::lower(path2[i]);

         if (ch1 != ch2)
            return false;
      }

      return true;
   }
   else return false;
}

bool PathUtil :: ifExist(path_t path)
{
   return checkDir(path, 0) == 0;
}

bool PathUtil :: checkExtension(path_t path, path_t extension)
{
   size_t namepos = path.findLast(PATH_SEPARATOR) + 1;
   size_t pos = path.findLastSub(namepos, '.', NOTFOUND_POS);
   if (pos != NOTFOUND_POS) {
      return extension.compare(path.str() + pos + 1);
   }
   else return extension.empty();
}

void PathUtil :: combineCanonicalized(PathString& target, path_t subpath)
{
   PathString upperMask("..");

   if (target[target.length() - 1] == PATH_SEPARATOR)
      target.truncate(target.length() - 1);

   while (subpath.startsWith(*upperMask) && subpath[2] == PATH_SEPARATOR) {
      size_t index = (*target).findLast(PATH_SEPARATOR);
      if (index != NOTFOUND_POS) {
         target.truncate(index);

         subpath = subpath + 3;
      }
      else break;
   }

   target.combine(subpath);
}