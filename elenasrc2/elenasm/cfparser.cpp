//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  :  ELENA VM Script Engine
//
//                                              (C)2011-2016, by Alexei Rakov
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
//#define SCOPE_KEYWORD         "$scope"
//#define VAR_KEYWORD           "$var"
//#define MAPPING_KEYWORD       "$newvar"
//#define IDLE_MAPPING_KEYWORD  "$new"
//#define ANY_KEYWORD           "$any"

#define REFERENCE_MODE        1
#define IDENTIFIER_MODE       2
#define LITERAL_MODE          3
#define NUMERIC_MODE          4
#define EOF_MODE              5
#define IDLE_MODE             6

//const char* dfaSymbolic[4] =
//{
//        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
//        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
//        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
//        "*********dd**d******************************************************************************************************************"
//};

// --- CFParser ---

inline size_t createKey(size_t id, int index)
{
   return (id << cnSyntaxPower) + index;
}

bool normalApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return bm.state != dfaEOF && parser->compareToken(reader, bm, rule.terminal);
}

bool normalEOFApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return (bm.state == dfaEOF);
}

bool normalReferenceApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return (bm.state == dfaFullIdentifier);
}

bool normalIdentifierApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return (bm.state == dfaIdentifier);
}

bool normalLiteralApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return (bm.state == dfaQuote);
}

bool nonterminalApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return true;
}

bool normalNumericApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return (bm.state == dfaInteger || bm.state == dfaLong || bm.state == dfaReal);
}

//bool scopeNonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   CFParser::Mapping* old = token.mapping;
//
//   CFParser::Mapping mapping;
//
//   token.mapping = &mapping;
//
//   bool ret = applyNonterminal(rule, token, reader);
//
//   token.mapping = old;
//
//   return ret;
//}
//
//bool scopeNonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   CFParser::Mapping* old = token.mapping;
//
//   CFParser::Mapping mapping;
//
//   token.mapping = &mapping;
//
//   bool ret = applyNonterminalDSA(rule, token, reader);
//
//   token.mapping = old;
//
//   if (ret) {
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token);
//
//      return true;
//   }
//   else return false;
//
//   return ret;
//}
//
//bool newNonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   token.mapping->add(NULL, token.mapping->Count() + 1);
//
//   return applyNonterminal(rule, token, reader);
//}
//
//bool newNonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   token.mapping->add(NULL, token.mapping->Count() + 1);
//
//   bool ret = applyNonterminalDSA(rule, token, reader);
//
//   if (ret) {
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token);
//
//      return true;
//   }
//   else return false;
//
//   return ret;
//}
//
//bool normalLiteralApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaQuote)
//      return false;
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token);
//
//   token.writeLog();
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token);
//
//   return true;
//}
//
//bool normalNumericApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
//      return false;
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token);
//
//   token.writeLog();
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token);
//
//   return true;
//}
//
//bool normalIdentifierApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaIdentifier)
//      return false;
//
//   if (rule.prefixPtr) {
//      rule.applyPrefixDSARule(token);
//
//      token.writeLog();
//   }   
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr) {
//      rule.applyPostfixDSARule(token);
//   }
//
//   return true;
//}
//
//bool variableApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.mapping == NULL || token.state != dfaIdentifier || !token.mapping->exist(token.value))
//      return false;
//
//   ident_c s[12];
//   StringHelper::intToStr(token.mapping->get(token.value), s, 10);
//   token.writeLog(s);
//
//   return apply(rule, token, reader);
//}
//
//bool variableApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.mapping == NULL || token.state != dfaIdentifier || !token.mapping->exist(token.value))
//      return false;
//
//   if (rule.prefixPtr) {
//      rule.applyPrefixDSARule(token);
//
//      ident_c s[12];
//      StringHelper::intToStr(token.mapping->get(token.value), s, 10);
//      token.writeLog(s);
//   }
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr) {
//      rule.applyPostfixDSARule(token);
//   }
//
//   return true;
//}
//
//
//bool mappingApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.mapping == NULL || token.state != dfaIdentifier)
//      return false;
//
//   token.mapping->add(token.value, token.mapping->Count() + 1);
//
//   if (!apply(rule, token, reader)) {
//      token.mapping->erase(token.value);
//
//      return false;
//   }
//   else return true;
//}
//
//bool mappingApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.mapping == NULL || token.state != dfaIdentifier)
//      return false;
//
//   if (rule.prefixPtr) {
//      rule.applyPrefixDSARule(token);
//   }
//
//   token.mapping->add(token.value, token.mapping->Count() + 1);
//
//   if (!apply(rule, token, reader)) {
//      token.mapping->erase(token.value);
//
//      return false;
//   }      
//
//   if (rule.postfixPtr) {
//      rule.applyPostfixDSARule(token);
//   }
//
//   return true;
//}
//
//bool normalReferenceApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaFullIdentifier)
//      return false;
//
//   if (rule.prefixPtr) {
//      rule.applyPrefixDSARule(token);
//   }
//
//   token.writeLog();
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   if (rule.postfixPtr) {
//      rule.applyPostfixDSARule(token);
//   }
//
//   return true;
//}
//
//bool normalEOFApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   return token.state == dfaEOF;
//}
//
//bool chomskiApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
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
//bool chomskiApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (applyNonterminalDSA(rule, token, reader)) {
//      // additional nonterminal should be resolved as well
//      if (!applyNonterminal(rule, token, reader, true))
//         return false;
//
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token);
//
//      return true;
//   }
//   else return false;
//}
//
//bool epsApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   return true;
//}
//
//bool epsApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token);
//
//   if (rule.postfixPtr)
//      rule.applyPostfixDSARule(token);
//
//   return true;
//}
//
////bool anyApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
////{
////   if(token.state == dfaEOF)
////      return false;
////
////   return apply(rule, token, reader);
////}
////
////bool anyApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
////{
////   if(token.state == dfaEOF)
////      return false;
////
////   if (rule.prefixPtr)
////      rule.applyPrefixDSARule(token);
////
////   if (apply(rule, token, reader)) {
////      if (rule.postfixPtr)
////         rule.applyPostfixDSARule(token);
////
////      return true;
////   }
////   else return false;
////}

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

ident_t CFParser :: getBodyText(size_t ptr)
{
   return (ident_t)_body.get(ptr);
}

bool CFParser :: compareToken(_ScriptReader& reader, ScriptBookmark& bm, int rule)
{
   ident_t terminal = reader.lookup(bm);
   ident_t ruleTerminal = getBodyText(rule);

   return StringHelper::compare(terminal, ruleTerminal);
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
            //      case rtScope:
            //         rule.apply = dsaRule ? scopeNonterminalApplyRuleDSA : scopeNonterminalApplyRule;
            //         break;
            //      case rtNewIdleVariable:
            //         rule.apply = dsaRule ? newNonterminalApplyRuleDSA : newNonterminalApplyRule;
            //         break;
            //      case rtVariable:
            //         rule.apply = dsaRule ? variableApplyRuleDSA : variableApplyRule;
            //         break;
            //      case rtNewVariable:
            //         rule.apply = dsaRule ? mappingApplyRuleDSA : mappingApplyRule;
            //         break;
            //      //case rtAny:
            //      //   rule.apply = dsaRule ?  anyApplyRuleDSA :  anyApplyRule;
            //      //   break;
            case IDLE_MODE:
               rule.apply = nonterminalApplyRule;
               rule.nonterminal = 0;
               rule.terminal = 0;
               rule.type = rtEps;
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

size_t CFParser :: defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, size_t parentRuleId, size_t nonterminal, size_t terminal)
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

   size_t ruleId = mapRuleId(ns);

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
      if (prefixMode && reader.compare(REFERENCE_KEYWORD)) {
         rule.terminal = -1;
         rule.saveTo = saveReference;

         mode = REFERENCE_MODE;
         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
      }
      else if (prefixMode && reader.compare(IDENTIFIER_KEYWORD)) {
         rule.terminal = -1;
         rule.saveTo = saveReference;

         mode = IDENTIFIER_MODE;
         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
      }
      else if (prefixMode && reader.compare(LITERAL_KEYWORD)) {
         rule.terminal = -1;
         rule.saveTo = saveLiteral;

         mode = LITERAL_MODE;
         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
      }
      else if (prefixMode && reader.compare(NUMERIC_KEYWORD)) {
         rule.terminal = -1;
         rule.saveTo = saveReference;

         mode = NUMERIC_MODE;
         writer.writeChar((char)0);
         rule.postfixPtr = _body.Length();
      }
      else {
         ident_t token = reader.lookup(bm);

         writer.writeLiteral(token, getlength(token));
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
            rule.nonterminal = defineGrammarRule(reader, bm, ruleId, rule.nonterminal);
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
            rule.terminal = -1;
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

               rule.nonterminal = -1;
               rule.terminal = -1;
               applyMode = IDLE_MODE;
            }
            else if (reader.compare(EOF_KEYWORD)) {
               applyMode = EOF_MODE;
            }
            else if (reader.compare(REFERENCE_KEYWORD)) {
               applyMode = REFERENCE_MODE;
            }
//            else if (StringHelper::compare(token.value, SCOPE_KEYWORD)) {
//               type = rtScope;
//            }
//            else if (StringHelper::compare(token.value, IDLE_MAPPING_KEYWORD)) {
//               type = rtNewIdleVariable;
//            }
//            else if (StringHelper::compare(token.value, VAR_KEYWORD)) {
//               type = rtVariable;
//            }
//            else if (StringHelper::compare(token.value, MAPPING_KEYWORD)) {
//               type = rtNewVariable;
//            }
//            //      else if (ConstantIdentifier::compare(token.value, ANY_KEYWORD)) {
//      //         type = rtAny;
//      //      }
            else if (reader.compare(IDENTIFIER_KEYWORD)) {
               applyMode = IDENTIFIER_MODE;
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

//void CFParser :: parseDirective(_ScriptReader& reader)
//{
//   const wchar16_t* token = reader.read();
//   while (token[0] != ';') {
//      if (ConstantIdentifier::compare(token, "symbolic")) {
//         _symbolMode = true;
//      }
//      else throw EParseError(reader.info.column, reader.info.row);
//
//      token = reader.read();
//   }
//}

inline int writeDerivationItem(MemoryWriter& writer, int key, int terminal, int trace)
{
   int offset = writer.Position();
   writer.writeDWord(key);
   writer.writeDWord(terminal);
   writer.writeDWord(trace);

   return offset;
}

void CFParser :: predict(DerivationQueue& queue, DerivationItem item, _ScriptReader& reader, ScriptBookmark& bm, int terminalOffset, MemoryWriter& writer)
{
   //ident_t keyName = retrieveKey(_names.start(), item.ruleId, DEFAULT_STR);

   size_t key = createKey(item.ruleId, 1);
   Rule rule = _table.get(key);
   while (rule.type != rtNone) {
      if (rule.apply(rule, bm, reader, this)) {
         int offset = writeDerivationItem(writer, key, terminalOffset, item.trace);

         if (rule.type == rtEps) {
            offset = writeDerivationItem(writer, 0, 0, offset);

            if (item.next == -1) {
               queue.insert(DerivationItem(-1, offset, 0));
            }
            else {
               MemoryReader reader(&_body, item.next);
               int nonterminal = reader.getDWord();
               if (nonterminal == 0) {
                  offset = writeDerivationItem(writer, 0, 0, offset);
                  nonterminal = reader.getDWord();
               }
               int next = reader.getDWord();

               queue.insert(DerivationItem(nonterminal, offset, next));
            }
         }
         else if (rule.type != rtNormal) {
            int next = item.next;
            // if it is a chomksi form
            if (rule.type == rtChomski) {
               int previous = next;
               next = writer.Position();
               writer.writeDWord(0);
               writer.writeDWord(rule.terminal);
               writer.writeDWord(previous);
            }

            queue.insert(DerivationItem(rule.nonterminal, offset, next));
         }
         else if (rule.nonterminal == 0) {
            offset = writeDerivationItem(writer, 0, 0, offset);            

            if (item.next == -1) {
               queue.push(DerivationItem(-1, offset, 0));
            }
            else if (item.next > 0) {
               MemoryReader reader(&_body, item.next);
               int nonterminal = reader.getDWord();
               if (nonterminal == 0) {
                  offset = writeDerivationItem(writer, 0, 0, offset);
                  nonterminal = reader.getDWord();
               }
               int next = reader.getDWord();

               queue.push(DerivationItem(nonterminal, offset, next));
            }
         }
         else queue.push(DerivationItem(rule.nonterminal, offset, item.next));
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

   MemoryReader reader(&_body, offset);
   Stack<TraceItem> stack(TraceItem(0));
   TraceItem item;
   reader.read(&item, sizeof(TraceItem));
   while (true) {
      stack.push(item);
      if (item.previous == 0)
         break;

      reader.seek(item.previous);
      reader.read(&item, sizeof(TraceItem));
   }

   Stack<size_t> postfixes;
   while (stack.Count() > 0) {
      TraceItem item = stack.pop();
      if (item.ruleKey == 0) {
         size_t ptr = postfixes.pop();
         if (ptr)
            log.write(getBodyText(ptr));
      }
      else {
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

void CFParser :: parse(_ScriptReader& reader, TapeWriter& tapeWriter)
{
   ScriptLog log;

   size_t startId = mapRuleId("start");
   while (!reader.Eof()) {
      MemoryWriter writer(&_body);
      int presaved = writer.Position();

      int trace = buildDerivationTree(reader, startId, writer);
      generateOutput(trace, reader, log);

      IdentifierTextReader logReader((ident_t)log.getBody());
      ScriptReader scriptReader(&logReader);
      
      _baseParser->parse(scriptReader, tapeWriter);

      _body.trim(presaved);
      log.clear();
   }
}
