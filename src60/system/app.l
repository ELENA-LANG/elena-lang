// --- startUp ---

public sealed class StartUpEvents
{
   Func OnStop : event;
    
   Func1 OnError : event;
    
   internal constructor() {}

   internal stopping()
   {
      OnStop?.();
   }
    
   internal handlingError(e)
   {
      OnError?.(e)
   }
}

public static StartUpEvents startUpEvents = new StartUpEvents();

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
         startUpEvents.handlingError(err);

         console.writeLine(err);

         extern ExitLA(-1);
      }
   }
   finally
   {
      startUpEvents.stopping()
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
         startUpEvents.handlingError(err);

         forward program.error("ELENA", err);

         extern ExitLA(-1);
      }
   }
   finally
   {
      startUpEvents.stopping()
   }
}

private entrySymbol
    = entry();

private guiEntrySymbol
    = gui_entry();
