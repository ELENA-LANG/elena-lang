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
   enum RuleType
   {
      rtNone = 0,
      rtNormal,
      rtChomski,
      rtNonterminal,

//      rtNormal,
//      rtLiteral,
//      rtNumeric,
//      rtReference,
//      rtIdentifier,
//      rtScope,
//      rtVariable,
//      rtNewVariable,
//      rtNewIdleVariable,
////      rtAny,
//      rtEps,
//      rtEof
   };
   
   struct Rule
   {
      RuleType type;
      size_t   terminal;    // in chomski form it is additional nonterminal as well
      size_t   nonterminal;

      bool(*apply)(Rule& rule, ScriptBookmark& bm, _ScriptReader& reader, CFParser* parser);

      Rule()
      {
         type = rtNone;
         terminal = nonterminal = 0;
         apply = NULL;
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

//   typedef Map<ident_t, int> Mapping;
//
//   // --- Rule ---
//   struct Rule
//   {
//
//      size_t prefixPtr;
//      size_t postfixPtr;
//
//      bool(*apply)(CFParser::Rule& rule, CFParser::TokenInfo& token, _ScriptReader& reader);
//
//      void applyPrefixDSARule(CFParser::TokenInfo& token)
//      {
//         token.parser->writeDSARule(token, prefixPtr);
//      }
//
//      void applyPostfixDSARule(CFParser::TokenInfo& token)
//      {
//         token.parser->writeDSARule(token, postfixPtr);
//      }
//
//      Rule()
//      {
//         terminal = 0;
//         nonterminal = 0;
//         prefixPtr = 0;
//         postfixPtr = 0;
//      }
//   };
//
//   friend struct TokenInfo;

protected:
   _Parser*    _baseParser;
   SyntaxTable _table;
   NameMap     _names;
   MemoryDump  _body;

   size_t mapRuleId(ident_t name)
   {
      return mapKey(_names, name, _names.Count() + 1);
   }

   void defineApplyRule(Rule& rule, int terminalType);

   size_t writeBodyText(ident_t text);
   ident_t getBodyText(size_t ptr);

//   size_t defineDSARule(TokenInfo& token, _ScriptReader& reader);
//
//   void writeDSARule(TokenInfo& token, size_t ptr);

//   void compile(TokenInfo& token, CachedScriptReader& reader, _ScriptCompiler* compiler);

   void addRule(int id, Rule& rule);

   size_t defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, size_t nonterminal = 0);
   void defineGrammarRule(_ScriptReader& reader, ScriptBookmark& bm, Rule& rule);

   void saveTraceItem(TraceItem& item, ScriptLog& log, Rule& rule);

   void predict(DerivationQueue& queue, DerivationItem item, _ScriptReader& reader, ScriptBookmark& bm, int terminalOffset, MemoryWriter& writer);
   int buildDerivationTree(_ScriptReader& reader, size_t startRuleId, MemoryWriter& writer);
   void generateOutput(int offset, ScriptLog& log);

public:
   bool compareToken(_ScriptReader& reader, ScriptBookmark& bm, int rule);

   virtual bool parseGrammarRule(_ScriptReader& reader);

   virtual void parse(_ScriptReader& reader, TapeWriter& writer);

   CFParser(_Parser* baseParser)
      : _table(Rule())
   {
      _baseParser = baseParser;

      // all body pointers should be greater than zero
      _body.writeDWord(0, 0);
   }
};

} // _ELENA_

#endif // cfparserH
