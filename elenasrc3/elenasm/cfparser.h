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
   // --- ScriptEngineCFParser ---
   class ScriptEngineCFParser : public ScriptEngineParserBase
   {
   public:
      typedef MemoryMap<ustr_t, ref_t, Map_StoreUStr, Map_GetUStr> NameMap;

      enum RuleType
      {
         None     = 0x00,

         Normal   = 0x01,
      };

      typedef void (*SaveToSign)(ScriptEngineReaderBase& scriptReader, CFParser* parser, ref_t ptr, ScriptLog& log);

      struct Rule
      {
         RuleType type;
         pos_t    terminal;    // in chomski form it is additional nonterminal as well
         pos_t    nonterminal;
         pos_t    prefix1Ptr;
         pos_t    prefix2Ptr;
         pos_t    postfix1Ptr;
         pos_t    postfix2Ptr;

         bool(*apply)(Rule& rule, ScriptBookmark& bm, ScriptEngineReaderBase& reader, CFParser* parser);
         SaveToSign saveTo;

         Rule()
         {
            type = RuleType::None;
            terminal = nonterminal = 0;
            postfix1Ptr = prefix1Ptr = 0;
            postfix2Ptr = prefix2Ptr = 0;
         }
      };

   protected:
      ScriptEngineParserBase* _baseParser;
      NameMap                 _names;
      MemoryDump              _body;

      ref_t mapRuleId(ustr_t name)
      {
         return mapKey(_names, name, _names.count() + 1);
      }

      void saveScript(ScriptEngineReaderBase& reader, Rule& rule, int& mode, bool forwardMode);

      void defineGrammarRule(ScriptEngineReaderBase& reader, ScriptBookmark& bm, Rule& rule, ref_t ruleId);

   public:
      bool parseGrammarRule(ScriptEngineReaderBase& reader) override;

      ScriptEngineCFParser(ScriptEngineParserBase* baseParser)
         : _names(0)
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
