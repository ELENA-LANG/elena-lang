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
//#define IDENTIFIER_KEYWORD    "$identifier"
//#define LITERAL_KEYWORD       "$literal"
//#define NUMERIC_KEYWORD       "$numeric"
//#define EPS_KEYWORD           "$eps"
#define EOF_KEYWORD           "$eof"
//#define SCOPE_KEYWORD         "$scope"
//#define VAR_KEYWORD           "$var"
//#define MAPPING_KEYWORD       "$newvar"
//#define IDLE_MAPPING_KEYWORD  "$new"
////#define ANY_KEYWORD           "$any"

#define REFERENCE_MODE        1
#define EOF_MODE              2

#define HINT_TERMINAL         0x80000000

////const char* dfaSymbolic[4] =
////{
////        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
////        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
////        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
////        "*********dd**d******************************************************************************************************************"
////};

// --- CFParser ---

inline size_t createKey(size_t id, int index)
{
   return (id << cnSyntaxPower) + index;
}

//inline bool apply(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   token.read(reader);
//   if (rule.nonterminal != 0) {
//      if(!token.parser->applyRule(rule.nonterminal, token, reader))
//         return false;
//   }
//
//   return true;
//}
//
//inline bool applyNonterminal(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader, bool chomski = false)
//{
//   if(!token.parser->applyRule(chomski ? rule.terminal : rule.nonterminal, token, reader)) {
//      return false;
//   }
//
//   return true;
//}
//
//inline bool applyNonterminalDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader, bool chomski = false)
//{
//   rule.applyPrefixDSARule(token);
//
//   if(!token.parser->applyRule(chomski ? rule.terminal : rule.nonterminal, token, reader)) {
//      return false;
//   }
//
//   return true;
//}

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

bool nonterminalApplyRule(CFParser::Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser)
{
   return true;
}

//bool normalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state == dfaEOF || !token.compare(rule.terminal))
//      return false;
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token);
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
//bool nonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if(applyNonterminal(rule, token, reader)) {
//      return true;
//   }
//   else return false;
//}
//
//bool nonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if(applyNonterminalDSA(rule, token, reader)) {
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token);
//
//      return true;
//   }
//   else return false;
//}
//
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
//bool normalLiteralApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaQuote)
//      return false;
//
//   token.writeLog();
//
//   if (!apply(rule, token, reader))
//      return false;
//
//   return true;
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
//bool normalNumericApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
//      return false;
//
//   token.writeLog();
//
//   if (!apply(rule, token, reader))
//      return false;
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
//bool normalIdentifierApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if (token.state != dfaIdentifier)
//      return false;
//
//   token.writeLog();
//
//   return apply(rule, token, reader);
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
            //      case rtLiteral:
            //         rule.apply = dsaRule ? normalLiteralApplyRuleDSA : normalLiteralApplyRule;
            //         break;
            //      case rtNumeric:
            //         rule.apply = dsaRule ? normalNumericApplyRuleDSA : normalNumericApplyRule;
            //         break;
            case REFERENCE_MODE:
               rule.apply = normalReferenceApplyRule;
               break;
            //      case rtIdentifier:
            //         rule.apply = dsaRule ?  normalIdentifierApplyRuleDSA :  normalIdentifierApplyRule;
            //         break;
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
            //      case rtEps:
            //         rule.apply = dsaRule ? epsApplyRuleDSA : epsApplyRule;
            //         break;
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

//bool CFParser :: applyRule(size_t ruleId, TokenInfo& token, _ScriptReader& reader)
//{
//   ident_t name = retrieveKey(_names.start(), ruleId, DEFAULT_STR);
//
//   size_t readerRollback = reader.Position();
//   size_t coderRollback = token.LogPosition();
//
//   RuleMap::Iterator it = _rules.getIt(ruleId);
//   while(!it.Eof()) {
//      CFParser::TokenInfo tokenCopy(token);
//
//      if ((*it).apply(*it, tokenCopy, reader)) {
//         tokenCopy.save(token);
//
//         return true;
//      }
//
//      // roll back
//      reader.seek(readerRollback);
//      token.trimLog(coderRollback);
//
//      it = _rules.getNextIt(ruleId, it);
//   }
//
//   return false;
//}
//
//bool CFParser :: applyRule(Rule& rule, TokenInfo& token, _ScriptReader& reader)
//{
//   if (rule.apply(rule, token, reader)) {
//      return true;
//   }
//   else return false;
//}
//
//size_t CFParser :: defineDSARule(TokenInfo& token, _ScriptReader& reader)
//{
//   ident_t s = token.getLog();
//   if (!emptystr(s)) {
//      MemoryWriter writer(&_body);
//
//      size_t ptr = writer.Position();
//
//      writer.writeLiteral(s);
//
//      token.clearLog();
//
//      return ptr;
//   }
//   else return 0;
//}

void CFParser :: addRule(int ruleId, Rule& rule)
{
   size_t key = createKey(ruleId, 1);
   while (_table.exist(key))
      key++;

   _table.add(key, rule);
}

size_t CFParser :: defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, size_t nonterminal)
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
   rule.nonterminal = nonterminal;

   defineGrammarRule(reader, bm, rule);

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
         rule.prefixPtr |= HINT_TERMINAL;
         rule.terminal = -1;

         mode = REFERENCE_MODE;
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

void CFParser :: defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   rule.type = rtNormal;
   int applyMode = 0;

   while (!reader.compare(";") || bm.state == dfaQuote) {
      if (bm.state == dfaQuote) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(reader, bm);
            break;
         }
         else rule.terminal = writeBodyText(reader.lookup(bm));
      }
      else if (reader.compare("<=")) {
         saveScript(reader, rule, applyMode);
      }
      else if (bm.state == dfaPrivate) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(reader, bm);
            break;
         }
         else {
            rule.terminal = -1;
//            rule.prefixPtr = defineDSARule(token, reader);
//
//            if (StringHelper::compare(token.value, LITERAL_KEYWORD)) {
//               type = rtLiteral;
//            }
//            else if (StringHelper::compare(token.value, NUMERIC_KEYWORD)) {
//               type = rtNumeric;
//            }
//            else if (StringHelper::compare(token.value, EPS_KEYWORD)) {
//               type = rtEps;
//            }
            /*else */if (reader.compare(EOF_KEYWORD)) {
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
//            else if (StringHelper::compare(token.value, IDENTIFIER_KEYWORD)) {
//               type = rtIdentifier;
//            }
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
               rule.terminal = defineGrammarRule(reader, bm, rule.terminal);
            }
            else rule.nonterminal = defineGrammarRule(reader, bm, rule.nonterminal);
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
//   ScriptLog log;
//   TokenInfo token(this, &log);

   ScriptBookmark bm = reader.read();

   if (bm.state != dfaIdentifier)
      throw EParseError(bm.column, bm.row);

   size_t ruleId = mapRuleId(reader.lookup(bm));

   reader.read();
   if (!reader.compare("::="))
      throw EParseError(bm.column, bm.row);

   bm = reader.read();

   Rule rule;
   defineGrammarRule(reader, bm, rule);

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

void CFParser :: predict(DerivationQueue& queue, DerivationItem item, _ScriptReader& reader, ScriptBookmark& bm, int terminalOffset, MemoryWriter& writer)
{
   size_t key = createKey(item.ruleId, 1);
   Rule rule = _table.get(key);
   while (rule.type != rtNone) {
      if (rule.apply(rule, bm, reader, this)) {
         int offset = writer.Position();
         writer.writeDWord(key);
         writer.writeDWord(terminalOffset);
         writer.writeDWord(item.trace);

         if (rule.type != rtNormal) {
            int next = item.next;
            // if it is a chomksi form
            if (rule.type == rtChomski) {
               int previous = next;
               next = writer.Position();
               writer.writeDWord(rule.terminal);
               writer.writeDWord(previous);
            }

            queue.insert(DerivationItem(rule.nonterminal, offset, next));
         }
         else if (rule.nonterminal == 0) {
            if (item.next == -1) {
               queue.push(DerivationItem(-1, offset, 0));
            }
            else if (item.next > 0) {
               MemoryReader reader(&_body, item.next);
               int nonterminal = reader.getDWord();
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
      if (current.ruleId == -1) {
         return current.trace;
      }
      
      while (current.ruleId != 0) {
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

      Rule rule = _table.get(item.ruleKey);
      if (rule.prefixPtr != 0) {
         if (test(rule.prefixPtr, HINT_TERMINAL)) {
            log.write(getBodyText(rule.prefixPtr & ~HINT_TERMINAL));

            ScriptBookmark bm;
            reader.seek(item.terminal);
            reader.read(&bm, sizeof(ScriptBookmark));

            log.write(scriptReader.lookup(bm));
         }
         else log.write(getBodyText(rule.prefixPtr));

         // HOTFIX: to prevent too long line
         log.write('\n');
      }

      if (rule.postfixPtr != 0)
         postfixes.push(rule.postfixPtr);
   }

   while (postfixes.Count() > 0) {
      size_t ptr = postfixes.pop();

      log.write(getBodyText(ptr));
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
