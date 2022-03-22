//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//
//		This header contains ELENA Parser table class implementation.
//
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "parsertable.h"

using namespace elena_lang;

// --- key mapping routines ---
inline size_t simpleParseRule(parse_key_t key)
{
   return key;
}

typedef HashTable<parse_key_t, parse_key_t, simpleParseRule, cnHashSize> SymbolHash;
typedef ParserTable::SyntaxHash SyntaxHash;
typedef ParserTable::TableHash TableHash;
typedef ParserTable::ParserStack ParserStack;

// --- ParserTable ---

inline pos_t tableKey(parse_key_t nonterminal, parse_key_t terminal)
{
   return (nonterminal << cnTablePower) + terminal;
}

inline void nextKey(SyntaxHash::Iterator& it)
{
   pos_t key = it.key();
   while (!it.eof() && key == it.key())
      ++it;
}

inline void copySubSet(SymbolHash& sour, SymbolHash& dest, parse_key_t sourKey, parse_key_t destKey, bool& added)
{
   SymbolHash::Iterator it = sour.getIt(sourKey);
   while (!it.eof() && it.key() == sourKey) {
      added |= dest.add(destKey, *it, true);

      ++it;
   }
}
inline bool copySubSet(SyntaxHash& sour, TableHash& dest, parse_key_t sourKey, parse_key_t destKey)
{
   // check if the rule is ambigous
   if (dest.exist(destKey))
      return false;

   SyntaxHash::Iterator it = sour.getIt(sourKey);
   while (!it.eof() && it.key() == sourKey) {
      dest.add(destKey, *it);

      ++it;
   }
   return true;
}

inline void add2stack(size_t key, TableHash::Iterator& it, ParserStack& stack)
{
   parse_key_t symbol = *it;

   ++it;
   if (!it.eof() && it.key() == key)
      add2stack(key, it, stack);

   stack.push(symbol);
}

ParserTable::ParserTable()
   : _symbols(0), _syntax(0), _table(0)
{
}

bool ParserTable :: registerSymbol(parse_key_t& key, ustr_t value, bool terminalOne)
{
   if (key >= pkAnySymbolMask)
      throw ParserOverflowError();

   if (terminalOne)
      key |= pkTerminal;

   return _symbols.add(value, key, true);
}

parse_key_t ParserTable :: resolveSymbol(ustr_t value)
{
   return _symbols.get(value);
}

parse_key_t ParserTable :: resolveSymbolByIndex(int index)
{
   auto it = _symbols.start();
   while (index > 0 && !it.eof()) {
      ++it;
      index--;
   }

   return !it.eof() ? *it : 0;
}

ustr_t ParserTable :: resolveKey(parse_key_t key)
{
   return _symbols.retrieve<parse_key_t>(nullptr, key, [](parse_key_t target_value, ustr_t key, parse_key_t value) { return target_value == value; });
}

bool ParserTable :: registerRule(parse_key_t* members, size_t length)
{
   if (length < 2)
      return false;

   parse_key_t l_symbol = members[0];

   // validate existing rules
   parse_key_t syntax_key = (l_symbol << cnSyntaxPower) + 1;

   while (_syntax.exist(syntax_key)) syntax_key++;

   for (size_t i = 1; i < length; i++) {
      _syntax.add(syntax_key, members[i]);
   }

   return true;
}

bool ParserTable :: readRule(parse_key_t l_symbol, int index, parse_key_t* members, size_t& length)
{
   length = 0;

   parse_key_t syntax_key = (l_symbol << cnSyntaxPower) + index;

   for (auto it = _syntax.getIt(syntax_key); !it.eof(); it = _syntax.nextIt(syntax_key, it)) {
      members[length++] = *it;
   }

   return length > 0;
}

bool ParserTable :: checkRule(parse_key_t l_symbol)
{
   parse_key_t syntax_key = (l_symbol << cnSyntaxPower) + 1;

   return _syntax.exist(syntax_key);
}

parse_key_t ParserTable :: generate()
{
   SymbolHash FIRSTsets(0);
   SymbolHash FOLLOWsets(0);

   // creating FIRSTsets
   FIRSTsets.add(pkEps, pkEps);

   bool added = false;
   do {
      added = false;
      SyntaxHash::Iterator rule = _syntax.start();
      while (!rule.eof()) {
         if (test(*rule, pkTerminal)) {
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
      while (!rule.eof()) {
         if ((prevKey == rule.key()) && !test(prev, pkTerminal)) {
            if (test(*rule, pkTerminal)) {
               added |= FOLLOWsets.add(prev, *rule, true);
            }
            else copySubSet(FIRSTsets, FOLLOWsets, *rule, prev, added);

            if (FIRSTsets.exist(*rule, pkEps)) {
               copySubSet(FOLLOWsets, FOLLOWsets, *rule, prev, added);
            }
         }
         prevKey = rule.key();
         prev = *rule;

         rule++;

         //HOTFIX : skip postfix member
         while (!rule.eof() && test(*rule, pkInjectable))
            rule++;

         if ((rule.eof() || prevKey != rule.key()) && !test(prev, pkTerminal)) {        // the last member in the rule
            copySubSet(FOLLOWsets, FOLLOWsets, prevKey >> cnSyntaxPower, prev, added);
         }
      }
   } while (added);

   // generating Parse table
   for (SyntaxHash::Iterator rule = _syntax.start(); !rule.eof(); nextKey(rule)) {
      parse_key_t l_symbol = rule.key() >> cnSyntaxPower;

      if (test(*rule, pkTerminal)) {
         if (!copySubSet(_syntax, _table, rule.key(), tableKey(l_symbol, *rule)))
            return l_symbol;
      }
      else {
         SymbolHash::Iterator it = FIRSTsets.getIt(*rule);
         while (!it.eof() && it.key() == *rule) {
            if (*it == pkEps) {
               SymbolHash::Iterator f_it = FOLLOWsets.getIt(l_symbol);
               while (!f_it.eof() && f_it.key() == l_symbol) {
                  if (*f_it != pkEps) {
                     // check if the rule is ambigous
                     if (_table.exist(tableKey(l_symbol, *f_it)))
                        return l_symbol;

                     //_table.add(tableKey(l_symbol, *it), nsEps);
                     copySubSet(_syntax, _table, rule.key(), tableKey(l_symbol, *f_it));
                  }
                  ++f_it;
                  //HOTFIX : skip postfix member
                  while (test(*f_it, pkInjectable))
                     ++f_it;
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

bool ParserTable::read(parse_key_t nonterminal, parse_key_t terminal, ParserStack& derivationStack)
{
   if (nonterminal == pkEps)
      return true;                                // epsilon move
   else {
      parse_key_t key = tableKey(nonterminal, terminal);

      TableHash::Iterator it = _table.getIt(key);
      if (!it.eof()) {
         add2stack(key, it, derivationStack);

         return true;
      }
      else return false;
   }
}

void ParserTable :: save(StreamWriter* writer)
{
   // count terminals
   pos_t terminalCount = _symbols.sum<int>(0, [](parse_key_t key)
   {
      return test(key, pkTerminal) ? 1 : 0;
   });

   writer->writePos(terminalCount);
   _symbols.forEach<StreamWriter*>(writer, [](StreamWriter* writer, ustr_t symbol, parse_key_t key)
   {
      if (test(key, pkTerminal)) {
         writer->write(&key, sizeof(key));
         writer->writeString(symbol, symbol.length_pos() + 1);
      }
   });

   writer->writePos(_table.count());
   _table.forEach<StreamWriter*>(writer, [](StreamWriter* writer, parse_key_t key, parse_key_t member)
   {
      writer->write(&key, sizeof(key));
      writer->write(&member, sizeof(member));
   });
}

void ParserTable :: load(StreamReader* reader)
{
   IdentifierString buffer;

   pos_t symbolCount = reader->getPos();
   for (pos_t i = 0; i < symbolCount; i++) {
      parse_key_t key = 0;
      reader->read(&key, sizeof(key));
      reader->readString(buffer);

      _symbols.add(buffer.str(), key);
   }

   pos_t tableCount = reader->getPos();
   for (pos_t i = 0; i < tableCount; i++) {
      parse_key_t key = 0;
      reader->read(&key, sizeof(key));

      parse_key_t member = 0;
      reader->read(&member, sizeof(member));

      _table.add(key, member);
   }
}
