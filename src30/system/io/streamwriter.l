#import system.
#import system'text.

// --- BinaryWriter ---

#class(limited) BinaryWriter
{
    #field(stream)theStream.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
    ]
    
    #method stream = theStream.

    #method close => theStream.
    
    #method(stacksafe) write &bytearray:anArray &int:aLength
    [
        theStream write &bytearray:anArray &int:aLength.
    ]

    #method write &byte:anInt
    [
        #var(byte:4)aBuffer.
        
        aBuffer@0 := anInt.
                
        $self write &bytearray:aBuffer &int:1.
    ]

    #method write &short:anInt
    [
        #var(byte:4)aBuffer.
        
        byteArrayHelper write &bytearray:aBuffer &int:0 &short:anInt.
                
        $self write &bytearray:aBuffer &int:2.
    ]

    #method write &char:anInt
    [
        #var(byte:4)aBuffer.
        
        byteArrayHelper write &bytearray:aBuffer &int:0 &char:anInt.
                
        $self write &bytearray:aBuffer &int:2.
    ]

    #method write &int:anInt
    [
        #var(byte:4)aBuffer.
        
        byteArrayHelper write &bytearray:aBuffer &int:0 &int:anInt.
                
        $self write &bytearray:aBuffer &int:4.
    ]

    #method write &long:aLong
    [
        #var(byte:8)aBuffer.
        
        byteArrayHelper write &bytearray:aBuffer &int:0 &long:aLong.
                
        $self write &bytearray:aBuffer &int:8.
    ]

    #method write &real:aLong
    [
        #var(byte:8)aBuffer.
        
        byteArrayHelper write &bytearray:aBuffer &int:0 &real:aLong.
                
        $self write &bytearray:aBuffer &int:8.
    ]
}

// --- TextWriter ---

#class(limited) TextWriter
{
    #field(textbuffer) theBuffer.
    #field(literal)theNewLineConstant.

    #constructor new
    [
        theBuffer := TextBuilder new.
    ]
    
    #method textwriter = $self.

    #method stream [ #throw NotSupportedException new. ]

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]    
    
    #method(stacksafe) write &chararray:anArray &int:aLength
    [
    ]
    
    #method writeLine : aLine
    [
        $self write:aLine write &literal:theNewLineConstant.
    ]
    
    #method write : anObject
    [
        anObject cast:%write &to:$self.
    ]
    
    #method write &literal:aLine
    [
        theBuffer clear write &literal:aLine.
        
        #var(int)aLength := theBuffer length.
        
        $self write &chararray:(theBuffer chararray) &int:aLength.
    ]
    
    #method write &wide:aLine
    [
        theBuffer clear write &wide:aLine.
        
        #var(int)aLength := theBuffer length.
        
        $self write &chararray:(theBuffer chararray) &int:aLength.
    ]
    
    #method writeLine
        <= write:'newLine.
}
