#define system.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(type:int)aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
        #var(type:int)aCommandLineLen := system'external'KERNEL32 lstrlenW &int:aCommandLinePtr.
        #var(type:chararray) aBuffer := CharArray new &length:aCommandLineLen.
        
        system'external'kernel32 lstrcpyW 
            &chararray:aBuffer
            &int:aCommandLinePtr.
            
        ^ LiteralValue new &length:aCommandLineLen &chararray:aBuffer .
    ]
}.

#symbol commandLine 
    = $CommandLineFactory new.
