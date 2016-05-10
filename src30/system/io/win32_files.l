#import system.
#import system'text.

// --- File Access Modes ---
#symbol(const,int) $FILE_READ_DATA     = 00000001h.
#symbol(const,int) $FILE_APPEND_DATA   = 00000004h.
#symbol(const,int) $FILE_GENERIC_READ  = 80000000h.
#symbol(const,int) $FILE_GENERIC_WRITE = 40000000h.

#symbol(const,int) $OPEN_EXISTING      = 0003h.
#symbol(const,int) $CREATE_ALWAYS      = 0002h.
#symbol(const,int) $OPEN_ALWAYS        = 0004h.

#symbol(const,int) $FILE_SHARE_READ    = 00000001h.
#symbol(const,int) $FILE_SHARE_WRITE   = 00000002h.

#symbol(const,int) $FILE_BEGIN = 0.
#symbol(const,int) $FILE_CURRENT = 1.
#symbol(const,int) $FILE_END = 2.

#symbol(const,int) $FILE_ATTRIBUTE_DIRECTORY = 10h.

// --- FileStream ---

#class(struct) FileStream :: Stream
{
    #field(handle) theHandle.
    
    #constructor openForRead &path:aPath
        <= %new &wide:(aPath wide) &int:$FILE_GENERIC_READ &int:$FILE_SHARE_READ &int:$OPEN_EXISTING &int:0.
    
    #constructor openForReWrite &path:aPath
        <= %new &wide:(aPath wide) &int:$FILE_GENERIC_WRITE &int:$FILE_SHARE_READ &int:$CREATE_ALWAYS &int:0.
    
    #constructor openForAppend &path:aPath
        <= %new &wide:(aPath wide) &int:$FILE_APPEND_DATA &int:$FILE_SHARE_READ &int:$OPEN_ALWAYS &int:0.
    
    #constructor openForEdit &path:aPath
        <= %new &wide:(aPath wide) &int:$FILE_GENERIC_WRITE &int:$FILE_SHARE_READ &int:$OPEN_EXISTING &int:0.
    
    #constructor new &wide:aPath &int:dwDesiredAccess &int:dwShareMode &int:dwCreationDisposition &int:dwFlagsAndAttributes
    [
        theHandle := system'external'KERNEL32 CreateFileW
                      &wide:aPath
                      &int:dwDesiredAccess
                      &int:dwShareMode
                      &int:0
                      &int:dwCreationDisposition
                      &int:dwFlagsAndAttributes
                      &int:0.
                      
        (-1 == theHandle)
            ? [ #throw IOException new:"Cannot open the file". ].
    ]
    
    #method write &bytearray:aDump &int:aLength
    [
        #var(int)aWritten := 0.
        system'external'KERNEL32 WriteFile
            &int       : theHandle
            &bytearray : aDump
            &int       : aLength
            &vint      : aWritten
            &int       : 0.
    ]

    #method read &bytearray:aDump &vint:aLength
    [
        system'external'KERNEL32 ReadFile
            &int       : theHandle
            &bytearray : aDump
            &int       : aLength
            &vint      : aLength
            &int       : 0.
    ]
    
    #method readIndex &vint:anIndex
    [
        #var(int) ret := system'external'KERNEL32 SetFilePointer
            &int : theHandle
            &int : 0
            &int : 0
            &int : $FILE_CURRENT.
            
        anIndex := ret.
    ] 
    
    #method writeIndex &int:anIndex
    [
        #var(int) ret := system'external'KERNEL32 SetFilePointer
            &int : theHandle
            &int : anIndex
            &int : 0
            &int : $FILE_BEGIN.
    ] 
    
    #method readLength &vint:aLength 
    [ 
        #var(int)n := system'external'KERNEL32 GetFileSize
            &int : theHandle
            &int:0.
    
        aLength := n. 
    ]
    
    #method writeLength &int:length
    [
        #var(int)pos := $self index.
        
        $self writeIndex &int:length.
        system'external'KERNEL32 SetEndOfFile &int:theHandle.
        
        (length > pos)
            ? [ $self writeIndex &int:pos. ].
    ]
    
    #method close
    [
        system'external'KERNEL32 CloseHandle
            &int:theHandle.
    ]
}

// --- fileStreamHelper ---

#symbol $fileStreamHelper =
{
    #method(stream)openForWrite &path:aPath &bool:anAppendMode
    [
        anAppendMode ? [ ^ FileStream openForAppend &path:aPath. ].
        
        ^ FileStream openForReWrite &path:aPath.
    ]
}.

// --- StreamWriter ---

#class StreamWriter :: TextWriter
{
    #field(stream) theStream.
    #field(encoder) theEncoder.

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
        #var(int)aTotal := aLength.
        #var(byte:64)aBuffer.
        #var(int)aBlockLength.
        #var(int)aBufferLength.
        #var(int)anOffset := 0.
        
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
    #field(stream)  theStream.
    #field(encoder) theEncoder.
    #field(bool)    theOpen.

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
        #var(int)aBufferLength := 64.
        #var(byte:64)aBuffer.
        theStream read &bytearray:aBuffer &vint:aBufferLength.
        (aBufferLength == 0)
            ? [ theOpen := false. aLength := 0. ]
            ! [
                #var(int)aRead := aBufferLength.

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

#class(extension:file_path)fileExtOp
{
    #method openForRead = FileStream openForRead &path:self.
    
    #method openForReWrite = FileStream openForReWrite &path:self.
    
    #method openForEdit = FileStream openForEdit &path:self.
    
    #method openForAppend = FileStream openForAppend &path:self.    
        
    #method textreader = StreamReader new &path:self.
    
    #method textwriter = StreamWriter new &path:self.
    
    #method logger = StreamWriter new &path:self &bool:true.
    
    #method(bool) is &available
    [
        #var(int) ret := system'external'KERNEL32 GetFileAttributesW &wide:(self wide).
        
        (ret != 0FFFFFFFFh)
            ? [
                ret := ret && $FILE_ATTRIBUTE_DIRECTORY.
                
                ^ ret == 0.
            ].
            
        ^ false.            
    ]
    
    #method delete
    [
        #var(int) ret := system'external'KERNEL32 DeleteFileW &wide:(self wide).
        
       ^ ret != 0.
    ]
}

// --- directoryExtOp

#class(extension:directory_path)directoryExtOp
{
    #method(bool) is &available
    [
        #var(int) ret := system'external'KERNEL32 GetFileAttributesW &wide:(self wide).
     
        (ret != 0FFFFFFFFh)
            ? [
                ret := ret && $FILE_ATTRIBUTE_DIRECTORY.
                
                ^ ret != 0.
            ].
            
        ^ false.            
    ]    
    
    #method create
    [
        #var(int) ret := system'external'KERNEL32 CreateDirectoryW
            &wide:(self wide)
            &int:0.
            
        (ret == 0)
            ? [ #throw IOException new:"Cannot create a directory". ].
    ]
    
    #method delete
    [
        #var(int) ret := system'external'KERNEL32 RemoveDirectoryW &wide:(self wide).
        
       ^ ret != 0.
    ]    
}