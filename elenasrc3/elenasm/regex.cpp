//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the pattern engine implementation
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "regex.h"

using namespace elena_lang;

const char* Postfix = "+*?";
const char* Reserved = "$^[]+*?()\\";

// --- RegExFactory ---

PatternRule* RegExFactory :: defineEscRule(char ch)
{
   if (ch == 'w') {
      return new AnyWordChar();
   }
   else if (ch == 'd') {
      return new DigitChar();
   }

   return new CharRule(ch);
}

PatternRule* RegExFactory :: defineRule(char ch)
{
   if (ch == '.') {
      return new AnyRule();
   }

   if (ustr_t(Reserved).find(ch) != NOTFOUND_POS)
   {
      return nullptr;
   }

   return new CharRule(ch);
}

PatternRule* RegExFactory :: defineRule(ustr_t regex, size_t& start, char ch)
{
   size_t i = start;
   PatternRule* rule = nullptr;
   if (regex[i + 1] == '-') {
      i += 2;

      char lastCh = regex[i];
      rule = new CharRangeRule(ch, lastCh);
   }
   else if (ch == '\\') {
      i++;
      ch = regex[i];
      rule = defineEscRule(ch);
   }
   else rule = defineRule(ch);

   start = i;

   return rule;
}


PatternRule* RegExFactory :: parseGroup(ustr_t regex, size_t& start, size_t end, PatternPool& pool)
{
   size_t i = start + 1;

   auto orList = new List<PatternRule*>(nullptr);
   while (i < end) {
      char ch = regex[i];

      if (ch == ']') {
         i++;
         break;
      }
      else {
         PatternRule* rule = defineRule(regex, i, ch);
         if (rule != nullptr) {
            pool.add(rule);// keep the list of unique rules
            orList->add(rule);
         }
         else return nullptr;
      }

      i++;
   }

   start = i;
   if (orList->count() == 1) {
      auto rule = *orList->start();

      freeobj(orList);

      return rule;
   }

   PatternRule* orRule = new OrRule(orList);
   pool.add(orRule);// keep the list of unique rules

   return orRule;
}

PatternRule* RegExFactory :: parseBrackets(ustr_t pattern, size_t& start, size_t end, PatternPool& pool)
{
   bool selectMode = true;
   auto orList = new List<PatternRule*>(nullptr);

   size_t i = start + 1;

   if (pattern[i] == '?') {
      i++;

      if (pattern.compareSub("<current>", i, 9)) {
         selectMode = true;
         i += 9;
      }
      else
      {
         freeobj(orList);

         return nullptr;
      }
   }

   bool closed = false;
   while (i < end) {
      PatternRule* rule = parseLine(pattern, i, end, ')', pool);
      if (!rule) {
         freeobj(orList);

         return nullptr;
      }         

      orList->add(rule);
      if (pattern[i] == ')') {
         i++;
         closed = true;

         break;
      }

      i++;
   }

   if (!closed) {
      freeobj(orList);

      return nullptr;
   }      

   start = i;
   if (orList->count() == 1) {
      auto rule = new NestedRule(*orList->start());

      if (selectMode) {
         rule->setMode(1);
      }

      pool.add(rule); // keep the list of unique rules

      freeobj(orList);

      return rule;
   }
   else if (selectMode) {
      freeobj(orList);

      return nullptr;
   }

   auto orRule = new OrRule(orList);
   pool.add(orRule); // keep the list of unique rules

   return orRule; 
}

PatternRule* RegExFactory :: parseLine(ustr_t pattern, size_t& start, size_t end, char terminator, PatternPool& pool)
{
   auto list = new List<PatternRule*>(nullptr);
   if (parseLine(pattern, *list, start, end, terminator, pool)) {
      auto first = linkRules(*list);

      freeobj(list);

      return first;
   }

   return nullptr;
}

PatternRule* RegExFactory :: linkRules(List<PatternRule*>& rules)
{
   for (auto it = rules.start(); !it.eof(); ++it)
   {
      auto next = it;
      next++;

      if (!next.eof()) {
         (*it)->setNext(*next);

         if ((*it)->getMode() == 1) {
            (*next)->setMode(2);
         }
      }
         
   }

   return *rules.start();
}

bool RegExFactory :: parseLine(ustr_t pattern, List<PatternRule*>& rules, size_t& start, size_t end, char terminator, PatternPool& pool)
{
   ustr_t postfix(Postfix);

   size_t i = start;
   while (i < end) {
      char ch = pattern[i];

      if (ch == terminator || ch == '|') {
         break;
      }
      else if (postfix.find(ch) != NOTFOUND_POS) {
         if (rules.count() == 0)
            return false;

         PatternRule* rule = nullptr;
         switch (ch) {
            case '?':
               rule = new OptionalRule(*rules.end());
               pool.add(rule); // keep the list of unique rules
               break;
            case '*':
               rule = new RecursiveRule(*rules.end());
               pool.add(rule); // keep the list of unique rules
               break;
            case '+':
            {
               PatternRule* last = *rules.end();
               PatternRule* nested = new NestedRule(last);
               rule = new RecursiveRule(last);
               *(rules.end()) = nested;
               rules.add(rule);

               pool.add(rule); // keep the list of unique rules
               pool.add(nested); // keep the list of unique rules

               break;
            }
            default:
               break;
         }

         *(rules.end()) = rule;
      }
      else {
         bool continueMode = false;
         PatternRule* rule = nullptr;

         switch (ch)
         {
            case '[':
               rule = parseGroup(pattern, i, end, pool);
               continueMode = true;
               break;
            case '(':
               rule = parseBrackets(pattern, i, end, pool);
               continueMode = true;
               break;
            default:
               rule = defineRule(pattern, i, ch);
               pool.add(rule); // keep the list of unique rules
               break;
         }

         if (!rule)
            return false;

         rules.add(rule);         

         if (continueMode)
            continue;
      }

      i++;
   }

   start = i;
   return true;
}

PatternRule* RegExFactory :: parsePattern(ustr_t regex, List<PatternRule*>& rules, size_t start, size_t end, PatternPool& pool)
{
   if (!parseLine(regex, rules, start, end, 0, pool)) {
      return nullptr;
   }

   if (start < end) {
      return nullptr;
   }

   rules.add(EOLRule::getInstance());

   return linkRules(rules);
}

ScriptEngineCFParser::SaverBase* RegExFactory :: generate(ustr_t pattern)
{
   if (pattern.startsWith("^\"") && pattern.endsWith("\"$")) {
      auto rules = new List<PatternRule*>(nullptr);
      PatternPool* pool = new PatternPool(nullptr);

      auto startRule = parsePattern(pattern, *rules, 2, pattern.length() - 2, *pool);
      if (startRule) {
         auto re = new QuoteRegEx(startRule, pool);

         freeobj(rules);

         return re;
      }
      else {
         freeobj(rules);
         freeobj(pool);

         return nullptr;
      }
   }

   return nullptr;
}

// --- RegEx ---

bool RegEx :: isMatched(ustr_t token, char)
{
   Stack<PatternRule*> parents(nullptr);

   size_t len = token.length();
   size_t index = 0;
   PatternRule* current = _startRule;
   while (index < len) {
      char ch = token[index];

      current = current->makeStep(token, index, ch, parents);
      if(!current) {
         return false;
      }

      index++;
   };

   return current->makeStep(token, len, '\0', parents) != nullptr;
}

void RegEx :: extract(ScriptEngineLog& log, ustr_t token)
{
   Stack<PatternRule*> parents(nullptr);

   size_t len = token.length();
   size_t index = 0;
   size_t start = 0;
   size_t end = len;
   PatternRule* current = _startRule;
   while (index < len) {
      switch (current->getMode()) {
         case 1:
            start = index;
            break;
         case 2:
            end = index;
            break;
         default:
            break;
      }

      char ch = token[index];

      current = current->makeStep(token, index, ch, parents);
      if (!current) {
         break;
      }

      index++;
   };

   if (end == len) {
      log.write(token + start);
   }
   else if ((end - start) < IDENTIFIER_LEN) {
      IdentifierString tmp(token + start, end - start);

      log.write(tmp.str());
   }
   else {
      DynamicString<char> tmp;
      tmp.copy(token + start, end - start);

      log.write(tmp.str());
   }
}

// --- QuoteRegEx ---

void QuoteRegEx :: saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log)
{
   ScriptBookmark bm;
   parser->readScriptBookmark(ptr, bm);

   log.writeCoord(bm.lineInfo);

   extract(log, scriptReader.lookup(bm));
}