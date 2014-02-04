// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(type:int)aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
        #var(type:int)aCommandLineLen := system'external'KERNEL32 lstrlenW &int:aCommandLinePtr.
        #var aCommandLine := LiteralValue new &length:aCommandLineLen.
        
        system'external'kernel32 lstrcpyW 
            &out'literal::aCommandLine
            &int:aCommandLinePtr.
            
        ^ aCommandLine.
    ]
}.

#symbol CommandLine 
    = $CommandLineFactory new.
