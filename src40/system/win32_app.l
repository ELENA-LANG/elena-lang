// --- CommandLine ---

singleton CommandLineFactory
{
    wide load()
    {
        dirty_ptr commandLinePtr := extern KERNEL32.GetCommandLineW();

        ^ cast wide(commandLinePtr);
    }
}

public commandLine = 
    CommandLineFactory.load();

// --- startUp ---

class StartUpEvents
{
    event Func OnStop;
    
    internal stopping()
    {
        ifnot (nil == OnStop)
            { OnStop() }
    }
}

public static startUpEvents = new StartUpEvents();

entry()
{
    intern system'core_routines'__exception.register();
    
    try
    {
        forward program()
    }
    finally
    {
       startUpEvents.stopping()
    }
    catch:(AbortException e)
    {
    }   
    catch:(Exception e)
    {
        //console.writeLine(e.Printable);
        
        extern elenart.Exit(-1);
    }
}

public symbol startUp
    = entry();