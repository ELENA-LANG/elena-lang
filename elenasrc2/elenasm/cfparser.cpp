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

#define REFERENCE_KEYWORD     "$reference"
#define IDENTIFIER_KEYWORD    "$identifier"
#define LITERAL_KEYWORD       "$literal"
#define NUMERIC_KEYWORD       "$numeric"
#define EPS_KEYWORD           "$eps"
#define EOF_KEYWORD           "$eof"

// --- ScriptReader :: Reader ---

CFParser::ScriptReader::Reader :: Reader(const char** dfa, TextReader* script)
   : parser(dfa, 4, script)
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

CFParser::ScriptReader :: ScriptReader(const char** dfa, TextReader* script)
   : _reader(dfa, script)
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

CFParser::CachedScriptReader :: CachedScriptReader(const char** dfa, TextReader* script)
   : ScriptReader(dfa, script)
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
   Terminal terminal;
   token.copyTo(&terminal);

   rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

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

   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);
      
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
   Terminal terminal;
   token.copyTo(&terminal);

   if(applyNonterminalDSA(rule, token, reader)) {
      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

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

   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

   return true;
}

bool normalNumericApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
      return false;

   if (!apply(rule, token, reader))
      return false;

   return true;
}

bool normalNumericApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
      return false;

   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

   return true;
}

bool normalIdentifierApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaIdentifier)
      return false;

   return apply(rule, token, reader);
}

bool normalIdentifierApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaIdentifier)
      return false;

   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

   return true;
}

bool normalReferenceApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaFullIdentifier)
      return false;

   return apply(rule, token, reader);
}

bool normalReferenceApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaFullIdentifier)
      return false;

   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

   return true;
}

bool normalEOFApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   return token.state == dfaEOF;
}

bool normalEOFApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (token.state != dfaEOF)
      return false;

   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

   return true;
}

bool chomskiApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   if (applyNonterminal(rule, token, reader)) {
      // additional nonterminal should be resolved as well
      if (applyNonterminal(rule, token, reader, true)) {
         return true;
      }
   }
   return false;
}

bool chomskiApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   Terminal terminal;
   token.copyTo(&terminal);

   if (applyNonterminalDSA(rule, token, reader)) {
      // additional nonterminal should be resolved as well
      if (!applyNonterminal(rule, token, reader, true))
         return false;

      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

      return true;
   }
   else return false;
}

bool epsApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   return true;
}

bool epsApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader)
{
   Terminal terminal;
   token.copyTo(&terminal);

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token.parser, token.compiler, &terminal);

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token.parser, token.compiler, &terminal);

   return true;
}

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
      case rtChomski:
         // in chomski form terminal field contains nonterminal as well
         rule.apply = dsaRule ? chomskiApplyRuleDSA : chomskiApplyRule;
         break;
      case rtLiteral:
         rule.apply = dsaRule ? normalLiteralApplyRuleDSA : normalLiteralApplyRule;
         break;
      case rtNumeric:
         rule.apply = dsaRule ? normalNumericApplyRuleDSA : normalNumericApplyRule;
         break;
      case rtReference:
         rule.apply = dsaRule ? normalReferenceApplyRuleDSA : normalReferenceApplyRule;
         break;
      case rtIdentifier:
         rule.apply = dsaRule ?  normalIdentifierApplyRuleDSA :  normalIdentifierApplyRule;
         break;
//      case rtAny:
//         rule.apply = anyApplyRule;
//         break;
      case rtEps:
         rule.apply = dsaRule ? epsApplyRuleDSA : epsApplyRule;
         break;
      case rtEof:
         rule.apply = dsaRule ?  normalEOFApplyRuleDSA :  normalEOFApplyRule;
         break;
   }
}

void CFParser :: defineDSARule(TokenInfo& token, ScriptReader& reader, size_t& ptr)
{
   reader.read(token);

   MemoryWriter writer(&_body);

   ptr = writer.Position();

   wchar_t lastState = 0;
   while (!ConstantIdentifier::compare(token.value, "]")) {
      if (token.state == dfaQuote) {
         writer.writeWideChar('\"');
         for (size_t i = 0 ; i < getlength(token.value) ; i++) {
            writer.writeWideChar(token.value[i]);
            if (token.value[i]=='"') {
               writer.writeWideChar(token.value[i]);
            }
         }
         writer.writeWideChar('\"');
      }
      else if (token.state == dfaEOF) {
         throw EParseError(token.column, token.row);
      }
      else {
         writer.writeWideChar(0x20);

         writer.writeWideLiteral(token.value, getlength(token.value));
      }
      lastState = token.state;

      reader.read(token);
   }

   writer.writeWideChar(0);
}

void CFParser :: defineGrammarRule(TokenInfo& token, ScriptReader& reader, Rule& rule)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   RuleType type = rtNormal;

   reader.read(token);

   // prefix dsa rule
   if (ConstantIdentifier::compare(token.value, "[")) {
      defineDSARule(token, reader, rule.prefixPtr);

      reader.read(token);
   }

   if (token.state == dfaQuote || token.state == dfaPrivate) {
      if (ConstantIdentifier::compare(token.value, LITERAL_KEYWORD)) {
         type = rtLiteral;
      }
      else if (ConstantIdentifier::compare(token.value, NUMERIC_KEYWORD)) {
         type = rtNumeric;
      }
      else if (ConstantIdentifier::compare(token.value, EPS_KEYWORD)) {
         type = rtEps;
      }
      else if (ConstantIdentifier::compare(token.value, EOF_KEYWORD)) {
         type = rtEof;
      }
      else if (ConstantIdentifier::compare(token.value, REFERENCE_KEYWORD)) {
         type = rtReference;
      }
//      else if (ConstantIdentifier::compare(token.value, ANY_KEYWORD)) {
//         type = rtAny;
//      }
      else if (ConstantIdentifier::compare(token.value, IDENTIFIER_KEYWORD)) {
         type = rtIdentifier;
      }
      else if(token.state == dfaQuote) {
         rule.terminal = writeBodyText(token.value);
      }
      reader.read(token);
   }

   if (token.state == dfaIdentifier) {
      rule.nonterminal = mapRuleId(token.value);

      reader.read(token);
      if (token.state == dfaIdentifier && rule.terminal == 0) {
         type = rtChomski;

         rule.terminal = mapRuleId(token.value);

         reader.read(token);
      }
   }

   // postfix dsa rule
   if (ConstantIdentifier::compare(token.value, "[")) {
      defineDSARule(token, reader, rule.postfixPtr);

      reader.read(token);
   }

   if (!ConstantIdentifier::compare(token.value, ";"))
      throw EParseError(token.column, token.row);

   defineApplyRule(rule, type);
}

void CFParser :: applyDSARule(_ScriptCompiler* compiler, size_t ptr, Terminal* terminal)
{
   WideLiteralTextReader subScript(getBodyText(ptr));

   compiler->compile(&subScript, terminal);
}

bool CFParser :: applyRule(size_t ruleId, TokenInfo& token, CachedScriptReader& reader)
{
   size_t readerRollback = reader.Position();
   size_t coderRollback = token.compiler->Position();

   CFParser::TokenInfo tokenCopy(token);

   RuleMap::Iterator it = _rules.getIt(ruleId);
   while(!it.Eof()) {
      if ((*it).apply(*it, tokenCopy, reader)) {
         tokenCopy.save(token);

         return true;
      }

      // roll back
      reader.seek(readerRollback);
      token.compiler->trim(coderRollback);
      token.save(tokenCopy);

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

void CFParser :: generate(TextReader* script)
{
   TokenInfo token(this, NULL);
   ScriptReader reader(NULL, script);
   while(reader.read(token)) {
      if (token.state != dfaIdentifier)
         throw EParseError(token.column, token.row);

      size_t ruleId = mapRuleId(token.value);

      reader.read(token);
      if (ConstantIdentifier::compare(token.value, "::=")) {
         Rule rule;

         defineGrammarRule(token, reader, rule);

         _rules.add(ruleId, rule);
      }
      else throw EParseError(token.column, token.row);
   }
}

void CFParser :: parse(TextReader* script, _ScriptCompiler* compiler)
{
   CachedScriptReader reader(_dfa, script);

   TokenInfo token(this, compiler);

   size_t readerRollback = reader.Position();
   size_t outputRollback = compiler->Position();
   size_t startId = mapRuleId(ConstantIdentifier("start"));

   reader.read(token);

   if (!applyRule(startId, token, reader))
      throw EParseError(token.column, token.row);

   if (token.state != dfaEOF)
      throw EParseError(token.column, token.row);
}

//// --- InlineScriptParser ---
//
//void InlineScriptParser :: parseRole(TextSourceReader& source, wchar16_t* token, Terminal* terminal)
//{
//   LineInfo info = source.read(token, IDENTIFIER_LEN);
//
//   _writer.writeCallCommand(token);
//   _writer.writeCommand(POP_ROLE_MESSAGE_ID);
//}
//
//void InlineScriptParser :: parseNewObject(TextSourceReader& source, wchar16_t* token, Terminal* terminal)
//{
//   LineInfo info = source.read(token, IDENTIFIER_LEN);
//
//   _writer.writeCommand(NEW_TAPE_MESSAGE_ID, token);
//}
