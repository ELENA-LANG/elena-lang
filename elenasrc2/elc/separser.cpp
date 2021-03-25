//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Script Engine Parser class implementation.
//
//                                              (C)2005-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "separser.h"

using namespace _ELENA_;

#ifdef _WIN32

#define SCRIPTENGINE_LIB Path("elenasm.dll").str()

#elif _LINUX

#define SCRIPTENGINE_LIB "/usr/lib/elena/libelenasm.so"

#endif

ScriptParser :: ScriptParser()
   : _library(SCRIPTENGINE_LIB)
{
   _status[0] = 0;
   _encoding = feUTF8;

#ifdef _WIN32
   _InterpretScript = (void*(__cdecl*)(const char*))_library.loadFunction("InterpretScript");
   _InterpretFile = (void*(__cdecl*)(const char*,int,bool))_library.loadFunction("InterpretFile");
   _Release = (void(__cdecl*)(void*))_library.loadFunction("Release");
   _GetStatus = (int(__cdecl*)(char*,int))_library.loadFunction("GetStatus");
#else
   *(void **)(&_InterpretScript) = _library.loadFunction("InterpretScript");
   *(void **)(&_InterpretFile) = _library.loadFunction("InterpretFile");
   *(void **)(&_Release) = _library.loadFunction("Release");
   *(void **)(&_GetStatus) = _library.loadFunction("GetStatus");

#endif
}

bool ScriptParser :: setOption(ident_t option, ident_t projectPath)
{
   if (!option.startsWith("[[")) {
      if (option[0] == '~') {
         // if it is a standart grammar
         _InterpretFile(option, _encoding, false);
      }
      else {
         if (!emptystr(projectPath)) {
            IdentifierString grammarPath(projectPath);

            grammarPath.append('\\');
            grammarPath.append(option);

            _InterpretFile(grammarPath, _encoding, false);
         }
         else _InterpretFile(option, _encoding, false);
      }
   }
   else _InterpretScript(option);

   int length = _GetStatus(NULL, 0);

   return length == 0;
}

void ScriptParser :: parse(path_t filePath, SyntaxTree& tree)
{
   IdentifierString s(filePath);

   void* tape = _InterpretFile(s, _encoding, false);
   if (!tape) {
      int length = _GetStatus(_status, 256);
      _status[length] = 0;

      throw ScriptError(_status);
   }
   else {
      bool valid = false;

      DumpReader reader(tape, 8);
      size_t length = 8;
      if (reader.getDWord() == 0) {
         length = reader.getDWord();
         reader.setSize(length + 4);

         valid = true;
      }

      if (valid) {
         MemoryDump temp;
         temp.load(&reader, length - 4);

         tree.load(&temp);
      }

      _Release(tape);
   }
}
