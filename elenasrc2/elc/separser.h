//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Script Engine Parser class declaration.
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef separserH
#define separserH 1

#include "syntaxtree.h"

#ifdef _WIN32
#include "win32\syslibloader.h"
#elif _LINUX
#include "linux32/syslibloader.h"
#endif

namespace _ELENA_
{

// --- ScriptError ---

struct ScriptError
{
   IdentifierString error;

   ScriptError()
      : error((const char*)NULL)
   {
   }
   ScriptError(const char* message)
      : error(message)
   {
   }
};

// --- ScriptParser ---

class ScriptParser
{
   SysLibraryLoader _library;
   char             _status[256];

   void*(*_InterpretScript)(const char* script);
   void*(*_InterpretFile)(const char* path, int encoding, bool autoDetect);
   void(*_Release)(void* tape);
   int (*_GetStatus)(char* error, int maxLength);

   int _encoding;

public:
   bool setOption(ident_t option, ident_t projectPath);

   void parse(path_t filePath, SyntaxTree& tree);

   ScriptParser();
};

} // _ELENA_

#endif // parserH
