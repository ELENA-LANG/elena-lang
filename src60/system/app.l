entry()
{
   try
   {
      forward program();
   }
   catch::
   {
      function(AbortException e)
      {
      }
    
      function(Exception err)
      {
         console.writeLine(err);

         extern ExitLA(-1);
      }
   }
}

private entrySymbol
    = entry();
