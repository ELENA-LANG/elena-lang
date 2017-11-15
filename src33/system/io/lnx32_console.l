#import system.

// --- $globalVariables ---

#symbol $globalVariables =
{
    #method(handle) stdout
    [
       #var(dirty_ptr)aVariable.
       #var(handle)aLibHandle := system'external'libdl dlopen 
                                          &literal:"libc.so.6"
                                          &int:1.

       aVariable := system'external'libdl dlsym
                        &int:aLibHandle  
                        &literal:"stdout".

       #var(int)aHandle.
       aVariable read &vint:aHandle.

       ^ aHandle.  
    ]
}.

// --- ConsoleWriter ---

#class(sealed,struct) $ConsoleWriter
{
    #field(handle) theHandle.

    #constructor(stacksafe) new &handle:aHandle
    [
       theHandle := aHandle. 
    ]

    #method write &wide:aLiteral
    [
        $self write &literal:(aLiteral literal).
    ]

    #method write &literal : aLiteral
    [
        #var(int)n := 0.
        #var(int)l.
        #var(byte)vc.

        aLiteral readLength &vint:l.
        #loop (n < l)?
        [
            aLiteral read &int:n &vbyte:vc.

            system'external'libc putchar &byte:vc.

            n := n + 1.
        ].

        // !!check the result of the operation
    ]
    
    #method clear
    [
    ]

    #method refresh
    [
        system'external'libc fflush &int:theHandle.
    ]
            
    #method free
    [
    ]
}

// --- ConsoleReader ---

#class(sealed,embeddable) $ConsoleReader
{
    #constructor new
    [
    ]

    #method(stacksafe) read &vbyte:aRetVal
    [    
        #var(int) ch := system'external'libc getchar.

        byteConvertor convert &int:ch &vbyte:aRetVal. 
    ]

    #method(stacksafe) read &bytearray:aBuffer &vint:aRetVal
    [
        #var(int) i := 0.
        #loop (aRetVal > i) ?
        [
            #var(byte)ch.
            $self read &vbyte:ch.

            aBuffer@i := ch.

            i += 1.

            (ch == 0Ah)
               ? [ aRetVal := i.  ^ $self. ].
        ].

        aRetVal := i.
    ]

    #method read &wide
        = $self read &literal wide.

    #method read &literal
    [
        #var(byte:127)aBuffer.
        #var(int)aLength.
        #var anOutput := emptyLiteralValue.
        
        #var(bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &bytearray:aBuffer &vint:aLength.

            #var(byte)ch := aBuffer@(aLength - 1).

            (ch == 0Ah)
                ? [
                    aLength := aLength - 1.
                    
                    continue := false.
                ].
            
            #var aTemp := LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
            
            anOutput := anOutput + aTemp.
        ].

        ^ anOutput.
    ]

    #method read &vchar:aRetVal
    [
       #var(byte)ch.
       $self read &vbyte:ch.

       charConvertor convert &byte:ch &vchar:aRetVal.  
    ]
          
    #method char
    [
        #var(char)aRetVal.
        $self read &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]

    #method close
    [
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new &handle:($globalVariables stdout).

#symbol stdConsoleReader = $ConsoleReader new.
