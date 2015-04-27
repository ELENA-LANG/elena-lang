#define system.

// --- $ConsoleHandleFactory --

#symbol $ConsoleHandleFactory =
{
    #method(type:handle) new &int_attributes:anAttributes
    [ 
        #var(type:handle)aHandle := system'external'KERNEL32 GetStdHandle &int:anAttributes.
        
        ^ Handle new &handle:aHandle.
    ]
}.

// --- ConsoleWriter ---

#class(struct:embeddable) $ConsoleWriter
{
    #field(type:handle) theHandle.

    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method write &literal:aLiteral
    [
        $self write &wide:(aLiteral wide).
    ]

    #method write &wide : aLiteral
    [
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.

        system'external'KERNEL32 WriteConsoleW
            &handle  :  theHandle
            &wide    :  aLiteral
            &int     :  aLength
            &int     :  0                       // !! temporally we do not use it
            &int     :  0.             

        // !!check the result of the operation
    ]
    
    #method free
    [
        system'external'KERNEL32 CloseHandle
            &handle:theHandle.
    ]
}

// --- ConsoleReader ---

#class(sealed,struct:embeddable) $ConsoleReader
{
    #field(type:handle) theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method read &vchar:aChar
    [
        #var(type:bytearray,size:20)aDump.
        #var(type:int)aNumberRead.
        #var(type:short)anEventType.
        #var(type:int)aKeyDown.
        
        #var found := false.
        #loop found !
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        : theHandle
                &bytearray     : aDump
                &int           : 1
                &vint          : aNumberRead.

            aDump read &index:0 &vshort:anEventType.
            (anEventType == 1)
                ? [  
                    aDump read &index:4 &vint:aKeyDown.
                    
                    found := aKeyDown == 1.
                ]
                ! [ found := false. ].
        ].

        aDump read &index:14 &vchar:aChar.
    ]

    #method read &vint:aKey
    [
        #var(type:bytearray,size:20)aDump.
        #var(type:int)aNumberRead.
        #var(type:short)anEventType.
        #var(type:int)aKeyDown.
        
        #var found := false.
        #loop found !
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        : theHandle
                &bytearray     : aDump
                &int           : 1
                &vint          : aNumberRead.

            aDump read &index:0 &vshort:anEventType.
            (anEventType == 1)
                ? [  
                    aDump read &index:4 &vint:aKeyDown.
                    
                    found := aKeyDown == 1.
                ]
                ! [ found := false. ].
        ].
                    
        aDump read &index:8 &vint:aKey.
    ]

    #method read &shortarray:aBuffer &vlength:aRetVal
    [
        system'external'KERNEL32 ReadConsoleW
            &handle     : theHandle
            &shortarray : aBuffer
            &int        : aRetVal
            &vint       : aRetVal
            &int        : 0.
    ]
    
    #method read &wide
    [
        #var(type:shortarray,size:127)aBuffer.
        #var(type:length)aLength.
        #var anOutput := emptyWideLiteralValue.
        
        #var(type:bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &shortarray:aBuffer &vlength:aLength.
            
            #var(type:short)aShort.
            aBuffer read &index:(aLength - 1) &vshort:aShort.
            (aShort == 0Ah)
                ? [
                    aLength := aLength - 2.
                    
                    continue := false.
                ].
            
            #var aTemp := WideLiteralValue $new &length:aLength &index:0 &shortarray:aBuffer.
            
            anOutput := anOutput + aTemp.
        ].

        ^ anOutput.
    ]

    #method read &literal
        = $self read &wide literal.

    #method int
    [
        #var(type:int)aRetVal := $self int.
        
        ^ IntNumber new &int:aRetVal.
    ]
            
    #method char
    [
        #var(type:char)aRetVal.
        $self read &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]

    #method free
    [
        system'external'KERNEL32 CloseHandle
            &handle:theHandle.
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new &handle:($ConsoleHandleFactory new &int_attributes:0FFFFFFF5h).

#symbol stdConsoleReader = $ConsoleReader new &handle:($ConsoleHandleFactory new &int_attributes:0FFFFFFF6h).
