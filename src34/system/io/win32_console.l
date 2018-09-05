import system'text.

// --- IConsoleReader ---

public interface IConsoleReader
{
    abstract T<literal> readLiteral []
    
    abstract T<wide> readWide []
    
    abstract read(ref<int> retVal) []
    
    abstract read(ref<char> retVal) []
    
    abstract bool isKeyAvailable []
}

const int FILE_TYPE_CHAR = 2.

sealed struct CONSOLE_SCREEN_BUFFER_INFO
{
    T<COORD>      dwSize.    
    T<COORD>      dwCursorPosition.    
    short         wAttributes.
    T<SMALL_RECT> srWindow.
    T<COORD>      dwMaximumWindowSize.
    
    stacksafe embeddable constructor new
    [
    ]

    short rvprop SizeX :: readSizeXTo.
    
    short rvprop SizeY :: readSizeYTo.

    stacksafe readSizeXTo(ref<short> aRetVal)
    [
        dwSize readXTo(aRetVal).
    ]

    stacksafe readSizeYTo(ref<short> aRetVal)
    [
        dwSize readYTo(aRetVal).
    ]

    stacksafe readSizeTo(COORD r)
    [
        short x := dwSize X.
        short y := dwSize Y.
        
        r write(x, y).
    ]

    stacksafe readCursorPositionTo(COORD r)
    [
        T<COORD> aCopy := dwCursorPosition.
        
        short x := aCopy X.
        short y := aCopy Y.
        
        r write(x, y).
    ]
}

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
            [  ^ FileWriter new(h) ]
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
    
    clear
    [
        auto aBuffer := CONSOLE_SCREEN_BUFFER_INFO new.
        int  aLength := 0.
        int  aRetVal := 0.

        system'external'KERNEL32 GetConsoleScreenBufferInfo(theHandle, aBuffer).

        short aSizeX := aBuffer SizeX.
        short aSizeY := aBuffer SizeY.
                
        aLength := aSizeX * aSizeY.
        
        system'external'KERNEL32 FillConsoleOutputCharacterW(theHandle, T<short>(32), aLength, 0, &aRetVal).
        
        system'external'KERNEL32 SetConsoleCursorPosition(theHandle, 0).
    ]

    stacksafe setCursorPosition(int x, int y)
    [
        int coord := y * 10000h + x.
                
        system'external'KERNEL32 SetConsoleCursorPosition(theHandle, coord)
    ]

    refresh
    [
    ]

    free
    [
        system'external'KERNEL32 CloseHandle(theHandle).
    ]
}

// --- FileWriter ---

sealed class FileWriter
{
    handle theHandle.

    constructor new(handle aHandle)
    [
        theHandle := aHandle
    ]

    write(ByteArray buffer, int length)
    [
        int written := 0.
        system'external'KERNEL32 WriteFile
            (theHandle, buffer, length, &written, 0).
    ]

    write(wide literal)
    [
        T<byte,256> buffer.
        int         length := literal length.
        int         index := 0.
        int         buf_len := 256.
        int         subs_len := 256.
        while (length > 0)
        [
            buf_len := 256.
            
            if (length < 128)
                [ subs_len := length ] ;
                [ subs_len := 128 ].
            
            OEMEncoder convert(index, &subs_len, literal, T<ByteArray>(buffer), &buf_len).
            
            self write(buffer, buf_len).
            
            index := index + subs_len.
            length := length - subs_len.
        ].
    ]
    
    write(literal literal)
    [
        T<byte,256> buffer.
        int         length := literal length.
        int         index := 0.
        int         buf_len := 256.
        int         subs_len := 256.
        while (length > 0)
        [
            buf_len := 256.
            
            if (length < 128)
                [ subs_len := length ];
                [ subs_len := 128 ].
            
            OEMEncoder convert(index, &subs_len, literal, T<ByteArray>(buffer), &buf_len).
            
            self write(T<ByteArray>(buffer), buf_len).
            
            index := index + subs_len.
            length := length - subs_len
        ].
    ]
    
    clear
    [
    ]

    refresh
    [
    ]

    free
    [
        system'external'KERNEL32 CloseHandle(theHandle).
    ]
}

// --- ConsoleReader ---

sealed struct ConsoleReader :: Object, interface<IConsoleReader>
{
    handle theHandle.
    
    constructor new(handle aHandle)
    [
        theHandle := aHandle.
    ]

    stacksafe read(ref<char> aChar)
    [
        T<byte,20> aDump.
        int        aNumberRead := 0.
        short      anEventType := 0.
        int        aKeyDown := 0.
        
        bool found := false.
        until (found)
        [
            extern()
            [
                system'external'KERNEL32 ReadConsoleInputW(theHandle, T<ByteArray>(aDump), 1, &aNumberRead).
            ].

            byteArrayHelper read(T<ByteArray>(aDump), 0, &anEventType).
            if (anEventType == 1)
                [  
                    byteArrayHelper read(T<ByteArray>(aDump), 4, &aKeyDown).
                    
                    found := aKeyDown == 1
                ];
                [ found := false ]
        ].

        byteArrayHelper read(T<ByteArray>(aDump), 14, aChar).
    ]

    stacksafe read(ref<int> aKey)
    [
        T<byte,20> aDump.
        int        aNumberRead := 0.
        short      anEventType := 0.
        int        aKeyDown := 0.
        
        bool found := false.
        until (found)
        [
            extern()
            [
                system'external'KERNEL32 ReadConsoleInputW(theHandle, T<ByteArray>(aDump), 1, &aNumberRead).
            ].

            byteArrayHelper read(T<ByteArray>(aDump), 0, &anEventType).
            if (anEventType == 1)
                [  
                    byteArrayHelper read(T<ByteArray>(aDump), 4, &aKeyDown).
                    
                    found := aKeyDown == 1
                ];
                [ found := false ]
        ].
                    
        byteArrayHelper read(T<ByteArray>(aDump), 8, aKey)
    ]

    bool isKeyAvailable
    [
        T<byte,20> aDump.
        int        aNumberRead := 0.
        short      anEventType := 0.
        int        aKeyDown := 0.
        
        while (true)
        [
            aNumberRead := 0.
            
            extern()
            [
                system'external'KERNEL32 PeekConsoleInputW(theHandle, T<ByteArray>(aDump), 1, &aNumberRead).
            ].
                        
            if (0 == aNumberRead)
                [ ^ false ].
                                                                                    
            byteArrayHelper read(T<ByteArray>(aDump), 0, &anEventType).
            if (anEventType == 1)
                [ 
                    byteArrayHelper read(T<ByteArray>(aDump), 4, &aKeyDown).
                        
                    if (aKeyDown == 1)
                        [ ^ true ]
                ].

            extern()
            [
                system'external'KERNEL32 ReadConsoleInputW(theHandle, T<ByteArray>(aDump), 1, &aNumberRead).
            ].                
        ].
        
        ^ false.                   
    ]

    stacksafe read(ShortArray aBuffer, ref<int> aRetVal)
    [
        int len := aRetVal value.
        
        extern()
        [
            system'external'KERNEL32 ReadConsoleW(theHandle, aBuffer, len, aRetVal, 0).
        ].            
    ]
    
    T<wide> readWide
    [
        T<short,127> aBuffer.
        int          aLength := 0.
        var          anOutput := emptyWideLiteral.
        
        bool continue := true.
        while (continue)
        [
            aLength := 127.
            self read(T<ShortArray>(aBuffer), &aLength).
            
            short aShort := aBuffer[aLength - 1].
            if (aShort == 0Ah)
                [
                    aLength := aLength - 2.
                    
                    continue := false
                ].
            
            var aTemp := WideLiteralValue fromShortArray(0, aLength, T<ShortArray>(aBuffer)).
            
            anOutput := anOutput + aTemp.
        ].

        ^ anOutput
    ]

    T<literal> readLiteral
        = self readWide.

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
