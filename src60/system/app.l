entry()
{
   try
   {
      forward program();
   }
   catch(Exception err) {
     console.writeLine(err)
   }
}

private entrySymbol
    = entry();

// --- startUpSymbols ---
#new __string startUpSymbols;

// #let startUpSymbols += distributed preloadedSymbols;
#let startUpSymbols += entrySymbol;

