#define system.

// --- LRLN ---

#symbol newLine
    = "%r%n".

// --- $ConsoleHandle --

#class $ConsoleHandle : Handle
{
    #constructor new &attributes:anAttributes
    [
        theData << system'external'KERNEL32 GetStdHandle 
                      &int::anAttributes.
    ]
}

// --- ConsoleWriter ---

#symbol consoleHelper =
{
    _trimLine &literal:anOutput
        <_console_trimline>.
}.

#class $ConsoleWriter
{
    #field theHandle.
    
    #constructor new &handle:aHandle
    [
        theHandle := aHandle.
    ]

    #method write &literal : aLiteral
    [
        system'external'KERNEL32 WriteConsoleW
            &handle  :  theHandle
            &literal :  aLiteral
            &length  :: aLiteral
            &int     :: 0                       // !! temporally we do not use it
            &int     :: 0.             

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

    #method read &char
    [
        #var aDump := ByteArray new &length:20.
        #var(type:int)aNumberRead.

        control do:
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        : theHandle
                &out'bytearray : aDump
                &int           : 1
                &out'int       : aNumberRead.
        ]
        &until:[(primitive equal &bytearray:aDump &index:0 &short:1)
                    and:(primitive equal &bytearray:aDump &index:4 &int:1)].
                    
        #var(type:short)aChar.
        primitive read &bytearray:aDump &index:14 &out'short:aChar.
                    
        ^ CharValue new &char:aChar.
    ]

    #method read &int
    [
        #var aDump := ByteArray new &length:20.

        control do:
        [
            system'external'KERNEL32 ReadConsoleInputW
                &handle        : theHandle
                &out'bytearray : aDump
                &int           : 1
                &int           : 0.
        ]
        &until:[(primitive equal &bytearray:aDump &index:0 &short:1)
                    and:(primitive equal &bytearray:aDump &index:4 &int:1)].
                    
        #var(type:int)aValue := primitive read &bytearray:aDump &index:8.
                    
        ^ IntNumber new &int:aValue.
    ]

    // !! HOTFIX: additional method to pass the literal type
    #method $read &literal:aLiteral &length:aLength
    [
        system'external'KERNEL32 ReadConsoleW
            &handle       : theHandle
            &out'literal  : aLiteral
            &int          : aLength
            &out'length   : aLiteral
            &int          : 0.
    ]

    #method read &literal &length:aLength
    [
        #var aLiteral := LiteralValue new &length:aLength.

        $self $read &literal:aLiteral &length:aLength.
        
        ^ aLiteral.
    ]
    
    #method read &Line
    [
        #var anOutput := emptyLiteralValue.
        
        control do:
        [
            #var aTemp := $self read &literal &length:64.
            
            anOutput := anOutput + aTemp.
        ] 
        &until:[ consoleHelper _trimLine &literal:anOutput ].
        
        ^ anOutput.
    ]
    
    #method close
    [
        theHandle close.
    ]
}

// --- Console Standard Symbols

#symbol stdConsoleWriter = $ConsoleWriter new &handle:($ConsoleHandle new &attributes:0FFFFFFF5h).

#symbol stdConsoleReader = $ConsoleReader new &handle:($ConsoleHandle new &attributes:0FFFFFFF6h).
