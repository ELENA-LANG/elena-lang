#define system.
//#define system'text.

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

#class(struct) FileStream : Stream
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
            ? [ #throw IOException new:"Cannot open the file". ].
    ]
    
//    #method write &bytearray:aDump &length:aLength
//    [
//        system'external'KERNEL32 WriteFile
//            &handle    : theHandle
//            &bytearray : aDump
//            &int       : aLength
//            &int       : 0
//            &int       : 0.
//    ]

    #method read &bytearray:aDump &length:aLength &out'length:aRead
    [
        system'external'KERNEL32 ReadFile
            &handle      : theHandle
            &bytearray   : aDump
            &int         : aLength
            &out'length  : aRead
            &int         : 0.
    ]

//    #method read &bytearray:aDump &length:aLength
//    [
//        #var(type:int)aRead.
//        $self read &bytearray:aDump &length:aLength &out'length:aRead.
//        
//        ^ IntNumber new &int:aRead.
//    ]

    #method close
    [
        theHandle close.
    ]
}

////// --- StreamWriter ---
////
////#class StreamWriter : TextWriter
////{
////    #field theStream.
////    #field theEncoder.
////
////    #constructor new &stream:aStream &encoder:anEncoder
////    [
////        theStream := aStream.
////        theEncoder := anEncoder.
////
////        theNewLineConstant := system'io'newLine literal.
////    ]
////
////    #constructor new &path:aPath
////        <= %new &stream:(system'io'fileControl openForReWrite &path:aPath) &encoder:system'io'ansiLiteralEncoder.
////        
////    #method write &shortarray:anInput &length:aLength
////    [
////        #var(type:int)aTotal := aLength.
////        #var(type:bytearray,size:64)aBuffer.
////        #var(type:int)aBlockLength.
////        
////        #loop (aTotal > 0) ?
////        [
////            (aTotal < 64)
////                ? [ aBlockLength := aTotal. ]
////                ! [ aBlockLength := 64. ].
////                
////            theEncoder convert &shortarray:anInput &length:aBlockLength &bytearray:aBuffer &out'length:aBlockLength.
////            
////            theStream write &bytearray:aBuffer &length:aBlockLength.
////            
////            aTotal := aTotal - aBlockLength.
////        ].
////    ]
////        
////    #method write &literal:anOutput &length:aLength
////    [
////        #var(type:int)aTotal := aLength.
////        #var(type:bytearray,size:64)aBuffer.
////        #var(type:int)aBlockLength.
////        
////        #loop (aTotal > 0) ?
////        [
////            (aTotal < 64)
////                ? [ aBlockLength := aTotal. ]
////                ! [ aBlockLength := 64. ].
////                
////            theEncoder convert &literal:anOutput &length:aBlockLength &bytearray:aBuffer &out'length:aBlockLength.
////            
////            theStream write &bytearray:aBuffer &length:aBlockLength.
////            
////            aTotal := aTotal - aBlockLength.
////        ].
////    ]
////}

// --- StreamReader ---

#class StreamReader : TextReader
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
        <= %new &stream::(system'io'fileControl openForRead &path:aPath) &encoder:ansiLiteralEncoder.

    #method source = theStream.

    #method read &shortarray:anOutput &length:aMaxLength &out'length:anActualLength
    [
        #var(type:int)aLength.
        #var(type:bytearray,size:64)aBuffer.
        theStream read &bytearray:aBuffer &length:aMaxLength &out'length:aLength.
                
        theEncoder convert &bytearray:aBuffer &length:aLength &shortarray:anOutput &out'length:anActualLength.
    ]
    
    #method close
    [
        theStream close.
    ]
}

// --- fileControl ---

#symbol fileControl =
{
    openForRead &path:aPath
        = FileStream new &path:aPath &int_attributes:$FILE_GENERIC_READ &int_attributes:$FILE_SHARE_READ &int_attributes:$OPEN_EXISTING &int_attributes:0.
        
    openForReWrite &path:aPath
        = FileStream new &path:aPath &int_attributes:$FILE_GENERIC_WRITE &int_attributes:$FILE_SHARE_READ &int_attributes:$CREATE_ALWAYS &int_attributes:0.
}.