#define system.

// --- LRLN ---

#symbol(const) newLine
    = "%r%n".

// --- $ConsoleHandleFactory --

#symbol $ConsoleHandleFactory =
{
    new &int_attributes:anAttributes
    [ 
        #var(type:handle)aHandle := system'external'KERNEL32 GetStdHandle &int:anAttributes.
        
        ^ Handle new &handle:aHandle.
    ]
}.

// --- ConsoleWriter ---

#class $ConsoleWriter
{
    #field(type:handle) theHandle.

    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method write &literal : aLiteral
    [
        #var(type:int)aLength := aLiteral length.
        
        system'external'KERNEL32 WriteConsoleW
            &handle  :  theHandle
            &literal :  aLiteral
            &int     :  aLength
            &int     :  0                       // !! temporally we do not use it
            &int     :  0.             

        // !!check the result of the operation
    ]
    
    #method close
    [
        theHandle close.
    ]
}

// --- ConsoleReader ---

#class(sealed) $ConsoleReader
{
    #field(type:handle) theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method read &out'short:aChar
    [
        #var(type:bytearray,size:20)aDump.
        #var(type:int)aNumberRead.
        
        #var found := false.
        #loop found !
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        : theHandle
                &out'bytearray : aDump
                &int           : 1
                &out'int       : aNumberRead.

            found := (aDump equal &index:0 &short:1)and:(aDump equal &index:4 &int:1).
        ].

        aDump read &index:14 &out'short:aChar.
    ]

    #method read &out'int:aKey
    [
        #var(type:bytearray,size:20)aDump.
        #var(type:int)aNumberRead.
        
        #var found := false.
        #loop found !
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        : theHandle
                &out'bytearray : aDump
                &int           : 1
                &out'int       : aNumberRead.

            found := (aDump equal &index:0 &short:1)and:(aDump equal &index:4 &int:1).
        ].
                    
        aDump read &index:8 &out'int:aKey.
    ]

    #method read &shortarray:aBuffer &length:aLength &out'length:aRetVal
    [
        system'external'KERNEL32 ReadConsoleW
            &handle     : theHandle
            &shortarray : aBuffer
            &int        : aLength
            &out'length : aRetVal
            &int        : 0.
    ]
    
    #method read &line
    [
        #var(type:shortarray,size:127)aBuffer.
        #var(type:int)aLength.
        #var anOutput := emptyLiteralValue.
        
        #var(type:bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &shortarray:aBuffer &length:aLength &out'length:aLength.
            
            aBuffer equal &index:(aLength - 1) &short:0Ah
                ? [
                    aLength := aLength - 2.
                    
                    continue := false.
                ].
            
            #var aTemp := LiteralValue new &length:aLength &shortarray:aBuffer.
            
            anOutput := anOutput + aTemp.
        ].

        ^ anOutput.
    ]

    #method int
    [
        #var(type:int)aRetVal.
        $self read &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
            
    #method short
    [
        #var(type:short)aRetVal.
        $self read &out'short:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]
            
    #method close
    [
        theHandle close.
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new &handle::($ConsoleHandleFactory new &int_attributes:0FFFFFFF5h).

#symbol stdConsoleReader = $ConsoleReader new &handle::($ConsoleHandleFactory new &int_attributes:0FFFFFFF6h).
