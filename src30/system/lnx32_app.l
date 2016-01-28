// ==== ver 3.4.1 ===

#define system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new                                                                                                                                 
    [
        #var(type:bytearray,size:256)buf.
        #var(type:int)len.
        #var(type:int)argc := system'external core_getargc.

        #var args := Array new &int:argc.
        #var(type:int)i := 0.
        #loop (i < argc)?
        [
            len := system'external core_getarg &int:(i+1) &bytearray:buf &int:255.

            args setAt &int:i &object:(LiteralValue $new &int:0 &int:len &bytearray:buf).

            i := i + 1.
        ].

        ^ args.
    ]
}.

//#symbol commandLine 
//    = $CommandLineFactory new.

#symbol commandLineArguments
    = $CommandLineFactory new.

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
