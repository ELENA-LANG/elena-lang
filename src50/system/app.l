// --- startUp ---

class StartUpEvents
{
    event Func OnStop;
    
    event Func1 OnError;
    
    internal stopping()
    {
        ifnot (nil == OnStop)
            { OnStop() }
    }
    
    internal handlingError(e)
    {
        ifnot (nil == OnError)
            {   OnError(e) }
    }
}

public static StartUpEvents startUpEvents = new StartUpEvents();

public onConsoleStart()
{
    startUpEvents.OnError := (e){ console.writeLine(e.toPrintable()); }
}

entry()
{
    intern system'core_routines'__exception.register();
    
    forward onStart();
    
    try
    {
        forward program()
    }
    finally
    {
       startUpEvents.stopping()
    }
    catch::
    {
        function(AbortException e)
        {
        }
        
        function(Exception e)
        {
            startUpEvents.handlingError(e);
            
            extern elenart.Exit(-1);
        }
    }
}

public symbol startUp
    = entry();