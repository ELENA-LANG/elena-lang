//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "cfparser.h"

using namespace elena_lang;

// --- ScriptEngineCFParser ---

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

         if (rule.saveTo != nullptr || (!prefixMode && !reader.compare(CURRENT_KEYWORD)))
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

void ScriptEngineCFParser :: defineGrammarRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId)
{
   // read: terminal [nonterminal] ;
   // read: nonterminal [nonterminal2] ;

   rule.type = RuleType::Normal;
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

   defineApplyRule(rule, applyMode, forwardMode, bookmarkMode);
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
