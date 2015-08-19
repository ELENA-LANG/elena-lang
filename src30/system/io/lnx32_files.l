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
            ? [ #throw IOException new:"Cannot open the file". ].
    ]
  
    #method write &bytearray:aDump &int:aLength
    [
        system'external'libc fwrite
            &bytearray : aDump        
            &int       : 1
            &int       : aLength
            &handle    : theHandle.
    ]

    #method read &bytearray:aDump &vint:aLength
    [
        #var(type:int)n := system'external'libc fread
            &bytearray : aDump        
            &int       : 1
            &int       : aLength
            &handle    : theHandle.

        aLength << n.
    ]

//    #method readIndex &vint:anIndex
//    [
//        #var(type:int) ret := system'external'KERNEL32 SetFilePointer
//            &handle : theHandle
//            &int    : 0
//            &int    : 0
//            &int    : $FILE_CURRENT.
//            
//        anIndex << ret.
//    ] 
//    
//    #method writeIndex &int:anIndex
//    [
//        #var(type:int) ret := system'external'KERNEL32 SetFilePointer
//            &handle : theHandle
//            &int    : anIndex
//            &int    : 0
//            &int    : $FILE_BEGIN.
//    ] 
//    
//    #method readLength &vint:aLength 
//    [ 
//        #var(type:int)n := system'external'KERNEL32 GetFileSize
//            &handle : theHandle
//            &int:0.
//    
//        aLength << n. 
//    ]
    
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
        
    #method write &chararray:anInput &int:aLength
    [
        #var(type:int)aTotal := aLength.
        #var(type:bytearray,size:64)aBuffer.
        #var(type:int)aBlockLength.
        #var(type:int)aBufferLength.
        #var(type:int)anOffset := 0.
        
        #loop (aTotal > 0) ?
        [
            (aTotal < 64)
                ? [ aBlockLength := aTotal. ]
                ! [ aBlockLength := 64. ].
                
            theEncoder convert &int:anOffset &vint:aBlockLength &chararray:anInput &bytearray:aBuffer &vint:aBufferLength.
            
            theStream write &bytearray:aBuffer &int:aBufferLength.
            
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

    #method read &chararray:anOutput &vint:aLength
    [
        #var(type:int)aBufferLength := 64.
        #var(type:bytearray,size:64)aBuffer.
        theStream read &bytearray:aBuffer &vint:aBufferLength.
        (aBufferLength == 0)
            ? [ theOpen := false. aLength << 0. ]
            ! [
                #var(type:int)aRead := aBufferLength.
                        
                theEncoder convert &int:0 &vint:aBufferLength &bytearray:aBuffer &chararray:anOutput &vint:aLength.
                
                (aRead > aBufferLength)
                    ? [ theStream reduceIndex &int:(aRead - aBufferLength). ].
            ].
    ]
    
    #method close
    [
        theStream close.
    ]
}

// --- BinaryFileReader ---

#class(sealed)BinaryFileReader :: BinaryReader
{
    #constructor new &path:aPath
        <= %new &stream:(FileStream openForRead &path:aPath).
}

// --- BinaryFileWriter ---

#class(sealed)BinaryFileWriter :: BinaryWriter
{
    #constructor new &path:aPath
        <= %new &stream:(FileStream openForReWrite &path:aPath).
}

// --- fileOp ---

#class(extension:file)fileOp
{
    #method openForRead = FileStream openForRead &path:self.
    
    #method openForReWrite = FileStream openForReWrite &path:self.
    
    #method openForAppend = FileStream openForAppend &path:self.    
        
    #method textreader = StreamReader new &path:self.
    
    #method textwriter = StreamWriter new &path:self.
    
    #method logger = StreamWriter new &path:self &bool:true.
/*    
    #method(type:bool) is &available
    [
        #var(type:int) ret := system'external'kernel32 GetFileAttributesW &wide:(self wide).
        
        ^ (ret != 0FFFFFFFFh).
    ]
    
    #method delete
    [
        #var(type:int) ret := system'external'kernel32 DeleteFileW &wide:(self wide).
        
       ^ ret != 0.
    ]
*/
}
