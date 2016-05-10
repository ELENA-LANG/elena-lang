// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(dirty_ptr)aCommandLinePtr.
        aCommandLinePtr set &int:(system'external'KERNEL32 GetCommandLineW).

        ^ aCommandLinePtr wide.
    ]
}.

#symbol commandLine = 
    $CommandLineFactory new.

// --- startUp ---

#class StartUp
{
    #field theFinializeEvent.

    #method set &onAppEnd:aHandle
    [
        ($nil == theFinializeEvent)
            ? [ theFinializeEvent := aHandle. ]
            ! [ theFinializeEvent := Function1Group new &args:theFinializeEvent:aHandle. ].
    ]
    
    #method start
    [
        'program eval
            | on:
            {
                Abort = e [].
                
                ! = e [ console writeLine:(e literal). ].
            }.

        $self $onStop:$nil.
    ]
    
    #method $onStop : arg
    [
        ($nil == theFinializeEvent)
            ! [ theFinializeEvent eval:arg ].
    ]
}

#static startUp = StartUp new.
