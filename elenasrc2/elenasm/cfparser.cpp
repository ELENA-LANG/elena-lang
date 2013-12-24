//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                              (C)2011-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cfparser.h"

using namespace _ELENA_;
using namespace _ELENA_TOOL_;

//#define REFERENCE_KEYWORD     "$reference"
//#define IDENTIFIER_KEYWORD    "$identifier"
#define LITERAL_KEYWORD       "$literal"
//#define NUMERIC_KEYWORD       "$numeric"
//#define EPS_KEYWORD           "$eps"
#define EOF_KEYWORD           "$eof"
//#define ANY_KEYWORD           "$any"

const char* dfaSymbolic[4] =
{
        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
        "*********dd**d******************************************************************************************************************"
};

// --- ScriptReader :: Reader ---

CFParser::ScriptReader::Reader :: Reader(TextReader* script)
   : parser(4, script)
{
}

bool CFParser::ScriptReader::Reader :: read()
{
   try {
      token = parser.read(buffer, IDENTIFIER_LEN);

      return (token.state == dfaEOF) ? false : true;
   }
   catch(InvalidChar) {
      throw EParseError(token.column, token.row);
   }
   catch(LineTooLong) {
      throw EParseError(token.column, token.row);
   }
}

// --- ScriptReader ---

CFParser::ScriptReader :: ScriptReader(TextReader* script)
   : _reader(script)
{
}

bool CFParser::ScriptReader :: read(TokenInfo& token)
{
   if (_reader.read()) {
      token.column = _reader.token.column;
      token.row = _reader.token.row;
      token.state = _reader.token.state;
      if (_reader.token.state == dfaQuote) {
         QuoteTemplate<TempString> quote(_reader.token.line);

         //!!HOTFIX: what if the literal will be longer than 0x100?
         size_t length = getlength(quote);
         StringHelper::copy(_reader.buffer, quote, length);
         _reader.buffer[length] = 0;

         token.value = _reader.buffer;
      }
      else token.value = _reader.token.line;

      return true;
   }
   else {
      token.state = dfaEOF;
      token.value = NULL;

      return false;
   }
}

// --- CFParser::CachedScriptReader ---

CFParser::CachedScriptReader :: CachedScriptReader(TextReader* script)
   : ScriptReader(script)
{
   _position = 0;
}

void CFParser::CachedScriptReader :: cache()
{
   if (_reader.read()) {
      MemoryWriter writer(&_buffer);

      writer.writeDWord(_reader.token.column);
      writer.writeDWord(_reader.token.row);
      writer.writeChar(_reader.token.state);
      if (_reader.token.state == dfaQuote) {
         QuoteTemplate<TempString> quote(_reader.token.line);

         writer.writeWideLiteral(quote, getlength(quote) + 1);
      }
      else writer.writeWideLiteral(_reader.token.line, getlength(_reader.token.line) + 1);
   }
}

bool CFParser::CachedScriptReader :: read(TokenInfo& token)
{
   // read from the outer reader if the cache is empty
   if (_position >= _buffer.Length()) {
      cache();
   }

   // read from the cache
   if (_position < _buffer.Length()) {
      MemoryReader reader(&_buffer, _position);

      reader.readDWord(token.column);
      reader.readDWord(token.row);
      reader.readChar(token.state);

      token.value = reader.getWideLiteral();

      _position = reader.Position();

      return true;
   }
   else {
      token.state = dfaEOF;
      token.value = NULL;

      return false;
   }
}

// --- CFParser ---

inline bool apply(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   reader.read(token);
   if (rule.nonterminal != 0) {
      if(!token.parser->applyRule(rule.nonterminal, token, reader))
         return false;
   }

   return true;
}

inline bool applyNonterminal(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader, bool chomski = false)
{
   if(!token.parser->applyRule(chomski ? rule.terminal : rule.nonterminal, token, reader)) {
      return false;
   }

   return true;
}

inline bool applyNonterminalDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader, bool chomski = false)
{
   rule.applyPrefixDSARule(token);

   if(!token.parser->applyRule(chomski ? rule.terminal : rule.nonterminal, token, reader)) {
      return false;
   }

   return true;
}

bool normalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state == dfaEOF || !token.compare(rule.terminal))
      return false;

   return apply(rule, token, reader);
}

bool normalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state == dfaEOF || !token.compare(rule.terminal))
      return false;

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token);

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token);
      
   return true;
}

bool nonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if(applyNonterminal(rule, token, reader)) {
      return true;
   }
   else return false;
}

bool nonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if(applyNonterminalDSA(rule, token, reader)) {
      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token);

      return true;
   }
   else return false;
}

bool normalLiteralApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaQuote)
      return false;

   if (!apply(rule, token, reader))
      return false;

   return true;
}

bool normalLiteralApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaQuote)
      return false;

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token);

   token.writeLog();

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token);

   return true;
}

//bool normalNumericApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
//      return false;
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   return true;
//}
//
//bool normalNumericApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
//      return false;
//
//   Terminal terminal;
//   token.copyTo(&terminal);
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
//
//   return true;
//}
//
//bool normalIdentifierApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (token.state != dfaIdentifier)
//      return false;
//
//   return apply(rule, token, reader);
//}
//
//bool normalIdentifierApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (token.state != dfaIdentifier)
//      return false;
//
//   Terminal terminal;
//   token.copyTo(&terminal);
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
//
//   return true;
//}
//
//bool normalReferenceApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (token.state != dfaFullIdentifier)
//      return false;
//
//   return apply(rule, token, reader);
//}
//
//bool normalReferenceApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (token.state != dfaFullIdentifier)
//      return false;
//
//   Terminal terminal;
//   token.copyTo(&terminal);
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
//
//   return true;
//}

bool normalEOFApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   return token.state == dfaEOF;
}

bool normalEOFApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaEOF)
      return false;

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token);

   return true;
}

//bool chomskiApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if (applyNonterminal(rule, token, reader)) {
//      // additional nonterminal should be resolved as well
//      if (applyNonterminal(rule, token, reader, true)) {
//         return true;
//      }
//   }
//   return false;
//}
//
//bool chomskiApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   Terminal terminal;
//   token.copyTo(&terminal);
//
//   if (applyNonterminalDSA(rule, token, reader)) {
//      // additional nonterminal should be resolved as well
//      if (!applyNonterminal(rule, token, reader, true))
//         return false;
//
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
//
//      return true;
//   }
//   else return false;
//}
//
//bool epsApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   return true;
//}
//
//bool epsApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   Terminal terminal;
//   token.copyTo(&terminal);
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
//
//   return true;
//}
//
//bool anyApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if(token.state == dfaEOF)
//      return false;
//
//   return apply(rule, token, reader);
//}
//
//bool anyApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
//{
//   if(token.state == dfaEOF)
//      return false;
//
//   Terminal terminal;
//   token.copyTo(&terminal);
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);
//
//   if (apply(rule, token, reader)) {
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
//
//      return true;
//   }
//   else return false;
//}

size_t CFParser :: writeBodyText(const wchar16_t* text)
{
   MemoryWriter writer(&_body);

   size_t position = writer.Position();
   writer.writeWideLiteral(text);

   return position;
}

const wchar16_t* CFParser :: getBodyText(size_t ptr)
{
   return (const wchar16_t*)_body.get(ptr);
}

void CFParser :: defineApplyRule(Rule& rule, RuleType type)
{
   bool dsaRule = (rule.postfixPtr != 0) || (rule.prefixPtr != 0);

   switch (type) {
      case rtNormal:
         if (rule.terminal == 0) {
            rule.apply = dsaRule ? nonterminalApplyRuleDSA : nonterminalApplyRule;
         }
         else rule.apply = dsaRule ? normalApplyRuleDSA : normalApplyRule;
         break;
//      case rtChomski:
//         // in chomski form terminal field contains nonterminal as well
//         rule.apply = dsaRule ? chomskiApplyRuleDSA : chomskiApplyRule;
//         break;
      case rtLiteral:
         rule.apply = dsaRule ? normalLiteralApplyRuleDSA : normalLiteralApplyRule;
         break;
//      case rtNumeric:
//         rule.apply = dsaRule ? normalNumericApplyRuleDSA : normalNumericApplyRule;
//         break;
//      case rtReference:
//         rule.apply = dsaRule ? normalReferenceApplyRuleDSA : normalReferenceApplyRule;
//         break;
//      case rtIdentifier:
//         rule.apply = dsaRule ?  normalIdentifierApplyRuleDSA :  normalIdentifierApplyRule;
//         break;
//      case rtAny:
//         rule.apply = dsaRule ?  anyApplyRuleDSA :  anyApplyRule;
//         break;
//      case rtEps:
//         rule.apply = dsaRule ? epsApplyRuleDSA : epsApplyRule;
//         break;
      case rtEof:
         rule.apply = dsaRule ?  normalEOFApplyRuleDSA :  normalEOFApplyRule;
         break;
   }
}

size_t CFParser :: defineDSARule(TokenInfo& token, ScriptReader& reader)
{
   const wchar16_t* s = token.getLog();
   if (!emptystr(s)) {
      MemoryWriter writer(&_body);

      size_t ptr = writer.Position();

      writer.writeWideLiteral(s);

      token.clearLog();

      return ptr;
   }
   else return 0;
}

size_t CFParser :: defineGrammarRule(TokenInfo& token, ScriptReader& reader)
{
   ReferenceNs ns;
   int   index = 0;
   do {
      ns.copy("inline");
      ns.appendHex(index++);

      if (!_names.exist(ns))
         break;

   } while (true);

   size_t ruleId = mapRuleId(ns);

   Rule rule;

   defineGrammarRule(token, reader, rule);

   _rules.add(ruleId, rule);

   return ruleId;
}

void CFParser :: defineGrammarRule(TokenInfo& token, ScriptReader& reader, Rule& rule)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   RuleType type = rtNormal;

   while (token.value[0] != ';') {
      if (token.state == dfaQuote) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(token, reader);
            break;
         }
         else rule.terminal = writeBodyText(token.value);
      }
      else if (token.state == dfaPrivate) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(token, reader);
            break;
         }
         else {
            rule.prefixPtr = defineDSARule(token, reader);

            if (ConstantIdentifier::compare(token.value, LITERAL_KEYWORD)) {
               type = rtLiteral;
            }
      //      else if (ConstantIdentifier::compare(token.value, NUMERIC_KEYWORD)) {
      //         type = rtNumeric;
      //      }
      //      else if (ConstantIdentifier::compare(token.value, EPS_KEYWORD)) {
      //         type = rtEps;
      //      }
            else if (ConstantIdentifier::compare(token.value, EOF_KEYWORD)) {
               type = rtEof;
            }
      //      else if (ConstantIdentifier::compare(token.value, REFERENCE_KEYWORD)) {
      //         type = rtReference;
      //      }
      //      else if (ConstantIdentifier::compare(token.value, ANY_KEYWORD)) {
      //         type = rtAny;
      //      }
      //      else if (ConstantIdentifier::compare(token.value, IDENTIFIER_KEYWORD)) {
      //         type = rtIdentifier;
      //      }
         }
      }
      else if (token.state == dfaIdentifier) {
         if (rule.nonterminal == 0) {
            rule.prefixPtr = defineDSARule(token, reader);

            rule.nonterminal = mapRuleId(token.value);
         }

//      if (token.state == dfaIdentifier && rule.terminal == 0) {
//         type = rtChomski;
//
//         rule.terminal = mapRuleId(token.value);
//
//         reader.read(token);
//      }
      }
      else if (token.value[0] == '%' || token.value[0] == '.' || token.value[0] == '&') {
         // save the next token directly to output
         token.writeLog();         
         reader.read(token);
         token.writeLog();
      }
      else token.writeLog();

      reader.read(token);      
   }

   rule.postfixPtr = defineDSARule(token, reader);

   defineApplyRule(rule, type);
}

void CFParser :: writeDSARule(TokenInfo& token, size_t ptr)
{
   token.buffer->write(getBodyText(ptr));
}

bool CFParser :: applyRule(size_t ruleId, TokenInfo& token, CachedScriptReader& reader)
{
   size_t readerRollback = reader.Position();
   size_t coderRollback = token.LogPosition();

   RuleMap::Iterator it = _rules.getIt(ruleId);
   while(!it.Eof()) {
      CFParser::TokenInfo tokenCopy(token);

      if ((*it).apply(*it, tokenCopy, reader)) {
         tokenCopy.save(token);

         return true;
      }

      // roll back
      reader.seek(readerRollback);
      token.trimLog(coderRollback);

      it = _rules.getNextIt(ruleId, it);
   }

   return false;
}

bool CFParser :: applyRule(Rule& rule, TokenInfo& token, CachedScriptReader& reader)
{
   if (rule.apply(rule, token, reader)) {
      return true;
   }
   else return false;
}

void CFParser :: compile(TokenInfo& token, CachedScriptReader& reader, _ScriptCompiler* compiler)
{
   while(reader.read(token)) {
      if (token.compare("]]")) {
         return;
      }
      else if (token.compare("#define")) {
         reader.read(token);

         if (token.state != dfaIdentifier)
            throw EParseError(token.column, token.row);

         size_t ruleId = mapRuleId(token.value);

         reader.read(token);
         if (ConstantIdentifier::compare(token.value, "::=")) {
            Rule rule;

            reader.read(token);
            defineGrammarRule(token, reader, rule);

            _rules.add(ruleId, rule);
         }
         else throw EParseError(token.column, token.row);
      }
      // compile the line
      else if (token.compare(";")) {
         WideLiteralTextReader script(token.getLog());

         compiler->compile(&script);

         token.clearLog();
      }
      else token.writeLog();
   }
}

void CFParser :: parse(TextReader* script, _ScriptCompiler* compiler)
{
   CachedScriptReader reader(script);
   ScriptLog log;

   TokenInfo token(this, &log);

   reader.read(token);

   // if it is script derective
   if (token.compare("[[")) {
      compile(token, reader, compiler);

      reader.read(token);
   }
   // if it is switch derective
   if (token.compare("@")) {
      reader.read(token);
      if (token.compare("@")) {
         reader.switchDFA(dfaSymbolic);

         reader.read(token);
      }
      else throw EParseError(token.column, token.row);
   }

   size_t readerRollback = reader.Position();
   //size_t outputRollback = compiler->Position();
   size_t startId = mapRuleId(ConstantIdentifier("start"));

   if (token.state != dfaEOF) {
      if (!applyRule(startId, token, reader))
         throw EParseError(token.column, token.row);

      if (token.state != dfaEOF)
         throw EParseError(token.column, token.row);

      // compile the body
      WideLiteralTextReader script(token.getLog());

      compiler->compile(&script);

      token.clearLog();
   }
}
