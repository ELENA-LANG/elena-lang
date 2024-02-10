//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This header contains Parser table class declaration.
//
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef PARSERTABLE_H
#define PARSERTABLE_H

#include "elena.h"

namespace elena_lang
{
   // --- Parser key constants ---
   constexpr parse_key_t pkStart          = 1;
   constexpr parse_key_t pkEps            = 2; 
   // NOTE : the keys below are special ones used to manipulate the tree construction
   constexpr parse_key_t pkClose          = 3; 
   constexpr parse_key_t pkDiscard        = 4; // NOTE: should be the last in the rule

   constexpr parse_key_t pkTraceble       = 0x01000;  // masks
   constexpr parse_key_t pkTerminal       = 0x02000;
   constexpr parse_key_t pkInjectable     = 0x0C000;
   constexpr parse_key_t pkRenaming       = 0x08000;
   constexpr parse_key_t pkAnySymbolMask  = 0x0F000;
   constexpr parse_key_t pkMaxKey         = 0x0FFFF;

   constexpr size_t cnSyntaxPower         = 0x0008;
   constexpr size_t cnTablePower          = 0x0010;

   constexpr size_t cnHashSize            = 0x0100;              // the parse table hash size

   // --- ParserOverflowError ---
   class ParserOverflowError : public ExceptionBase {};

   // --- ParserTable ---
   class ParserTable
   {
   public:
      typedef MemoryMap<ustr_t, parse_key_t, Map_StoreUStr, Map_GetUStr> SymbolMap;
      typedef /*MemoryHashTable*/Map<parse_key_t, parse_key_t> SyntaxHash;
      typedef /*MemoryHashTable*/Map<parse_key_t, parse_key_t> TableHash;
      typedef Stack<parse_key_t> ParserStack;

   private:
      SymbolMap  _symbols;
      SyntaxHash _syntax;
      TableHash  _table;

   public:
      bool registerNonterminal(parse_key_t key, ustr_t value)
      {
         return registerSymbol(key, value, false);
      }
      bool registerSymbol(parse_key_t& key, ustr_t value, bool terminalOne);

      bool registerRule(parse_key_t* members, size_t length);
      bool readRule(parse_key_t key, int index,  parse_key_t* members, size_t& length);

      bool checkRule(parse_key_t l_nonterminal);

      parse_key_t resolveSymbol(ustr_t value);
      parse_key_t resolveSymbolByIndex(int index);
      ustr_t resolveKey(parse_key_t key);

      Pair<parse_key_t, parse_key_t> generate();

      bool read(parse_key_t nonterminal, parse_key_t terminal, ParserStack& derivationStack);

      void save(StreamWriter* writer);
      void load(StreamReader* reader);

      void clearRules()
      {
         _syntax.clear();
      }

      ParserTable();
   };
}

#endif
