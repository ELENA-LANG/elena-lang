//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                               (C)2011-2015 by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "session.h"
#include "cfparser.h"
#include "inlineparser.h"
#include "elenavm.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

// --- ScriptReader ---

ident_t Session::ScriptReader :: read()
{
   info = reader.read(token, LINE_LEN);
   if (info.state == dfaQuote) {
      QuoteTemplate<TempString> quote(info.line);

         //!!HOTFIX: what if the literal will be longer than 0x100?
         size_t length = getlength(quote);
         StringHelper::copy(token, quote, length, length);
         token[length] = 0;
   }  

   return token;
}

// --- CachedScriptReader ---

void Session::CachedScriptReader :: cache()
{
   ScriptReader::read();

   MemoryWriter writer(&_buffer);

   writer.writeDWord(info.column);
   writer.writeDWord(info.row);
   writer.writeChar(info.state);
   writer.writeLiteral(token, getlength(token) + 1);
}

ident_t Session::CachedScriptReader :: read()
{
   // read from the outer reader if the cache is empty
   if (_cacheMode && _position >= _buffer.Length()) {
      cache();
   }

   // read from the cache
   if (_position < _buffer.Length()) {
      MemoryReader reader(&_buffer, _position);

      reader.readDWord(info.column);
      reader.readDWord(info.row);
      reader.readChar(info.state);

      ident_t s = reader.getLiteral(DEFAULT_STR);
      size_t length = getlength(s);
      StringHelper::copy(token, s, length, length);

      _position = reader.Position();

      return token;
   }
   else return ScriptReader::read();
}

// --- Session ---

Session :: Session()
{
   _currentParser = new InlineScriptParser();
}

Session :: ~Session()
{
   freeobj(_currentParser);
}

//////void* Session :: translateScript(const wchar16_t* name, TextReader* source)
//////{
//////   ScriptVMCompiler compiler;
//////
//////   _Parser* parser = _parsers.get(name);
//////   if (parser == NULL) {
//////      parser = new CFParser();
//////
//////      _parsers.add(name, parser, true);
//////   }
//////
//////   parser->parse(source, &compiler);
//////
//////   // the tape should be explicitly releases with FreeLVMTape function
//////   return compiler.generate();
//////}

void Session::parseDirectives(MemoryDump& tape, _ScriptReader& reader)
{
   TapeWriter writer(&tape);

   do {
      ident_t token = reader.read();

      if (token[0]==';') {
         break;
      }
      else if(StringHelper::compare(token, "#start")) {
         writer.writeCommand(START_VM_MESSAGE_ID);
      }
      else if (StringHelper::compare(token, "#config")) {
         token = reader.read();
         if (reader.info.state == dfaIdentifier) {
            writer.writeCommand(LOAD_VM_MESSAGE_ID, token);
         }
         else throw EParseError(reader.info.column, reader.info.row);
      }
      else if (StringHelper::compare(token, "#map")) {
         token = reader.read();

         IdentifierString forward;
         if (reader.info.state == dfaFullIdentifier) {
            forward.append(token);

            token = reader.read();
            if (!StringHelper::compare(token, "="))
               throw EParseError(reader.info.column, reader.info.row);

            token = reader.read();
            if (reader.info.state == dfaFullIdentifier) {
               forward.append('=');
               forward.append(token);
               writer.writeCommand(MAP_VM_MESSAGE_ID, forward);
            }
            else throw EParseError(reader.info.column, reader.info.row);
         }
         else throw EParseError(reader.info.column, reader.info.row);
      }
      else if (StringHelper::compare(token, "#use")) {
         token = reader.read();

         if (reader.info.state == dfaQuote) {
            writer.writeCommand(USE_VM_MESSAGE_ID, reader.token);
         }
         else {
            IdentifierString package;
            package.append(token);

            token = reader.read();
            if (!StringHelper::compare(token, "="))
               throw EParseError(reader.info.column, reader.info.row);

            token = reader.read();
            if (reader.info.state == dfaQuote) {
               package.append('=');
               package.append(reader.token);
               writer.writeCommand(USE_VM_MESSAGE_ID, package);
            }
            else throw EParseError(reader.info.column, reader.info.row);
         }
      }
      else return;
   }
   while(true);
}

void Session :: parseMetaScript(MemoryDump& tape, CachedScriptReader& reader)
{
   int saved = reader.Position();

   ident_t token = reader.read();
   if (StringHelper::compare(token, "[[")) {
      saved = reader.Position();
      token = reader.read();
      while (!StringHelper::compare(token, "]]")) {
         if(StringHelper::compare(token, "#define")) {
            _currentParser->parseGrammarRule(reader);
         }
         else if (StringHelper::compare(token, "#grammar")) {
            token = reader.read();

            if (!StringHelper::compare(token, "cf")) {
               _currentParser = new CFParser(_currentParser);
            }
            else throw EParseError(reader.info.column, reader.info.row);
         }
//         else if(ConstantIdentifier::compare(token, "#mode")) {
//            _currentParser->parseDirective(reader);
//         }
         else {
            reader.seek(saved);

            parseDirectives(tape, reader);
         }
         saved = reader.Position();
         token = reader.read();
      }
   }
   else reader.seek(saved);

   reader.clearCache();
}

void Session :: parseScript(MemoryDump& tape, _ScriptReader& reader)
{
   TapeWriter writer(&tape);

   _currentParser->parse(reader, writer);
}

int Session :: translate(TextReader* source, bool standalone)
{
   _lastError.clear();

   CachedScriptReader scriptReader(source);
   MemoryDump         tape;

   parseMetaScript(tape, scriptReader);
   parseScript(tape, scriptReader);

   int retVal = standalone ? Interpret(tape.get(0)) : Evaluate(tape.get(0));

   // copy vm error if retVal is zero
   if (!retVal)
      _lastError.copy(GetLVMStatus());

   return retVal;
}

int Session :: translate(ident_t script, bool standalone)
{
   try {
      IdentifierTextReader reader(script);

      return translate(&reader, standalone);
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
}

int Session :: translate(path_t path, int encoding, bool autoDetect, bool standalone)
{
   try {
      TextFileReader reader(path, encoding, autoDetect);

      if (!reader.isOpened()) {
         _lastError.copy("Cannot open the script file:");

         ident_c tmp[_MAX_PATH];
         Path::savePath(path, tmp, _MAX_PATH);
         _lastError.append(tmp);

         return NULL;
      }

      return translate(&reader, standalone);
   }
////   catch(EUnrecognizedException) {
////      _lastError.copy("Unrecognized expression");
////
////      return NULL;
////   }
////   catch(EInvalidExpression e) {
////      _lastError.copy("Rule ");
////      _lastError.append(e.nonterminal);
////      _lastError.append(": invalid expression at ");
////      _lastError.appendInt(e.row);
////      _lastError.append(':');
////      _lastError.appendInt(e.column);
////
////      return NULL;
////   }
   catch(EParseError e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
}
