//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                               (C)2023 by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "scriptmachine.h"

#include "cfparser.h"
#include "scriptparser.h"
#include "textparser.h"
#include "transformer.h"

using namespace elena_lang;

// --- ScriptEngine ---

ScriptEngine :: ScriptEngine(path_t rootPath)
   : _rootPath(rootPath), _parsers(nullptr), _tape(16384)
{
   _lastId = 0;
}

int ScriptEngine :: newScope()
{
   return ++_lastId;
}

ScriptEngineParserBase* ScriptEngine :: newParser(int id, ParserType type)
{
   ScriptEngineParserBase* baseOne = _parsers.get(id);
   ScriptEngineParserBase* newOne = nullptr;

   if (baseOne && testany((int)type, (int)ParserType::BaseParseMask)) {
      throw InvalidOperationError("This parser should be created first");
   }

   switch (type) {
      case ParserType::Text:
         newOne = new ScriptEngineTextParser();
         break;
      case ParserType::CF:
         newOne = new ScriptEngineCFParser(baseOne);
         break;
      default:
         throw InvalidOperationError("Unknown parser type");
   }

   _parsers.exclude(id);
   _parsers.add(id, newOne);

   return newOne;
}

ScriptEngineParserBase* ScriptEngine :: getParser(int id)
{
   ScriptEngineParserBase* parser = _parsers.get(id);
   if (parser) {
      return parser;
   }
   else throw InvalidOperationError("Scope is not created");
}

void ScriptEngine :: parseDirectives(ScriptEngineReaderBase& reader)
{
   return;
}

void ScriptEngine :: parseMetaScript(int id, ScriptEngineReaderBase& reader)
{
   ScriptEngineParserBase* parser = nullptr;

   reader.read();

   if (reader.compare("[[")) {
      while (!reader.compare("]]")) {
         ScriptBookmark bm = reader.read();
         if (reader.compare("#grammar")) {
            reader.read();

            if (reader.compare("cf")) {
               parser = newParser(id, ParserType::CF);
            }
            else if (reader.compare("text")) {
               parser = newParser(id, ParserType::Text);
            }
            else throw SyntaxError("unrecognized parser", bm.lineInfo);
         }
         else {
            if (!parser)
               parser = getParser(id);

            if (reader.compare("#define")) {
               parser->parseGrammarRule(reader);
            }
            else parseDirectives(reader);
         }
      }
   }
   else reader.reset();
}

void ScriptEngine :: parseScript(int id, ScriptEngineReaderBase& reader)
{
   ScriptEngineParserBase* parser = getParser(id);

   parser->parse(reader, &_tape);
}

void* ScriptEngine :: translate(int id, UStrReader* source)
{
   _lastError.clear();

   ScriptEngineReader scriptReader(source);
   pos_t offset = _tape.length();
   if (offset != 0)
      throw InvalidOperationError("Tape is not released");

   parseMetaScript(id, scriptReader);
   parseScript(id, scriptReader);

   return _tape.get(offset);
}

void* ScriptEngine :: translate(int id, ustr_t script)
{
   try {
      IdentifierTextReader reader(script);

      return translate(id, &reader);
   }
   catch (SyntaxError& e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.lineInfo.row);
      _lastError.append(':');
      _lastError.appendInt(e.lineInfo.column);

      return nullptr;
   }
   catch (InvalidChar& e) {
      _lastError.copy("Invalid char at ");
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
   catch (InvalidChar& e) {
      _lastError.copy("Invalid char at ");
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
