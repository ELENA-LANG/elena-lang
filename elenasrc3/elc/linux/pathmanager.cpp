//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains implementation of PathHelper
//              used to retrieve data / config paths
//                                              (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "clicommon.h"
// --------------------------------------------------------------------------
#include "pathmanager.h"

using namespace elena_lang;

// --- PathHelper ---

path_t PathHelper :: retrievePath(const char** filesToLookFor, size_t listLength, path_t defaultPath)
{
   if (pathCache && pathCache->exist(defaultPath))
      return pathCache->get(defaultPath);

   char appPath[PATH_MAX] = { 0 };

   if (!readlink("/proc/self/exe", appPath, PATH_MAX) == -1)
      return defaultPath;

   for (size_t i = 0; i < listLength; i++) {
      PathString fullPath(appPath, filesToLookFor[i]);

      if (!PathUtil::ifExist(*fullPath))
         return defaultPath;
   }

   if (!pathCache) {
      pathCache = new PathCache(nullptr);
   }

   pathCache->add(defaultPath, path_t(appPath).clone());
   return pathCache->get(defaultPath);
}

path_t PathHelper :: retrieveFilePath(path_t defaultPath)
{
   if (pathCache && pathCache->exist(defaultPath))
      return pathCache->get(defaultPath);

   char appPath[PATH_MAX] = { 0 };

   if (!readlink("/proc/self/exe", appPath, PATH_MAX) == -1)
      return defaultPath;

   PathString fullPath(appPath);
   FileName fileName(defaultPath);
   fullPath.combine(*fileName);
   if (!PathUtil::ifExist(*fullPath))
      return defaultPath;

   pathCache->add(defaultPath, (*fullPath).clone());
   return pathCache->get(defaultPath);
}