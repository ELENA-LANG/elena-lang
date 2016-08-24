#import system.
#import system'text.

#subject console_screen_buffer_info = CONSOLE_SCREEN_BUFFER_INFO.

#symbol(int,const)FILE_TYPE_CHAR = 2.

#class(embeddable,sealed)CONSOLE_SCREEN_BUFFER_INFO
{
    #field(win_coord)dwSize.    
    #field(win_coord)dwCursorPosition.    
    #field(short)wAttributes.
    #field(win_small_rect)srWindow.
    #field(win_coord)dwMaximumWindowSize.

    #method(stacksafe,outwprop:SizeX) readSizeX &vshort:aRetVal
    [
        dwSize readX &vshort:aRetVal.
    ]

    #method(stacksafe,outwprop:SizeY) readSizeY &vshort:aRetVal
    [
        dwSize readY &vshort:aRetVal.
    ]

    #method(stacksafe) readSize &win_coord:aCoord
    [
        #var(short)x := dwSize X.
        #var(short)y := dwSize Y.
        
        aCoord write &short:x &short:y.
    ]

    #method(stacksafe) readCursorPosition &win_coord:aCoord
    [
        #var(win_coord)aCopy := dwCursorPosition.
        
        #var(short)x := aCopy X.
        #var(short)y := aCopy Y.
        
        aCoord write &short:x &short:y.
    ]
}

// --- $ConsoleHandleFactory --

#symbol $ConsoleHandleFactory =
{
    #method(handle) new &attributes:anAttributes
    [ 
        #var(handle) aHandle := system'external'KERNEL32 GetStdHandle &int:(anAttributes int).
        
        ^ Handle new &handle:aHandle.
    ]
    
    #method newWriter
    [
        #var(handle)handle := $ConsoleHandleFactory new &attributes:0FFFFFFF5h.
        #var(int)dwMode := system'external'KERNEL32 GetFileType &int:handle.
        (dwMode == FILE_TYPE_CHAR)
            ? [ ^ $ConsoleWriter new &handle:handle. ]
            ! [ ^ $FileWriter new &handle:handle. ].
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

    #method write &literal:aLiteral
    [
        $self write &wide:(aLiteral wide).
    ]

    #method write &wide : s
    [
        #var(int)len := s length.

        system'external'KERNEL32 WriteConsoleW
            &int  :  theHandle
            &wide :  s
            &int  :  len
            &int  :  0                       // !! temporally we do not use it
            &int  :  0.             

        // !!check the result of the operation
    ]
    
    #method clear
    [
        #var(console_screen_buffer_info)aBuffer.
        #var(int)aLength.
        #var(int)aRetVal.
        
        system'external'KERNEL32 GetConsoleScreenBufferInfo &int:theHandle &console_screen_buffer_info:aBuffer.

        #var(short)aSizeX := aBuffer SizeX.
        #var(short)aSizeY := aBuffer SizeY.
                
        aLength := aSizeX * aSizeY.
        
        system'external'KERNEL32 FillConsoleOutputCharacterW &int:theHandle &short:32 &int:aLength &int:0 &vint:aRetVal.
        
        system'external'KERNEL32 SetConsoleCursorPosition &int:0.
    ]

    #method refresh
    [
    ]

    #method free
    [
        system'external'KERNEL32 CloseHandle
            &int:theHandle.
    ]
}

// --- $FileWriter ---

#class(sealed,embeddable) $FileWriter
{
    #field(handle) theHandle.

    #constructor(stacksafe) new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method(stacksafe) write &bytearray:buffer &int:length
    [
        #var(int)written := 0.
        system'external'KERNEL32 WriteFile
            &int       : theHandle
            &bytearray : buffer
            &int       : length
            &vint      : written
            &int       : 0.
    ]

    #method write &wide : literal
    [
        #var(byte:255)buffer.
        #var(int)length := literal length.
        #var(int)index := 0.
        #var(int)buf_len := 255.
        #var(int)subs_len := 255.
        #loop (index < length)?
        [
            buf_len := 255.
            
            (length < 128)
                ? [ subs_len := length. ]  
                ! [ subs_len := 128. ].
            
            OEMEncoder convert &int:index &vint:subs_len &wide:literal &bytearray:buffer &vint:buf_len.
            
            $self write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]
    
    #method write &literal : literal
    [
        #var(byte:255)buffer.
        #var(int)length := literal length.
        #var(int)index := 0.
        #var(int)buf_len := 255.
        #var(int)subs_len := 255.
        #loop (index < length)?
        [
            buf_len := 255.
            
            (length < 128)
                ? [ subs_len := length. ]  
                ! [ subs_len := 128. ].
            
            OEMEncoder convert &int:index &vint:subs_len &literal:literal &bytearray:buffer &vint:buf_len.
            
            $self write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]
    
    #method clear
    [
    ]

    #method refresh
    [
    ]

    #method free
    [
        system'external'KERNEL32 CloseHandle
            &int:theHandle.
    ]
}

// --- ConsoleReader ---

#class(sealed,embeddable) $ConsoleReader
{
    #field(handle) theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method(stacksafe) read &vchar:aChar
    [
        #var(byte:20)aDump.
        #var(int)aNumberRead.
        #var(short)anEventType.
        #var(int)aKeyDown.
        #var(int)aHandle := theHandle.
        
        #var(bool) found := false.
        #loop found !
        [
            #extern
            [
                system'external'KERNEL32 ReadConsoleInputW
                    &int           : aHandle
                    &bytearray     : aDump
                    &int           : 1
                    &vint          : aNumberRead.
            ].

            byteArrayHelper read &bytearray:aDump &int:0 &vshort:anEventType.
            (anEventType == 1)
                ? [  
                    byteArrayHelper read &bytearray:aDump &int:4 &vint:aKeyDown.
                    
                    found := aKeyDown == 1.
                ]
                ! [ found := false. ].
        ].

        byteArrayHelper read &bytearray:aDump &int:14 &vchar:aChar.
    ]

    #method(stacksafe,suppress:w2) read &vint:aKey
    [
        #var(byte:20)aDump.
        #var(int)aNumberRead.
        #var(short)anEventType.
        #var(int)aKeyDown.
        
        #var found := false.
        #loop found !
        [
            #extern
            [
                system'external'KERNEL32 ReadConsoleInputW
                    &int           : theHandle
                    &bytearray     : aDump
                    &int           : 1
                    &vint          : aNumberRead.
            ].

            byteArrayHelper read &bytearray:aDump &int:0 &vshort:anEventType.
            (anEventType == 1)
                ? [  
                    aDump read &int:4 &vint:aKeyDown.
                    
                    found := aKeyDown == 1.
                ]
                ! [ found := false. ].
        ].
                    
        byteArrayHelper read &bytearray:aDump &int:8 &vint:aKey.
    ]

    #method(stacksafe) read &shortarray:aBuffer &vint:aRetVal
    [
        #extern
        [
            system'external'KERNEL32 ReadConsoleW
                &int        : theHandle
                &shortarray : aBuffer
                &int        : aRetVal
                &vint       : aRetVal
                &int        : 0.
        ].            
    ]
    
    #method read &wide
    [
        #var(short:127)aBuffer.
        #var(int)aLength.
        #var anOutput := emptyWideLiteralValue.
        
        #var(bool) continue := true.
        #loop continue ?
        [
            aLength := 127.
            $self read &shortarray:aBuffer &vint:aLength.
            
            #var(short)aShort := aBuffer@(aLength - 1).
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
        #var(int)aRetVal.
        $self read &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
            
    #method char
    [
        #var(char)aRetVal.
        $self read &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]

    #method free
    [
        system'external'KERNEL32 CloseHandle
            &int:theHandle.
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleHandleFactory newWriter.

#symbol stdConsoleReader = $ConsoleReader new &handle:($ConsoleHandleFactory new &attributes:0FFFFFFF6h).
