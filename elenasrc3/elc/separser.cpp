//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Script Engine Parser class declaration.
//
//                                              (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "separser.h"

#include "parser.h"

#ifdef _MSC_VER

#include "windows\winsyslibloader.h"

#define SCRIPTENGINE_LIB PathString("elenasm60.dll").str()

#else

#define SCRIPTENGINE_LIB "/usr/lib/elena/libelenasm60.so"

#endif

using namespace elena_lang;

// --- ScriptParser ---

ScriptParser :: ScriptParser()
{
   _encoding = FileEncoding::UTF8;

#ifdef _MSC_VER

   _library = new WinSysLibraryLoader(SCRIPTENGINE_LIB);

   _InterpretScript = (void* (__cdecl*)(const char*))_library->loadFunction("InterpretScriptSMLA");
   _InterpretFile = (void* (__cdecl*)(const char*, int, bool))_library->loadFunction("InterpretFileSMLA");
   _GetStatus = (size_t(__cdecl*)(char*, size_t))_library->loadFunction("GetStatusSMLA");
   _Release = (void(__cdecl*)(void*))_library->loadFunction("ReleaseSMLA");

#else

   _library = new ElfSysLibraryLoader(SCRIPTENGINE_LIB);

   *(void **)(&_InterpretScript) = _library.loadFunction("InterpretScriptSMLA");
   *(void **)(&_InterpretFile) = _library.loadFunction("InterpretFileSMLA");
   *(void **)(&_GetStatus) = _library.loadFunction("GetStatusSMLA");
   *(void **)(&_Release) = _library.loadFunction("ReleaseSMLA");

#endif
}

bool ScriptParser :: setOption(ustr_t option, path_t projectPath)
{
   if (!option.startsWith("[[")) {
      if (option[0] == '~') {
         // if it is a standart grammar
         _InterpretFile(option, (int)_encoding, false);
      }
      else {
         if (!emptystr(projectPath)) {
            IdentifierString grammarPath(projectPath);

            grammarPath.append('\\');
            grammarPath.append(option);

            _InterpretFile(*grammarPath, (int)_encoding, false);
         }
         else _InterpretFile(option, (int)_encoding, false);
      }
   }
   else _InterpretScript(option);

   size_t length = _GetStatus(nullptr, 0);

   return length == 0;
}

void ScriptParser :: parse(path_t filePath, SyntaxTree& tree)
{
   IdentifierString pathStr(filePath);

   void* tape = _InterpretFile(*pathStr, (int)_encoding, false);
   if (!tape) {
      size_t length = _GetStatus(_status, 254);
      _status[length] = '\n';
      _status[length + 1] = 0;

      throw ParserError(_status, {}, nullptr);
   }
   else {
      bool valid = false;

      DumpReader reader(tape, 8);
      pos_t length = 8;
      if (reader.getDWord() == 0) {
         length = reader.getDWord();
         reader.setSize(length + 4);

         valid = true;
      }

      if (valid) {
         MemoryDump temp;
         temp.load(reader, length - 4);

         tree.load(&temp);
      }

      _Release(tape);
   }
}
