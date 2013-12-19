//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                               (C)2011-2013 by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "session.h"
#include "cfparser.h"
#include "inlineparser.h"

using namespace _ELENA_;

const char* dfaSymbolic[4] =
{
        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
        "aaaaaaaaaddaadaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
};

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

// --- Session ---

Session :: Session()
   : _parsers(NULL, freeobj)
{
}

Session :: ~Session()
{
}

void* Session :: translateScript(const wchar16_t* name, TextReader* source)
{
   ScriptVMCompiler compiler;

   _Parser* parser = _parsers.get(name);
   if (parser == NULL) {
      parser = new CFParser(/*symbolicMode ? dfaSymbolic : */NULL);

      _parsers.add(name, parser, true);
   }

   parser->parse(source, &compiler);

   // the tape should be explicitly releases with FreeLVMTape function
   return compiler.generate();
}

void* Session :: traceScript(const wchar16_t* name, TextReader* source)
{
   //ScriptLog log;

   //// if it is a inline parsing mode
   //if (ConstantIdentifier::compare(name, "inline")) {
   //   log.compile(source, NULL);
   //}
   //else {
   //   _Parser* parser = _parsers.get(name);

   //   parser->parse(source, &log);
   //}

   //// the tape should be explicitly releases with FreeLVMTape function
   //return log.generate();

   return NULL; // !! temporal
}

void* Session :: translate(const wchar16_t* name, TextReader* source, int mode)
{
   _lastError.clear();

   // check script type
   bool symbolicMode = test(mode, SYMBOLIC_MODE);

   mode &= MASK_MODE;

   //if (test(mode, TRACE_MODE)) {
   //   return traceScript(name, source);
   //}
   return translateScript(name, source);
}

void* Session :: translate(const wchar16_t* name, const wchar16_t* script, int mode)
{
   try {
      WideLiteralTextReader reader(script);

      return translate(name, &reader, mode);
   }
   catch(EUnrecognizedException) {
      _lastError.copy("Unrecognized expression");

      return NULL;
   }
   catch(EInvalidExpression e) {
      _lastError.copy("Rule ");
      _lastError.append(e.nonterminal);
      _lastError.append(": invalid expression at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
   catch(EParseError e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
}

void* Session :: translate(const wchar16_t* name, const wchar16_t* path, int encoding, bool autoDetect, int mode)
{
   try {
      TextFileReader reader(path, encoding, autoDetect);

      if (!reader.isOpened()) {
         _lastError.copy("Cannot open the script file:");
         _lastError.append(path);

         return NULL;
      }

      return translate(name, &reader, mode);
   }
   catch(EUnrecognizedException) {
      _lastError.copy("Unrecognized expression");

      return NULL;
   }
   catch(EInvalidExpression e) {
      _lastError.copy("Rule ");
      _lastError.append(e.nonterminal);
      _lastError.append(": invalid expression at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
   catch(EParseError e) {
      _lastError.copy("Invalid syntax at ");
      _lastError.appendInt(e.row);
      _lastError.append(':');
      _lastError.appendInt(e.column);

      return NULL;
   }
}

void Session :: free(void* tape)
{
   freestr((char*)tape);
}
