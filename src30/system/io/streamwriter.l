#define system.
#define system'text.

// --- BinaryWriter ---

#class(limited) BinaryWriter
{
    #field(type:stream)theStream.
    
    #constructor new &stream:aStream
    [
        theStream := aStream.
    ]
    
    #method stream = theStream.

    #method close => theStream.
    
    #method write &bytearray:anArray &length:aLength
    [
        theStream write &bytearray:anArray &length:aLength.
    ]

    #method write &byte:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &byte:anInt.
                
        $self write &bytearray:aBuffer &length:1.
    ]

    #method write &short:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &short:anInt.
                
        $self write &bytearray:aBuffer &length:2.
    ]

    #method write &char:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &char:anInt.
                
        $self write &bytearray:aBuffer &length:2.
    ]

    #method write &int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &index:0 &int:anInt.
                
        $self write &bytearray:aBuffer &length:4.
    ]

    #method write &long:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        
        aBuffer write &index:0 &long:aLong.
                
        $self write &bytearray:aBuffer &length:8.
    ]

    #method write &real:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        
        aBuffer write &index:0 &real:aLong.
                
        $self write &bytearray:aBuffer &length:8.
    ]
}

// --- TextWriter ---

#class(limited) TextWriter
{
    #field(type:textbuffer) theBuffer.
    #field(type:literal)theNewLineConstant.

    #constructor new
    [
        theBuffer := TextBuffer new.
    ]
    
    #method stream [ #throw NotSupportedException new. ]

    #method encoder [ #throw NotSupportedException new. ]

    #method close [ ]    
    
    #method write &chararray:anArray &length:aLength
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
        
        #var(type:int)aLength.
        theBuffer readLength &vint:aLength.
        
        $self write &chararray:(theBuffer chararray) &length:aLength.
    ]
    
    #method write &wide:aLine
    [
        theBuffer clear write &wide:aLine.
        
        #var(type:int)aLength := theBuffer length.
        
        $self write &chararray:(theBuffer chararray) &length:aLength.
    ]
}
