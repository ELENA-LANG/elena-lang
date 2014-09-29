#define system.
#define system'text.

// --- File Access Modes ---
#symbol(const) $FILE_READ_DATA     = 00000001h.
#symbol(const) $FILE_APPEND_DATA   = 00000004h.
#symbol(const) $FILE_GENERIC_READ  = 80000000h.
#symbol(const) $FILE_GENERIC_WRITE = 40000000h.

#symbol(const) $OPEN_EXISTING      = 0003h.
#symbol(const) $CREATE_ALWAYS      = 0002h.
#symbol(const) $OPEN_ALWAYS        = 0004h.

#symbol(const) $FILE_SHARE_READ    = 00000001h.
#symbol(const) $FILE_SHARE_WRITE   = 00000002h.

// --- FileStream ---

#class FileStream : Handle
{
    #constructor new &literal:aPath &int:dwDesiredAccess &int:dwShareMode &int:dwCreationDisposition &int:dwFlagsAndAttributes
    [
        theData := system'external'KERNEL32 CreateFileW
                      &literal:aPath
                      &int:dwDesiredAccess
                      &int:dwShareMode
                      &int:0
                      &int:dwCreationDisposition
                      &int:dwFlagsAndAttributes
                      &int:0.
                      
        (-1 == theData)
            ? [ #throw IOException new:"Cannot open the file". ].
    ]
    
    #method write &bytearray:aDump &length:aLength
    [
        system'external'KERNEL32 WriteFile
            &handle    : $self
            &bytearray : aDump
            &int       : aLength
            &int       : 0
            &int       : 0.
    ]

    #method read &bytearray:aDump &out'length:aLength
    [
        system'external'KERNEL32 ReadFile
            &handle      : $self
            &bytearray   : aDump
            &int         : aLength
            &out'length  : aLength
            &int         : 0.
    ]
}

// --- StreamReader ---

#class StreamReader : TextReader
{
    #field theStream.
    #field theEncoder.

    #constructor new &stream:aStream &encoder:anEncoder
    [
        theStream := aStream.
        theEncoder := anEncoder.

        theOutput := TextBuffer new.
        theOpen := true.
    ]

    #constructor new &path:aPath
        <= %new &stream:(system'io'fileControl openForRead &path:aPath) &encoder:system'io'ansiLiteralEncoder.

    #method read &shortarray:anOutput &out'length:aLength
    [
        #var(type:bytearray,size:64)aBuffer.
        theStream read &bytearray:aBuffer &out'length:aLength.
                
        theEncoder convert &bytearray:aBuffer &length:aLength &shortarray:anOutput &out'length:aLength.
    ]
    
    #method close
    [
        theStream close.
    ]
}

// --- StreamWriter ---

#class StreamWriter : TextWriter
{
    #field theStream.
    #field theEncoder.

    #constructor new &stream:aStream &encoder:anEncoder
    [
        theStream := aStream.
        theEncoder := anEncoder.

        theNewLineConstant := system'io'newLine literal.
    ]

    #constructor new &path:aPath
        <= %new &stream:(system'io'fileControl openForReWrite &path:aPath) &encoder:system'io'ansiLiteralEncoder.
        
    #method write &shortarray:anInput &length:aLength
    [
        #var(type:int)aTotal := aLength.
        #var(type:bytearray,size:64)aBuffer.
        #var(type:int)aBlockLength.
        
        #loop (aTotal > 0) ?
        [
            (aTotal < 64)
                ? [ aBlockLength := aTotal. ]
                ! [ aBlockLength := 64. ].
                
            theEncoder convert &shortarray:anInput &length:aBlockLength &bytearray:aBuffer &out'length:aBlockLength.
            
            theStream write &bytearray:aBuffer &length:aBlockLength.
            
            aTotal := aTotal - aBlockLength.
        ].
    ]
        
    #method write &literal:anOutput &length:aLength
    [
        #var(type:int)aTotal := aLength.
        #var(type:bytearray,size:64)aBuffer.
        #var(type:int)aBlockLength.
        
        #loop (aTotal > 0) ?
        [
            (aTotal < 64)
                ? [ aBlockLength := aTotal. ]
                ! [ aBlockLength := 64. ].
                
            theEncoder convert &literal:anOutput &length:aBlockLength &bytearray:aBuffer &out'length:aBlockLength.
            
            theStream write &bytearray:aBuffer &length:aBlockLength.
            
            aTotal := aTotal - aBlockLength.
        ].
    ]
}

// --- fileControl ---

#symbol fileControl =
{
    openForRead &path:aPath
        = FileStream new &literal::aPath &int:$FILE_GENERIC_READ &int:$FILE_SHARE_READ &int:$OPEN_EXISTING &int:0.
        
    openForReWrite &path:aPath
        = FileStream new &literal::aPath &int:$FILE_GENERIC_WRITE &int:$FILE_SHARE_READ &int:$CREATE_ALWAYS &int:0.
}.