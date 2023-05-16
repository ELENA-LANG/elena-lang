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
