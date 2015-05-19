#define system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(type:int)aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
        #var(type:int)aCommandLineLen := system'external'KERNEL32 lstrlenW &int:aCommandLinePtr.
        #var(type:shortarray) aBuffer := ShortArray new &length:aCommandLineLen.
        
        system'external'kernel32 lstrcpyW 
            &shortarray:aBuffer
            &int:aCommandLinePtr.
            
        ^ WideLiteralValue $new &length:aCommandLineLen &index:0 &shortarray:aBuffer .
    ]
}.

#symbol commandLine 
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
