// --- CommandLine ---

class $CommandLineFactory =
{
    new
    [
        dirty_ptr aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.

        ^ aCommandLinePtr wide.
    ]
}.

symbol commandLine = 
    $CommandLineFactory new.

// --- startUp ---

class $StartUpEvents
{
    func1 onStop :: event(theFinializeEvent).
    
    $onStop : arg
    [
        ifnot ($nil == theFinializeEvent)
            [ theFinializeEvent eval:arg ]
    ]
}

static startUpEvents = $StartUpEvents new.

symbol startUp = 
    {
        start : events
        [
            try('program eval)
                {
                    Abort : e []
                    
                    generic : e [ console writeLine:(e literal) ]
                }.
    
            events $onStop:$nil
        ]        
    }
    start:startUpEvents.
