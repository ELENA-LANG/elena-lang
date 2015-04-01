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
        system'external'libc puts
            &literal    :  aLiteral.             

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
/*  
    #method read &vchar:aChar
    [
        #var(type:bytearray,size:20)aDump.
        #var(type:vint)aNumberRead.
        #var(type:vshort)anEventType.
        #var(type:vint)aKeyDown.
        
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
        #var(type:vint)aNumberRead.
        #var(type:vshort)anEventType.
        #var(type:vint)aKeyDown.
        
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
        #var(type:int)aLength := aRetVal.
        system'external'KERNEL32 ReadConsoleW
            &handle     : theHandle
            &shortarray : aBuffer
            &int        : aLength
            &vint       : aRetVal
            &int        : 0.
    ]
    
    #method read &line
    [
        #var(type:shortarray,size:127)aBuffer.
        #var(type:vlength)aLength.
        #var anOutput := emptyLiteralValue.
        
        #var(type:bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &shortarray:aBuffer &vlength:aLength.
            
            #var(type:vshort)aShort.
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

    #method int
    [
        #var(type:int)aRetVal := $self int.
        
        ^ IntNumber new &int:aRetVal.
    ]
            
    #method char
    [
        #var(type:char)aRetVal := $self char.
        
        ^ CharValue new &char:aRetVal.
    ]
*/            
    #method close
    [
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new.

#symbol stdConsoleReader = $ConsoleReader new.
