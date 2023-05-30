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
   typedef Map<int, ScriptEngineParserBase*, nullptr, nullptr, freeobj> ParserMap;

   // --- ScriptEngine ---
   class ScriptEngine
   {
      enum class ParserType
      {
         BaseParseMask = 0x0F,

         //ptVMBuild = 0x01,
         //ptTree = 0x02,
         CF = 0x10,
         //ptTransform = 0x20,
         Text = 0x30,
         Build = 0x40
      };

      PathString        _rootPath;

      int               _lastId;
      ParserMap         _parsers;

      MemoryDump        _tape;

      String<char, 512> _lastError;

      ScriptEngineParserBase* newParser(int id, ParserType type);
      ScriptEngineParserBase* getParser(int id);

      void parseDirectives(ScriptEngineReaderBase& reader);
      void parseMetaScript(int id, ScriptEngineReaderBase& reader);
      void parseScript(int id, ScriptEngineReaderBase& reader);

      void* translate(int id, UStrReader* source);

   public:
      int newScope();

      void* translate(int id, path_t path, FileEncoding encoding, bool autoDetect);
      void* translate(int id, ustr_t script);

      void free(void* tape);

      pos_t getLength(void* tape);

      ustr_t getLastError()
      {
         return _lastError.str();
      }

      ScriptEngine(path_t rootPath);
      ~ScriptEngine() = default;
   };

}

#endif

