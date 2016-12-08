//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef sessionH
#define sessionH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- Session ---

class Session
{
   _Parser*          _currentParser;
   Path              _rootPath;

   String<char, 512> _lastError;
   MemoryDump        _tape;

   void parseDirectives(MemoryDump& tape, _ScriptReader& reader);
   void parseMetaScript(MemoryDump& tape, _ScriptReader& reader);
   void parseScript(MemoryDump& tape, _ScriptReader& reader);

   void* translate(TextReader* source);

public:
   ident_t getLastError()
   {
      const char* error = _lastError;

      return !emptystr(error) ? error : NULL;
   }

   void* translate(ident_t script);
   void* translate(path_t path, int encoding, bool autoDetect);
   void free(void*)
   {
      _tape.trim(0);
   }

   Session(path_t rootPath);
   virtual ~Session();
};

} // _elena_

#endif // sessionh
