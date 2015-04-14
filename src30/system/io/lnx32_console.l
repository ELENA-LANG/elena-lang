#define system.

// --- ConsoleWriter ---

#class $ConsoleWriter
{
    #constructor new
    [
    ]

    #method write &wide:aLiteral
    [
        $self write &literal:(aLiteral literal).
    ]

    #method write &literal : aLiteral
    [
        #var(type:int)n := 0.
        #var(type:vint)l.
        #var(type:vbyte)vc.

        aLiteral readLength &vint:l.
        #loop (n < l)?
        [
            aLiteral read &index:n &vbyte:vc.

            system'external'libc putchar &byte:vc.

            n := n + 1.
        ].

        // !!check the result of the operation
    ]
    
    #method close
    [
    ]
}

// --- ConsoleReader ---

#class $ConsoleReader
{
    #constructor new
    [
    ]

    #method read &vbyte:aRetVal
    [    
        #var(type:byte) ch := system'external'libc getchar.

        aRetVal << ch.
    ]

    #method read &bytearray:aBuffer &vlength:aRetVal
    [
        #var(type:int) i := 0.
        #loop (aRetVal > i) ?
        [
            #var(type:vbyte)ch.
            $self read &vbyte:ch.
               
            aBuffer write &index:i &byte:ch.

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
        #var(type:vlength)aLength.
        #var anOutput := emptyLiteralValue.
        
        #var(type:bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &bytearray:aBuffer &vlength:aLength.
            
            #var(type:vbyte)ch.
            aBuffer read &index:(aLength - 1) &vbyte:ch.
            (ch == 0Ah)
                ? [
                    aLength := aLength - 1.
                    
                    continue := false.
                ].
            
            #var aTemp := LiteralValue $new &length:aLength &index:0 &bytearray:aBuffer.
            
            anOutput := anOutput + aTemp.
        ].

        ^ anOutput.
    ]

  /*  
    #method int
    [
        #var(type:int)aRetVal := $self int.
        
        ^ IntNumber new &int:aRetVal.
    ]
*/ 
    #method read &vchar:aRetVal
    [
       #var(type:vbyte)ch.
       $self read &vbyte:ch.

       charConvertor convert &byte:ch &vchar:aRetVal.  
    ]
           
    #method char
    [
        #var(type:vchar)aRetVal.
        $self read &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]

    #method close
    [
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new.

#symbol stdConsoleReader = $ConsoleReader new.
