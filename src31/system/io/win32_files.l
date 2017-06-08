import system'text.

// --- File Access Modes ---
const int $FILE_READ_DATA     = 00000001h.
const int $FILE_APPEND_DATA   = 00000004h.
const int $FILE_GENERIC_READ  = 80000000h.
const int $FILE_GENERIC_WRITE = 40000000h.

const int $OPEN_EXISTING      = 0003h.
const int $CREATE_ALWAYS      = 0002h.
const int $OPEN_ALWAYS        = 0004h.

const int $FILE_SHARE_READ    = 00000001h.
const int $FILE_SHARE_WRITE   = 00000002h.

const int $FILE_BEGIN = 0.
const int $FILE_CURRENT = 1.
const int $FILE_END = 2.

const int $FILE_ATTRIBUTE_DIRECTORY = 10h.

// --- FileStream ---

struct FileStream :: Stream
{
    handle theHandle.
    
    constructor openForRead path:aPath
        <= new wide:aPath int:$FILE_GENERIC_READ int:$FILE_SHARE_READ int:$OPEN_EXISTING int:0.
    
    constructor openForReWrite path:aPath
        <= new wide:aPath int:$FILE_GENERIC_WRITE int:$FILE_SHARE_READ int:$CREATE_ALWAYS int:0.
    
    constructor openForAppend path:aPath
        <= new wide:aPath int:$FILE_APPEND_DATA int:$FILE_SHARE_READ int:$OPEN_ALWAYS int:0.
    
    constructor openForEdit path:aPath
        <= new wide:aPath int:$FILE_GENERIC_WRITE int:$FILE_SHARE_READ int:$OPEN_EXISTING int:0.
    
    constructor new wide:aPath int:dwDesiredAccess int:dwShareMode int:dwCreationDisposition int:dwFlagsAndAttributes
    [
        theHandle := system'external'KERNEL32 CreateFileW
                      wide:aPath
                      int:dwDesiredAccess
                      int:dwShareMode
                      int:0
                      int:dwCreationDisposition
                      int:dwFlagsAndAttributes
                      int:0.
                      
        if(-1 == theHandle)
            [ IOException new literal:"Cannot open the file"; raise ]
    ]
    
    write bytearray:aDump int:aLength
    [
        int aWritten := 0.
        system'external'KERNEL32 WriteFile
            int       : theHandle
            bytearray : aDump
            int       : aLength
            vint      : aWritten
            int       : 0.
    ]

    read bytearray:aDump vint:aLength
    [
        system'external'KERNEL32 ReadFile
            int       : theHandle
            bytearray : aDump
            int       : aLength
            vint      : aLength
            int       : 0.
    ]
    
    readIndexTo vint:anIndex
    [
        int ret := system'external'KERNEL32 SetFilePointer
            int : theHandle
            int : 0
            int : 0
            int : $FILE_CURRENT.
            
        anIndex int := ret.
    ] 
    
    writeIndex int:anIndex
    [
        int ret := system'external'KERNEL32 SetFilePointer
            int : theHandle
            int : anIndex
            int : 0
            int : $FILE_BEGIN.
    ] 
    
    readLengthTo vint:aLength 
    [ 
        int n := system'external'KERNEL32 GetFileSize
            int : theHandle
            int:0.
    
        aLength int := n. 
    ]
    
    writeLength int:length
    [
        int pos := $self index.
        
        $self writeIndex int:length.
        system'external'KERNEL32 SetEndOfFile int:theHandle.
        
        if(length > pos)
            [ $self writeIndex int:pos ]
    ]
    
    close
    [
        system'external'KERNEL32 CloseHandle
            int:theHandle.
    ]
}

// --- fileStreamHelper ---

class $fileStreamHelper =
{
    stream openForWrite path:aPath bool:anAppendMode
    [
        if(anAppendMode) [ ^ FileStream openForAppend path:aPath ].
        
        ^ FileStream openForReWrite path:aPath
    ]
}.

// --- StreamWriter ---

class StreamWriter :: TextWriter
{
    stream  theStream.
    encoder theEncoder.

    constructor new stream:aStream encoder:anEncoder
        <= new;
    [
        theStream := aStream.
        theEncoder := anEncoder.

        theNewLineConstant := system'text'newLine literal.
    ]

    constructor new path:aPath
        <= new stream:(FileStream openForReWrite path:aPath) encoder:UTF8Encoder.
        
    constructor new path:aPath bool:anAppendMode
        <= new stream:($fileStreamHelper openForWrite path:aPath bool:anAppendMode) encoder:UTF8Encoder.
        
    stream = theStream.        
        
    encoder = theEncoder.        
        
    write chararray:anInput int:aLength
    [
        int  aTotal := aLength.
        byte aBuffer(64).
        int  aBlockLength := 0.
        int  aBufferLength := 0.
        int  anOffset := 0.
        
        while (aTotal > 0)
        [
            if(aTotal < 64)
                [ aBlockLength := aTotal ];
                [ aBlockLength := 64 ].
                
            theEncoder convert int:anOffset vint:aBlockLength chararray:anInput bytearray:aBuffer vint:aBufferLength.
            
            theStream write bytearray:aBuffer int:aBufferLength.
            
            aTotal := aTotal - aBlockLength.
            anOffset := anOffset + aBlockLength
        ]
    ]
    
    close
    [
        theStream close
    ]
}

// --- StreamReader ---

class StreamReader :: TextReader
{
    stream  theStream.
    encoder theEncoder.
    bool    theOpen.

    constructor new stream:aStream encoder:anEncoder
        <= new;
    [
        theStream := aStream.
        theEncoder := anEncoder.
        theOpen := true.
    ]

    constructor new path:aPath
        <= new stream:(FileStream openForRead path:aPath) encoder:UTF8Encoder.

    constructor new path:aPath encoder:anEncoder
        <= new stream:(FileStream openForRead path:aPath) encoder:anEncoder.

    available = theOpen.

    stream = theStream.

    encoder = theEncoder.

    read chararray:anOutput vint:aLength
    [
        int  aBufferLength := 64.
        byte aBuffer(64).
        theStream read bytearray:aBuffer vint:aBufferLength.
        if(aBufferLength == 0)
            [ theOpen := false. aLength int := 0 ];
            [
                int aRead := aBufferLength.

                theEncoder convert int:0 vint:aBufferLength bytearray:aBuffer chararray:anOutput vint:aLength.
                
                if(aRead > aBufferLength)
                    [ theStream reduceIndex int:(aRead - aBufferLength) ]
            ].           
    ]
    
    close
    [
        theStream close.
    ]
}

// --- BinaryFileReader ---

sealed BinaryFileReader :: BinaryReader
{
    constructor new path:aPath
        <= new stream:(FileStream openForRead path:aPath).
}

// --- BinaryFileWriter ---

sealed BinaryFileWriter :: BinaryWriter
{
    constructor new path:aPath
        <= new stream:(FileStream openForReWrite path:aPath).
}

// --- fileOp ---

file_path extension fileExtOp
{
    openForRead = FileStream openForRead path:self.
    
    openForReWrite = FileStream openForReWrite path:self.
    
    openForEdit = FileStream openForEdit path:self.
    
    openForAppend = FileStream openForAppend path:self.    
        
    textreader = StreamReader new path:self.
    
    textwriter = StreamWriter new path:self.
    
    logger = StreamWriter new path:self bool:true.
    
    bool is available
    [
        int ret := system'external'KERNEL32 GetFileAttributesW wide:(self wide).
        
        if(ret != 0FFFFFFFFh)
            [
                ret := ret && $FILE_ATTRIBUTE_DIRECTORY.
                
                ^ ret == 0
            ].
            
        ^ false
    ]
    
    delete
    [
        int ret := system'external'KERNEL32 DeleteFileW wide:(self wide).
        
       ^ ret != 0
    ]
}

// --- directoryExtOp

directory_path extension directoryExtOp
{
    bool is available
    [
        int ret := system'external'KERNEL32 GetFileAttributesW wide:(self wide).
     
        if(ret != 0FFFFFFFFh)
            [
                ret := ret && $FILE_ATTRIBUTE_DIRECTORY.
                
                ^ ret != 0
            ].
            
        ^ false
    ]    
    
    create
    [
        int ret := system'external'KERNEL32 CreateDirectoryW
            wide:(self wide)
            int:0.
            
        if(ret == 0)
            [ IOException new literal:"Cannot create a directory"; raise ]
    ]
    
    delete
    [
        int ret := system'external'KERNEL32 RemoveDirectoryW wide:(self wide).
        
       ^ ret != 0
    ]    
}