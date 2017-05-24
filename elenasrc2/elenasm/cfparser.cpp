//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  :  ELENA VM Script Engine
//
//                                              (C)2011-2017, by Alexei Rakov
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

#define REFERENCE_MODE        1
#define IDENTIFIER_MODE       2
#define LITERAL_MODE          3
#define NUMERIC_MODE          4
#define EOF_MODE              5
#define IDLE_MODE             6
#define EOL_MODE              7
#define LETTER_MODE           8

const char* dfaSymbolic[4] =
{
        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
        "*********dd**d******************************************************************************************************************"
};

// --- CFParser ---

inline size_t createKey(size_t id, int index)
{
   return (id << cnSyntaxPower) + index;
}

bool normalApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return bm.state != dfaEOF && parser->compareToken(reader, bm, rule.terminal);
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

   log.write(scriptReader.lookup(bm));
}

void saveLiteral(_ScriptReader& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log)
{
   ScriptBookmark bm;
   parser->readScriptBookmark(ptr, bm);

   log.writeQuote(scriptReader.lookup(bm));
}

void saveLiteralContent(_ScriptReader& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log)
{
   ScriptBookmark bm;
   parser->readScriptBookmark(ptr, bm);

   log.write(scriptReader.lookup(bm));
}

void CFParser :: readScriptBookmark(size_t ptr, ScriptBookmark& bm)
{
   MemoryReader reader(&_body, ptr);
   reader.read(&bm, sizeof(ScriptBookmark));
}

size_t CFParser :: writeBodyText(ident_t text)
{
   MemoryWriter writer(&_body);

   size_t position = writer.Position();
   writer.writeLiteral(text);

   return position;
}

const char* CFParser :: getBodyText(size_t ptr)
{
   return (const char*)_body.get(ptr);
}

bool CFParser :: compareToken(_ScriptReader& reader, ScriptBookmark& bm, int rule)
{
   ident_t terminal = reader.lookup(bm);
   ident_t ruleTerminal = getBodyText(rule);

   return terminal.compare(ruleTerminal);
}

void CFParser :: defineApplyRule(Rule& rule, int mode)
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
            default:
               rule.apply = normalApplyRule;
               break;
         }
      }
   }
   else if (rule.type == rtChomski) {
      rule.apply = nonterminalApplyRule;
   }      

}

void CFParser :: addRule(int ruleId, Rule& rule)
{
   size_t key = createKey(ruleId, 1);
   while (_table.exist(key))
      key++;

   _table.add(key, rule);
}

void CFParser :: defineIdleGrammarRule(ref_t ruleId)
{
   Rule rule;
   rule.nonterminal = (size_t)-1;
   rule.terminal = (size_t)-1;
   rule.type = rtNormal;
   defineApplyRule(rule, IDLE_MODE);
   addRule(ruleId, rule);
}

size_t CFParser :: defineStarGrammarRule(ref_t parentRuleId, size_t nonterminal)
{
   size_t ruleId = autonameRule(parentRuleId);

   // define B -> AB
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = ruleId;
   recRule.type = rtChomski;
   defineApplyRule(recRule, 0);
   addRule(ruleId, recRule);

   // define B -> eps
   defineIdleGrammarRule(ruleId);

   return ruleId;
}

size_t CFParser :: defineOptionalGrammarRule(ref_t parentRuleId, size_t nonterminal)
{
   size_t ruleId = autonameRule(parentRuleId);

   // define B -> A
   Rule recRule;
   recRule.nonterminal = nonterminal;
   recRule.terminal = 0;
   recRule.type = rtNormal;
   defineApplyRule(recRule, 0);
   addRule(ruleId, recRule);

   // define B -> eps
   defineIdleGrammarRule(ruleId);

   return ruleId;
}

size_t CFParser :: definePlusGrammarRule(ref_t parentRuleId, size_t nonterminal)
{
   size_t ruleId = autonameRule(parentRuleId);

   // define B -> AB
   Rule rule;
   rule.nonterminal = nonterminal;
   rule.terminal = defineStarGrammarRule(ruleId, nonterminal);
   rule.type = rtChomski;
   defineApplyRule(rule, 0);
   addRule(ruleId, rule);

   return ruleId;
}

size_t CFParser :: autonameRule(size_t parentRuleId)
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

size_t CFParser :: defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, size_t parentRuleId, size_t nonterminal, size_t terminal)
{
   size_t ruleId = autonameRule(parentRuleId);

   Rule rule;
   rule.nonterminal = nonterminal;
   rule.terminal = terminal;

   defineGrammarRule(reader, bm, rule, ruleId);

   addRule(ruleId, rule);

   return ruleId;
}

void CFParser :: saveScript(_ScriptReader& reader, Rule& rule, int& mode)
{
   bool prefixMode = false;
   if (rule.terminal == 0 && rule.nonterminal == 0) {
      prefixMode = true;
      rule.prefixPtr = _body.Length();
   }
   else rule.postfixPtr = _body.Length();

   MemoryWriter writer(&_body);
   ScriptBookmark bm = reader.read();
   while (!reader.compare("=>") || bm.state == dfaQuote) {
      if (bm.state == dfaPrivate) {
         if (rule.saveTo != NULL || !prefixMode)
            throw EParseError(bm.column, bm.row);

         if (reader.compare(REFERENCE_KEYWORD)) {
            rule.terminal = (size_t)-1;
            rule.saveTo = saveReference;

            mode = REFERENCE_MODE;
         }
         else if (reader.compare(IDENTIFIER_KEYWORD)) {
            rule.terminal = (size_t)-1;
            rule.saveTo = saveReference;

            mode = IDENTIFIER_MODE;
         }
         else if (reader.compare(LITERAL_KEYWORD)) {
            rule.terminal = (size_t)-1;
            rule.saveTo = saveLiteralContent;

            mode = LITERAL_MODE;
         }
         else if (reader.compare(NUMERIC_KEYWORD)) {
            rule.terminal = (size_t)-1;
            rule.saveTo = saveReference;

            mode = NUMERIC_MODE;
         }
         if (reader.compare(CURRENT_KEYWORD)) {
            rule.saveTo = saveReference;
         }

         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
         prefixMode = false;
      }
      else if (bm.state == dfaQuote && reader.compare(REFERENCE_KEYWORD)) {
         rule.terminal = (size_t)-1;
         rule.saveTo = saveLiteral;

         mode = REFERENCE_MODE;

         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
         prefixMode = false;
      }
      else if (bm.state == dfaQuote && reader.compare(IDENTIFIER_KEYWORD)) {
         rule.terminal = (size_t)-1;
         rule.saveTo = saveLiteral;

         mode = IDENTIFIER_MODE;

         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
         prefixMode = false;
      }
      else if (bm.state == dfaQuote && reader.compare(LITERAL_KEYWORD)) {
         rule.terminal = (size_t)-1;
         rule.saveTo = saveLiteral;

         mode = LITERAL_MODE;

         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
         prefixMode = false;
      }
      else if (bm.state == dfaQuote && reader.compare(NUMERIC_KEYWORD)) {
         rule.terminal = (size_t)-1;
         rule.saveTo = saveLiteral;

         mode = NUMERIC_MODE;

         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
         prefixMode = false;
      }
      else {
         if (prefixMode && rule.saveTo != 0) {
            throw EParseError(bm.column, bm.row);
         }

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

void CFParser :: defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   rule.type = rtNormal;
   int applyMode = 0;

   while (!reader.compare(";") || bm.state == dfaQuote) {
      if (bm.state == dfaQuote) {
         if (rule.terminal) {
            if (rule.nonterminal != 0) {
               if (rule.type == rtChomski) {
                  rule.terminal = defineGrammarRule(reader, bm, ruleId, rule.terminal);
               }
               else rule.nonterminal = defineGrammarRule(reader, bm, ruleId, rule.nonterminal);
            }
            else rule.nonterminal = defineGrammarRule(reader, bm, ruleId);
            break;
         }
         else if (rule.nonterminal) {
            rule.type = rtChomski;
            rule.terminal = defineGrammarRule(reader, bm, ruleId);
            break;
         }
         else rule.terminal = writeBodyText(reader.lookup(bm));
      }
      else if (reader.compare("<=")) {
         saveScript(reader, rule, applyMode);
      }
      else if (reader.compare("*") || reader.compare("+") || reader.compare("?")) {
         if (rule.terminal != 0 && rule.type == rtChomski) {            
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
      }
      else if (bm.state == dfaPrivate) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(reader, bm, ruleId, rule.nonterminal);
            break;
         }
         else if (rule.nonterminal) {
            rule.type = rtChomski;
            rule.terminal = defineGrammarRule(reader, bm, ruleId);
            break;
         }
         else {
            rule.terminal = (size_t)-1;
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

               rule.nonterminal = (size_t)-1;
               rule.terminal = (size_t)-1;
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
         }
      }
      else if (bm.state == dfaIdentifier) {
         if (rule.nonterminal == 0) {
            //rule.prefixPtr = defineDSARule(token, reader);

            rule.nonterminal = mapRuleId(reader.lookup(bm));
         }
         else if (rule.terminal == 0) {
            rule.type = rtChomski;

            rule.terminal = mapRuleId(reader.lookup(bm));
         }
         else {
            if (rule.type == rtChomski) {
               rule.terminal = defineGrammarRule(reader, bm, ruleId, rule.terminal);
            }
            else rule.nonterminal = defineGrammarRule(reader, bm, ruleId, rule.nonterminal);
            break;
         }
      }

      bm = reader.read();
   }

//   rule.postfixPtr = defineDSARule(token, reader);

   defineApplyRule(rule, applyMode);
}

bool CFParser :: parseGrammarRule(_ScriptReader& reader)
{
   ScriptBookmark bm = reader.read();

   if (bm.state != dfaIdentifier)
      throw EParseError(bm.column, bm.row);

   size_t ruleId = mapRuleId(reader.lookup(bm));

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

inline int writeDerivationItem(MemoryWriter& writer, int key, int terminal, int trace)
{
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
      offset = writeDerivationItem(writer, 0, 0, offset);
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
      offset = writeDerivationItem(writer, 0, 0, offset);
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

   size_t key = createKey(item.ruleId, 1);
   Rule rule = _table.get(key);
   while (rule.type != rtNone) {
      if (rule.apply(rule, bm, reader, this)) {
         int offset = writeDerivationItem(writer, key, terminalOffset, item.trace);
         int next = writeTrailItem(writer, 0, item.next);

         if (rule.type == rtEps) {
            MemoryReader nextReader(&_body, (pos_t)next);
            readTailItemAndInsert(nextReader, writer, queue, offset);
         }
         else if (rule.type != rtNormal) {
            // if it is a chomksi form
            if (rule.type == rtChomski) {
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

int CFParser :: buildDerivationTree(_ScriptReader& reader, size_t startRuleId, MemoryWriter& writer)
{
   DerivationQueue predictions;
   predictions.push(DerivationItem(startRuleId, 0, -1));

   ScriptBookmark bm;
   while (predictions.Count() > 0) {
      predictions.push(DerivationItem(0));

      bm = reader.read();
      int terminalOffset = writer.Position();
      writer.write(&bm, sizeof(ScriptBookmark));

      DerivationItem current = predictions.pop();
      
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

void CFParser :: generateOutput(int offset, _ScriptReader& scriptReader, ScriptLog& log)
{
   if (offset == 0)
      return;

   MemoryReader reader(&_body, (pos_t)offset);
   Stack<TraceItem> stack(TraceItem(0));
   TraceItem item;
   reader.read(&item, sizeof(TraceItem));
   while (true) {
      stack.push(item);
      if (item.previous == 0)
         break;

      reader.seek((pos_t)item.previous);
      reader.read(&item, sizeof(TraceItem));
   }

   Stack<size_t> postfixes;
   while (stack.Count() > 0) {
      item = stack.pop();
      if (item.ruleKey == 0) {
         size_t ptr = postfixes.pop();
         if (ptr)
            log.write(getBodyText(ptr));
      }
      else {
         ident_t keyName = retrieveKey(_names.start(), item.ruleKey >> 8, DEFAULT_STR);

         Rule rule = _table.get(item.ruleKey);
         if (rule.prefixPtr != 0) {
            log.write(getBodyText(rule.prefixPtr));

            if (rule.saveTo != NULL)
               rule.saveTo(scriptReader, this, item.terminal, log);

            // HOTFIX: to prevent too long line
            log.write('\n');
         }

         postfixes.push(rule.postfixPtr);
      }

   }
   log.write((char)0);
}

void CFParser :: parse(_ScriptReader& reader, MemoryDump* output)
{
   if (_symbolMode)
      reader.switchDFA(dfaSymbolic);

   ScriptLog log;

   size_t startId = mapRuleId("start");
   MemoryWriter writer(&_body);

   int trace = buildDerivationTree(reader, startId, writer);
   if (!reader.Eof()) {
      ScriptBookmark bm = reader.read();
      if (bm.state != dfaEOF)
         throw EParseError(bm.column, bm.row);
   }

   generateOutput(trace, reader, log);

   IdentifierTextReader logReader((const char*)log.getBody());
   ScriptReader scriptReader(&logReader);
      
   _baseParser->parse(scriptReader, output);

}
