import system'threading.

// --- threadControl ---

public singleton threadControl
{
    start(Func f)
    [
        var aThread := Thread new(f).
        
        aThread start.
        
        ^ aThread.
    ]
    
    sleep (IntNumber n)
    [
        @system'threading'threadControl sleep(n).
    ]
}
