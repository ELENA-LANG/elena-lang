#define system.
#define system'text.

// --- File Access Modes ---
#symbol(const,type:int) $FILE_READ_DATA     = 00000001h.
#symbol(const,type:int) $FILE_APPEND_DATA   = 00000004h.
#symbol(const,type:int) $FILE_GENERIC_READ  = 80000000h.
#symbol(const,type:int) $FILE_GENERIC_WRITE = 40000000h.

#symbol(const,type:int) $OPEN_EXISTING      = 0003h.
#symbol(const,type:int) $CREATE_ALWAYS      = 0002h.
#symbol(const,type:int) $OPEN_ALWAYS        = 0004h.

#symbol(const,type:int) $FILE_SHARE_READ    = 00000001h.
#symbol(const,type:int) $FILE_SHARE_WRITE   = 00000002h.

// --- FileStream ---

#class(struct) FileStream :: Stream
{
    #field(type:handle) theHandle.
    
    #constructor openForRead &path:aPath
        <= %new &path:aPath &int_attributes:$FILE_GENERIC_READ &int_attributes:$FILE_SHARE_READ &int_attributes:$OPEN_EXISTING &int_attributes:0.
    
    #constructor openForReWrite &path:aPath
        <= %new &path:aPath &int_attributes:$FILE_GENERIC_WRITE &int_attributes:$FILE_SHARE_READ &int_attributes:$CREATE_ALWAYS &int_attributes:0.
    
    #constructor openForAppend &path:aPath
        <= %new &path:aPath &int_attributes:$FILE_APPEND_DATA &int_attributes:$FILE_SHARE_READ &int_attributes:$OPEN_ALWAYS &int_attributes:0.
    
    #constructor new &path:aPath &int_attributes:dwDesiredAccess &int_attributes:dwShareMode &int_attributes:dwCreationDisposition &int_attributes:dwFlagsAndAttributes
    [
        theHandle := system'external'KERNEL32 CreateFileW
                      &wide:(aPath wide)
                      &int:dwDesiredAccess
                      &int:dwShareMode
                      &int:0
                      &int:dwCreationDisposition
                      &int:dwFlagsAndAttributes
                      &int:0.
                      
        (-1 == theHandle)
            ? [ #throw IOException new &message:"Cannot open the file". ].
    ]
    
    #method write &bytearray:aDump &length:aLength
    [
        system'external'KERNEL32 WriteFile
            &handle    : theHandle
            &bytearray : aDump
            &int       : aLength
            &int       : 0
            &int       : 0.
    ]

    #method read &bytearray:aDump &vlength:aLength
    [
        system'external'KERNEL32 ReadFile
            &handle      : theHandle
            &bytearray   : aDump
            &int         : aLength
            &vint        : aLength
            &int         : 0.
    ]
    
    #method close
    [
        system'external'KERNEL32 CloseHandle
            &handle:theHandle.
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
