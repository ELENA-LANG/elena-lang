//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM Script Engine
//
//                                             (C)2011-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef cfparserH
#define cfparserH 1

////#include "elena.h"
#include "textsource.h"
#include "session.h"

namespace _ELENA_
{

typedef _ELENA_TOOL_::TextSourceReader  SourceReader;

// --- CFPrarser ---

class CFParser : public _Parser
{
public:
   // --- TokenInfo ---

   struct TokenInfo
   {
      CFParser*        parser;
      _ScriptCompiler* compiler;

      char             state;
      const wchar16_t* value;
      size_t           row, column;

      void copyTo(Terminal* terminal)
      {
         size_t length = getlength(value);
         StringHelper::copy(terminal->value, value, length);
         terminal->value[length] = 0;

         terminal->col = column;
         terminal->row = row;
         terminal->state = state;
      }

      bool empty()
      {
         return emptystr(value);
      }
   
      bool compare(const wchar16_t* param)
      {
         return StringHelper::compare(value, param);
      }
   
      bool compare(size_t param)
      {
         const wchar_t* s = parser->getBodyText(param);

         return StringHelper::compare(value, s);
      }
   
//      const wchar16_t* resolvePtr(size_t param)
//      {
//         return parser->getBodyText(param);
//      }

      void save(TokenInfo& token)
      {
         token.parser = parser;
         token.compiler = compiler;
         token.row = row;
         token.column = column;
         token.value = value;
         token.state = state;
      }

      TokenInfo(CFParser* parser, _ScriptCompiler* compiler)
      {
         this->parser = parser;
         this->compiler = compiler;
         row = column = 0;
         value = NULL;
         state = 0;
      }
      TokenInfo(TokenInfo& token)
      {
         parser = token.parser;
         compiler = token.compiler;
         row = token.row;
         column = token.column;
         value = token.value;
         state = token.state;
      }
   };
   
   // --- ScriptReader ---

   class ScriptReader
   {
   public:
      struct Reader
      {
         SourceReader parser;
         LineInfo     token;
         wchar16_t    buffer[IDENTIFIER_LEN + 1];
   
         bool read();
   
         Reader(const char** dfa, TextReader* script);
      };

   protected:
      Reader     _reader;

   public:
      virtual bool read(TokenInfo& token);

      ScriptReader(const char** dfa, TextReader* script);
   };

   // --- CachedScriptReader ---
   
   class CachedScriptReader : public ScriptReader
   {
   protected:
      MemoryDump _buffer;
      size_t     _position;
   
      void cache();
   
   public:
      size_t Position() { return _position; }
   
      void seek(size_t position)
      {
         _position = position;
      }
   
      void reread(TokenInfo& token);

      void clearCache()
      {
         _buffer.trim(0);
         _position = 0;
      }
   
      virtual bool read(TokenInfo& token);
   
      CachedScriptReader(const char** table, TextReader* script);
   };

   enum RuleType
   {
      rtNormal,
      rtChomski,
      rtLiteral,
      rtNumeric,
      rtReference,
      rtIdentifier,
//      rtAny,
      rtEps,
      rtEof
   };

   // --- Rule ---
   struct Rule
   {
      size_t terminal;    // in chomski form it could be additional nonterminal as well
      size_t nonterminal;

      size_t prefixPtr;
      size_t postfixPtr;

      bool(*apply)(CFParser::Rule& rule, CFParser::TokenInfo& token, CFParser::CachedScriptReader& reader);

      void applyPrefixDSARule(CFParser* parser, _ScriptCompiler* compiler, Terminal* terminal)
      {
         parser->applyDSARule(compiler, prefixPtr, terminal);
      }

      void applyPostfixDSARule(CFParser* parser, _ScriptCompiler* compiler, Terminal* terminal)
      {
         parser->applyDSARule(compiler, postfixPtr, terminal);
      }

      Rule()
      {
         terminal = 0;
         nonterminal = 0;
         prefixPtr = 0;
         postfixPtr = 0;
      }
   };

   friend struct TokenInfo;

   typedef MemoryMap<size_t, Rule>             RuleMap;
   typedef MemoryMap<const wchar16_t*, size_t> NameMap;

protected:
   const char** _dfa;
   NameMap      _names;
   RuleMap      _rules;
   MemoryDump   _body;

   size_t mapRuleId(const wchar16_t* name)
   {
      return mapKey(_names, name, _names.Count() + 1);
   }

   void defineApplyRule(Rule& rule, RuleType type);

   size_t writeBodyText(const wchar16_t* text);
   const wchar16_t* getBodyText(size_t ptr);

   void defineGrammarRule(TokenInfo& token, ScriptReader& reader, Rule& rule);
   void defineDSARule(TokenInfo& token, ScriptReader& reader, size_t& ptr);

   void applyDSARule(_ScriptCompiler* compiler, size_t ptr, Terminal* terminal);

public:
   bool applyRule(Rule& rule, TokenInfo& token, CachedScriptReader& reader);
   bool applyRule(size_t ruleId, TokenInfo& token, CachedScriptReader& reader);

   virtual void generate(TextReader* script);
   virtual void parse(TextReader* script, _ScriptCompiler* compiler);

   CFParser(const char** dfa)
      : _rules(Rule())
   {
      _dfa = dfa;

      // all body pointers should be greater than zero
      _body.writeDWord(0, 0);
   }
};

//// --- ParserWrapper ---
//
//class ParserWrapper : public _CodeGeneator
//{
//   _Parser*       _parser;
//   _CodeGeneator* _compiler;
//
//   bool           _nested;
//
//public:
//   void addParser(_Parser* parser)
//   {
//      if (_parser == NULL) {
//         _parser = parser;
//      }
//      else if (!_nested) {
//         _nested = true;
//
//         _compiler = new ParserWrapper(parser, _compiler);
//      }
//      else ((ParserWrapper*)_compiler)->addParser(parser);
//   }
//
//   virtual void generate(TextReader* script, Terminal* terminal)
//   {
//      _parser->parse(script, _compiler);
//   }
//
//   virtual size_t Position()
//   {
//      return _compiler->Position();
//   }
//
//   virtual void trim(size_t position)
//   {
//      _compiler->trim(position);
//   }
//
//   ParserWrapper(_Parser* parser, _CodeGeneator* compiler)
//   {
//      _parser = parser;
//      _compiler = compiler;
//      _nested = false;
//   }
//   ParserWrapper(_CodeGeneator* compiler)
//   {
//      _parser = NULL;
//      _compiler = compiler;
//      _nested = false;
//   }
//   virtual ~ParserWrapper()
//   {
//      if (_nested)
//         freeobj((ParserWrapper*)_compiler);
//   }
//};
//
//// --- InlineScriptParser ---
//
//class InlineScriptParser : public _CodeGeneator
//{
//   TapeWriter _writer;
//
//   void parseRole(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token, Terminal* terminal);
//   void parseVariable(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token);
//   void parseNewObject(_ELENA_TOOL_::TextSourceReader& source, wchar16_t* token, Terminal* terminal);
//
//public:
//   virtual void generate(TextReader* script, Terminal* terminal);
//
//   void close()
//   {
//   }
//};

} // _ELENA_

#endif // cfparserH
