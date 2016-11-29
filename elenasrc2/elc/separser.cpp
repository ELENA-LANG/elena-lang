//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This file contains ELENA Script Engine Parser class implementation.
//
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "separser.h"

using namespace _ELENA_;

ScriptParser :: ScriptParser()
   : _library(Path("elenasm.dll"))
{
   _encoding = feUTF8;

   _InterpretScript = _library.loadFunction("InterpretScript");
   _InterpretFile = _library.loadFunction("InterpretFile");
   _Release = _library.loadFunction("Release");
   _GetStatus = _library.loadFunction("GetStatus");
}

void ScriptParser :: setOption(ident_t option)
{
   if (option.startsWith("[[")) {
      _InterpretScript(option);
   }
   else _InterpretFile(option, _encoding, false);
}

void ScriptParser :: parse(path_t filePath, SyntaxTree& tree)
{
   IdentifierString s(filePath);

   void* tape = _InterpretFile(s, _encoding, false);
   bool valid = false;
   
   DumpReader reader(tape, 8);
   size_t length = 8;
   if (reader.getDWord() == 0) {
      size_t length = reader.getDWord();
      reader.setSize(length + 4);

      valid = true;
   }

   if (valid) {
      MemoryDump temp;
      temp.load(&reader, length - 8);

      tree.load(&temp);
   }

   _Release(tape);
}