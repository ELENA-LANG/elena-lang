import system'text.

// --- BinaryWriter ---

public interface BinaryWriter
{
    T<Stream> theStream.
    
    constructor new(Stream aStream)
    [
        theStream := aStream.
    ]
    
    T<Stream> stream = theStream.

    close => theStream.
    
    stacksafe write(ByteArray anArray, int aLength)
    [
        theStream write(anArray, aLength).
    ]

    write(byte anInt)
    [
        V<byte,4> aBuffer.
        
        aBuffer[0] := anInt.
                
        self write(T<ByteArray>(aBuffer), 1).
    ]

    write(short anInt)
    [
        V<byte,4> aBuffer.
        
        byteArrayHelper write(T<ByteArray>(aBuffer), 0, anInt).
                
        self write(T<ByteArray>(aBuffer), 2).
    ]

    write(char anInt)
    [
        V<byte,4> aBuffer.
        
        byteArrayHelper write(T<ByteArray>(aBuffer), 0, anInt).
                
        self write(T<ByteArray>(aBuffer), 4).
    ]

    write(int anInt)
    [
        V<byte,4> aBuffer.
        
        byteArrayHelper write(T<ByteArray>(aBuffer), 0, anInt).
                
        self write(T<ByteArray>(aBuffer), 4).
    ]

    write(long aLong)
    [
        V<byte,8> aBuffer.
        
        byteArrayHelper write(T<ByteArray>(aBuffer), 0, aLong).
                
        self write(T<ByteArray>(aBuffer), 8).
    ]

    write(real aLong)
    [
        V<byte,8> aBuffer.
        
        byteArrayHelper write(T<ByteArray>(aBuffer), 0, aLong).
                
        self write(T<ByteArray>(aBuffer), 8).
    ]
}

// --- TextWriter ---

public interface TextWriter
{
    T<TextBuilder> theBuffer.
    literal        theNewLineConstant.

    constructor new
    [
        theBuffer := TextBuilder new.
    ]
    
    abstract T<Stream> stream [ ]

    abstract T<Encoder> encoder [ ]

    abstract close [ ]
    
    abstract stacksafe write(CharArray anArray, int aLength)
    [
    ]
    
    writeLine : aLine
    [
        self write:aLine; write:theNewLineConstant.
    ]
    
    write : anObject
        <= write(anObject literal).
    
    write(literal aLine)
    [
        theBuffer clear; write(aLine).
        
        int aLength := theBuffer length.
        
        self write(T<CharArray>(theBuffer), aLength).
    ]
    
    write(wide aLine)
    [
        theBuffer clear; write(aLine).
        
        int aLength := theBuffer length.
        
        self write(T<CharArray>(theBuffer), aLength).
    ]
    
    writeLine
        <= write:forwards'newLine.
}
