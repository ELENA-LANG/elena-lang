#define system.

// --- LRLN ---

#symbol newLine
    = "%r%n".

// --- $ConsoleHandle --

#class $ConsoleHandle : Handle
{
    #constructor new &attributes:anAttributes
    [
        theData := system'external'KERNEL32 GetStdHandle 
                      &int::anAttributes.
    ]
}

// --- ConsoleWriter ---

#class $ConsoleWriter
{
    #field theHandle.

    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method write &literal : aLiteral
    [
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.
        
        system'external'KERNEL32 WriteConsoleW
            &handle  :: theHandle
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

#class $ConsoleReader
{
    #field theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method read &out'short:aChar
    [
        #var(type:bytearray,size:20)aDump.
        #var(type:int)aNumberRead.

        #loop
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        :: theHandle
                &out'bytearray : aDump
                &int           : 1
                &out'int       : aNumberRead.

            false equal:((aDump equal &index:0 &short:1)and:(aDump equal &index:4 &int:1)).
        ].

        aDump read &index:14 &out'short:aChar.
    ]

//    #method read &int
//    [
//        #var aDump := ByteArray new &length:20.
//
//        control do:
//        [
//            system'external'KERNEL32 ReadConsoleInputW
//                &handle        : theHandle
//                &out'bytearray : aDump
//                &int           : 1
//                &int           : 0.
//        ]
//        &until:[(primitive equal &bytearray:aDump &index:0 &short:1)
//                    and:(primitive equal &bytearray:aDump &index:4 &int:1)].
//                    
//        #var(type:int)aValue := primitive read &bytearray:aDump &index:8.
//                    
//        ^ IntNumber new &int:aValue.
//    ]

    #method read &shortarray:aBuffer &out'length:aLength
    [
        system'external'KERNEL32 ReadConsoleW
            &handle    :: theHandle
            &shortarray : aBuffer
            &int        : aLength
            &out'length : aLength
            &int        : 0.
    ]
    
    #method read &Line
    [
        #var(type:shortarray,size:127)aBuffer.
        #var(type:int)aLength.
        #var anOutput := emptyLiteralValue.
        #var continue := true.

        #loop
        [
            aLength := 128.
            $self read &shortarray:aBuffer &out'length:aLength.
            
            aBuffer equal &index:(aLength - 1) &short:0Ah
                ? [
                    aLength := aLength - 2.
                    
                    continue := false.
                ].
            
            #var aTemp := LiteralValue new &length:aLength &shortarray:aBuffer.
            
            anOutput := anOutput + aTemp.
            
            continue bool
        ].

        ^ anOutput.
    ]
    
    #method close
    [
        theHandle close.
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new &handle::($ConsoleHandle new &attributes:0FFFFFFF5h).

#symbol stdConsoleReader = $ConsoleReader new &handle::($ConsoleHandle new &attributes:0FFFFFFF6h).
