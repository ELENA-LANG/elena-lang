//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef cfparserH
#define cfparserH 1

#include "scriptengine.h"

namespace _ELENA_
{

// --- CFPrarser ---

class CFParser : public _Parser
{
public:
   typedef Map<ident_t, int> Mapping;

   enum RuleType
   {
      rtNone = 0,
      rtNormal,
      rtChomski,
      rtNonterminal,
      rtEps,
   };
   
   struct Rule
   {
      RuleType type;
      size_t   terminal;    // in chomski form it is additional nonterminal as well
      size_t   nonterminal;
      size_t   prefixPtr;
      size_t   postfixPtr;

      bool(*apply)(Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser);
      void (*saveTo)(_ScriptReader& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log);

      Rule()
      {
         type = rtNone;
         terminal = nonterminal = 0;
         apply = NULL;
         saveTo = NULL;
         postfixPtr = prefixPtr = 0;
      }
   };

   struct DerivationItem
   {
      size_t ruleId;
      int    trace;    // derivation trace
      int    next;     // tailing nonterminal

      DerivationItem()
      {
         ruleId = 0;
         trace = 0;
         next = 0;
      }
      DerivationItem(int ruleId)
      {
         this->ruleId = ruleId;
         this->trace = 0;
         this->next = 0;
      }
      DerivationItem(int ruleId, int trace, int next)
      {
         this->ruleId = ruleId;
         this->trace = trace;
         this->next = next;
      }
   };

   struct TraceItem
   {
      int ruleKey;
      int terminal;
      int previous;

      TraceItem()
      {
         ruleKey = terminal = previous = 0;
      }
      TraceItem(int key)
      {
         ruleKey = key;
         terminal = previous = 0;
      }
   };

   typedef Queue<DerivationItem> DerivationQueue;
   typedef MemoryHashTable<size_t, Rule, syntaxRule, cnHashSize> SyntaxTable;
   typedef MemoryMap<ident_t, size_t> NameMap;

protected:
   _Parser*          _baseParser;
   SyntaxTable       _table;
   NameMap           _names;
   MemoryDump        _body;

   bool              _symbolMode;

   size_t mapRuleId(ident_t name)
   {
      return mapKey(_names, name, _names.Count() + 1);
   }

   size_t autonameRule(size_t parentRuleId);

   void defineApplyRule(Rule& rule, int terminalType);

   size_t writeBodyText(ident_t text);
   const char* getBodyText(size_t ptr);

   void addRule(int id, Rule& rule);

   void saveScript(_ScriptReader& reader, Rule& rule, int& mode);
   size_t defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, ref_t ruleId, size_t nonterminal = 0, size_t terminal = 0);
   void defineIdleGrammarRule(ref_t ruleId);
   size_t defineOptionalGrammarRule(ref_t ruleId, size_t nonterminal);
   size_t defineStarGrammarRule(ref_t ruleId, size_t nonterminal);
   size_t definePlusGrammarRule(ref_t ruleId, size_t nonterminal);
   void defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId);

   void predict(DerivationQueue& queue, DerivationItem item, _ScriptReader& reader, ScriptBookmark& bm, int terminalOffset, MemoryWriter& writer);
   int buildDerivationTree(_ScriptReader& reader, size_t startRuleId, MemoryWriter& writer);
   void generateOutput(int offset, _ScriptReader& reader, ScriptLog& log);

public:
   void readScriptBookmark(size_t ptr, ScriptBookmark& bm);

   bool compareToken(_ScriptReader& reader, ScriptBookmark& bm, int rule);

   virtual bool parseGrammarRule(_ScriptReader& reader);

   virtual void parse(_ScriptReader& reader, MemoryDump* output);

   virtual bool parseGrammarMode(_ScriptReader& reader);

   CFParser(_Parser* baseParser)
      : _table(Rule())
   {
      _baseParser = baseParser;

      // all body pointers should be greater than zero
      _body.writeDWord(0, 0);

      _symbolMode = false;
   }

   virtual ~CFParser()
   {
      freeobj(_baseParser);
   }
};

} // _ELENA_

#endif // cfparserH
