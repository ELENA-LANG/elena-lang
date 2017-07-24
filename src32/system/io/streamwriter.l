import system'text.

// --- BinaryWriter ---

limited class BinaryWriter
{
    stream theStream.
    
    constructor new stream:aStream
    [
        theStream := aStream.
    ]
    
    stream = theStream.

    close => theStream.
    
    stacksafe write bytearray:anArray int:aLength
    [
        theStream write bytearray:anArray int:aLength.
    ]

    write byte:anInt
    [
        byte aBuffer(4).
        
        aBuffer@0 := anInt.
                
        $self write bytearray:aBuffer int:1.
    ]

    write short:anInt
    [
        byte aBuffer(4).
        
        byteArrayHelper write bytearray:aBuffer int:0 short:anInt.
                
        $self write bytearray:aBuffer int:2.
    ]

    write char:anInt
    [
        byte aBuffer(4).
        
        byteArrayHelper write bytearray:aBuffer int:0 char:anInt.
                
        $self write bytearray:aBuffer int:2
    ]

    write int:anInt
    [
        byte aBuffer(4).
        
        byteArrayHelper write bytearray:aBuffer int:0 int:anInt.
                
        $self write bytearray:aBuffer int:4.
    ]

    write long:aLong
    [
        byte aBuffer(8).
        
        byteArrayHelper write bytearray:aBuffer int:0 long:aLong.
                
        $self write bytearray:aBuffer int:8.
    ]

    write real:aLong
    [
        byte aBuffer(8).
        
        byteArrayHelper write bytearray:aBuffer int:0 real:aLong.
                
        $self write bytearray:aBuffer int:8
    ]
}

// --- TextWriter ---

limited class TextWriter
{
    textbuffer theBuffer.
    literal    theNewLineConstant.

    constructor new
    [
        theBuffer := TextBuilder new.
    ]
    
    textwriter = $self.

    stream [ NotSupportedException new; raise ]

    encoder [ NotSupportedException new; raise ]

    close [ ]    
    
    stacksafe write chararray:anArray int:aLength
    [
    ]
    
    writeLine : aLine
    [
        $self write:aLine; write literal:theNewLineConstant.
    ]
    
    write : anObject
    [
        anObject cast:%write to:$self.
    ]
    
    write literal:aLine
    [
        theBuffer clear; write literal:aLine.
        
        int aLength := theBuffer length.
        
        $self write chararray:theBuffer int:aLength.
    ]
    
    write wide:aLine
    [
        theBuffer clear; write wide:aLine.
        
        int aLength := theBuffer length.
        
        $self write chararray:theBuffer int:aLength.
    ]
    
    writeLine
        <= write:'newLine.
}
