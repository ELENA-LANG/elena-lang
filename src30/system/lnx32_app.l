#import system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new                                                                                                                                 
    [
        #var(byte:256)buf.
        #var(int)len.
        #var(int)argc := system'external core_getargc.

        #var args := Array new &int:argc.
        #var(int)i := 0.
        #loop (i < argc)?
        [
            len := system'external core_getarg &int:(i+1) &bytearray:buf &int:255.

            args setAt &int:i &object:(LiteralValue $new &int:0 &int:len &bytearray:buf).

            i += 1.
        ].

        ^ args.
    ]
}.

//#symbol commandLine 
//    = $CommandLineFactory new.

#symbol commandLineArguments
    = $CommandLineFactory new.

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
