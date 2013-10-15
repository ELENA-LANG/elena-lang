//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Compiler
//               
//		This header contains ELENA Parser table class declaration.
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef parserTableH
#define parserTableH 1

namespace _ELENA_
{

// --- ParserTable class ---

class ParserTable
{
public:
// --- Build-in constants ---
   const static int mskTerminal = 0x2000;
   const static int nsEps       = 2;

private:
   SymbolMap  _symbols;
   SyntaxHash _syntax;
   TableHash  _table;

public:
   const wchar16_t* retrieveSymbol(int symbol)
   {
      return retrieveKey(_symbols.start(), symbol, DEFAULT_STR);
   }

   bool registerSymbol(int symbol, const wchar16_t* value);
   void registerRule(int l_symbol, int* r_symbols, size_t length);

   int generate();   

   void load(StreamReader* reader);
   void save(StreamWriter* writer); 
	
   int defineSymbol(const wchar16_t* terminal);

   bool read(int nonterminal, int terminal, ParserStack& derivationStack);

   ParserTable();
};

} // _ELENA_

#endif // parserTableH
