#import system.
#import system'text.

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
    
    #method(stacksafe) write &bytearray:anArray &int:aLength
    [
        theStream write &bytearray:anArray &int:aLength.
    ]

    #method write &byte:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &int:0 &byte:anInt.
                
        $self write &bytearray:aBuffer &int:1.
    ]

    #method write &short:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &int:0 &short:anInt.
                
        $self write &bytearray:aBuffer &int:2.
    ]

    #method write &char:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &int:0 &char:anInt.
                
        $self write &bytearray:aBuffer &int:2.
    ]

    #method write &int:anInt
    [
        #var(type:bytearray,size:4)aBuffer.
        
        aBuffer write &int:0 &int:anInt.
                
        $self write &bytearray:aBuffer &int:4.
    ]

    #method write &long:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        
        aBuffer write &int:0 &long:aLong.
                
        $self write &bytearray:aBuffer &int:8.
    ]

    #method write &real:aLong
    [
        #var(type:bytearray,size:8)aBuffer.
        
        aBuffer write &int:0 &real:aLong.
                
        $self write &bytearray:aBuffer &int:8.
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
        
        #var(type:int)aLength.
        theBuffer readLength &vint:aLength.
        
        $self write &chararray:(theBuffer chararray) &int:aLength.
    ]
    
    #method write &wide:aLine
    [
        theBuffer clear write &wide:aLine.
        
        #var(type:int)aLength.
        theBuffer readLength &vint:aLength.
        
        $self write &chararray:(theBuffer chararray) &int:aLength.
    ]
    
    #method writeLine
        <= write:'newLine.
}
