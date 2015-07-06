#define system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(type:dirty_ptr)aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.

        ^ aCommandLinePtr wide.
    ]
}.

#symbol commandLine = 
    $CommandLineFactory new.

// --- startUp ---

#symbol startUp =
{
    #method start
    [
        'program eval
            | on:
            {
                Abort = e [].
                
                ! = e [ console writeLine:(e literal). ].
            }
    ]
}.
