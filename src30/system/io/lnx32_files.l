#define system.
#define system'text.

// --- FileStream ---

#class(struct) FileStream :: Stream
{
    #field(type:handle) theHandle.
  
    #constructor openForRead &path:aPath
        <= %new &path:aPath &attributes:"r+".
    
    #constructor openForReWrite &path:aPath
        <= %new &path:aPath &attributes:"w+".

    #constructor openForAppend &path:aPath
        <= %new &path:aPath &attributes:"a".

    #constructor new &path:aPath &attributes:Attributes
    [
        theHandle := system'external'libc fopen
                      &literal:(aPath literal)
                      &literal:(Attributes literal).
                      
        (0 == theHandle)
            ? [ #throw IOException new &message:"Cannot open the file". ].
    ]
  
    #method write &bytearray:aDump &length:aLength
    [
        system'external'libc fwrite
            &bytearray : aDump        
            &int       : 1
            &int       : aLength
            &handle    : theHandle.
    ]

    #method read &bytearray:aDump &vlength:aLength
    [
        #var(type:int)n := system'external'libc fread
            &bytearray : aDump        
            &int       : 1
            &int       : aLength
            &handle    : theHandle.

        aLength << n.
    ]

    #method close
    [
        system'external'libc fclose &handle:theHandle.
    ]
}

// --- fileStreamHelper ---

#symbol $fileStreamHelper =
{
    #method(type:stream)openForWrite &path:aPath &bool:anAppendMode
    [
        anAppendMode ? [ ^ FileStream openForAppend &path:aPath. ].
        
        ^ FileStream openForReWrite &path:aPath.
    ]
}.

// --- StreamWriter ---

#class StreamWriter :: TextWriter
{
    #field(type:stream) theStream.
    #field(type:encoder) theEncoder.

    #constructor new &stream:aStream &encoder:anEncoder
        <= (new)
    [
        theStream := aStream.
        theEncoder := anEncoder.

        theNewLineConstant := system'text'newLine literal.
    ]

    #constructor new &path:aPath
        <= %new &stream:(FileStream openForReWrite &path:aPath) &encoder:UTF8Encoder.
        
    #constructor new &path:aPath &bool:anAppendMode
        <= %new &stream:($fileStreamHelper openForWrite &path:aPath &bool:anAppendMode) &encoder:UTF8Encoder.
        
    #method stream = theStream.        
        
    #method encoder = theEncoder.        
        
    #method write &chararray:anInput &length:aLength
    [
        #var(type:int)aTotal := aLength.
        #var(type:bytearray,size:64)aBuffer.
        #var(type:length)aBlockLength.
        #var(type:length)aBufferLength.
        #var(type:int)anOffset := 0.
        
        #loop (aTotal > 0) ?
        [
            (aTotal < 64)
                ? [ aBlockLength := aTotal. ]
                ! [ aBlockLength := 64. ].
                
            theEncoder convert &index:anOffset &vlength:aBlockLength &chararray:anInput &bytearray:aBuffer &vlength:aBufferLength.
            
            theStream write &bytearray:aBuffer &length:aBufferLength.
            
            aTotal := aTotal - aBlockLength.
            anOffset := anOffset + aBlockLength.
        ].
    ]
    
    #method close
    [
        theStream close.
    ]
}

// --- StreamReader ---

#class StreamReader :: TextReader
{
    #field(type:stream) theStream.
    #field(type:encoder) theEncoder.
    #field(type:bool) theOpen.

    #constructor new &stream:aStream &encoder:anEncoder
        <= ( %new )
    [
        theStream := aStream.
        theEncoder := anEncoder.
        theOpen := true.
    ]

    #constructor new &path:aPath
        <= %new &stream:(FileStream openForRead &path:aPath) &encoder:UTF8Encoder.

    #constructor new &path:aPath &encoder:anEncoder
        <= %new &stream:(FileStream openForRead &path:aPath) &encoder:anEncoder.

    #method available = theOpen.

    #method stream = theStream.

    #method encoder = theEncoder.

    #method read &chararray:anOutput &vlength:aLength
    [
        #var(type:int)aBufferLength := 64.
        #var(type:bytearray,size:64)aBuffer.
        theStream read &bytearray:aBuffer &vlength:aBufferLength.
        (aBufferLength == 0)
            ? [ theOpen := false. aLength << 0. ]
            ! [
                #var(type:int)aRead := aBufferLength.
                        
                theEncoder convert &index:0 &vlength:aBufferLength &bytearray:aBuffer &chararray:anOutput &vlength:aLength.
                
                (aRead > aBufferLength)
                    ? [ theStream reduce &index:(aRead - aBufferLength). ].
            ].
    ]
    
    #method close
    [
        theStream close.
    ]
}
