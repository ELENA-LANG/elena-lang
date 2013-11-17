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

void ScriptLog :: compile(TextReader* source, Terminal* terminal)
{
   _text_t buffer[BLOCK_SIZE];
   DynamicString<wchar16_t> line;
   MemoryWriter writer(&_log);
   while (source->readString(line, buffer)) {
      int offset = 0;
      int index = line.find('$');
      while (index != -1) {
         writer.writeWideLiteral(line + offset, index);
         if (ConstantIdentifier::compare(line + offset + index, "$terminal")) {
            writer.writeWideLiteral(terminal->value, getlength(terminal->value));
            index += 9;
         }
         else if (ConstantIdentifier::compare(line + offset + index, "$literal")) {
            writer.writeWideChar('"');
            writer.writeWideLiteral(terminal->value, getlength(terminal->value));
            writer.writeWideChar('"');
            index += 8;
         }
         else {
            writer.writeWideChar('$');
            index++;
         }

         offset += index;
         index = line.find(offset, '$');
      }

      writer.writeWideLiteral(line + offset, line.Length() - offset);
   }
   writer.writeWideChar('\r');
   writer.writeWideChar('\n');
}

void* ScriptLog :: generate()
{
   MemoryWriter writer(&_log);

   writer.writeWideChar(0);

   return _log.extract();
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
   InlineParser inlineParser;

   // if it is a inline parsing mode
   if (ConstantIdentifier::compare(name, "inline")) {
      inlineParser.compile(source, NULL);
   }
   else {
      _Parser* parser = _parsers.get(name);

      parser->parse(source, &inlineParser);
   }

   // the tape should be explicitly releases with FreeLVMTape function
   return inlineParser.generate();
}

void* Session :: traceScript(const wchar16_t* name, TextReader* source)
{
   ScriptLog log;

   // if it is a inline parsing mode
   if (ConstantIdentifier::compare(name, "inline")) {
      log.compile(source, NULL);
   }
   else {
      _Parser* parser = _parsers.get(name);

      parser->parse(source, &log);
   }

   // the tape should be explicitly releases with FreeLVMTape function
   return log.generate();
}

//void Processor :: createLALRParser(const wchar16_t* name, TextReader* source, bool symbolicMode)
//{
//   _Parser* parser = _parsers.get(name);
//   if (parser == NULL) {
//      parser = new LALRParser(symbolicMode ? dfaSymbolic : NULL);
//
//      _parsers.add(name, parser, true);
//   }
//
//   parser->generate(source);
//}

void Session :: createCFParser(const wchar16_t* name, TextReader* source, bool symbolicMode)
{
   _Parser* parser = _parsers.get(name);
   if (parser == NULL) {
      parser = new CFParser(symbolicMode ? dfaSymbolic : NULL);

      _parsers.add(name, parser, true);
   }

   parser->generate(source);
}

void* Session :: translate(const wchar16_t* name, TextReader* source, int mode)
{
   _lastError.clear();

   // check script type
   bool symbolicMode = test(mode, SYMBOLIC_MODE);

   mode &= MASK_MODE;

   if (mode == CFGRAMMAR_MODE) {
      createCFParser(name, source, symbolicMode);

      return (void*)-1;
   }
   else if (test(mode, TRACE_MODE)) {
      return traceScript(name, source);
   }
//   else if (mode == LALRDSARULE_MODE) {
//      createLALRParser(names, source, symbolicMode);
//
//      return (void*)-1;
//   }
   else return translateScript(name, source);
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
