//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Script Engine
//
//                                               (C)2023-25, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CFPARSER_H
#define CFPARSER_H

#include "scriptparser.h"

namespace elena_lang
{
   constexpr size_t cnCFHashSize = 0x0100;              // the hash size
   constexpr size_t cnCFSyntaxPower = 0x0008;

   inline pos_t syntaxRuleCF(pos_t key)
   {
      return key >> cnCFSyntaxPower;
   }

   // --- ScriptEngineCFParser ---
   class ScriptEngineCFParser : public ScriptEngineParserBase
   {
   public:
      typedef MemoryMap<ustr_t, ref_t, Map_StoreUStr, Map_GetUStr> NameMap;

      enum RuleType
      {
         TypeMask          = 0x0F,

         None              = 0x00,
         Normal            = 0x01,
         Chomski           = 0x02,
         Nonterminal       = 0x03,
         Eps               = 0x04,
         WithForward       = 0x10,
         WithBookmark      = 0x20,
         WithBufferOutput  = 0x40,
      };

      enum class RuleTypeModifier
      {
         None,
         ForwardMode,
         BookmarkMode,
         BufferMode
      };

      class SaverBase
      {
      public:
         virtual bool isMatched(ustr_t/* token*/, char/* state*/)
         {
            return true;
         }

         virtual void saveTo(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log) = 0;

         virtual ~SaverBase() = default;
      };

      struct Rule
      {
         RuleType type;
         pos_t    terminal;    // in chomski form it is additional nonterminal as well
         pos_t    nonterminal;
         pos_t    prefix1Ptr;
         pos_t    prefix2Ptr;
         pos_t    postfix1Ptr;
         pos_t    postfix2Ptr;

         bool(*apply)(Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, ScriptEngineCFParser* parser);

         SaverBase* saver;

         Rule()
         {
            type = RuleType::None;
            terminal = nonterminal = 0;
            postfix1Ptr = prefix1Ptr = 0;
            postfix2Ptr = prefix2Ptr = 0;

            apply = nullptr;
            saver = nullptr;
         }
      };

      struct DerivationItem
      {
         pos_t ruleId;
         pos_t trace;    // derivation trace
         pos_t next;     // tailing nonterminal

         DerivationItem()
         {
            ruleId = 0;
            trace = 0;
            next = 0;
         }
         DerivationItem(pos_t ruleId)
         {
            this->ruleId = ruleId;
            this->trace = 0;
            this->next = 0;
         }
         DerivationItem(pos_t ruleId, pos_t trace, pos_t next)
         {
            this->ruleId = ruleId;
            this->trace = trace;
            this->next = next;
         }
      };

      struct TraceItem
      {
         pos_t ruleKey;
         pos_t terminal;
         pos_t previous;

         TraceItem()
         {
            ruleKey = terminal = previous = 0;
         }
         TraceItem(pos_t key)
         {
            ruleKey = key;
            terminal = previous = 0;
         }
      };

      //typedef MemoryHashTable<pos_t, Rule, syntaxRuleCF, cnCFHashSize, Map_StoreUInt, Map_GetUInt> SyntaxTable;
      typedef HashTable<pos_t, Rule, syntaxRuleCF, cnCFHashSize> SyntaxTable;
      typedef Queue<DerivationItem> DerivationQueue;
      typedef List<SaverBase*, freeobj> RegExList;

   protected:
      ScriptEngineParserBase* _baseParser;
      SyntaxTable             _table;
      NameMap                 _names;
      MemoryDump              _body;
      ScriptEngineLog         _buffer;

      bool                    _symbolMode;
      bool                    _generating;

      RegExList               _regExList;

      ref_t mapRuleId(ustr_t name)
      {
         return mapKey(_names, name, _names.count() + 1);
      }

      const char* getBodyText(pos_t ptr);

      pos_t writeRegExprBodyText(ScriptEngineReaderBase& reader, int& mode);
      pos_t writeBodyText(ustr_t text);

      ref_t autonameRule(ref_t parentRuleId);

      void defineApplyRule(Rule& rule, int terminalType, RuleTypeModifier modifier);

      void setScriptPtr(ScriptBookmark& bm, Rule& rule, bool prefixMode);

      void saveScript(ScriptEngineReaderBase& reader, Rule& rule, int& mode, bool forwardMode);

      pos_t defineGrammarRuleMember(ScriptEngineReaderBase& reader, ScriptBookmark& bm, ref_t ruleId, pos_t nonterminal = 0, pos_t terminal = 0);
      pos_t defineGrammarBrackets(ScriptEngineReaderBase& reader, ScriptBookmark& bm, ref_t ruleId);

      pos_t defineChomskiGrammarRule(ref_t ruleId, pos_t nonterminal, pos_t nonterminal2);
      pos_t defineOptionalGrammarRule(ref_t ruleId, pos_t nonterminal);
      pos_t definePlusGrammarRule(ref_t ruleId, pos_t nonterminal);
      pos_t defineStarGrammarRule(ref_t ruleId, pos_t nonterminal);

      void defineIdleGrammarRule(ref_t ruleId);
      void defineGrammarRuleMemberPostfix(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId);
      void defineGrammarRuleMember(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId, int& applyMode);

      int defineIfRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule);

      void defineGrammarRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId);

      void addRule(int id, Rule& rule);

      void predict(DerivationQueue& queue, DerivationItem item, ScriptEngineReaderBase& reader, ScriptBookmark& bm, pos_t terminalOffset, MemoryWriter& writer);
      pos_t buildDerivationTree(ScriptEngineReaderBase& reader, ref_t startRuleId, MemoryWriter& writer);
      void generateOutput(pos_t offset, ScriptEngineReaderBase& reader, ScriptEngineLog& log);

      void insertForwards(Stack<Pair<int, int>>& forwards, int level, ScriptEngineLog& log);

      SaverBase* createRegEx(ScriptBookmark& bm, ustr_t regex);

   public:
      void flushBuffer(ScriptEngineLog& log)
      {
         log.flush(_buffer);
      }

      void clearBuffer()
      {
         _buffer.clear();
      }

      void saveRuleOutput(Rule& rule, pos_t terminal, ScriptEngineReaderBase& scriptReader, ScriptEngineLog& log);
      void saveRuleOutputToBuffer(Rule& rule, pos_t terminal, ScriptEngineReaderBase& scriptReader);

      void readScriptBookmark(pos_t ptr, ScriptBookmark& bm);

      bool compareToken(ScriptEngineReaderBase& reader, ScriptBookmark& bm, int rule);
      bool compareTokenWithAny(ScriptEngineReaderBase& reader, ScriptBookmark& bm, int rule);

      bool parseDirective(ScriptEngineReaderBase&, MemoryDump*) override;

      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      void parse(ScriptEngineReaderBase& reader, MemoryDump* output) override;

      ScriptEngineCFParser(ScriptEngineParserBase* baseParser)
         : _table({}), _names(0), _regExList(nullptr)
      {
         _baseParser = baseParser;

         // all body pointers should be greater than zero
         MemoryBase::writeDWord(&_body, 0, 0);

         _symbolMode = _generating = false;
      }
      virtual ~ScriptEngineCFParser()
      {
         freeobj(_baseParser);
      }

   };
}

#endif
