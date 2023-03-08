//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Script Engine
//
//                                              (C)2023, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef SCRIPTMACHINE_H
#define SCRIPTMACHINE_H

#include "scriptparser.h"

namespace elena_lang
{
   // --- ScriptEngine ---
   class ScriptEngine
   {
      PathString         _rootPath;

      int                _lastId;

      MemoryDump         _tape;

      String<char, 512>  _lastError;

      void parseMetaScript(ScriptEngineReaderBase& reader);
      void parseScript();

      void* translate(int id, UStrReader* source);

   public:
      int newScope();

      void* translate(int id, path_t path, FileEncoding encoding, bool autoDetect);

      void free(void* tape);

      ustr_t getLastError()
      {
         return _lastError.str();
      }

      ScriptEngine(path_t rootPath);
      ~ScriptEngine() = default;
   };

}

#endif

