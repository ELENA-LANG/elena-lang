// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(dirty_ptr)aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
//        aCommandLinePtr set &int:(system'external'KERNEL32 GetCommandLineW).

        ^ aCommandLinePtr wide.
    ]
}.

#symbol commandLine = 
    $CommandLineFactory new.

// --- startUp ---

#class $StartUpEvents
{
    #field theFinializeEvent.
    
    #method set &onAppEnd:aHandle
    [
        ($nil == theFinializeEvent)
            ? [ theFinializeEvent := aHandle. ]
            ! [ theFinializeEvent := Function1Group new &args:(theFinializeEvent,aHandle). ].
    ]
    
    #method $onStop : arg
    [
        ($nil == theFinializeEvent)
            ! [ theFinializeEvent eval:arg ].
    ]
}

#static startUpEvents = $StartUpEvents new.

#symbol startUp = 
    {
        #method start : events
        [
            #var s := 'program.
            
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
