//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Parser table class implementation.
//
//                                              (C)2005-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "parsertable.h"

using namespace _ELENA_ ;

typedef HashTable<int, int, simpleRule, cnHashSize> SymbolHash;

// --- ParserTable's hash table auxiliary routines ---

inline pos_t tableKey(int nonterminal, int terminal)
{
   return (nonterminal << cnTablePower) + terminal;
}

inline void nextKey(SyntaxHash::Iterator& it)
{
   pos_t key = it.key();
   while (!it.Eof() && key == it.key())
      it++;
}

inline void add2stack(size_t key, TableHash::Iterator& it, Stack<int>& stack)
{
   pos_t symbol = *it;

   it++;
   if (!it.Eof() && it.key()==key)
      add2stack(key, it, stack);

   stack.push(symbol);
}

inline void copySubSet(SymbolHash& sour, SymbolHash& dest, int sourKey, int destKey, bool& added)
{
   SymbolHash::Iterator it = sour.getIt(sourKey);
   while (!it.Eof() && it.key() == sourKey) {
      added |= dest.add(destKey, *it, true);

      it++;
   }
}

inline bool copySubSet(SyntaxHash& sour, TableHash& dest, pos_t sourKey, pos_t destKey)
{
   // check if the rule is ambigous
   if (dest.exist(destKey))
      return false;

   SyntaxHash::Iterator it = sour.getIt(sourKey);
   while (!it.Eof() && it.key() == sourKey) {
      dest.add(destKey, *it);

      it++;
   }
   return true;
}

// --- ParserTable class ---

ParserTable :: ParserTable()
   : _symbols(0), _table(0), _syntax(0)
{
}

int ParserTable :: defineSymbol(ident_t terminal)
{
   return _symbols.get(terminal);
}

bool ParserTable :: registerSymbol(int symbol, ident_t value)
{
   return _symbols.add(value, symbol, true);
}

void ParserTable :: registerRule(int l_symbol, int* r_symbols, size_t length)
{
   int key = (l_symbol << cnSyntaxPower) + 1;

   while (_syntax.exist(key)) key++;

   for (size_t i = 0 ; i < length ; i++) {
      _syntax.add(key, r_symbols[i]);
   }
}

bool ParserTable :: read(int nonterminal, int terminal, Stack<int>& stack)
{
   if (nonterminal == nsEps)
      return true;                                // epsilon move
   else {
      int key = tableKey(nonterminal, terminal);

      TableHash::Iterator it = _table.getIt(key);
      if (!it.Eof()) {
         add2stack(key, it, stack);

         return true;
      }
      else return false;
   }
}

// method returns rule id if the syntax ambigous or zero
int ParserTable :: generate()
{
   SymbolHash FIRSTsets(0);
   SymbolHash FOLLOWsets(0);

  // creating FIRSTsets
   FIRSTsets.add(nsEps, nsEps);

   bool added = false;
   do {
      added = false;
      SyntaxHash::Iterator rule = _syntax.start();
      while (!rule.Eof()) {
         if (test(*rule, mskTerminal)) {
            added |= FIRSTsets.add(rule.key() >> cnSyntaxPower, *rule, true);
         }
         else copySubSet(FIRSTsets, FIRSTsets, *rule, rule.key() >> cnSyntaxPower, added);

         nextKey(rule);
      }

   } while (added);

  // creating FOLLOWsets
   do {
      added = false;

      SyntaxHash::Iterator rule = _syntax.start();
      pos_t prevKey = 0;
      pos_t prev = 0;
      while (!rule.Eof()) {
         if ((prevKey == rule.key()) && !test(prev, mskTerminal)) {
            if (test(*rule, mskTerminal)) {
               added |= FOLLOWsets.add(prev, *rule, true);
            }
            else copySubSet(FIRSTsets, FOLLOWsets, *rule, prev, added);

            if (FIRSTsets.exist(*rule, nsEps)) {
               copySubSet(FOLLOWsets, FOLLOWsets, *rule, prev, added);
            }
         }
         prevKey = rule.key();
         prev = *rule;

         rule++;

         if ((rule.Eof() || prevKey != rule.key()) && !test(prev, mskTerminal)) {        // the last member in the rule
            copySubSet(FOLLOWsets, FOLLOWsets, prevKey >> cnSyntaxPower, prev, added);
         }
      }
   } while (added);

  // generating Parse table
   for (SyntaxHash::Iterator rule = _syntax.start() ; !rule.Eof() ; nextKey(rule)) {
      int l_symbol = rule.key() >> cnSyntaxPower;

      if (test(*rule, mskTerminal)) {
		   if (!copySubSet(_syntax, _table, rule.key(), tableKey(l_symbol, *rule)))
            return l_symbol;
      }
      else {
         SymbolHash::Iterator it = FIRSTsets.getIt(*rule);
         while (!it.Eof() && it.key() == *rule) {
            if (*it == nsEps) {
               SymbolHash::Iterator f_it = FOLLOWsets.getIt(l_symbol);
               while (!f_it.Eof() && f_it.key() == l_symbol) {
                  if (*f_it != nsEps) {
                     // check if the rule is ambigous
                     if (_table.exist(tableKey(l_symbol, *f_it)))
                        return l_symbol;

                     //_table.add(tableKey(l_symbol, *it), nsEps);
                     copySubSet(_syntax, _table, rule.key(), tableKey(l_symbol, *f_it));
                  }
                  f_it++;
               }
            }
            else {
               if (!copySubSet(_syntax, _table, rule.key(), tableKey(l_symbol, *it)))
                  return l_symbol;
            }
            it++;
         }
      }
   }
   return 0;
}

void ParserTable :: load(StreamReader* reader)
{
  // load terminals
   _symbols.read(reader);

  // load table
   _table.read(reader);
}

void ParserTable :: save(StreamWriter* writer)
{
  // save only terminal symbols
   SymbolMap  terminals;
   SymbolMap::Iterator it = _symbols.start();
   while (!it.Eof()) {
      if (test(*it, mskTerminal)) {
         terminals.add(it.key(), *it);
      }
      it++;
   }
   terminals.write(writer);
   //_symbols.write(writer);

  // save table
   _table.write(writer);
}
