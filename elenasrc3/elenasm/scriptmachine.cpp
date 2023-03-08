//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                               (C)2023 by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "scriptmachine.h"

#include "scriptparser.h"
#include "textparser.h"

using namespace elena_lang;

// --- ScriptEngine ---

ScriptEngine :: ScriptEngine(path_t rootPath)
   : _rootPath(rootPath), _tape(16384)
{
   _lastId = 0;
}

int ScriptEngine :: newScope()
{
   return ++_lastId;
}

void ScriptEngine :: parseMetaScript(ScriptEngineReaderBase& reader)
{
   reader.read();

   if (reader.compare("[[")) {
      while (!reader.compare("]]")) {
         ScriptBookmark bm = reader.read();
         if (reader.compare("#grammar")) {
            reader.read();

            throw SyntaxError("unrecognized parser", bm.lineInfo);
         }

      }
   }
}

void ScriptEngine :: parseScript()
{
   
}

void* ScriptEngine :: translate(int id, UStrReader* source)
{
   _lastError.clear();

   ScriptEngineReader scriptReader(source);
   pos_t offset = _tape.length();
   if (offset != 0)
      throw InvalidOperationError("Tape is not released");

   parseMetaScript(scriptReader);
   parseScript();

   return _tape.get(offset);
}

void* ScriptEngine :: translate(int id, path_t path, FileEncoding encoding, bool autoDetect)
{
   try {
      PathString scriptPath(_rootPath);
      if (path[0] == '~') {
         scriptPath.combine(path + 2);
      }
      else scriptPath.copy(path);

      TextFileReader reader(*scriptPath, encoding, autoDetect);

      if (!reader.isOpen()) {
         size_t length = _MAX_PATH;
         char buf[_MAX_PATH];
         path.copyTo(buf, length);
         buf[length] = 0;

         _lastError.copy("Cannot open the script file:");
         _lastError.append(buf);

         return nullptr;
      }

      return translate(id, &reader);
   }
   catch (SyntaxError& e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.lineInfo.row);
      _lastError.append(':');
      _lastError.appendInt(e.lineInfo.column);

      return nullptr;
   }
   catch (InvalidOperationError& e) {
      _lastError.copy("Invalid operation ");
      _lastError.append(':');
      _lastError.append(e.Error());

      return nullptr;
   }

}

void ScriptEngine :: free(void* tape)
{
   // !! temporal solution : presuming only one tape at once
   _tape.trim(0);
}
