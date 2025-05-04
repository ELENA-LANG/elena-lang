//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the regex engine declaration
//
//                                             (C)2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef REGEX_H
#define REGEX_H

#include "cfparser.h"

namespace elena_lang
{
   // --- RegEx ---
   class PatternRule
   {
   protected:

   public:
      virtual void setNext(PatternRule* next) = 0;

      static bool isLetter(char ch)
      {
         if (ch >= 0x41 && ch <= 0x5A)
         {
            return true;
         };

         if (ch >= 0x61 && ch <= 0x7A)
         {
            return true;
         };

         if (ch >= 0x370 && ch <= 0x3FB)
         {
            return true;
         };

         if (ch >= 0x400 && ch <= 0x4FF)
         {
            return true;
         };

         return false;
      }

      static bool isDigit(char ch)
      {
         if (ch >= '0' && ch <= '9')
         {
            return true;
         };

         return false;
      }

      virtual void setMode(int mode) = 0;
      virtual int getMode() = 0;

      virtual PatternRule* gotoNext(Stack<PatternRule*>& parents) = 0;
      virtual PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) = 0;

      virtual ~PatternRule() = default;
   };

   typedef List<PatternRule*, freeobj> PatternPool;

   class LinkRule : public PatternRule
   {
   protected:
      PatternRule* nextRule;
      int _mode;

   public:
      void setMode(int mode) override
      {
         _mode = mode;
      }

      int getMode() override
      {
         return _mode;
      }

      void setNext(PatternRule* next) override
      {
         nextRule = next;
      }

      PatternRule* gotoNext(Stack<PatternRule*>& parents) override
      {
         if (!nextRule) {
            return parents.pop()->gotoNext(parents);
         }

         return nextRule;
      }

      LinkRule()
         : _mode(0), nextRule(nullptr)
      {
      }
   };

   class GroupRule : public LinkRule
   {
   protected:
      List<PatternRule*>* _rules;

   public:
      GroupRule()
      {
         _rules = nullptr;
      }
      GroupRule(List<PatternRule*>* rules)
      {
         _rules = rules;
      }

      ~GroupRule() override
      {
         freeobj(_rules);
      }
   };

   class NestedRule : public LinkRule
   {
   protected:
      PatternRule* child;

   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         parents.push(this);
         PatternRule* next = child->makeStep(s, index, ch, parents);
         if (!next) {
            parents.pop();
         };

         return next;
      }

      NestedRule(PatternRule* child)
         : child(child)
      {

      }
   };

   class OptionalRule : public NestedRule
   {
   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         parents.push(this);
         PatternRule* next = child->makeStep(s, index, ch, parents);
         if (!next) {
            parents.pop();

            return gotoNext(parents)->makeStep(s, index, ch, parents);
         };

         return next;
      }

      OptionalRule(PatternRule* child)
         : NestedRule(child)
      {

      }
   };

   class RecursiveRule : public NestedRule
   {
   public:
      PatternRule* gotoNext(Stack<PatternRule*>& parents) override
      {
         return this;
      }

      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         parents.push(this);
         PatternRule* next = child->makeStep(s, index, ch, parents);
         if (!next) {
            parents.pop();

            return NestedRule::gotoNext(parents)->makeStep(s, index, ch, parents);
         };

         return next;
      }

      RecursiveRule(PatternRule* child)
         : NestedRule(child)
      {

      }
   };

   class OrRule : public GroupRule
   {
   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         parents.push(this);

         for (auto it = _rules->start(); !it.eof(); ++it) {
            PatternRule* next = (*it)->makeStep(s, index, ch, parents);
            if (next) {
               return next;
            }
         }

         parents.pop();

         return nullptr;
      }

      OrRule(List<PatternRule*>* list)
         : GroupRule(list)
      {
      }
   };

   class CharRangeRule : public LinkRule
   {
      char chFrom;
      char chTill;

   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         if (ch >= chFrom && ch <= chTill) {
            return gotoNext(parents);
         }

         return nullptr;
      }

      CharRangeRule(char from, char till)
         : chFrom(from), chTill(till)
      {
      }
   };

   class AnyWordChar : public LinkRule
   {
   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         if (PatternRule::isLetter(ch)) {
            return gotoNext(parents);
         }
         if (PatternRule::isDigit(ch)) {
            return gotoNext(parents);
         }

         return nullptr;
      }

      AnyWordChar() 
      {
      }
   };

   class DigitChar : public LinkRule
   {
   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         if (PatternRule::isDigit(ch)) {
            return gotoNext(parents);
         }

         return nullptr;
      }

      DigitChar()
      {
      }
   };

   class AnyRule : public LinkRule
   {
   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         if (ch == 10 || ch == 0) {
            return nullptr;
         }

         if (index < s.length()) {
            return gotoNext(parents);
         }

         return nullptr;
      }

      AnyRule()
      {
      }
   };

   class CharRule : public LinkRule
   {
   protected:
      char _ch;

   public:
      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         if (_ch == ch) {
            return gotoNext(parents);
         }

         return nullptr;
      }

      CharRule(char ch)
         : _ch(ch)
      {

      }
   };

   class EOLRule : public PatternRule
   {
   public:
      void setNext(PatternRule* next) override
      {
         throw AbortError();
      }

      PatternRule* makeStep(ustr_t s, size_t index, char ch, Stack<PatternRule*>& parents) override
      {
         return ch == 0 ? this : nullptr;
      }

      PatternRule* gotoNext(Stack<PatternRule*>& parents) override
      {
         return nullptr;
      }

      int getMode() override
      {
         return 0;
      }

      void setMode(int mode) override
      {
         if (mode != 2) {
            throw AbortError();
         }         
      }

      static PatternRule* getInstance()
      {
         static EOLRule rule;

         return &rule;
      }
   };

   class RegEx : public ScriptEngineCFParser::SaverBase
   {
   protected:
      PatternRule* _startRule;
      PatternPool* _pool;

   public:
      bool isMatched(ustr_t token, char) override;

      void extract(ScriptEngineLog& log, ustr_t token);

      RegEx(PatternRule* startRule, PatternPool* pool)
         : _startRule(startRule), _pool(pool)
      {
      }

      virtual ~RegEx()
      {
         freeobj(_pool);
      }
   };

   class QuoteRegEx : public RegEx
   {
   public:
      bool isMatched(ustr_t token, char state) override
      {
         if (state == dfaQuote) {
            return RegEx::isMatched(token, state);
         }

         return false;
      }

      void saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log) override;

      QuoteRegEx(PatternRule* startRule, PatternPool* pool)
         : RegEx(startRule, pool)
      {

      }
   };

   // --- RegExFactory ---
   class RegExFactory
   {
      static PatternRule* defineEscRule(char ch);
      static PatternRule* defineRule(ustr_t regex, size_t& start, char ch);
      static PatternRule* defineRule(char ch);

      static PatternRule* parseGroup(ustr_t regex, size_t& start, size_t end, PatternPool& pool);
      static PatternRule* parseBrackets(ustr_t regex, size_t& start, size_t end, PatternPool& pool);

      static PatternRule* parseLine(ustr_t regex, size_t& start, size_t end, char terminator, PatternPool& pool);

      static PatternRule* linkRules(List<PatternRule*>& rules);

      static bool parseLine(ustr_t regex, List<PatternRule*>& rules, size_t& start, size_t end, char terminator, PatternPool& pool);
      static PatternRule* parsePattern(ustr_t regex, List<PatternRule*>& rules, size_t start, size_t end, PatternPool& pool);

   public:
      static ScriptEngineCFParser::SaverBase* generate(ustr_t regex);
   };

}

#endif
