// ==== ver 3.4.1 ===

#define system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    #method(stacksafe) $getArgC &vint:aLength
       => system'internal'lnx_getargc.

    #method(stacksafe) $getArg &int:anIndex &bytearray:aBuffer &vint:aLength
       => system'internal'lnx_getarg.

    new                                                                                                                                 
    [
        #var(type:bytearray,size:256)buf.
        #var(type:int)len.
        #var(type:int)argc.
        $self $getArgC &vint:argc.

        #var args := Array new &int:argc.
        #var(type:int)i := 0.
        #loop (i < argc)?
        [
            len := 256.
            $self $getArg &int:(i + 1) &bytearray:buf &vint:len.

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
