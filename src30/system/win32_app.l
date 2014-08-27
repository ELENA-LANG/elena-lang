#define system.

// --- referenceTableControl ---

#symbol referenceTableControl =
{
    allocate : anObject &out'int:anIndex
        <api_alloc_index>.

    free &int:anIndex
        <api_free_index>.
}.

// --- CommandLine ---

#symbol $CommandLineFactory =
{
    new
    [
        #var(type:int)aCommandLinePtr := system'external'KERNEL32 GetCommandLineW.
        #var(type:int)aCommandLineLen := system'external'KERNEL32 lstrlenW &int:aCommandLinePtr.
        #var aBuffer := ShortArray new &length:aCommandLineLen.
        
        system'external'kernel32 lstrcpyW 
            &shortarray::aBuffer
            &int:aCommandLinePtr.
            
        ^ LiteralValue new &length:aCommandLineLen &shortarray::aBuffer .
    ]
}.

#symbol CommandLine 
    = $CommandLineFactory new.
