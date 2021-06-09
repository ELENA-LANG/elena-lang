//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  :  ELENA VM Script Engine
//
//                                              (C)2011-2021, by Alexei Rakov
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
#define EOL_KEYWORD           "$eol"
#define ANYCHR_KEYWORD        "$chr" // > 32
#define CURRENT_KEYWORD       "$current"
#define CHARACTER_KEYWORD     "$character"

#define REFERENCE_MODE        1
#define IDENTIFIER_MODE       2
#define LITERAL_MODE          3
#define NUMERIC_MODE          4
#define EOF_MODE              5
#define IDLE_MODE             6
#define EOL_MODE              7
#define LETTER_MODE           8
#define MULTI_MODE            9
#define EXCLUDE_MODE          10
#define CHARACTER_MODE        11

const char* dfaSymbolic[4] =
{
        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
        "*********dd**d******************************************************************************************************************"
};

constexpr auto WITHFORWARD_MASK = 0x80000000;

// --- CFParser ---

inline pos_t createKey(pos_t id, int index)
{
   return (id << cnSyntaxPower) + index;
}

bool normalApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return bm.state != dfaEOF && parser->compareToken(reader, bm, rule.terminal);
}

bool multiselectApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return bm.state == dfaIdentifier && parser->compareTokenWithAny(reader, bm, rule.terminal);
}

bool excludeApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return bm.state == dfaIdentifier && !parser->compareTokenWithAny(reader, bm, rule.terminal);
}

bool normalEOFApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader&, CFParser*)
{
   return (bm.state == dfaEOF);
}

bool normalEOLApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader& reader, CFParser*)
{
   ident_t value = reader.lookup(bm);
   return (value[0] == '\n');
}

bool normalLetterApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader& reader, CFParser*)
{
   ident_t value = reader.lookup(bm);
   return (value[0] >= 'A' && value[0] <= 'Z') || (value[0] >= 'a' && value[0] <= 'z');
}

bool normalReferenceApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader&, CFParser*)
{
   return (bm.state == dfaFullIdentifier);
}

bool normalIdentifierApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader&, CFParser*)
{
   return (bm.state == dfaIdentifier);
}

bool normalCharacterApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader& reader, CFParser*)
{
   if (bm.state == dfaPrivate) {
      ident_t value = reader.lookup(bm);
      for (pos_t i = 0; i < getlength(value); i++) {
         if (value[i] < '0' && value[i] > '9')
            return false;
      }

      return true;
   }
   else return false;
}

bool normalLiteralApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader&, CFParser*)
{
   return (bm.state == dfaQuote);
}

bool nonterminalApplyRule(CFParser::Rule&, ScriptBookmark&, _ScriptReader&, CFParser*)
{
   return true;
}

bool normalNumericApplyRule(CFParser::Rule&, ScriptBookmark& bm, _ScriptReader&, CFParser*)
{
   return (bm.state == dfaInteger || bm.state == dfaLong || bm.state == dfaReal);
}

void saveReference(_ScriptReader& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log)
{
   ScriptBookmark bm;
   parser->readScriptBookmark(ptr, bm);

   log.writeCoord(bm.column, bm.row);
   log.write(scriptReader.lookup(bm));
}

void saveLiteral(_ScriptReader& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log)
{
   ScriptBookmark bm;
   parser->readScriptBookmark(ptr, bm);

   log.writeCoord(bm.column, bm.row);
   log.writeQuote(scriptReader.lookup(bm));
}

void saveLiteralContent(_ScriptReader& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log)
{
   ScriptBookmark bm;
   parser->readScriptBookmark(ptr, bm);

   log.writeCoord(bm.column, bm.row);
   log.write(scriptReader.lookup(bm));
}

void CFParser :: readScriptBookmark(pos_t ptr, ScriptBookmark& bm)
{
   MemoryReader reader(&_body, ptr);
   reader.read(&bm, sizeof(ScriptBookmark));
}

pos_t CFParser :: writeBodyText(ident_t text)
{
   MemoryWriter writer(&_body);

   pos_t position = writer.Position();
   writer.writeLiteral(text);

   return position;
}

pos_t CFParser :: writeRegExprBodyText(_ScriptReader& reader, int& mode)
{
   MemoryWriter writer(&_body);

   pos_t position = writer.Position();
   bool inversionMode = false;

   ScriptBookmark bm = reader.read();
   if (reader.compare("!")) {
      bm = reader.read();
      inversionMode = true;
   }

   bool tokenExpected = true;
   while (!reader.compare(")") || bm.state == dfaQuote) {
      if (bm.state == dfaQuote && tokenExpected) {
         writer.writeLiteral(reader.lookup(bm));

         tokenExpected = false;
      }
      else if (!tokenExpected && reader.compare("|")) {
         tokenExpected = true;
      }
      else throw EParseError(bm.column, bm.row);

      bm = reader.read();
   }
   writer.writeChar((char)0);

   mode = inversionMode ? EXCLUDE_MODE : MULTI_MODE;

   return position;
}

const char* CFParser :: getBodyText(pos_t ptr)
{
   return (const char*)_body.get(ptr);
}

bool CFParser :: compareToken(_ScriptReader& reader, ScriptBookmark& bm, int rule)
{
   ident_t terminal = reader.lookup(bm);
   ident_t ruleTerminal = getBodyText(rule);

   return terminal.compare(ruleTerminal);
}

bool CFParser :: compareTokenWithAny(_ScriptReader& reader, ScriptBookmark& bm, int rule)
{
   ident_t terminal = reader.lookup(bm);
   ident_t ruleTerminal = getBodyText(rule);
   do {
      if (terminal.compare(ruleTerminal))
         return true;

      rule += getlength(ruleTerminal) + 1;
      ruleTerminal = getBodyText(rule);
   } while (!emptystr(ruleTerminal));

   return false;
}

void CFParser :: defineApplyRule(Rule& rule, int mode, bool forwardMode, bool bookmarkMode)
{
//   bool dsaRule = (rule.postfixPtr != 0) || (rule.prefixPtr != 0);

   if (rule.type == rtNormal) {
      if (rule.terminal == 0) {
         rule.type = rtNonterminal;

         rule.apply = nonterminalApplyRule;
      }
      else {
         switch (mode) {
            case EOF_MODE:
               rule.apply = normalEOFApplyRule;
               break;
            case LITERAL_MODE:
               rule.apply = normalLiteralApplyRule;
               break;
            case NUMERIC_MODE:
               rule.apply = normalNumericApplyRule;
               break;
            case REFERENCE_MODE:
               rule.apply = normalReferenceApplyRule;
               break;
            case IDENTIFIER_MODE:
               rule.apply = normalIdentifierApplyRule;
               break;
            case CHARACTER_MODE:
               rule.apply = normalCharacterApplyRule;
               break;
            case IDLE_MODE:
               rule.apply = nonterminalApplyRule;
               rule.nonterminal = 0;
               rule.terminal = 0;
               rule.type = rtEps;
               break;
            case EOL_MODE:
               rule.apply = normalEOLApplyRule;
               break;
            case LETTER_MODE:
               rule.apply = normalLetterApplyRule;
               break;
            case MULTI_MODE:
               rule.apply = multiselectApplyRule;
               break;
            case EXCLUDE_MODE:
               rule.apply = excludeApplyRule;
               break;
            default:
               rule.apply = normalApplyRule;
               break;
         }
      }
   }
   else if (rule.type == rtChomski) {
      rule.apply = nonterminalApplyRule;
   }

   if (forwardMode)
      rule.type = (RuleType)(rule.type | rtWithForward);
   if (bookmarkMode)
      rule.type = (RuleType)(rule.type | rtWithBookmark);
}

void CFParser :: addRule(int ruleId, Rule& rule)
{
   pos_t key = createKey(ruleId, 1);
   while (_table.exist(key))
      key++;

   _table.add(key, rule);
}

void CFParser :: defineIdleGrammarRule(ref_t ruleId)
{
   Rule rule;
   rule.nonterminal = INVALID_REF;
   rule.terminal = INVALID_REF;
   rule.type = rtNormal;
   defineApplyRule(rule, IDLE_MODE, false, false);
   addRule(ruleId, rule);
}

pos_t CFParser :: defineChomskiGrammarRule(ref_t parentRuleId, pos_t nonterminal, pos_t terminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define C -> AB
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = terminal;
   recRule.type = rtChomski;
   defineApplyRule(recRule, 0, false, false);
   addRule(ruleId, recRule);

   return ruleId;
}

pos_t CFParser :: defineStarGrammarRule(ref_t parentRuleId, pos_t nonterminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define B -> AB
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = ruleId;
   recRule.type = rtChomski;
   defineApplyRule(recRule, 0, false, false);
   addRule(ruleId, recRule);

   // define B -> eps
   defineIdleGrammarRule(ruleId);

   return ruleId;
}

pos_t CFParser :: defineOptionalGrammarRule(ref_t parentRuleId, pos_t nonterminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define B -> A
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = 0;
   recRule.type = rtNormal;
   defineApplyRule(recRule, 0, false, false);
   addRule(ruleId, recRule);

   // define B -> eps
   defineIdleGrammarRule(ruleId);

   return ruleId;
}

pos_t CFParser :: definePlusGrammarRule(ref_t parentRuleId, pos_t nonterminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define B -> AB
   Rule rule;
   rule.nonterminal = nonterminal;
   rule.terminal = defineStarGrammarRule(ruleId, nonterminal);
   rule.type = rtChomski;
   defineApplyRule(rule, 0, false, false);
   addRule(ruleId, rule);

   return ruleId;
}

ref_t CFParser :: autonameRule(ref_t parentRuleId)
{
   ReferenceNs ns;
   int   index = 0;
   do {
      ns.copy(retrieveKey(_names.start(), parentRuleId, DEFAULT_STR));
      ns.append('.');
      ns.appendHex(index++);

      if (!_names.exist(ns))
         break;

   } while (true);

   return mapRuleId(ns);
}

void CFParser :: setScriptPtr(ScriptBookmark& bm, Rule& rule, bool prefixMode)
{
   if (prefixMode) {
      if (rule.postfix1Ptr)
         throw EParseError(bm.column, bm.row);

      rule.postfix1Ptr = _body.Length();
   }
   else {
      if (rule.postfix1Ptr)
         throw EParseError(bm.column, bm.row);

      rule.postfix2Ptr = _body.Length();
   }
}

pos_t CFParser :: defineGrammarBrackets(_ScriptReader& reader, ScriptBookmark& bm, pos_t parentRuleId)
{
   pos_t ruleId = autonameRule(parentRuleId);

   Rule rule;
   rule.type = rtNormal;
   int applyMode = 0;

   bm = reader.read();
   while (!reader.compare("}")) {
      if (reader.compare("|")) {
         defineApplyRule(rule, applyMode, false, false);
         addRule(ruleId, rule);

         rule = Rule();
         rule.type = rtNormal;
         applyMode = 0;

         bm = reader.read();
      }
      else defineGrammarRuleMember(reader, bm, rule, ruleId, applyMode);
   }

   defineApplyRule(rule, applyMode, false, false);

   addRule(ruleId, rule);

   bm = reader.read();
   if (bm.state == dfaQuote) {
   }
   else if (reader.compare("+")) {
      ruleId = definePlusGrammarRule(parentRuleId, ruleId);

      bm = reader.read();
   }
   else if (reader.compare("?")) {
      ruleId = defineOptionalGrammarRule(parentRuleId, ruleId);

      bm = reader.read();
   }
   else if (reader.compare("*")) {
      ruleId = defineStarGrammarRule(parentRuleId, ruleId);

      bm = reader.read();
   }

   return ruleId;
}

void CFParser :: defineGrammarRuleMemberPostfix(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId)
{
   bm = reader.read();
   if (bm.state == dfaQuote) {
   }
   else if (reader.compare("*") || reader.compare("+") || reader.compare("?")) {
      if (rule.terminal != 0 && (rule.type % rtTypeMask) == rtChomski) {
         if (reader.compare("+")) {
            rule.terminal = definePlusGrammarRule(ruleId, rule.terminal);
         }
         else if (reader.compare("?")) {
            rule.terminal = defineOptionalGrammarRule(ruleId, rule.terminal);
         }
         else rule.terminal = defineStarGrammarRule(ruleId, rule.terminal);
      }
      else if (rule.nonterminal != 0) {
         if (reader.compare("+")) {
            rule.nonterminal = definePlusGrammarRule(ruleId, rule.nonterminal);
         }
         else if (reader.compare("?")) {
            rule.nonterminal = defineOptionalGrammarRule(ruleId, rule.nonterminal);
         }
         else rule.nonterminal = defineStarGrammarRule(ruleId, rule.nonterminal);
      }
      else throw EParseError(bm.column, bm.row);

      bm = reader.read();
   }
}

pos_t CFParser :: defineGrammarRuleMember(_ScriptReader& reader, ScriptBookmark& bm, pos_t parentRuleId,
   pos_t nonterminal, pos_t terminal)
{
   ref_t ruleId = autonameRule(parentRuleId);

   Rule rule;
   rule.type = rtNormal;
   rule.nonterminal = nonterminal;
   rule.terminal = terminal;

   int applyMode = 0;
   defineGrammarRuleMember(reader, bm, rule, ruleId, applyMode);
   defineApplyRule(rule, applyMode, false, false);

   addRule(ruleId, rule);

   return ruleId;
}

void CFParser :: saveScript(_ScriptReader& reader, Rule& rule, int& mode, bool forwardMode)
{
   bool prefixMode = false;
   if (rule.terminal == 0 && rule.nonterminal == 0) {
      prefixMode = true;
      rule.prefix1Ptr = _body.Length();
   }
   else rule.prefix2Ptr = _body.Length();

   MemoryWriter writer(&_body);
   ScriptBookmark bm = reader.read();
   while (!reader.compare("=>") || bm.state == dfaQuote) {
      if (bm.state == dfaPrivate) {
         if (forwardMode)
            throw EParseError(bm.column, bm.row);

         if (rule.saveTo != NULL || (!prefixMode && !reader.compare(CURRENT_KEYWORD)))
            throw EParseError(bm.column, bm.row);

         if (reader.compare(REFERENCE_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saveTo = saveReference;

            mode = REFERENCE_MODE;
         }
         else if (reader.compare(IDENTIFIER_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saveTo = saveReference;

            mode = IDENTIFIER_MODE;
         }
         else if (reader.compare(CHARACTER_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saveTo = saveLiteralContent;

            mode = CHARACTER_MODE;
         }
         else if (reader.compare(LITERAL_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saveTo = saveLiteralContent;

            mode = LITERAL_MODE;
         }
         else if (reader.compare(NUMERIC_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saveTo = saveReference;

            mode = NUMERIC_MODE;
         }
         if (reader.compare(CURRENT_KEYWORD)) {
            rule.saveTo = saveReference;
         }

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);

   //      rule.postfixPtr = _body.Length();
   //      prefixMode = false;
      }
      else if (bm.state == dfaQuote && reader.compare(REFERENCE_KEYWORD)) {
         if (forwardMode)
            throw EParseError(bm.column, bm.row);

         rule.terminal = INVALID_REF;
         rule.saveTo = saveLiteral;

         mode = REFERENCE_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(IDENTIFIER_KEYWORD)) {
         if (forwardMode)
            throw EParseError(bm.column, bm.row);

         rule.terminal = INVALID_REF;
         rule.saveTo = saveLiteral;

         mode = IDENTIFIER_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(LITERAL_KEYWORD)) {
         if (forwardMode)
            throw EParseError(bm.column, bm.row);

         rule.terminal = INVALID_REF;
         rule.saveTo = saveLiteral;

         mode = LITERAL_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(NUMERIC_KEYWORD)) {
         if (forwardMode)
            throw EParseError(bm.column, bm.row);

         rule.terminal = INVALID_REF;
         rule.saveTo = saveLiteral;

         mode = NUMERIC_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(CHARACTER_KEYWORD)) {
         if (forwardMode)
            throw EParseError(bm.column, bm.row);

         rule.terminal = INVALID_REF;
         rule.saveTo = saveLiteral;

         mode = CHARACTER_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else {
         ident_t token = reader.lookup(bm);

         if (bm.state == dfaQuote) {
            ScriptLog::writeQuote(writer, token);
         }
         else writer.writeLiteral(token, getlength(token));
         writer.writeChar(' ');
      }

      bm = reader.read();
   }
   writer.writeChar((char)0);
}

void CFParser :: defineGrammarRuleMember(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId, int& applyMode)
{
   if (bm.state == dfaQuote || reader.compare("(")) {
      if (rule.terminal) {
         if (rule.nonterminal != 0) {
            if (rule.type == rtChomski) {
               rule.terminal = defineGrammarRuleMember(reader, bm, ruleId, rule.terminal);
            }
            else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId, rule.nonterminal);
         }
         else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId);
      }
      else if (rule.nonterminal) {
         rule.type = rtChomski;
         rule.terminal = defineGrammarRuleMember(reader, bm, ruleId);
      }
      else {
         if (bm.state != dfaQuote) {
            rule.terminal = writeRegExprBodyText(reader, applyMode);
         }
         else rule.terminal = writeBodyText(reader.lookup(bm));

         defineGrammarRuleMemberPostfix(reader, bm, rule, ruleId);
      }
   }
   else if (reader.compare("{")) {
      int bracketRuleId = defineGrammarBrackets(reader, bm, ruleId);

      if (rule.nonterminal == 0) {
         //rule.prefixPtr = defineDSARule(token, reader);

         rule.nonterminal = bracketRuleId;
      }
      else if (rule.terminal == 0) {
         rule.type = rtChomski;

         rule.terminal = bracketRuleId;
      }
      else {
         if (rule.type == rtChomski) {
            rule.terminal = defineChomskiGrammarRule(ruleId, rule.terminal, bracketRuleId);
         }
         else rule.nonterminal = defineChomskiGrammarRule(ruleId, rule.nonterminal, bracketRuleId);
      }
   }
   else if (bm.state == dfaPrivate) {
      if (rule.terminal) {
         rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId, rule.nonterminal);
      }
      else if (rule.nonterminal) {
         rule.type = rtChomski;
         rule.terminal = defineGrammarRuleMember(reader, bm, ruleId);
      }
      else {
         rule.terminal = INVALID_REF;
         //            rule.prefixPtr = defineDSARule(token, reader);

         if (reader.compare(LITERAL_KEYWORD)) {
            applyMode = LITERAL_MODE;
         }
         else if (reader.compare(NUMERIC_KEYWORD)) {
            applyMode = NUMERIC_MODE;
         }
         else if (reader.compare(EPS_KEYWORD)) {
            if (rule.nonterminal)
               throw EParseError(bm.column, bm.row);

            rule.nonterminal = INVALID_REF;
            rule.terminal = INVALID_REF;
            applyMode = IDLE_MODE;
         }
         else if (reader.compare(EOF_KEYWORD)) {
            applyMode = EOF_MODE;
         }
         else if (reader.compare(REFERENCE_KEYWORD)) {
            applyMode = REFERENCE_MODE;
         }
         else if (reader.compare(IDENTIFIER_KEYWORD)) {
            applyMode = IDENTIFIER_MODE;
         }
         else if (reader.compare(EOL_KEYWORD)) {
            applyMode = EOL_MODE;
         }
         else if (reader.compare(ANYCHR_KEYWORD)) {
            applyMode = LETTER_MODE;
         }

         defineGrammarRuleMemberPostfix(reader, bm, rule, ruleId);
      }
   }
   else if (bm.state == dfaIdentifier) {
      if (rule.nonterminal == 0 || rule.terminal == 0) {
         if (rule.nonterminal == 0) {
            //rule.prefixPtr = defineDSARule(token, reader);

            rule.nonterminal = mapRuleId(reader.lookup(bm));
         }
         else {
            rule.type = rtChomski;

            rule.terminal = mapRuleId(reader.lookup(bm));
         }

         defineGrammarRuleMemberPostfix(reader, bm, rule, ruleId);
      }
      else {
         if (rule.type == rtChomski) {
            rule.terminal = defineGrammarRuleMember(reader, bm, ruleId, rule.terminal);
         }
         else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId, rule.nonterminal);
      }
   }
   else EParseError(bm.column, bm.row);
}

void CFParser :: defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId)
{
//   // read: terminal [nonterminal] ;
//   // read: nonterminal [nonterminal2] ;

   rule.type = rtNormal;
   int applyMode = 0;
   bool forwardMode = false;
   bool bookmarkMode = false;

    while (!reader.compare(";") || bm.state == dfaQuote) {
      if (bm.state != dfaQuote && reader.compare("<=")) {
         saveScript(reader, rule, applyMode, forwardMode);

         bm = reader.read();
      }
      else if (bm.state != dfaQuote && reader.compare("^")) {
         bm = reader.read();
         if (bm.state != dfaQuote && reader.compare("<=")) {
            forwardMode = true;

            saveScript(reader, rule, applyMode, forwardMode);

            bm = reader.read();
         }
         else throw EParseError(bm.column, bm.row);
      }
      else if (bm.state != dfaQuote && reader.compare("$")) {
         if (!rule.prefix1Ptr) {
            bookmarkMode = true;
         }
         else throw EParseError(bm.column, bm.row);

         bm = reader.read();
      }
      else defineGrammarRuleMember(reader, bm, rule, ruleId, applyMode);
   }

////   rule.postfixPtr = defineDSARule(token, reader);

   defineApplyRule(rule, applyMode, forwardMode, bookmarkMode);
}

bool CFParser :: parseGrammarRule(_ScriptReader& reader)
{
   ScriptBookmark bm = reader.read();

   if (bm.state != dfaIdentifier)
      throw EParseError(bm.column, bm.row);

   ref_t ruleId = mapRuleId(reader.lookup(bm));

   reader.read();
   if (!reader.compare("::="))
      throw EParseError(bm.column, bm.row);

   bm = reader.read();

   Rule rule;
   defineGrammarRule(reader, bm, rule, ruleId);

   addRule(ruleId, rule);

   return true;
}

bool CFParser :: parseGrammarMode(_ScriptReader& reader)
{
   ScriptBookmark bm = reader.read();
   while (!reader.compare(";")) {
      if (reader.compare("symbolic")) {
         _symbolMode = true;
      }
//      else throw EParseError(reader.info.column, reader.info.row);

      bm = reader.read();
   }

   return true;
}

inline int writeDerivationItem(MemoryWriter& writer, int key, int terminal, int trace, bool forwardMode)
{
   if (forwardMode)
      key |= WITHFORWARD_MASK;

   int offset = writer.Position();
   writer.writeDWord(key);
   writer.writeDWord(terminal);
   writer.writeDWord(trace);

   return offset;
}

inline int writeTrailItem(MemoryWriter& writer, int nonterminal, int next)
{
   int offset = writer.Position();
   writer.writeDWord(nonterminal);
   writer.writeDWord(next);

   return offset;
}

inline void readTailItemAndPush(MemoryReader& reader, MemoryWriter& writer, CFParser::DerivationQueue& queue, int offset)
{
   int nonterminal = reader.getDWord();
   pos_t next = reader.getDWord();

   while (nonterminal == 0) {
      offset = writeDerivationItem(writer, 0, 0, offset, false);
      if (next != -1) {
         reader.seek(next);
         nonterminal = reader.getDWord();
         next = reader.getDWord();
      }
      else break;
   }

   if (next == -1) {
      queue.push(CFParser::DerivationItem(-1, offset, 0));
   }
   else queue.push(CFParser::DerivationItem(nonterminal, offset, next));
}

inline void readTailItemAndInsert(MemoryReader& reader, MemoryWriter& writer, CFParser::DerivationQueue& queue, int offset)
{
   int nonterminal = reader.getDWord();
   pos_t next = reader.getDWord();

   while (nonterminal == 0) {
      offset = writeDerivationItem(writer, 0, 0, offset, false);
      if (next != (pos_t)-1) {
         reader.seek(next);
         nonterminal = reader.getDWord();
         next = reader.getDWord();
      }
      else break;
   }

   if (next == -1) {
      queue.insert(CFParser::DerivationItem(-1, offset, 0));
   }
   else queue.insert(CFParser::DerivationItem(nonterminal, offset, next));
}

void CFParser :: predict(DerivationQueue& queue, DerivationItem item, _ScriptReader& reader, ScriptBookmark& bm, int terminalOffset, MemoryWriter& writer)
{
   ident_t keyName = retrieveKey(_names.start(), item.ruleId, DEFAULT_STR);

   pos_t key = createKey(item.ruleId, 1);
   Rule rule = _table.get(key);

   while (rule.type != rtNone) {
      int ruleType = rule.type & rtTypeMask;

      if (rule.apply(rule, bm, reader, this)) {
         int offset = writeDerivationItem(writer, key, terminalOffset, item.trace, testany(rule.type, rtWithForward | rtWithBookmark));
         int next = writeTrailItem(writer, 0, item.next);

         if (ruleType == rtEps) {
            MemoryReader nextReader(&_body, (pos_t)next);
            readTailItemAndInsert(nextReader, writer, queue, offset);
         }
         else if (ruleType != rtNormal) {
            // if it is a chomksi form
            if (ruleType == rtChomski) {
               next = writeTrailItem(writer, rule.terminal, next);
            }

            queue.insert(DerivationItem(rule.nonterminal, offset, next));
         }
         else if (rule.nonterminal == 0) {
            MemoryReader nextReader(&_body, (pos_t)next);
            readTailItemAndPush(nextReader, writer, queue, offset);
         }
         else queue.push(DerivationItem(rule.nonterminal, offset, next));
      }

      rule = _table.get(++key);
   }
}

int CFParser :: buildDerivationTree(_ScriptReader& reader, ref_t startRuleId, MemoryWriter& writer)
{
   DerivationQueue predictions;
   predictions.push(DerivationItem(startRuleId, 0, -1));

   ScriptBookmark bm;
   while (predictions.Count() > 0) {
      //auto p_it = predictions.start();
      //while (!p_it.Eof()) {
      //   auto r = *p_it;

      //   ident_t rName = retrieveKey(_names.start(), r.ruleId, DEFAULT_STR);
      //   if (getlength(rName) != 0) {
      //      printf("%s\n", rName.c_str());
      //   }
      //   else printf("?\n");

      //   p_it++;
      //}
      //printf("\n");

      predictions.push(DerivationItem(0));

      DerivationItem current = predictions.pop();
      if (reader.Eof()) {
         if (current.ruleId == -1) {
            return current.trace;
         }
         else throw EParseError(bm.column, bm.row);
      }         

      bm = reader.read();
      int terminalOffset = writer.Position();
      writer.write(&bm, sizeof(ScriptBookmark));
      while (current.ruleId != 0) {
         if (current.ruleId == -1) {
            return current.trace;
         }

         predict(predictions, current, reader, bm, terminalOffset, writer);

         current = predictions.pop();
      }      
   }

   throw EParseError(bm.column, bm.row);
}

constexpr auto POSTFIXSAVE_MODE = 0x80000000;

inline void clearPreviousForwards(Stack<Pair<int, int>>& forwards, int level)
{
   //if (forwards.peek().value2 < level)
   //   return;

   auto f = forwards.pop();
   while (f.value1 || f.value2 != level)
      f = forwards.pop();
}

void CFParser :: insertForwards(Stack<Pair<int, int>>& forwards, int level, ScriptLog& log)
{
   int minLevel = INT_MAX;

   auto it = forwards.start();
   while (!it.Eof()) {
      auto f = *it;
      if (f.value2 <= level)
         break;

      if (!f.value1) {
         if (minLevel > f.value2)
            minLevel = f.value2;
      }
      else if (f.value2 <= minLevel) {
         log.write(getBodyText(f.value1));
      }

      it++;
   }
}

void CFParser :: generateOutput(int offset, _ScriptReader& scriptReader, ScriptLog& log)
{
   if (offset == 0)
      return;

   Stack<Pair<int, int>> forwards(Pair<int,int>(0,-1));

   MemoryReader reader(&_body, (pos_t)offset);
   Stack<TraceItem> stack(TraceItem(0));
   TraceItem item;
   reader.read(&item, sizeof(TraceItem));
   int level = 0;
   while (true) {
      if (!item.ruleKey) {
         level++;
      }
      else level--;

      // if forward declaration
      if (test(item.ruleKey, WITHFORWARD_MASK)) {
         //int key = (item.ruleKey & ~WITHFORWARD_MASK) >> 8;
         //ident_t keyName = retrieveKey(_names.start(), key, DEFAULT_STR);

         Rule rule = _table.get(item.ruleKey & ~WITHFORWARD_MASK);
         if (test(rule.type, rtWithBookmark)) {
            forwards.push(Pair<int, int>(0, level));
         }
         else if (test(rule.type, rtWithForward)) {
            forwards.push(Pair<int,int>(rule.prefix1Ptr, level));
         }
      }
      stack.push(item);

      if (item.previous == 0)
         break;

      //    save into forwards

      reader.seek((pos_t)item.previous);
      reader.read(&item, sizeof(TraceItem));
   }

   // NOTE: reset level to -1 to match the backward calculated levels 
   level = -1;
   Stack<pos_t> postfixes;
   Stack<SaveToSign> functions;
   while (stack.Count() > 0) {
      item = stack.pop();
      if (item.ruleKey == 0) {
         level--;

         pos_t ptr = postfixes.pop();
         if (ptr) {
            if (test(ptr, POSTFIXSAVE_MODE)) {
               ptr &= ~POSTFIXSAVE_MODE;

               log.write(getBodyText(ptr));

               auto saveTo = functions.pop();
               int terminal = postfixes.pop();

               saveTo(scriptReader, this, terminal, log);

               ptr = postfixes.pop();
            }
            log.write(getBodyText(ptr));
         }            
      }
      else {
         level++;

         //for (int i = 0; i < level; i++)
         //   printf(" ");

         //ident_t keyName = retrieveKey(_names.start(), (item.ruleKey & ~WITHFORWARD_MASK) >> 8, DEFAULT_STR);
         //printf("%s\n", keyName.c_str());

         Rule rule = _table.get(item.ruleKey & ~WITHFORWARD_MASK);

         if (test(rule.type, rtWithBookmark)) {
            clearPreviousForwards(forwards, level);

            insertForwards(forwards, level, log);
         }

         if (rule.prefix1Ptr != 0) {
            pos_t lineLen = 0;
            if (!test(rule.type, rtWithForward))
               lineLen += log.write(getBodyText(rule.prefix1Ptr));

            if (rule.saveTo != NULL)
               rule.saveTo(scriptReader, this, item.terminal, log);

            // HOTFIX: to prevent too long line
            if (lineLen > 0) {
               log.write('\n');
            }
            else log.write(' ');

            log.write(getBodyText(rule.postfix1Ptr));
         }

         if (rule.postfix2Ptr) {
            postfixes.push(rule.postfix2Ptr);

            postfixes.push(item.terminal);
            functions.push(rule.saveTo);
            postfixes.push(rule.prefix2Ptr | POSTFIXSAVE_MODE);
         }
         else postfixes.push(rule.prefix2Ptr);
      }

   }
   log.write((char)0);
}

void CFParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   if (_symbolMode)
      reader.switchDFA(dfaSymbolic);

   ScriptLog log;

   ref_t startId = mapRuleId("start");
   MemoryWriter writer(&_body);

   int trace = buildDerivationTree(reader, startId, writer);
   if (!reader.Eof()) {
      ScriptBookmark bm = reader.read();
      if (bm.state != dfaEOF)
         throw EParseError(bm.column, bm.row);
   }

   generateOutput(trace, reader, log);

   IdentifierTextReader logReader((const char*)log.getBody());
   ScriptReader scriptReader(&logReader, log.getCoordinateMap());
      
   _baseParser->parse(scriptReader, output);
}
