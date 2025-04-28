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
#include <unistd.h>

using namespace elena_lang;

// --- PathHelper ---

PathHelper::PathMap* PathHelper::pathCache = nullptr;

inline bool loadAppPath(char* appPath, size_t len)
{
#if defined(__FreeBSD__) 

   int mib[4];
   mib[0] = CTL_KERN;
   mib[1] = KERN_PROC;
   mib[2] = KERN_PROC_PATHNAME;
   mib[3] = -1;
   sysctl(mib, 4, appPath, len, nullptr, 0);

#elif defined(__unix__)

   if (readlink("/proc/self/exe", appPath, len) == -1)
      return false;

#endif

   size_t index = path_t(appPath).findLast(PATH_SEPARATOR);
   if (index != NOTFOUND_POS)
      appPath[index] = 0;

   return true;
}

path_t PathHelper :: retrievePath(const char* filesToLookFor[], size_t listLength, path_t defaultPath)
{
   if (pathCache && pathCache->exist(defaultPath))
      return pathCache->get(defaultPath);

   char appPath[FILENAME_MAX] = { 0 };
   if (!loadAppPath(appPath, FILENAME_MAX))
      return defaultPath;

   for (size_t i = 0; i < listLength; i++) {
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

path_t PathHelper :: retrieveFilePath(path_t defaultPath)
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
