//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains implementation of PathHelper
//              used to retrieve data / config paths
//                                              (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "pathmanager.h"
#include <unistd.h>

#include <mach-o/dyld.h>

using namespace elena_lang;

// --- PathHelper ---

MacOSPathHelper::PathMap* MacOSPathHelper::pathCache = nullptr;

inline bool loadAppPath(char* appPath, unsigned int len)
{
   int res = _NSGetExecutablePath(appPath, &len);
   assert(res == 0);

   size_t index = path_t(appPath).findLast(PATH_SEPARATOR);
   if (index != NOTFOUND_POS)
      appPath[index] = 0;

   return true;
}

path_t MacOSPathHelper :: retrievePath(const char* filesToLookFor[], unsigned int listLength, path_t defaultPath)
{
   if (pathCache && pathCache->exist(defaultPath))
      return pathCache->get(defaultPath);

   char appPath[FILENAME_MAX] = { 0 };
   if (!loadAppPath(appPath, FILENAME_MAX))
      return defaultPath;

   for (unsigned int i = 0; i < listLength; i++) {
      PathString fullPath(appPath, filesToLookFor[i]);

      if (!PathUtil::ifExist(*fullPath))
         return defaultPath;
   }

   if (!pathCache) {
      pathCache = new PathMap(nullptr);
   }

   pathCache->add(defaultPath, path_t(appPath).clone());
   return pathCache->get(defaultPath);
}

path_t MacOSPathHelper :: retrieveFilePath(path_t defaultPath)
{
   if (pathCache && pathCache->exist(defaultPath))
      return pathCache->get(defaultPath);

   char appPath[FILENAME_MAX] = { 0 };
   if (!loadAppPath(appPath, FILENAME_MAX))
      return defaultPath;

   PathString fullPath(appPath);
   FileNameString fileName(defaultPath, true);
   fullPath.combine(*fileName);
   if (!PathUtil::ifExist(*fullPath))
      return defaultPath;

   if (!pathCache) {
      pathCache = new PathMap(nullptr);
   }

   pathCache->add(defaultPath, (*fullPath).clone());
   return pathCache->get(defaultPath);
}
