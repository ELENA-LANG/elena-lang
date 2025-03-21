//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cfparser.h"
#include "regex.h"

using namespace elena_lang;

constexpr auto REFERENCE_KEYWORD       = "$reference";
constexpr auto IDENTIFIER_KEYWORD      = "$identifier";
constexpr auto LITERAL_KEYWORD         = "$literal";
constexpr auto NUMERIC_KEYWORD         = "$numeric";
constexpr auto EPS_KEYWORD             = "$eps";
constexpr auto IF_KEYWORD              = "$if";             // NOTE : conditional eps rule 
constexpr auto EOF_KEYWORD             = "$eof";
constexpr auto EOL_KEYWORD             = "$eol";
constexpr auto ANYCHR_KEYWORD          = "$chr";            // > 32
constexpr auto CURRENT_KEYWORD         = "$current";
constexpr auto CHARACTER_KEYWORD       = "$character";
constexpr auto BUFFER_KEYWORD          = "$buffer";     // NOTE : quote containing a number
constexpr auto REGEX_KEYWORD           = "$regex";

constexpr auto REFERENCE_MODE       = 1;
constexpr auto IDENTIFIER_MODE      = 2;
constexpr auto LITERAL_MODE         = 3;
constexpr auto NUMERIC_MODE         = 4;
constexpr auto EOF_MODE             = 5;
constexpr auto IDLE_MODE            = 6;
constexpr auto EOL_MODE             = 7;
constexpr auto LETTER_MODE          = 8;
constexpr auto MULTI_MODE           = 9;
constexpr auto EXCLUDE_MODE         = 10;
constexpr auto CHARACTER_MODE       = 11;
constexpr auto IF_MODE              = 12;
constexpr auto IFNOT_MODE           = 13;
constexpr auto BUFFER_MODE          = 15;
constexpr auto REGEX_MODE           = 17;

constexpr auto WITHFORWARD_MASK = 0x80000000;
constexpr auto POSTFIXSAVE_MODE = 0x80000000;

//#define TRACING_INFO

inline void readToken(ScriptEngineReaderBase& reader, ScriptBookmark& bm, ustr_t token)
{
   reader.read();
   if (!reader.compare(token))
      throw SyntaxError("invalid grammar rule", bm.lineInfo);
}

class ReferenceSaver : public ScriptEngineCFParser::SaverBase
{
public:
   void saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log) override
   {
      ScriptBookmark bm;
      parser->readScriptBookmark(ptr, bm);

      log.writeCoord(bm.lineInfo);
      log.write(scriptReader.lookup(bm));
   }

   static ScriptEngineCFParser::SaverBase* getInstance()
   {
      static ReferenceSaver instance;

      return &instance;
   }
};

class LiteralContentSaver : public ScriptEngineCFParser::SaverBase
{
public:
   void saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log) override
   {
      ScriptBookmark bm;
      parser->readScriptBookmark(ptr, bm);

      log.writeCoord(bm.lineInfo);
      log.write(scriptReader.lookup(bm));
   }

   static ScriptEngineCFParser::SaverBase* getInstance()
   {
      static LiteralContentSaver instance;

      return &instance;
   }
};

class LiteralSaver : public ScriptEngineCFParser::SaverBase
{
public:
   void saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log) override
   {
      ScriptBookmark bm;
      parser->readScriptBookmark(ptr, bm);

      log.writeCoord(bm.lineInfo);
      log.writeQuote(scriptReader.lookup(bm));
   }

   static ScriptEngineCFParser::SaverBase* getInstance()
   {
      static LiteralSaver instance;

      return &instance;
   }
};

class BufferSaver : public ScriptEngineCFParser::SaverBase
{
public:
   void saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log) override
   {
      parser->flushBuffer(log);
      parser->clearBuffer();
   }

   static ScriptEngineCFParser::SaverBase* getInstance()
   {
      static BufferSaver instance;

      return &instance;
   }
};

bool nonterminalApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark&, ScriptEngineReaderBase&, ScriptEngineCFParser*)
{
   return true;
}

bool normalEOFApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase&, ScriptEngineCFParser*)
{
   return (bm.state == dfaEOF);
}

bool normalLiteralApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase&, ScriptEngineCFParser*)
{
   return (bm.state == dfaQuote);
}

bool normalNumericApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase&, ScriptEngineCFParser*)
{
   return (bm.state == dfaInteger || bm.state == dfaLong || bm.state == dfaReal);
}

bool normalReferenceApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase&, ScriptEngineCFParser*)
{
   return (bm.state == dfaReference);
}

bool normalIdentifierApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase&, ScriptEngineCFParser*)
{
   return (bm.state == dfaIdentifier);
}

bool normalCharacterApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser*)
{
   if (bm.state == dfaPrivate) {
      ustr_t value = reader.lookup(bm);
      for (pos_t i = 0; i < getlength(value); i++) {
         if (value[i] < '0' && value[i] > '9')
            return false;
      }

      return true;
   }
   else return false;
}

bool normalEOLApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser*)
{
   ustr_t value = reader.lookup(bm);
   return (value[0] == '\n');
}

bool normalLetterApplyRule(ScriptEngineCFParser::Rule&, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser*)
{
   ustr_t value = reader.lookup(bm);
   return (value[0] >= 'A' && value[0] <= 'Z') || (value[0] >= 'a' && value[0] <= 'z');
}

bool multiselectApplyRule(ScriptEngineCFParser::Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser* parser)
{
   return bm.state == dfaIdentifier && parser->compareTokenWithAny(reader, bm, rule.terminal);
}

bool excludeApplyRule(ScriptEngineCFParser::Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser* parser)
{
   return bm.state == dfaIdentifier && !parser->compareTokenWithAny(reader, bm, rule.terminal);
}

bool excludeSpecialApplyRule(ScriptEngineCFParser::Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser* parser)
{
   return !parser->compareTokenWithAny(reader, bm, rule.terminal);
}

bool normalApplyRule(ScriptEngineCFParser::Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser* parser)
{
   return bm.state != dfaEOF && parser->compareToken(reader, bm, rule.terminal);
}

bool regexApplyRule(ScriptEngineCFParser::Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser* parser)
{
   return rule.saver->isMatched(reader.lookup(bm), bm.state);
}

// --- ScriptEngineCFParser ---

inline pos_t createKey(pos_t id, int index)
{
   return (id << cnCFSyntaxPower) + index;
}

const char* ScriptEngineCFParser :: getBodyText(pos_t ptr)
{
   return (const char*)_body.get(ptr);
}

pos_t ScriptEngineCFParser :: writeRegExprBodyText(ScriptEngineReaderBase& reader, int& mode)
{
   MemoryWriter writer(&_body);

   pos_t position = writer.position();
   bool inversionMode = false;

   ScriptBookmark bm = reader.read();
   if (reader.compare("!")) {
      bm = reader.read();
      inversionMode = true;
   }

   bool tokenExpected = true;
   while (!reader.compare(")") || bm.state == dfaQuote) {
      if (bm.state == dfaQuote && tokenExpected) {
         writer.writeString(reader.lookup(bm));

         tokenExpected = false;
      }
      else if (!tokenExpected && reader.compare("|")) {
         tokenExpected = true;
      }
      else throw SyntaxError("invalid grammar rule", bm.lineInfo);

      bm = reader.read();
   }
   writer.writeChar((char)0);

   mode = inversionMode ? EXCLUDE_MODE : MULTI_MODE;

   return position;
}

pos_t ScriptEngineCFParser :: writeBodyText(ustr_t text)
{
   MemoryWriter writer(&_body);

   pos_t position = writer.position();
   writer.writeString(text);

   return position;
}

ref_t ScriptEngineCFParser :: autonameRule(ref_t parentRuleId)
{
   IdentifierString ns;
   int   index = 0;
   do {
      ustr_t key = _names.retrieve<ref_t>(DEFAULT_STR, parentRuleId, [](ref_t reference, ustr_t key, ref_t current)
         {
            return current == reference;
         });

      ns.copy(key);
      ns.append('.');
      ns.appendHex(index++);

      if (!_names.exist(*ns))
         break;

   } while (true);

   return mapRuleId(*ns);
}

void ScriptEngineCFParser :: setScriptPtr(ScriptBookmark& bm, Rule& rule, bool prefixMode)
{
   if (prefixMode) {
      if (rule.postfix1Ptr)
         throw SyntaxError("invalid grammar rule", bm.lineInfo);

      rule.postfix1Ptr = _body.length();
   }
   else {
      if (rule.postfix1Ptr)
         throw SyntaxError("invalid grammar rule", bm.lineInfo);

      rule.postfix2Ptr = _body.length();
   }
}

ScriptEngineCFParser::SaverBase* ScriptEngineCFParser::createRegEx(ScriptBookmark& bm, ustr_t regex)
{
   auto regEx = RegExFactory::generate(regex);
   if (!regEx) {
      throw SyntaxError("invalid grammar rule", bm.lineInfo);
   }

   _regExList.add(regEx);

   return regEx;
}

void ScriptEngineCFParser :: defineApplyRule(Rule& rule, int mode, RuleTypeModifier modifier)
{
   if (rule.type == RuleType::Normal) {
      if (rule.terminal == 0) {
         rule.type = RuleType::Nonterminal;

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
               rule.type = RuleType::Eps;
               break;
            case IFNOT_MODE:
               rule.apply = excludeSpecialApplyRule;
               rule.nonterminal = 0;
               rule.type = RuleType::Eps;
               break;
            case IF_MODE:
               rule.apply = normalApplyRule;
               rule.nonterminal = 0;
               rule.type = RuleType::Eps;
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
            case REGEX_MODE:
               rule.apply = regexApplyRule;
               break;
            default:
               rule.apply = normalApplyRule;
               break;
         }
      }
   }
   else if (rule.type == RuleType::Chomski) {
      rule.apply = nonterminalApplyRule;
   }

   switch (modifier) {
      case RuleTypeModifier::ForwardMode:
         rule.type = (RuleType)(rule.type | RuleType::WithForward);
         break;
      case RuleTypeModifier::BookmarkMode:
         rule.type = (RuleType)(rule.type | RuleType::WithBookmark);
         break;
      case RuleTypeModifier::BufferMode:
         rule.type = (RuleType)(rule.type | RuleType::WithBufferOutput);
         break;
      default:
         break;
   }
}

void ScriptEngineCFParser :: saveScript(ScriptEngineReaderBase& reader, Rule& rule, int& mode, bool forwardMode)
{
   bool prefixMode = false;
   if (rule.terminal == 0 && rule.nonterminal == 0) {
      prefixMode = true;
      rule.prefix1Ptr = _body.length();
   }
   else rule.prefix2Ptr = _body.length();

   MemoryWriter writer(&_body);
   ScriptBookmark bm = reader.read();
   while (!reader.compare("=>") || bm.state == dfaQuote) {
      if (bm.state == dfaPrivate) {
         if (forwardMode)
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         if (rule.saver != nullptr || (!prefixMode && !reader.compare(CURRENT_KEYWORD)))
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         if (reader.compare(REFERENCE_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saver = ReferenceSaver::getInstance();

            mode = REFERENCE_MODE;
         }
         else if (reader.compare(IDENTIFIER_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saver = ReferenceSaver::getInstance();

            mode = IDENTIFIER_MODE;
         }
         else if (reader.compare(CHARACTER_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saver = LiteralContentSaver::getInstance();

            mode = CHARACTER_MODE;
         }
         else if (reader.compare(LITERAL_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saver = LiteralContentSaver::getInstance();

            mode = LITERAL_MODE;
         }
         else if (reader.compare(NUMERIC_KEYWORD)) {
            rule.terminal = INVALID_REF;
            rule.saver = ReferenceSaver::getInstance();

            mode = NUMERIC_MODE;
         }
         if (reader.compare(CURRENT_KEYWORD)) {
            rule.saver = ReferenceSaver::getInstance();
         }
         else if (reader.compare(BUFFER_KEYWORD)) {
            rule.saver = BufferSaver::getInstance();
         }
         else if (reader.compare(REGEX_KEYWORD)) {
            bm = reader.read();

            rule.terminal = INVALID_REF;
            rule.saver = createRegEx(bm, reader.lookup(bm));

            mode = REGEX_MODE;
         }

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(REFERENCE_KEYWORD)) {
         if (forwardMode)
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         rule.terminal = INVALID_REF;
         rule.saver = LiteralSaver::getInstance();

         mode = REFERENCE_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(IDENTIFIER_KEYWORD)) {
         if (forwardMode)
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         rule.terminal = INVALID_REF;
         rule.saver = LiteralSaver::getInstance();

         mode = IDENTIFIER_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(LITERAL_KEYWORD)) {
         if (forwardMode)
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         rule.terminal = INVALID_REF;
         rule.saver = LiteralSaver::getInstance();

         mode = LITERAL_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(NUMERIC_KEYWORD)) {
         if (forwardMode)
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         rule.terminal = INVALID_REF;
         rule.saver = LiteralSaver::getInstance();

         mode = NUMERIC_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else if (bm.state == dfaQuote && reader.compare(CHARACTER_KEYWORD)) {
         if (forwardMode)
            throw SyntaxError("invalid grammar rule", bm.lineInfo);

         rule.terminal = INVALID_REF;
         rule.saver = LiteralSaver::getInstance();

         mode = CHARACTER_MODE;

         writer.writeChar((char)0);
         setScriptPtr(bm, rule, prefixMode);
      }
      else {
         ustr_t token = reader.lookup(bm);

         if (bm.state == dfaQuote) {
            ScriptEngineLog::writeQuote(writer, token);
         }
         else writer.writeString(token, token.length_pos());
         writer.writeChar(' ');
      }

      bm = reader.read();
   }
   writer.writeChar((char)0);
}

pos_t ScriptEngineCFParser :: defineGrammarBrackets(ScriptEngineReaderBase& reader, ScriptBookmark& bm, ref_t parentRuleId)
{
   pos_t ruleId = autonameRule(parentRuleId);

   Rule rule;
   rule.type = RuleType::Normal;
   int applyMode = 0;

   bm = reader.read();
   while (!reader.compare("}")) {
      if (reader.compare("|")) {
         defineApplyRule(rule, applyMode, RuleTypeModifier::None);
         addRule(ruleId, rule);

         rule = Rule();
         rule.type = RuleType::Normal;
         applyMode = 0;

         bm = reader.read();
      }
      else defineGrammarRuleMember(reader, bm, rule, ruleId, applyMode);
   }

   defineApplyRule(rule, applyMode, RuleTypeModifier::None);

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

pos_t ScriptEngineCFParser :: defineGrammarRuleMember(ScriptEngineReaderBase& reader, ScriptBookmark& bm, ref_t parentRuleId, pos_t nonterminal, pos_t terminal)
{
   ref_t ruleId = autonameRule(parentRuleId);

   Rule rule;
   rule.type = RuleType::Normal;
   rule.nonterminal = nonterminal;
   rule.terminal = terminal;

   int applyMode = 0;
   defineGrammarRuleMember(reader, bm, rule, ruleId, applyMode);
   defineApplyRule(rule, applyMode, RuleTypeModifier::None);

   addRule(ruleId, rule);

   return ruleId;
}

pos_t ScriptEngineCFParser :: defineStarGrammarRule(ref_t parentRuleId, pos_t nonterminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define B -> AB
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = ruleId;
   recRule.type = RuleType::Chomski;
   defineApplyRule(recRule, 0, RuleTypeModifier::None);
   addRule(ruleId, recRule);

   // define B -> eps
   defineIdleGrammarRule(ruleId);

   return ruleId;
}

pos_t ScriptEngineCFParser :: definePlusGrammarRule(ref_t parentRuleId, pos_t nonterminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define B -> AB
   Rule rule;
   rule.nonterminal = nonterminal;
   rule.terminal = defineStarGrammarRule(ruleId, nonterminal);
   rule.type = RuleType::Chomski;
   defineApplyRule(rule, 0, RuleTypeModifier::None);
   addRule(ruleId, rule);

   return ruleId;
}

pos_t ScriptEngineCFParser :: defineOptionalGrammarRule(ref_t parentRuleId, pos_t nonterminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define B -> A
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = 0;
   recRule.type = RuleType::Normal;
   defineApplyRule(recRule, 0, RuleTypeModifier::None);
   addRule(ruleId, recRule);

   // define B -> eps
   defineIdleGrammarRule(ruleId);

   return ruleId;
}

pos_t ScriptEngineCFParser :: defineChomskiGrammarRule(ref_t parentRuleId, pos_t nonterminal, pos_t terminal)
{
   pos_t ruleId = autonameRule(parentRuleId);

   // define C -> AB
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = terminal;
   recRule.type = RuleType::Chomski;
   defineApplyRule(recRule, 0, RuleTypeModifier::None);
   addRule(ruleId, recRule);

   return ruleId;
}

void ScriptEngineCFParser :: defineIdleGrammarRule(ref_t ruleId)
{
   Rule rule;
   rule.nonterminal = INVALID_POS;
   rule.terminal = INVALID_POS;
   rule.type = RuleType::Normal;
   defineApplyRule(rule, IDLE_MODE, RuleTypeModifier::None);
   addRule(ruleId, rule);
}

void ScriptEngineCFParser :: defineGrammarRuleMemberPostfix(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId)
{
   bm = reader.read();
   if (bm.state == dfaQuote) {
   }
   else if (reader.compare("*") || reader.compare("+") || reader.compare("?")) {
      if (rule.terminal != 0 && (rule.type % RuleType::TypeMask) == RuleType::Chomski) {
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
      else throw SyntaxError("invalid grammar rule", bm.lineInfo);

      bm = reader.read();
   }
}

int ScriptEngineCFParser :: defineIfRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule)
{
   bm = reader.read();

   rule.terminal = INVALID_POS;

   int applyMode = IF_MODE;

   if (bm.state != dfaQuote) {
      rule.terminal = writeRegExprBodyText(reader, applyMode);
      if (applyMode == EXCLUDE_MODE)
         applyMode = IFNOT_MODE;
   }
   else rule.terminal = writeBodyText(reader.lookup(bm));

   return applyMode;
}

void ScriptEngineCFParser :: defineGrammarRuleMember(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId, int& applyMode)
{
   if (bm.state == dfaQuote || reader.compare("(")) {
      if (rule.terminal) {
         if (rule.nonterminal != 0) {
            if (rule.type == RuleType::Chomski) {
               rule.terminal = defineGrammarRuleMember(reader, bm, ruleId, rule.terminal);
            }
            else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId, rule.nonterminal);
         }
         else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId);
      }
      else if (rule.nonterminal) {
         rule.type = RuleType::Chomski;
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
         rule.type = RuleType::Chomski;

         rule.terminal = bracketRuleId;
      }
      else {
         if (rule.type == RuleType::Chomski) {
            rule.terminal = defineChomskiGrammarRule(ruleId, rule.terminal, bracketRuleId);
         }
         else rule.nonterminal = defineChomskiGrammarRule(ruleId, rule.nonterminal, bracketRuleId);
      }
   }
   else if (bm.state == dfaPrivate) {
      if (rule.terminal) {
         if (rule.type == RuleType::Chomski) {
            rule.terminal = defineGrammarRuleMember(reader, bm, ruleId, rule.terminal);
         }
         else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId, rule.nonterminal);
      }
      else if (rule.nonterminal) {
         rule.type = RuleType::Chomski;
         rule.terminal = defineGrammarRuleMember(reader, bm, ruleId);
      }
      else {
         rule.terminal = INVALID_POS;
         //            rule.prefixPtr = defineDSARule(token, reader);

         if (reader.compare(LITERAL_KEYWORD)) {
            applyMode = LITERAL_MODE;
         }
         else if (reader.compare(NUMERIC_KEYWORD)) {
            applyMode = NUMERIC_MODE;
         }
         else if (reader.compare(EPS_KEYWORD)) {
            if (rule.nonterminal)
               throw SyntaxError("invalid grammar rule", bm.lineInfo);

            rule.nonterminal = INVALID_POS;
            rule.terminal = INVALID_POS;
            applyMode = IDLE_MODE;
         }
         else if (reader.compare(IF_KEYWORD)) {
            if (rule.nonterminal)
               throw SyntaxError("invalid grammar rule", bm.lineInfo);

            applyMode = defineIfRule(reader, bm, rule);
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
            rule.type = RuleType::Chomski;

            rule.terminal = mapRuleId(reader.lookup(bm));
         }

         defineGrammarRuleMemberPostfix(reader, bm, rule, ruleId);
      }
      else {
         if (rule.type == RuleType::Chomski) {
            rule.terminal = defineGrammarRuleMember(reader, bm, ruleId, rule.terminal);
         }
         else rule.nonterminal = defineGrammarRuleMember(reader, bm, ruleId, rule.nonterminal);
      }
   }
   else throw SyntaxError("invalid grammar rule", bm.lineInfo);
}

inline pos_t writeDerivationItem(MemoryWriter& writer, int key, int terminal, int trace, bool forwardMode)
{
   if (forwardMode)
      key |= WITHFORWARD_MASK;

   pos_t offset = writer.position();
   writer.writeDWord(key);
   writer.writeDWord(terminal);
   writer.writeDWord(trace);

   return offset;
}

inline pos_t writeTrailItem(MemoryWriter& writer, pos_t nonterminal, pos_t next)
{
   pos_t offset = writer.position();
   writer.writeDWord(nonterminal);
   writer.writeDWord(next);

   return offset;
}

inline void readTailItemAndInsert(MemoryReader& reader, MemoryWriter& writer, ScriptEngineCFParser::DerivationQueue& queue, pos_t offset)
{
   pos_t nonterminal = reader.getDWord();
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
      queue.insert({ INVALID_POS, offset, 0 });
   }
   else queue.insert({ nonterminal, offset, next });
}

inline void readTailItemAndPush(MemoryReader& reader, MemoryWriter& writer, ScriptEngineCFParser::DerivationQueue& queue, pos_t offset)
{
   pos_t nonterminal = reader.getDWord();
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
      queue.push({ INVALID_POS, offset, 0 });
   }
   else queue.push({ nonterminal, offset, next });
}

void ScriptEngineCFParser :: predict(DerivationQueue& queue, DerivationItem item, ScriptEngineReaderBase& reader, ScriptBookmark& bm, pos_t terminalOffset, MemoryWriter& writer)
{
#ifdef TRACING_INFO
   ustr_t keyName = _names.retrieve<ref_t>(DEFAULT_STR, item.ruleId, [](ref_t reference, ustr_t key, ref_t current)
      {
         return current == reference;
      });
#endif

   pos_t key = createKey(item.ruleId, 1);
   Rule rule = _table.get(key);

   while (rule.type != RuleType::None) {
      int ruleType = rule.type & RuleType::TypeMask;

      if (rule.apply(rule, bm, reader, this)) {
         pos_t offset = writeDerivationItem(writer, key, terminalOffset, item.trace, testany(rule.type, RuleType::WithForward | RuleType::WithBookmark));
         pos_t next = writeTrailItem(writer, 0, item.next);

         if (ruleType == RuleType::Eps) {
            MemoryReader nextReader(&_body, (pos_t)next);
            readTailItemAndInsert(nextReader, writer, queue, offset);
         }
         else if (ruleType != RuleType::Normal) {
            // if it is a chomksi form
            if (ruleType == RuleType::Chomski) {
               next = writeTrailItem(writer, rule.terminal, next);
            }

            queue.insert({ rule.nonterminal, offset, next });
         }
         else if (rule.nonterminal == 0) {
            MemoryReader nextReader(&_body, (pos_t)next);
            readTailItemAndPush(nextReader, writer, queue, offset);
         }
         else queue.push({ rule.nonterminal, offset, next });
      }

      rule = _table.get(++key);
   }
}

pos_t ScriptEngineCFParser :: buildDerivationTree(ScriptEngineReaderBase& reader, ref_t startRuleId, MemoryWriter& writer)
{
   DerivationQueue predictions({});
   predictions.push(DerivationItem(startRuleId, 0, -1));

   ScriptBookmark bm;
   while (predictions.count() > 0) {
      //auto p_it = predictions.start();
      //while (!p_it.Eof()) {
      //   auto r = *p_it;

      //   ustr_t rName = retrieveKey(_names.start(), r.ruleId, DEFAULT_STR);
      //   if (getlength(rName) != 0) {
      //      printf("%s\n", rName.str());
      //   }
      //   else printf("?\n");

      //   p_it++;
      //}
      //printf("\n");

      predictions.push(DerivationItem(0));

      DerivationItem current = predictions.pop();
      if (reader.eof()) {
         if (current.ruleId == INVALID_POS) {
            return current.trace;
         }
         else throw SyntaxError("invalid syntax", bm.lineInfo);
      }

      bm = reader.read();
      pos_t terminalOffset = writer.position();
      writer.write(&bm, sizeof(ScriptBookmark));
      while (current.ruleId != 0) {
         if (current.ruleId == INVALID_POS) {
            return current.trace;
         }

         predict(predictions, current, reader, bm, terminalOffset, writer);

         current = predictions.pop();
      }
   }

   throw SyntaxError("invalid syntax", bm.lineInfo);
}

void ScriptEngineCFParser :: defineGrammarRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   rule.type = RuleType::Normal;
   int applyMode = 0;
   RuleTypeModifier modifier = RuleTypeModifier::None;

   while (!reader.compare(";") || bm.state == dfaQuote) {
      if (bm.state != dfaQuote && reader.compare("<=")) {
         saveScript(reader, rule, applyMode, modifier == RuleTypeModifier::ForwardMode);

         bm = reader.read();
      }
      else if (bm.state != dfaQuote && reader.compare("%")) {
         readToken(reader, bm, "<=");

         assert(modifier == RuleTypeModifier::None);
         modifier = RuleTypeModifier::BufferMode;

         saveScript(reader, rule, applyMode, false);

         rule.type = RuleType::Eps;
         rule.apply = nonterminalApplyRule;

         bm = reader.read();
      }
      else if (bm.state != dfaQuote && reader.compare("^")) {
         bm = reader.read();
         if (bm.state != dfaQuote && reader.compare("<=")) {
            assert(modifier == RuleTypeModifier::None);
            modifier = RuleTypeModifier::ForwardMode;

            saveScript(reader, rule, applyMode, true);

            bm = reader.read();
         }
         else throw SyntaxError("invalid grammar rule", bm.lineInfo);
      }
      else if (bm.state != dfaQuote && reader.compare("$")) {
         if (!rule.prefix1Ptr) {
            assert(modifier == RuleTypeModifier::None);
            modifier = RuleTypeModifier::BookmarkMode;
         }
         else throw SyntaxError("invalid grammar rule : cannot have prefix / postfix segments", bm.lineInfo);

         bm = reader.read();
      }
      else defineGrammarRuleMember(reader, bm, rule, ruleId, applyMode);
   }

   defineApplyRule(rule, applyMode, modifier);
}

void ScriptEngineCFParser :: addRule(int ruleId, Rule& rule)
{
   pos_t key = createKey(ruleId, 1);
   while (_table.exist(key))
      key++;

   _table.add(key, rule);
}

void ScriptEngineCFParser :: readScriptBookmark(pos_t ptr, ScriptBookmark& bm)
{
   MemoryReader reader(&_body, ptr);
   reader.read(&bm, sizeof(ScriptBookmark));
}

bool ScriptEngineCFParser :: parseGrammarRule(ScriptEngineReaderBase& reader)
{
   ScriptBookmark bm = reader.read();

   if (bm.state != dfaIdentifier)
      throw SyntaxError("invalid grammar rule", bm.lineInfo);

   ref_t ruleId = mapRuleId(reader.lookup(bm));

   reader.read();
   if (!reader.compare("::="))
      throw SyntaxError("invalid grammar rule", bm.lineInfo);

   bm = reader.read();

   Rule rule;
   defineGrammarRule(reader, bm, rule, ruleId);

   addRule(ruleId, rule);

   return true;
}

bool ScriptEngineCFParser :: compareToken(ScriptEngineReaderBase& reader, ScriptBookmark& bm, int rule)
{
   ustr_t terminal = reader.lookup(bm);
   ustr_t ruleTerminal = getBodyText(rule);

   return terminal.compare(ruleTerminal);
}

bool ScriptEngineCFParser :: compareTokenWithAny(ScriptEngineReaderBase& reader, ScriptBookmark& bm, int rule)
{
   ustr_t terminal = reader.lookup(bm);
   ustr_t ruleTerminal = getBodyText(rule);
   do {
      if (terminal.compare(ruleTerminal))
         return true;

      rule += getlength_int(ruleTerminal) + 1;
      ruleTerminal = getBodyText(rule);
   } while (!emptystr(ruleTerminal));

   return false;
}

void ScriptEngineCFParser :: insertForwards(Stack<Pair<int, int>>& forwards, int level, ScriptEngineLog& log)
{
   while (forwards.count() > 0 && forwards.peek().value2 <= level) {
      auto bm = forwards.pop();
      if (!bm.value1)
         break;

      log.write(getBodyText(bm.value1));
   }
}

bool ScriptEngineCFParser :: parseDirective(ScriptEngineReaderBase& reader, MemoryDump*)
{
   do {
      if (reader.compare(";")) {
         break;
      }
      else if (reader.compare("symbolic")) {
         _symbolMode = true;
      }
      else return false;

      reader.read();
   } while (true);

   return true;
}

inline void updateForwardLevel(Stack<Pair<int, int>>& forwards, int minLevel)
{
   for (auto it = forwards.start(); !it.eof(); ++it) {
      auto bm = *it;
      if (bm.value2 < minLevel)
         break;

      bm.value2 = minLevel;
      *it = bm;
   }
}

void ScriptEngineCFParser :: saveRuleOutputToBuffer(Rule& rule, pos_t terminal, ScriptEngineReaderBase& scriptReader)
{
   saveRuleOutput(rule, terminal, scriptReader, _buffer);
}

void ScriptEngineCFParser :: saveRuleOutput(Rule& rule, pos_t terminal, ScriptEngineReaderBase& scriptReader, ScriptEngineLog& log)
{
   pos_t lineLen = 0;
   if (!test(rule.type, RuleType::WithForward))
      lineLen += log.write(getBodyText(rule.prefix1Ptr));

   if (rule.saver != NULL)
      rule.saver->saveTo(scriptReader, this, terminal, log);

   // HOTFIX: to prevent too long line
   if (lineLen > 0x10) {
      log.write('\n');
   }
   else log.write(' ');

   log.write(getBodyText(rule.postfix1Ptr));
}

void ScriptEngineCFParser :: generateOutput(pos_t offset, ScriptEngineReaderBase& scriptReader, ScriptEngineLog& log)
{
   assert(!_generating);

   _generating = true;

   if (offset == 0)
      return;

   IdentifierString buffer;

   Stack<Pair<int, int>> bookmarks(Pair<int, int>(0, -1));
   Stack<Pair<int, int>> forwards(Pair<int, int>(0, -1));

   MemoryReader reader(&_body, (pos_t)offset);
   Stack<TraceItem> stack({ });
   TraceItem item = {};
   reader.read(&item, sizeof(TraceItem));
   int level = 0;
   bool forwardAvailable = false;
   while (true) {
      if (!item.ruleKey) {
         level++;
      }
      else level--;

      // if forward declaration
      if (test(item.ruleKey, WITHFORWARD_MASK)) {
#ifdef TRACING_INFO
         ustr_t keyName = _names.retrieve<ref_t>(DEFAULT_STR, (item.ruleKey & ~WITHFORWARD_MASK) >> 8, [](ref_t reference, ustr_t key, ref_t current)
            {
               return current == reference;
            });
#endif

         Rule rule = _table.get(item.ruleKey & ~WITHFORWARD_MASK);
         if (test(rule.type, RuleType::WithBookmark)) {
            bookmarks.push({ 0, level });
            while (forwards.count() > 0 && forwards.peek().value2 > level) {
               auto bm = forwards.pop();

               bookmarks.push({ bm.value1, level });
            }

            forwardAvailable = forwards.count() > 0;
         }
         else if (test(rule.type, RuleType::WithForward)) {
            forwardAvailable = true;

            forwards.push(Pair<int, int>(rule.prefix1Ptr, level));
         }
      }
      else if (forwardAvailable)
         updateForwardLevel(forwards, level);

      stack.push(item);

      if (item.previous == 0)
         break;

      reader.seek(item.previous);
      reader.read(&item, sizeof(TraceItem));
   }

   // NOTE: reset level to -1 to match the backward calculated levels 
   level = -1;
   Stack<pos_t> postfixes(0);
   Stack<SaverBase*> functions(nullptr);
   while (stack.count() > 0) {
      item = stack.pop();
      if (item.ruleKey == 0) {
         level--;

         pos_t ptr = postfixes.pop();
         if (ptr) {
            if (test(ptr, POSTFIXSAVE_MODE)) {
               ptr &= ~POSTFIXSAVE_MODE;

               log.write(getBodyText(ptr));

               auto saver = functions.pop();
               int terminal = postfixes.pop();

               saver->saveTo(scriptReader, this, terminal, log);

               ptr = postfixes.pop();
            }
            log.write(getBodyText(ptr));
         }
      }
      else {
         level++;

#ifdef TRACING_INFO
         for (int i = 0; i < level; i++)
            printf(" ");

         ustr_t keyName = _names.retrieve<ref_t>(DEFAULT_STR, (item.ruleKey & ~WITHFORWARD_MASK) >> 8, [](ref_t reference, ustr_t key, ref_t current)
            {
               return current == reference;
            });

         printf("%s\n", keyName.str());
#endif

         Rule rule = _table.get(item.ruleKey & ~WITHFORWARD_MASK);

         if (test(rule.type, RuleType::WithBookmark)) {
            insertForwards(bookmarks, level, log);
         }

         if (test(rule.type, RuleType::WithBufferOutput)) {
            saveRuleOutputToBuffer(rule, item.terminal, scriptReader);
         }
         else if (rule.prefix1Ptr != 0) {
            saveRuleOutput(rule, item.terminal, scriptReader, log);
         }

         if (rule.postfix2Ptr) {
            postfixes.push(rule.postfix2Ptr);

            postfixes.push(item.terminal);
            functions.push(rule.saver);
            postfixes.push(rule.prefix2Ptr | POSTFIXSAVE_MODE);
         }
         else postfixes.push(rule.prefix2Ptr);
      }

   }
   log.write((char)0);

   _generating = false;
}

void ScriptEngineCFParser :: parse(ScriptEngineReaderBase& reader, MemoryDump* output)
{
   if (_symbolMode)
      reader.turnSymbolMode();

   ScriptEngineLog log;

   ref_t startId = mapRuleId("start");
   MemoryWriter writer(&_body);

   pos_t trace = buildDerivationTree(reader, startId, writer);
   if (!reader.eof()) {
      ScriptBookmark bm = reader.read();
      if (bm.state != dfaEOF)
         throw SyntaxError("invalid syntax", bm.lineInfo);
   }

   generateOutput(trace, reader, log);

   IdentifierTextReader logReader((const char*)log.getBody());
   ScriptEngineReader scriptReader(&logReader, log.getCoordinateMap());

   _baseParser->parse(scriptReader, output);
}
