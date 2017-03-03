// --- CommandLine ---

class $CommandLineFactory =
{
    new
    [
        dirty_ptr aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
//        aCommandLinePtr set &int:(system'external'KERNEL32 GetCommandLineW).

        ^ aCommandLinePtr wide.
    ]
}.

symbol commandLine = 
    $CommandLineFactory new.

// --- startUp ---

class $StartUpEvents
{
    object theFinializeEvent.
    
    set &onAppEnd:aHandle
    [
        ($nil == theFinializeEvent)
            ? [ theFinializeEvent := aHandle. ]
            /*! [ theFinializeEvent := Function1Group new &args:(theFinializeEvent,aHandle). ]*/.
    ]
    
    $onStop : arg
    [
        ($nil == theFinializeEvent)
            ! [ theFinializeEvent eval:arg ].
    ]
}

static startUpEvents = $StartUpEvents new.

symbol startUp = 
    {
        start : events
        [
            'program eval
                | on:
                {
                    Abort = e [].
                    
                    ! = e [ console writeLine:(e literal). ].
                }.
    
            events $onStop:$nil.
        ]        
    }
    start:startUpEvents.
