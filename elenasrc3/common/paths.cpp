//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Common Library
//
//		This file contains ELENA Engine File class implementations.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "common.h"
// -------------------------------------------------------
#include "paths.h"

using namespace elena_lang;

#ifdef _MSC_VER

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
   dirPath.copySubPath(path);

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

#elif __GNUG__

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
   dirPath.copySubPath(path);

   if (!dirPath.empty()) {
      if (checkDir(dirPath.str(), 0) != 0) {
         createDir(dirPath.str());

         return checkDir(dirPath.str(), 0) == 0;
      }
   }
   return true;
}

#endif

bool PathUtil :: checkExtension(path_t path, path_t extension)
{
   size_t namepos = path.findLast(PATH_SEPARATOR) + 1;
   size_t pos = path.findLastSub(namepos, '.', NOTFOUND_POS);
   if (pos != NOTFOUND_POS) {
      return extension.compare(path.str() + pos + 1);
   }
   else return extension.empty();
}
