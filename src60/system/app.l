#if (__project["_Win"]) // Windows

singleton CommandLineFactory
{
    wide load()
    {
        pointer commandLinePtr := extern KERNEL32.GetCommandLineW();

        ^ cast wide(commandLinePtr);
    }
}

public CommandLine = 
    CommandLineFactory.load();

#else // Linux & FreeBSD

// --- CommandLine ---

singleton CommandLineFactory
{
    load()
    {
        byte buf[256];
        int len := 0;
        int argc := extern GetArgCLA();

        var args := Array.allocate(argc);
        for (int i := 0; i < argc; i++) {
            len := extern GetArgLA(i, buf, 255);
            
            args[i] := String.fromByteArray(0, len, buf)
        };

        ^ args
    }
}

public CommandLineArguments =
    CommandLineFactory.load();

#endif

// --- startUp ---

public sealed class StartUpEvents
{
   Func OnStop : event;
    
   Func1 OnError : event;
    
   internal constructor new() {}

   internal stopping()
   {
      OnStop?.();
   }
    
   internal handlingError(e)
   {
      OnError?.(e)
   }
}

public static StartUpEvents startUpEvents = StartUpEvents.new();

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

         Console.writeLine(err);

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
