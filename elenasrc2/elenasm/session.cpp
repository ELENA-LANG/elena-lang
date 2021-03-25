//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                               (C)2011-2020 by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "session.h"
#include "cfparser.h"
#include "inlineparser.h"
#include "treeparser.h"
#include "transformer.h"

#ifdef _WIN32
#elif _LINUX

#define _MAX_PATH 256 // !! temporal

#endif // _WIN32

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

// --- Session ---

Session::Session(path_t rootPath)
   : _rootPath(rootPath), _tape(16384), _parsers(NULL, freeobj)
{
   _lastId = 0;
}

Session :: ~Session()
{
}

_Parser* Session :: getParser(int id)
{
   _Parser* parser = _parsers.get(id);
   if (parser) {
      return parser;
   }
   else throw EInvalidOperation("Scope is not created");
}

_Parser* Session :: newParser(int id, ParserType type)
{
   _Parser* baseOne = _parsers.get(id);
   _Parser* newOne = NULL;

   if (!baseOne && !testany(type, BaseParseMask)) {
      baseOne = new VMTapeParser();
   }
   else if (baseOne && testany(type, BaseParseMask)) {
      throw EInvalidOperation("This parser should be created first");
   }

   switch (type)
   {
      case _ELENA_::Session::ptVMBuild:
         newOne = new VMTapeParser();
         break;
      case _ELENA_::Session::ptCF:
         newOne = new CFParser(baseOne);
         break;
      case _ELENA_::Session::ptTransform:
         newOne = new Transformer(baseOne);
         break;
      case _ELENA_::Session::ptTree:
         newOne = new TreeScriptParser();
         break;
      case _ELENA_::Session::ptBuild:
         newOne = new Builder();
         break;
      case _ELENA_::Session::ptText:
         newOne = new TextParser();
         break;
      default:
         throw EInvalidOperation("Unknown parser type");
   }

   _parsers.exclude(id);
   _parsers.add(id, newOne);

   return newOne;
}

void Session :: parseDirectives(MemoryDump& tape, _ScriptReader& reader)
{
   TapeWriter writer(&tape);

   do {
      if (reader.compare(";")) {
         break;
      }
      else if(reader.compare("#start")) {
         writer.writeCommand(START_VM_MESSAGE_ID);
      }
      else if (reader.compare("#config")) {
         ScriptBookmark bm = reader.read();
         if (bm.state == dfaIdentifier) {
            writer.writeCommand(LOAD_VM_MESSAGE_ID, reader.lookup(bm));
         }
      }
      else if (reader.compare("#map")) {
         ScriptBookmark bm = reader.read();

         IdentifierString forward;
         if (bm.state == dfaFullIdentifier) {
            forward.append(reader.lookup(bm));

            bm = reader.read();
            if (!reader.compare("="))
               throw EParseError(bm.column, bm.row);

            bm = reader.read();
            if (bm.state == dfaFullIdentifier) {
               forward.append('=');
               forward.append(reader.lookup(bm));
               writer.writeCommand(MAP_VM_MESSAGE_ID, forward);
            }
            else throw EParseError(bm.column, bm.row);
         }
         else throw EParseError(bm.column, bm.row);
      }
      else if (reader.compare("#use")) {
         ScriptBookmark bm = reader.read();

         if (bm.state == dfaQuote) {
            writer.writeCommand(USE_VM_MESSAGE_ID, reader.lookup(bm));
         }
         else {
            IdentifierString package;
            package.append(reader.lookup(bm));

            bm = reader.read();
            if (!reader.compare("="))
               throw EParseError(bm.column, bm.row);

            bm = reader.read();
            if (bm.state == dfaQuote) {
               package.append('=');
               package.append(reader.lookup(bm));
               writer.writeCommand(USE_VM_MESSAGE_ID, package);
            }
            else throw EParseError(bm.column, bm.row);
         }
      }
      else return;

      reader.read();
   }
   while(true);
}

void Session :: parseMetaScript(int id, MemoryDump& tape, _ScriptReader& reader)
{
   _Parser* parser = NULL;

   reader.read();

   if (reader.compare("[[")) {
      while (!reader.compare("]]")) {
         ScriptBookmark bm = reader.read();
         if (reader.compare("#grammar")) {
            reader.read();

            if (reader.compare("cf")) {
               parser = newParser(id, Session::ptCF);
            }
            else if (reader.compare("vmbuild")) {
               parser = newParser(id, Session::ptVMBuild);
            }
            else if (reader.compare("transform")) {
               parser = newParser(id, Session::ptTransform);
            }
            else if (reader.compare("tree")) {
               parser = newParser(id, Session::ptTree);
            }
            else if (reader.compare("text")) {
               parser = newParser(id, Session::ptText);
            }
            else if (reader.compare("build")) {
               parser = newParser(id, Session::ptBuild);
            }
            else throw EParseError(bm.column, bm.row);
         }
         else {
            if (!parser)
               parser = getParser(id);

            if (reader.compare("#define")) {
               parser->parseGrammarRule(reader);
            }
            else if (reader.compare("#set")) {
               reader.read();
               if (reader.compare("postfix")) {
                  bm = reader.read();

                  if (!parser->setPostfix(reader.lookup(bm)))
                     throw EParseError(bm.column, bm.row);
               }
               else throw EParseError(bm.column, bm.row);
            }
            else if (reader.compare("#mode")) {
               parser->parseGrammarMode(reader);
            }
            else parseDirectives(tape, reader);
         }
      }
   }
   else reader.reset();
}

void Session :: parseScript(int id, MemoryDump& tape, _ScriptReader& reader)
{
   _Parser* parser = getParser(id);

   TapeWriter writer(&tape);

   parser->parse(reader, &tape);
}

void* Session :: translate(int id, TextReader* source)
{
   _lastError.clear();

   ScriptReader scriptReader(source);
   int offset = _tape.Length();
   if (offset != 0)
      throw EInvalidOperation("Tape is not released");

   parseMetaScript(id, _tape, scriptReader);
   parseScript(id, _tape, scriptReader);

   return _tape.get(offset);
}

int Session :: newScope()
{
   return ++_lastId;
}

void* Session :: translate(int id, ident_t script)
{
   try {
      IdentifierTextReader reader(script);

      return translate(id, &reader);
   }
//   catch(EUnrecognizedException) {
//      _lastError.copy("Unrecognized expression");
//
//      return NULL;
//   }
//   catch(EInvalidExpression e) {
//      _lastError.copy("Rule ");
//      _lastError.append(e.nonterminal);
//      _lastError.append(": invalid expression at ");
//      _lastError.appendInt(e.row);
//      _lastError.append(':');
//      _lastError.appendInt(e.column);
//
//      return NULL;
//   }
   catch(EParseError e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
   catch (EInvalidOperation e) {
      _lastError.copy("Invalid operation ");
      _lastError.append(':');
      _lastError.append(e.Error());

      return NULL;
   }
   catch (_Exception) {
      _lastError.copy("Internal Error");

      return NULL;
   }
}

void* Session :: translate(int id, path_t path, int encoding, bool autoDetect)
{
   try {
      Path scriptPath(_rootPath);
      if (path[0] == '~') {
         scriptPath.combine(path + 2);
      }
      else scriptPath.copy(path);

      TextFileReader reader(scriptPath.c_str(), encoding, autoDetect);

      if (!reader.isOpened()) {
         size_t length = _MAX_PATH;
         String<char, _MAX_PATH> tmp;
         path.copyTo((char*)tmp, length);
         tmp[length] = 0;

         _lastError.copy("Cannot open the script file:");
         _lastError.append(tmp);

         return NULL;
      }

      return translate(id, &reader);
   }
//   catch(EUnrecognizedException) {
//      _lastError.copy("Unrecognized expression");
//
//      return NULL;
//   }
//   catch(EInvalidExpression e) {
//      _lastError.copy("Rule ");
//      _lastError.append(e.nonterminal);
//      _lastError.append(": invalid expression at ");
//      _lastError.appendInt(e.row);
//      _lastError.append(':');
//      _lastError.appendInt(e.column);
//
//      return NULL;
//   }
   catch(EParseError e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
   catch (EInvalidOperation e) {
      _lastError.copy("Invalid operation ");
      _lastError.append(':');
      _lastError.append(e.Error());

      return NULL;
   }
}
