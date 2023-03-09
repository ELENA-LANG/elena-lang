//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2023, by Aleksey Rakov
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
         TypeMask       = 0x0F,

         None           = 0x00,
         Normal         = 0x01,
         Chomski        = 0x02,
         Nonterminal    = 0x03,
         Eps            = 0x04,
         WithForward    = 0x10,
         WithBookmark   = 0x20,
      };

      typedef void (*SaveToSign)(ScriptEngineReaderBase& scriptReader, ScriptEngineCFParser* parser, ref_t ptr, ScriptEngineLog& log);

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
         SaveToSign saveTo;

         Rule()
         {
            type = RuleType::None;
            terminal = nonterminal = 0;
            postfix1Ptr = prefix1Ptr = 0;
            postfix2Ptr = prefix2Ptr = 0;

            apply = nullptr;
            saveTo = nullptr;
         }
      };

      typedef MemoryHashTable<pos_t, Rule, syntaxRuleCF, cnCFHashSize, Map_StoreUInt, Map_GetUInt> SyntaxTable;

   protected:
      ScriptEngineParserBase* _baseParser;
      SyntaxTable             _table;
      NameMap                 _names;
      MemoryDump              _body;

      ref_t mapRuleId(ustr_t name)
      {
         return mapKey(_names, name, _names.count() + 1);
      }

      const char* getBodyText(pos_t ptr);

      pos_t writeRegExprBodyText(ScriptEngineReaderBase& reader, int& mode);
      pos_t writeBodyText(ustr_t text);

      ref_t autonameRule(ref_t parentRuleId);

      void defineApplyRule(Rule& rule, int terminalType, bool forwardMode, bool bookmarkMode);

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

      void defineGrammarRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId);

      void addRule(int id, Rule& rule);

   public:
      void readScriptBookmark(pos_t ptr, ScriptBookmark& bm);

      bool compareToken(ScriptEngineReaderBase& reader, ScriptBookmark& bm, int rule);
      bool compareTokenWithAny(ScriptEngineReaderBase& reader, ScriptBookmark& bm, int rule);

      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      ScriptEngineCFParser(ScriptEngineParserBase* baseParser)
         : _table({}), _names(0)
      {
         _baseParser = baseParser;
      }
      virtual ~ScriptEngineCFParser()
      {
         freeobj(_baseParser);
      }

   };
}

#endif
