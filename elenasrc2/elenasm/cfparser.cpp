//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  :  ELENA VM Script Engine
//
//                                              (C)2011-2015, by Alexei Rakov
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
#define SCOPE_KEYWORD         "$scope"
#define VAR_KEYWORD           "$var"
#define MAPPING_KEYWORD       "$newvar"
#define IDLE_MAPPING_KEYWORD  "$new"
//#define ANY_KEYWORD           "$any"

//const char* dfaSymbolic[4] =
//{
//        ".????????dd??d??????????????????bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
//        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
//        "????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????",
//        "*********dd**d******************************************************************************************************************"
//};

// --- CFParser ---

inline bool apply(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   token.read(reader);
   if (rule.nonterminal != 0) {
      if(!token.parser->applyRule(rule.nonterminal, token, reader))
         return false;
   }

   return true;
}

inline bool applyNonterminal(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader, bool chomski = false)
{
   if(!token.parser->applyRule(chomski ? rule.terminal : rule.nonterminal, token, reader)) {
      return false;
   }

   return true;
}

inline bool applyNonterminalDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader, bool chomski = false)
{
   rule.applyPrefixDSARule(token);

   if(!token.parser->applyRule(chomski ? rule.terminal : rule.nonterminal, token, reader)) {
      return false;
   }

   return true;
}

bool normalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state == dfaEOF || !token.compare(rule.terminal))
      return false;

   return apply(rule, token, reader);
}

bool normalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
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

bool nonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if(applyNonterminal(rule, token, reader)) {
      return true;
   }
   else return false;
}

bool nonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if(applyNonterminalDSA(rule, token, reader)) {
      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token);

      return true;
   }
   else return false;
}

bool scopeNonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   CFParser::Mapping* old = token.mapping;

   CFParser::Mapping mapping;

   token.mapping = &mapping;

   bool ret = applyNonterminal(rule, token, reader);

   token.mapping = old;

   return ret;
}

bool scopeNonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   CFParser::Mapping* old = token.mapping;

   CFParser::Mapping mapping;

   token.mapping = &mapping;

   bool ret = applyNonterminalDSA(rule, token, reader);

   token.mapping = old;

   if (ret) {
      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token);

      return true;
   }
   else return false;

   return ret;
}

bool newNonterminalApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   token.mapping->add(NULL, token.mapping->Count() + 1);

   return applyNonterminal(rule, token, reader);
}

bool newNonterminalApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   token.mapping->add(NULL, token.mapping->Count() + 1);

   bool ret = applyNonterminalDSA(rule, token, reader);

   if (ret) {
      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token);

      return true;
   }
   else return false;

   return ret;
}

bool normalLiteralApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaQuote)
      return false;

   token.writeLog();

   if (!apply(rule, token, reader))
      return false;

   return true;
}

bool normalLiteralApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
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

bool normalNumericApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
      return false;

   token.writeLog();

   if (!apply(rule, token, reader))
      return false;

   return true;
}

bool normalNumericApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaInteger && token.state != dfaLong && token.state != dfaReal)
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

bool normalIdentifierApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaIdentifier)
      return false;

   token.writeLog();

   return apply(rule, token, reader);
}

bool normalIdentifierApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaIdentifier)
      return false;

   if (rule.prefixPtr) {
      rule.applyPrefixDSARule(token);

      token.writeLog();
   }   

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr) {
      rule.applyPostfixDSARule(token);
   }

   return true;
}

bool variableApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.mapping == NULL || token.state != dfaIdentifier || !token.mapping->exist(token.value))
      return false;

   ident_c s[12];
   StringHelper::intToStr(token.mapping->get(token.value), s, 10);
   token.writeLog(s);

   return apply(rule, token, reader);
}

bool variableApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.mapping == NULL || token.state != dfaIdentifier || !token.mapping->exist(token.value))
      return false;

   if (rule.prefixPtr) {
      rule.applyPrefixDSARule(token);

      ident_c s[12];
      StringHelper::intToStr(token.mapping->get(token.value), s, 10);
      token.writeLog(s);
   }

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr) {
      rule.applyPostfixDSARule(token);
   }

   return true;
}


bool mappingApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.mapping == NULL || token.state != dfaIdentifier)
      return false;

   token.mapping->add(token.value, token.mapping->Count() + 1);

   if (!apply(rule, token, reader)) {
      token.mapping->erase(token.value);

      return false;
   }
   else return true;
}

bool mappingApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.mapping == NULL || token.state != dfaIdentifier)
      return false;

   if (rule.prefixPtr) {
      rule.applyPrefixDSARule(token);
   }

   token.mapping->add(token.value, token.mapping->Count() + 1);

   if (!apply(rule, token, reader)) {
      token.mapping->erase(token.value);

      return false;
   }      

   if (rule.postfixPtr) {
      rule.applyPostfixDSARule(token);
   }

   return true;
}

bool normalReferenceApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaFullIdentifier)
      return false;

   token.writeLog();

   return apply(rule, token, reader);
}

bool normalReferenceApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaFullIdentifier)
      return false;

   if (rule.prefixPtr) {
      rule.applyPrefixDSARule(token);
   }

   token.writeLog();

   if (!apply(rule, token, reader))
      return false;

   if (rule.postfixPtr) {
      rule.applyPostfixDSARule(token);
   }

   return true;
}

bool normalEOFApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   return token.state == dfaEOF;
}

bool normalEOFApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (token.state != dfaEOF)
      return false;

   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token);

   return true;
}

bool chomskiApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (applyNonterminal(rule, token, reader)) {
      // additional nonterminal should be resolved as well
      if (applyNonterminal(rule, token, reader, true)) {
         return true;
      }
   }
   return false;
}

bool chomskiApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (applyNonterminalDSA(rule, token, reader)) {
      // additional nonterminal should be resolved as well
      if (!applyNonterminal(rule, token, reader, true))
         return false;

      if (rule.postfixPtr)
         rule.applyPostfixDSARule(token);

      return true;
   }
   else return false;
}

bool epsApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   return true;
}

bool epsApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
{
   if (rule.prefixPtr)
      rule.applyPrefixDSARule(token);

   if (rule.postfixPtr)
      rule.applyPostfixDSARule(token);

   return true;
}

//bool anyApplyRule(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if(token.state == dfaEOF)
//      return false;
//
//   return apply(rule, token, reader);
//}
//
//bool anyApplyRuleDSA(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader)
//{
//   if(token.state == dfaEOF)
//      return false;
//
//   if (rule.prefixPtr)
//      rule.applyPrefixDSARule(token);
//
//   if (apply(rule, token, reader)) {
//      if (rule.postfixPtr)
//         rule.applyPostfixDSARule(token);
//
//      return true;
//   }
//   else return false;
//}

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
      case rtScope:
         rule.apply = dsaRule ? scopeNonterminalApplyRuleDSA : scopeNonterminalApplyRule;
         break;
      case rtNewIdleVariable:
         rule.apply = dsaRule ? newNonterminalApplyRuleDSA : newNonterminalApplyRule;
         break;
      case rtVariable:
         rule.apply = dsaRule ? variableApplyRuleDSA : variableApplyRule;
         break;
      case rtNewVariable:
         rule.apply = dsaRule ? mappingApplyRuleDSA : mappingApplyRule;
         break;
      //case rtAny:
      //   rule.apply = dsaRule ?  anyApplyRuleDSA :  anyApplyRule;
      //   break;
      case rtEps:
         rule.apply = dsaRule ? epsApplyRuleDSA : epsApplyRule;
         break;
      case rtEof:
         rule.apply = dsaRule ?  normalEOFApplyRuleDSA :  normalEOFApplyRule;
         break;
   }
}

void CFParser :: writeDSARule(TokenInfo& token, size_t ptr)
{
   token.buffer->write(getBodyText(ptr));
   // HOTFIX: to prevent too long line
   token.buffer->write('\n'); 
}

bool CFParser :: applyRule(size_t ruleId, TokenInfo& token, _ScriptReader& reader)
{
   ident_t name = retrieveKey(_names.start(), ruleId, DEFAULT_STR);

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

bool CFParser :: applyRule(Rule& rule, TokenInfo& token, _ScriptReader& reader)
{
   if (rule.apply(rule, token, reader)) {
      return true;
   }
   else return false;
}

size_t CFParser :: defineDSARule(TokenInfo& token, _ScriptReader& reader)
{
   ident_t s = token.getLog();
   if (!emptystr(s)) {
      MemoryWriter writer(&_body);

      size_t ptr = writer.Position();

      writer.writeLiteral(s);

      token.clearLog();

      return ptr;
   }
   else return 0;
}

size_t CFParser :: defineGrammarRule(TokenInfo& token, _ScriptReader& reader, size_t nonterminal)
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

   defineGrammarRule(token, reader, rule);

   _rules.add(ruleId, rule);

   return ruleId;
}

void CFParser :: saveScript(TokenInfo& token, _ScriptReader& reader, Rule& rule)
{
   token.read(reader);
   while (!token.compare("=>") || token.state == dfaQuote) {
      token.writeLog();

      token.read(reader);
   }
}

void CFParser :: defineGrammarRule(TokenInfo& token, _ScriptReader& reader, Rule& rule)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   RuleType type = rtNormal;

   while (token.value[0] != ';' || token.state == dfaQuote) {

      if (token.state == dfaQuote) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(token, reader);
            break;
         }
         else rule.terminal = writeBodyText(token.value);
      }
      else if (token.compare("<=")) {
         saveScript(token, reader, rule);
      }
      else if (token.state == dfaPrivate) {
         if (rule.terminal) {
            rule.nonterminal = defineGrammarRule(token, reader);
            break;
         }
         else {
            rule.prefixPtr = defineDSARule(token, reader);

            if (StringHelper::compare(token.value, LITERAL_KEYWORD)) {
               type = rtLiteral;
            }
            else if (StringHelper::compare(token.value, NUMERIC_KEYWORD)) {
               type = rtNumeric;
            }
            else if (StringHelper::compare(token.value, EPS_KEYWORD)) {
               type = rtEps;
            }
            else if (StringHelper::compare(token.value, EOF_KEYWORD)) {
               type = rtEof;
            }
            else if (StringHelper::compare(token.value, REFERENCE_KEYWORD)) {
               type = rtReference;
            }
            else if (StringHelper::compare(token.value, SCOPE_KEYWORD)) {
               type = rtScope;
            }
            else if (StringHelper::compare(token.value, IDLE_MAPPING_KEYWORD)) {
               type = rtNewIdleVariable;
            }
            else if (StringHelper::compare(token.value, VAR_KEYWORD)) {
               type = rtVariable;
            }
            else if (StringHelper::compare(token.value, MAPPING_KEYWORD)) {
               type = rtNewVariable;
            }
            //      else if (ConstantIdentifier::compare(token.value, ANY_KEYWORD)) {
      //         type = rtAny;
      //      }
            else if (StringHelper::compare(token.value, IDENTIFIER_KEYWORD)) {
               type = rtIdentifier;
            }
         }
      }
      else if (token.state == dfaIdentifier) {
         if (rule.nonterminal == 0) {
            rule.prefixPtr = defineDSARule(token, reader);

            rule.nonterminal = mapRuleId(token.value);
         }
         else if (rule.terminal == 0) {
            type = rtChomski;

            rule.terminal = mapRuleId(token.value);
         }
         else {
            if (type == rtChomski) {
               rule.terminal = defineGrammarRule(token, reader, rule.terminal);
            }
            else rule.nonterminal = defineGrammarRule(token, reader, rule.nonterminal);
            break;
         }
      }

      token.read(reader);
   }

   rule.postfixPtr = defineDSARule(token, reader);

   defineApplyRule(rule, type);
}

bool CFParser :: parseGrammarRule(_ScriptReader& reader)
{
   ScriptLog log;
   TokenInfo token(this, &log);

   token.read(reader);

   if (token.state != dfaIdentifier)
      throw EParseError(reader.info.column, reader.info.row);

   size_t ruleId = mapRuleId(token.value);

   token.read(reader);
   if (!token.compare("::="))
      throw EParseError(reader.info.column, reader.info.row);

   token.read(reader);

   Rule rule;
   defineGrammarRule(token, reader, rule);

   _rules.add(ruleId, rule);

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

void CFParser :: parse(_ScriptReader& reader, TapeWriter& writer)
{
//   if (_symbolMode)
//      reader.switchDFA(dfaSymbolic);

   ScriptLog log;
   TokenInfo token(this, &log);

   token.read(reader);

   size_t readerRollback = reader.Position();
   //size_t outputRollback = compiler->Position();
   size_t startId = mapRuleId("start");

   if (token.state != dfaEOF) {
      if (!applyRule(startId, token, reader))
         throw EParseError(token.column, token.row);

      if (token.state != dfaEOF)
         throw EParseError(token.column, token.row);
   }

   IdentifierTextReader logReader((ident_t)log.getBody());
   CachedScriptReader scriptReader(&logReader);

   _baseParser->parse(scriptReader, writer);
}
