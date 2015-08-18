#import system.

#subject(class:CONSOLE_SCREEN_BUFFER_INFO)console_screen_buffer_info.

#class(struct:embeddable,sealed)CONSOLE_SCREEN_BUFFER_INFO
{
    #field(type:win_coord)dwSize.    
    #field(type:win_coord)dwCursorPosition.    
    #field(type:short)wAttributes.
    #field(type:win_small_rect)srWindow.
    #field(type:win_coord)dwMaximumWindowSize.

    #method console_screen_buffer_info = $self.

    #method(stacksafe) readSizeX &vshort:aRetVal
    [
        dwSize readX &vshort:aRetVal.
    ]

    #method(stacksafe) readSizeY &vshort:aRetVal
    [
        dwSize readY &vshort:aRetVal.
    ]

    #method(stacksafe) readSize &win_coord:aCoord
    [
        #var(type:short)x.
        #var(type:short)y.
        dwSize readX &vshort:x.
        dwSize readY &vshort:y.
        
        aCoord write &short:x &short:y.
    ]

    #method(stacksafe) readCursorPosition &win_coord:aCoord
    [
        #var(type:win_coord)aCopy := dwCursorPosition.
        
        #var(type:short)x.
        #var(type:short)y.
        aCopy readX &vshort:x.
        aCopy readY &vshort:y.
        
        aCoord write &short:x &short:y.
    ]
}

// --- $ConsoleHandleFactory --

#symbol $ConsoleHandleFactory =
{
    #method(type:handle) new &attributes:anAttributes
    [ 
        #var(type:handle)aHandle := system'external'KERNEL32 GetStdHandle &int:(anAttributes int).
        
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
    
    #method clear
    [
        #var(type:console_screen_buffer_info)aBuffer.
        #var(type:int)aLength.
        #var(type:short)aSizeX.
        #var(type:short)aSizeY.
        #var(type:int)aRetVal.
        
        system'external'KERNEL32 GetConsoleScreenBufferInfo &handle:theHandle &console_screen_buffer_info:aBuffer.
        
        aBuffer readSizeX &vshort:aSizeX.
        aBuffer readSizeY &vshort:aSizeY.
        aLength := aSizeX * aSizeY.
        
        system'external'KERNEL32 FillConsoleOutputCharacterW &handle:theHandle &short:32 &int:aLength &int:0 &vint:aRetVal.
        
        system'external'KERNEL32 SetConsoleCursorPosition &int:0.
    ]

    #method refresh
    [
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

    #method(stacksafe) read &vchar:aChar
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

            aDump read &int:0 &vshort:anEventType.
            (anEventType == 1)
                ? [  
                    aDump read &int:4 &vint:aKeyDown.
                    
                    found := aKeyDown == 1.
                ]
                ! [ found := false. ].
        ].

        aDump read &int:14 &vchar:aChar.
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

            aDump read &int:0 &vshort:anEventType.
            (anEventType == 1)
                ? [  
                    aDump read &int:4 &vint:aKeyDown.
                    
                    found := aKeyDown == 1.
                ]
                ! [ found := false. ].
        ].
                    
        aDump read &int:8 &vint:aKey.
    ]

    #method(stacksafe) read &shortarray:aBuffer &vint:aRetVal
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
        #var(type:int)aLength.
        #var anOutput := emptyWideLiteralValue.
        
        #var(type:bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &shortarray:aBuffer &vint:aLength.
            
            #var(type:short)aShort.
            aBuffer read &int:(aLength - 1) &vshort:aShort.
            (aShort == 0Ah)
                ? [
                    aLength := aLength - 2.
                    
                    continue := false.
                ].
            
            #var aTemp := WideLiteralValue $new &int:0 &int:aLength &shortarray:aBuffer.
            
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

#symbol stdConsoleWriter = $ConsoleWriter new &handle:($ConsoleHandleFactory new &attributes:0FFFFFFF5h).

#symbol stdConsoleReader = $ConsoleReader new &handle:($ConsoleHandleFactory new &attributes:0FFFFFFF6h).
