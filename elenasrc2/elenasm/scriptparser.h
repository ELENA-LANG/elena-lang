//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef scriptparserH
#define scriptparserH 1

//#include "common.h"
//#include "parser.h"
//#include "parsertable.h"

namespace _ELENA_
{

//// --- LALRParser ---
//
//class LALRParser : public _Parser
//{
//   struct Reader
//   {
//   private:
//      LALRParser*  _parser;
//      wchar16_t    _buffer[IDENTIFIER_LEN + 1];
//      SourceReader _reader;
//
//   public:
//      LineInfo info;
//      int      terminal;
//
//      void copyTo(Terminal& terminalInfo)
//      {
//         terminalInfo.col = info.column;
//         terminalInfo.row = info.row;
//         terminalInfo.state = info.state;
//
//         if (info.state == _ELENA_TOOL_::dfaQuote) {
//            QuoteTemplate<TempString> quote(info.line);
//
//            size_t length = getlength(quote);
//            StringHelper::copy(terminalInfo.value, quote, length);
//            terminalInfo.value[length] = 0;
//         }
//         else {
//            size_t length = getlength(info.line);
//            StringHelper::copy(terminalInfo.value, info.line, length);
//            terminalInfo.value[length] = 0;
//         }
//      }
//
//      void read();
//
//      Reader(LALRParser* parser, int tabSize, TextReader* source);
//   };
//
//   friend struct Reader;
//
//   struct DSARule
//   {
//      int prefixPtr;
//      int postfixPtr;
//
//      DSARule()
//      {  
//         prefixPtr = postfixPtr = 0;
//      }
//   };
//
//   typedef MemoryMap<int, DSARule> DSARuleMap;
//
//   const char** _dfa;
//   ParserTable  _table;
//   MemoryDump   _body;
//   DSARuleMap   _dsaRules;
//   size_t       _lastId;
//
//   void parseRuleWithDSA(int symbol, Reader& reader, ParserStack& stack, _CodeGeneator* compiler);
//   void parseRule(int symbol, Reader& reader, ParserStack& stack, _CodeGeneator* compiler);
//
//   int mapSymbol(const wchar16_t* name, size_t mask = 0);
//
//   void defineGrammarRule(int rule, SourceReader& reader, wchar16_t* token);
//   void defineDSARule(int rule, SourceReader& reader, wchar16_t* token);
//
//public:
//   virtual void parse(TextReader* script, _CodeGeneator* compiler);
//
//   virtual void generate(TextReader* script);
//
//   // if dfa is NULL standard DFA is used
//   LALRParser(const char** dfa)
//      : _dsaRules(DSARule())
//   {
//      _dfa = dfa;
//
//      _body.writeDWord(0, 0);
//
//      _lastId = 0;
//
//      //start symbol should be always 1 and eps equals to 2
//      mapSymbol(_T("start"));
//      mapSymbol(_T("$eps"));
//   }
//};

} // _ELENA_

#endif // scriptparserH
