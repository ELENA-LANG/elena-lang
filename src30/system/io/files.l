#define system.

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
    
    #constructor new &path:aPath &int_attributes:dwDesiredAccess &int_attributes:dwShareMode &int_attributes:dwCreationDisposition &int_attributes:dwFlagsAndAttributes
    [
        theHandle := system'external'KERNEL32 CreateFileW
                      &literal:aPath
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

    #method read &bytearray:aDump &length:aLength &out'int:aRead
    [
        system'external'KERNEL32 ReadFile
            &handle      : theHandle
            &bytearray   : aDump
            &int         : aLength
            &out'int     : aRead
            &int         : 0.
    ]

    #method close
    [
        theHandle close.
    ]
}

// --- fileControl ---

#symbol fileControl =
{
    #method(type:stream) openForRead &path:aPath
        = FileStream new &path:aPath &int_attributes:$FILE_GENERIC_READ &int_attributes:$FILE_SHARE_READ &int_attributes:$OPEN_EXISTING &int_attributes:0.
        
    #method(type:stream) openForReWrite &path:aPath
        = FileStream new &path:aPath &int_attributes:$FILE_GENERIC_WRITE &int_attributes:$FILE_SHARE_READ &int_attributes:$CREATE_ALWAYS &int_attributes:0.
}.

// --- StreamWriter ---

#class StreamWriter :: TextWriter
{
    #field(type:stream) theStream.
    #field(type:encoder) theEncoder.

    #constructor new &stream:aStream &encoder:anEncoder
    [
        theStream := aStream.
        theEncoder := anEncoder.

        theNewLineConstant := system'io'newLine literal.
    ]

    #constructor new &path:aPath
        <= %new &stream:(fileControl openForReWrite &path:aPath) &encoder:ansiLiteralEncoder.
        
    #method write &shortarray:anInput &length:aLength
    [
        #var(type:int)aTotal := aLength.
        #var(type:bytearray,size:64)aBuffer.
        #var(type:length)aBlockLength.
        
        #loop (aTotal > 0) ?
        [
            (aTotal < 64)
                ? [ aBlockLength := aTotal. ]
                ! [ aBlockLength := 64. ].
                
            aBlockLength := theEncoder convert &shortarray:anInput &length:aBlockLength &bytearray:aBuffer.
            
            theStream write &bytearray:aBuffer &length:aBlockLength.
            
            aTotal := aTotal - aBlockLength.
        ].
    ]
        
    #method write &literal:anOutput &length:aLength
    [
        #var(type:int)aTotal := aLength.
        #var(type:bytearray,size:64)aBuffer.
        #var(type:length)aBlockLength.
        
        #loop (aTotal > 0) ?
        [
            (aTotal < 64)
                ? [ aBlockLength := aTotal. ]
                ! [ aBlockLength := 64. ].
                
            aBlockLength := theEncoder convert &literal:anOutput &length:aBlockLength &bytearray:aBuffer.
            
            theStream write &bytearray:aBuffer &length:aBlockLength.
            
            aTotal := aTotal - aBlockLength.
        ].
    ]
}

// --- StreamReader ---

#class StreamReader :: TextReader
{
    #field(type:stream) theStream.
    #field(type:encoder) theEncoder.

    #constructor new &stream:aStream &encoder:anEncoder
        <= ( %new )
    [
        theStream := aStream.
        theEncoder := anEncoder.
    ]

    #constructor new &path:aPath
        <= %new &stream:(fileControl openForRead &path:aPath) &encoder:ansiLiteralEncoder.

    #method source = theStream.

    #method encoder = theEncoder.

    #method read &shortarray:anOutput &length:aMaxLength &out'length:anActualLength
    [
        #var(type:int)aLength.
        #var(type:bytearray,size:64)aBuffer.
        aLength := theStream read &bytearray:aBuffer &length:aMaxLength.
                
        anActualLength := theEncoder convert &bytearray:aBuffer &length:aLength &shortarray:anOutput.
    ]
    
    #method close
    [
        theStream close.
    ]
}
