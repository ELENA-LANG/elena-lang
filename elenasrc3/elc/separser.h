//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Script Engine Parser class declaration.
//
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef SEPARSER_H
#define SEPARSER_H

#include "clicommon.h"

namespace elena_lang
{
   // --- ScriptParser ---
   class ScriptParser
   {
      SysLibraryLoaderBase*   _library;

      FileEncoding            _encoding;
      char                    _status[256];

      void* (*_InterpretScript)(const char* script);
      void* (*_InterpretFile)(const char* path, int encoding, bool autoDetect);
      size_t(*_GetStatus)(char* error, size_t maxLength);
      void(*_Release)(void* tape);

   public:
      bool setOption(ustr_t option, path_t projectPath);

      void parse(path_t filePath, SyntaxTree& tree);

      ScriptParser();
   };

}

#endif
