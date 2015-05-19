#define system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    $getArgC &vint:aLength
       => system'internal'lnx_getargc.

    $getArg &int:anIndex &bytearray:aBuffer &vint:aLength
       => system'internal'lnx_getarg.

    new                                                                                                                                 
    [
        #var(type:bytearray,size:256)buf.
        #var(type:int)len.
        #var(type:int)argc.
        $self $getArgC &vint:argc.

        #var args := Array new &length:argc.
        #var(type:int)i := 0.
        #loop (i < argc)?
        [
            len := 256.
            $self $getArg &int:(i + 1) &bytearray:buf &vint:len.

            args setAt &index:i &object:(LiteralValue $new &length:len &index:0 &bytearray:buf).

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

#symbol startUp =
{
    #method start
    [
        'program eval
            | if &Error: e
            [
                console writeLine:(e literal).
            ].
    ]
}.
