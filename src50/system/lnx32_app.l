// --- CommandLine ---

singleton CommandLineFactory
{
    string load()
    {
/*        byte buf[256];
        int len := 0;
        int argc := extern core_getargc;

        var args := Array.allocate(argc);
        for (int i := 0, i < argc, i := i + 1) {
            len := extern core_getarg(i+1, buf, 255);
            
            args[i] := String.fromByteArray(0, len, buf)
        };

        ^ args*/
    }
}

//#symbol commandLine 
//    = $CommandLineFactory new.

public commandLineArguments = 
    CommandLineFactory.load();

//// --- startUp ---
//
//class StartUpEvents
//{
//    event Func OnStop;
//    
//    internal stopping()
//    {
//        ifnot (nil == OnStop)
//            { OnStop() }
//    }
//}
//
//public static startUpEvents = new StartUpEvents();

entry()
{
//    intern system'core_routines'__exception.register();
    
    try
    {
        forward program()
    }
//    finally
//    {
//       startUpEvents.stopping()
//    }
    catch::
    {
        function(AbortException e)
        {
        }
        
        function(Exception e)
        {
            console.writeLine(e.toPrintable());
            
            extern elenart.Exit(-1);
        }
    }
}

public symbol startUp
    = entry();