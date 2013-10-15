// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var aCommandLinePtr := IntNumber new &type'int:(system'external'KERNEL32 GetCommandLineW).
        #var aCommandLineLen := IntNumber new &type'int:(system'external'KERNEL32 lstrlenW &type'int:aCommandLinePtr).
        #var aCommandLine := system'external'kernel32 lstrcpyW 
            &out'type'widestr:(LiteralValue new &type'length:aCommandLineLen)
            &type'int:aCommandLinePtr.
            
        ^ aCommandLine.
    ]
}.

#symbol CommandLine 
    = $CommandLineFactory new.
