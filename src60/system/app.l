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

gui_entry()
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
         forward program.error("ELENA", err);

         extern ExitLA(-1);
      }
   }
}

private entrySymbol
    = entry();

private guiEntrySymbol
    = gui_entry();
