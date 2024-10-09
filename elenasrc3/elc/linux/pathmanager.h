//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains declaration of PathHelper
//              used to retrieve data / config paths
//                                              (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELC_PATHMANAGER_H
#define ELC_PATHMANAGER_H

namespace elena_lang
{
   // --- PathHelper ---
   class PathHelper
   {
      typedef Map<path_t, path_t, allocUStr, freeUStr, freepath> PathMap;

      static PathMap* pathCache;

   public:
      static path_t retrieveDataPath(const char** filesToLookFor, size_t listLength, path_t defaultPath);

      static path_t retrieveConfigPath(path_t defaultPath);
   };
}

#endif