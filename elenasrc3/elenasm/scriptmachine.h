//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Script Engine
//
//                                              (C)2023-2025, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef SCRIPTMACHINE_H
#define SCRIPTMACHINE_H

#include "scriptparser.h"

namespace elena_lang
{
   typedef Map<int, ScriptEngineParserBase*, nullptr, nullptr, freeobj> ParserMap;
   typedef Map<int, MemoryDump*, nullptr, nullptr, freeobj> TapeMap;

   // --- ScriptEngine ---
   class ScriptEngine
   {
      enum class ParserType
      {
         BaseParseMask  = 0x0F,

         VMBuild        = 0x01,
         Tree           = 0x02,
         CF             = 0x10,
         //ptTransform = 0x20,
         Text           = 0x30,
         Build          = 0x40
      };

      PathString        _rootPath;

      int               _lastId;
      ParserMap         _parsers;
      TapeMap           _tapes;

      String<char, 512> _lastError;

      ScriptEngineParserBase* newParser(int id, ParserType type);
      ScriptEngineParserBase* getParser(int id);

      void parseMetaScript(int id, ScriptEngineReaderBase& reader);
      void parseScript(int id, ScriptEngineReaderBase& reader);

      void* translate(int id, UStrReader* source);

      MemoryDump* getTape(int id);

   public:
      int newScope();

      void* translate(int id, path_t path, FileEncoding encoding, bool autoDetect);
      void* translate(int id, ustr_t script);

      void freeTape(int id);

      pos_t getLength(int id);

      ustr_t getLastError()
      {
         return _lastError.str();
      }

      void clearParserStack()
      {
         _parsers.clear();
      }

      ScriptEngine(path_t rootPath);
      ~ScriptEngine() = default;
   };

}

#endif

