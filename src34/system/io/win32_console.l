//import system'text.
//
//type console_screen_buffer_info = CONSOLE_SCREEN_BUFFER_INFO.

const int FILE_TYPE_CHAR = 2.

//sealed struct CONSOLE_SCREEN_BUFFER_INFO
//{
//    win_coord      dwSize.    
//    win_coord      dwCursorPosition.    
//    short          wAttributes.
//    win_small_rect srWindow.
//    win_coord      dwMaximumWindowSize.
//    
//    stacksafe embeddable constructor new
//    [
//    ]
//
//    short rvprop SizeX :: readSizeXTo.
//    
//    short rvprop SizeY :: readSizeYTo.
//
//    stacksafe readSizeXTo vshort:aRetVal
//    [
//        dwSize readXTo vshort:aRetVal.
//    ]
//
//    stacksafe readSizeYTo vshort:aRetVal
//    [
//        dwSize readYTo vshort:aRetVal.
//    ]
//
//    stacksafe readSizeTo win_coord:aCoord
//    [
//        short x := dwSize X.
//        short y := dwSize Y.
//        
//        aCoord write short:x short:y.
//    ]
//
//    stacksafe readCursorPositionTo win_coord:aCoord
//    [
//        win_coord aCopy := dwCursorPosition.
//        
//        short x := aCopy X.
//        short y := aCopy Y.
//        
//        aCoord write short:x short:y.
//    ]
//}

// --- ConsoleHandleFactory --

singleton ConsoleHandleFactory
{
    handle new(int anAttributes)
    [ 
        handle aHandle := system'external'KERNEL32 GetStdHandle(anAttributes).
        
        ^ aHandle.
    ]
    
    newWriter
    [
        handle h := ConsoleHandleFactory new(0FFFFFFF5h).
        int    dwMode := system'external'KERNEL32 GetFileType(h).
        if (dwMode == FILE_TYPE_CHAR)
            [ ^ ConsoleWriter new(h) ];
            [ ^ FileWriter new(h) ]
    ]
}

// --- ConsoleWriter ---

sealed struct ConsoleWriter
{
    handle theHandle.

    constructor new(handle aHandle)
    [
        theHandle := aHandle.
    ]

    write(literal aLiteral)
    [
        self write(T<wide>(aLiteral)).
    ]

    write(wide s)
    [
        int len := s length.

        system'external'KERNEL32 WriteConsoleW(theHandle,s,len,0,0).             

        // !!check the result of the operation
    ]
    
//    clear
//    [
//        console_screen_buffer_info aBuffer := CONSOLE_SCREEN_BUFFER_INFO new.
//        int                        aLength := 0.
//        int                        aRetVal := 0.
//
//        system'external'KERNEL32 GetConsoleScreenBufferInfo int:theHandle console_screen_buffer_info:aBuffer.
//
//        short aSizeX := aBuffer SizeX.
//        short aSizeY := aBuffer SizeY.
//                
//        aLength := aSizeX * aSizeY.
//        
//        system'external'KERNEL32 FillConsoleOutputCharacterW int:theHandle short:32 int:aLength int:0 vint:aRetVal.
//        
//        system'external'KERNEL32 SetConsoleCursorPosition int:theHandle int:0.
//    ]
//
//    stacksafe setCursorPosition int:x int:y
//    [
//        int coord := y * 10000h + x.
//                
//        system'external'KERNEL32 SetConsoleCursorPosition int:theHandle int:coord.
//    ]

    refresh
    [
    ]

//    free
//    [
//        system'external'KERNEL32 CloseHandle
//            int:theHandle.
//    ]
}

// --- FileWriter ---

sealed class FileWriter
{
    handle theHandle.

    constructor new(handle aHandle)
    [
        theHandle := aHandle
    ]

    write(bytearray buffer, int length)
    [
        int written := 0.
        system'external'KERNEL32 WriteFile
            (theHandle, buffer, length, &written, 0).
    ]

//    write wide : literal
//    [
//        byte<256> buffer.
//        int       length := literal length.
//        int       index := 0.
//        int       buf_len := 256.
//        int       subs_len := 256.
//        while (length > 0)
//        [
//            buf_len := 256.
//            
//            if (length < 128)
//                [ subs_len := length ] ;
//                [ subs_len := 128 ].
//            
//            OEMEncoder convert int:index vint:subs_len wide:literal bytearray:buffer vint:buf_len.
//            
//            $self write bytearray:buffer int:buf_len.
//            
//            index := index + subs_len.
//            length := length - subs_len.
//        ].
//    ]
//    
//    write literal : literal
//    [
//        byte<256> buffer.
//        int       length := literal length.
//        int       index := 0.
//        int       buf_len := 256.
//        int       subs_len := 256.
//        while (length > 0)
//        [
//            buf_len := 256.
//            
//            if (length < 128)
//                [ subs_len := length ];
//                [ subs_len := 128 ].
//            
//            OEMEncoder convert int:index vint:subs_len literal:literal bytearray:buffer vint:buf_len.
//            
//            $self write bytearray:buffer int:buf_len.
//            
//            index := index + subs_len.
//            length := length - subs_len
//        ].
//    ]
//    
//    clear
//    [
//    ]

    refresh
    [
    ]

//    free
//    [
//        system'external'KERNEL32 CloseHandle
//            int:theHandle.
//    ]
}

// --- ConsoleReader ---

sealed struct ConsoleReader
{
    handle theHandle.
    
    constructor new(handle aHandle)
    [
        theHandle := aHandle.
    ]

//    stacksafe read vchar:aChar
//    [
//        byte<20> aDump.
//        int      aNumberRead := 0.
//        short    anEventType := 0.
//        int      aKeyDown := 0.
//        int      aHandle := theHandle.
//        
//        bool found := false.
//        until (found)
//        [
//            extern()
//            [
//                system'external'KERNEL32 ReadConsoleInputW
//                    int           : aHandle
//                    bytearray     : aDump
//                    int           : 1
//                    vint          : aNumberRead.
//            ].
//
//            byteArrayHelper read bytearray:aDump int:0 vshort:anEventType.
//            if (anEventType == 1)
//                [  
//                    byteArrayHelper read bytearray:aDump int:4 vint:aKeyDown.
//                    
//                    found := aKeyDown == 1
//                ];
//                [ found := false ]
//        ].
//
//        byteArrayHelper read bytearray:aDump int:14 vchar:aChar.
//    ]
//
//    stacksafe read vint:aKey
//    [
//        byte<20> aDump.
//        int      aNumberRead := 0.
//        short    anEventType := 0.
//        int      aKeyDown := 0.
//        
//        bool found := false.
//        until (found)
//        [
//            extern()
//            [
//                system'external'KERNEL32 ReadConsoleInputW
//                    int           : theHandle
//                    bytearray     : aDump
//                    int           : 1
//                    vint          : aNumberRead
//            ].
//
//            byteArrayHelper read bytearray:aDump int:0 vshort:anEventType.
//            if (anEventType == 1)
//                [  
//                    byteArrayHelper read bytearray:aDump int:4 vint:aKeyDown.
//                    
//                    found := aKeyDown == 1
//                ];
//                [ found := false ]
//        ].
//                    
//        byteArrayHelper read bytearray:aDump int:8 vint:aKey
//    ]
//
//    bool isKeyAvailable
//    [
//        byte<20> aDump.
//        int      aNumberRead := 1.
//        short    anEventType := 0.
//        int      aKeyDown := 0.
//        
//        while (true)
//        [
//            aNumberRead := 0.
//            
//            system'external'KERNEL32 PeekConsoleInputW 
//                        int           : theHandle
//                        bytearray     : aDump
//                        int           : 1
//                        vint          : aNumberRead.
//                        
//            if (0 == aNumberRead)
//                [ ^ false ].
//                                                                                    
//            byteArrayHelper read bytearray:aDump int:0 vshort:anEventType.
//            if (anEventType == 1)
//                [ 
//                    byteArrayHelper read bytearray:aDump int:4 vint:aKeyDown.
//                        
//                    if (aKeyDown == 1)
//                        [ ^ true ]
//                ].
//
//            system'external'KERNEL32 ReadConsoleInputW
//                int           : theHandle
//                bytearray     : aDump
//                int           : 1
//                vint          : aNumberRead
//                
//        ].
//        
//        ^ false.                   
//    ]
//
//    stacksafe read shortarray:aBuffer vint:aRetVal
//    [
//        extern()
//        [
//            system'external'KERNEL32 ReadConsoleW
//                int        : theHandle
//                shortarray : aBuffer
//                int        : aRetVal
//                vint       : aRetVal
//                int        : 0
//        ].            
//    ]
//    
//    wide
//    [
//        short<127> aBuffer.
//        int        aLength := 0.
//        var        anOutput := emptyWideLiteralValue.
//        
//        bool continue := true.
//        while (continue)
//        [
//            aLength := 127.
//            $self read shortarray:aBuffer vint:aLength.
//            
//            short aShort := aBuffer[aLength - 1].
//            if (aShort == 0Ah)
//                [
//                    aLength := aLength - 2.
//                    
//                    continue := false
//                ].
//            
//            var aTemp := WideLiteralValue $new int:0 int:aLength shortarray:aBuffer.
//            
//            anOutput := anOutput + aTemp.
//        ].
//
//        ^ anOutput
//    ]
//
//    literal
//        = $self wide; literal.
//
//    int
//    [
//        int aRetVal := 0.
//        $self read vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//            
//    char
//    [
//        char aRetVal := $0.
//        $self read vchar:aRetVal.
//        
//        ^ CharValue new char:aRetVal
//    ]
//
//    free
//    [
//        system'external'KERNEL32 CloseHandle
//            int:theHandle.
//    ]
}

// --- Console Standard Symbols

public symbol stdConsoleWriter = ConsoleHandleFactory newWriter.

public symbol stdConsoleReader = ConsoleReader new(ConsoleHandleFactory new(0FFFFFFF6h)).
