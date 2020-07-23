//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sessionH
#define sessionH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- Session ---

class Session
{
   const int BaseParseMask = 0x0F;

   enum ParserType
   {
      ptVMBuild   = 0x01,
      ptTree      = 0x02,
      ptCF        = 0x10,
      ptTransform = 0x20,
      ptText      = 0x30,
      ptBuild     = 0x40
   };

   int                _lastId;
   Map<int, _Parser*> _parsers;
   Path               _rootPath;

   String<char, 512>  _lastError;
   MemoryDump         _tape;

   _Parser* newParser(int id, ParserType type);
   _Parser* getParser(int id);

   void parseDirectives(MemoryDump& tape, _ScriptReader& reader);
   void parseMetaScript(int id, MemoryDump& tape, _ScriptReader& reader);
   void parseScript(int id, MemoryDump& tape, _ScriptReader& reader);

   void* translate(int id, TextReader* source);

public:
   ident_t getLastError()
   {
      const char* error = _lastError;

      return !emptystr(error) ? error : NULL;
   }

   int newScope();

   void* translate(int id, ident_t script);
   void* translate(int id, path_t path, int encoding, bool autoDetect);
   void free(void*)
   {
      _tape.trim(0);
   }

   Session(path_t rootPath);
   virtual ~Session();
};

} // _elena_

#endif // sessionh
