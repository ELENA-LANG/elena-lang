// ==== ver 3.4.1 ===

#define system.

// --- $globalVariables ---

#symbol $globalVariables =
{
    stdout
    [
       #var(type:dirty_ptr)aVariable.
       #var(type:handle)aLibHandle := system'external'libdl dlopen 
                                          &literal:"libc.so.6"
                                          &int:1.

       aVariable := system'external'libdl dlsym
                        &int:aLibHandle  
                        &literal:"stdout".

       #var(type:int)aHandle.
       aVariable read &vint:aHandle.

       ^ Handle new &int:aHandle.  
    ]
}.

// --- ConsoleWriter ---

#class(sealed) $ConsoleWriter
{
    #field(type:handle) theHandle.

    #constructor new &handle:aHandle
    [
       theHandle := aHandle. 
    ]

    #method write &wide:aLiteral
    [
        $self write &literal:(aLiteral literal).
    ]

    #method write &literal : aLiteral
    [
        #var(type:int)n := 0.
        #var(type:int)l.
        #var(type:byte)vc.

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

#class(sealed) $ConsoleReader
{
    #constructor new
    [
    ]

    #method(stacksafe) read &vbyte:aRetVal
    [    
        #var(type:int) ch := system'external'libc getchar.

        byteConvertor convert &int:ch &vbyte:aRetVal. 
    ]

    #method(stacksafe) read &bytearray:aBuffer &vint:aRetVal
    [
        #var(type:int) i := 0.
        #loop (aRetVal > i) ?
        [
            #var(type:byte)ch.
            $self read &vbyte:ch.

            aBuffer write &int:i &byte:ch.

            i := i + 1.

            (ch == 0Ah)
               ? [ aRetVal << i.  ^ $self. ].
        ].

        aRetVal << i.
    ]

    #method read &wide
        = $self read &literal wide.

    #method read &literal
    [
        #var(type:bytearray,size:127)aBuffer.
        #var(type:int)aLength.
        #var anOutput := emptyLiteralValue.
        
        #var(type:bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &bytearray:aBuffer &vint:aLength.

            #var(type:byte)ch.
            aBuffer read &int:(aLength - 1) &vbyte:ch.

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
       #var(type:byte)ch.
       $self read &vbyte:ch.

       charConvertor convert &byte:ch &vchar:aRetVal.  
    ]
          
    #method char
    [
        #var(type:char)aRetVal.
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
