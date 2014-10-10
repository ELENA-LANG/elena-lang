//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                               (C)2011-2014 by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "session.h"
#include "cfparser.h"
//#include "inlineparser.h"
#include "elenavm.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

// --- ScriptLog ---

void ScriptLog :: write(wchar16_t ch)
{
   MemoryWriter writer(&_log);

   writer.writeWideChar(ch);
}

void ScriptLog :: write(const wchar16_t* token)
{
   MemoryWriter writer(&_log);

   writer.writeWideLiteral(token, getlength(token));
   writer.writeWideChar(' ');
}

// --- ScriptReader ---

const wchar16_t* Session::ScriptReader :: read()
{
   info = reader.read(token, LINE_LEN);
   if (info.state == dfaQuote) {
      QuoteTemplate<TempString> quote(info.line);

         //!!HOTFIX: what if the literal will be longer than 0x100?
         size_t length = getlength(quote);
         StringHelper::copy(token, quote, length);
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
   writer.writeWideLiteral(token, getlength(token) + 1);
}

const wchar16_t* Session::CachedScriptReader :: read()
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

      const wchar16_t* s = reader.getWideLiteral();
      StringHelper::copy(token, s, getlength(s));

      _position = reader.Position();

      return token;
   }
   else return ScriptReader::read();
}

// --- Session ---

Session :: Session()
   : _parsers(NULL, freeobj)
{
   _currentParser = NULL;
}

Session :: ~Session()
{
}

//void* Session :: translateScript(const wchar16_t* name, TextReader* source)
//{
//   ScriptVMCompiler compiler;
//
//   _Parser* parser = _parsers.get(name);
//   if (parser == NULL) {
//      parser = new CFParser();
//
//      _parsers.add(name, parser, true);
//   }
//
//   parser->parse(source, &compiler);
//
//   // the tape should be explicitly releases with FreeLVMTape function
//   return compiler.generate();
//}

void Session :: parseMetaScript(MemoryDump& tape, CachedScriptReader& reader)
{
   int saved = reader.Position();

   const wchar16_t* token = reader.read();
   if (ConstantIdentifier::compare(token, "[[")) {
      saved = reader.Position();
      token = reader.read();
      while (!ConstantIdentifier::compare(token, "]]")) {
         if(ConstantIdentifier::compare(token, "#define")) {
            if (!_currentParser) {
               _currentParser = new CFParser();

               _parsers.add(ConstantIdentifier("default"), _currentParser);
            }

            _currentParser->parseGrammarRule(reader);
         }
         else if(ConstantIdentifier::compare(token, "#mode")) {
            _currentParser->parseDirective(reader);
         }
         else {
            reader.seek(saved);

            _scriptParser.parseDirectives(tape, reader);
         }
         saved = reader.Position();
         token = reader.read();
      }
   }
   else reader.seek(saved);

   reader.clearCache();
}

void Session :: parseScript(_Parser* parser, MemoryDump& tape, _ScriptReader& reader)
{
   if (parser) {
      ScriptLog log;

      _currentParser->parse(reader, log);

      parseScript(tape, (const wchar16_t*)log.getBody());
   }
   else _scriptParser.parseScript(tape, reader);
}

void Session :: parseScript(MemoryDump& tape, const wchar16_t* script)
{
   WideLiteralTextReader reader(script);
   CachedScriptReader scriptReader(&reader);

   parseScript(NULL, tape, scriptReader);
}

int Session :: translate(TextReader* source, bool standalone)
{
   _lastError.clear();

   CachedScriptReader scriptReader(source);
   MemoryDump         tape;

   parseMetaScript(tape, scriptReader);
   parseScript(_currentParser, tape, scriptReader);

   int retVal = standalone ? Interpret(tape.get(0)) : Evaluate(tape.get(0));

   // copy vm error if retVal is zero
   if (!retVal)
      _lastError.copy(GetLVMStatus());

   return retVal;
}

int Session :: translate(const wchar16_t* script, bool standalone)
{
   try {
      WideLiteralTextReader reader(script);

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

int Session :: translate(const wchar16_t* path, int encoding, bool autoDetect, bool standalone)
{
   try {
      TextFileReader reader(path, encoding, autoDetect);

      if (!reader.isOpened()) {
         _lastError.copy("Cannot open the script file:");
         _lastError.append(path);

         return NULL;
      }

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
