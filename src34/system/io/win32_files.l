import system'text.

// --- File Access Modes ---
const int FILE_READ_DATA     = 00000001h.
const int FILE_APPEND_DATA   = 00000004h.
const int FILE_GENERIC_READ  = 80000000h.
const int FILE_GENERIC_WRITE = 40000000h.

const int OPEN_EXISTING      = 0003h.
const int CREATE_ALWAYS      = 0002h.
const int OPEN_ALWAYS        = 0004h.

const int FILE_SHARE_READ    = 00000001h.
const int FILE_SHARE_WRITE   = 00000002h.

const int FILE_BEGIN = 0.
const int FILE_CURRENT = 1.
const int FILE_END = 2.

const int FILE_ATTRIBUTE_DIRECTORY = 10h.

// --- FileStream ---

struct FileStream :: Stream
{
    handle theHandle.
    
    constructor openForRead:aPath
        <= new(T<wide>(aPath), FILE_GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0).
    
    constructor openForReWrite:aPath
        <= new(T<wide>(aPath), FILE_GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, 0).
    
    constructor openForAppend:aPath
        <= new(T<wide>(aPath), FILE_APPEND_DATA, FILE_SHARE_READ, OPEN_ALWAYS, 0).
    
    constructor openForEdit:aPath
        <= new(T<wide>(aPath), FILE_GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING, 0).
    
    constructor new(wide aPath, int dwDesiredAccess, int dwShareMode, int dwCreationDisposition, int dwFlagsAndAttributes)
    [
        theHandle := system'external'KERNEL32 CreateFileW(
                      aPath,
                      dwDesiredAccess,
                      dwShareMode,
                      0,
                      dwCreationDisposition,
                      dwFlagsAndAttributes,
                      0).
                      
        if(-1 == theHandle)
            [ IOException new("Cannot open the file"); raise ]
    ]
    
    write(bytearray aDump, int aLength)
    [
        int aWritten := 0.
        system'external'KERNEL32 WriteFile(
            theHandle,
            aDump,
            aLength,
            &aWritten,
            0).
    ]

    read(bytearray aDump, ref<int> aLength)
    [
        system'external'KERNEL32 ReadFile(
            theHandle,
            aDump,
            aLength,
            aLength,
            0).
    ]
    
    readIndexTo(ref<int> anIndex)
    [
        int ret := system'external'KERNEL32 SetFilePointer(
            theHandle,
            0,
            0,
            FILE_CURRENT).
            
        anIndex value := ret.
    ] 
    
    writeIndex(int anIndex)
    [
        int ret := system'external'KERNEL32 SetFilePointer(
            theHandle,
            anIndex,
            0,
            FILE_BEGIN).
    ] 
    
    readLengthTo(ref<int> aLength) 
    [ 
        int n := system'external'KERNEL32 GetFileSize(
            theHandle,
            0).
    
        aLength value := n. 
    ]
    
    writeLength(int length)
    [
        int pos := self index.
        
        self writeIndex(length).
        system'external'KERNEL32 SetEndOfFile(theHandle).
        
        if(length > pos)
            [ self writeIndex(pos) ]
    ]
    
    close
    [
        system'external'KERNEL32 CloseHandle(theHandle).
    ]
}

//// --- fileStreamHelper ---
//
//class $fileStreamHelper =
//{
//    stream openForWrite literal:aPath bool:anAppendMode
//    [
//        if(anAppendMode) [ ^ FileStream openForAppend:aPath ].
//        
//        ^ FileStream openForReWrite:aPath
//    ]
//}.

// --- StreamWriter ---

public class StreamWriter :: TextWriter
{
//    stream  theStream.
//    encoder theEncoder.
//
//    constructor new stream:aStream encoder:anEncoder
//        <= new;
//    [
//        theStream := aStream.
//        theEncoder := anEncoder.
//
//        theNewLineConstant := system'text'newLine literal.
//    ]
//
//    constructor new:aPath
//        <= new stream:(FileStream openForReWrite:aPath) encoder:UTF8Encoder.
//        
//    constructor new literal:aPath bool:anAppendMode
//        <= new stream($fileStreamHelper openForWrite(aPath, anAppendMode)) encoder:UTF8Encoder.
//        
//    stream = theStream.        
//        
//    encoder = theEncoder.        
//        
//    write chararray:anInput int:aLength
//    [
//        int      aTotal := aLength.
//        byte<64> aBuffer.
//        int      aBlockLength := 0.
//        int      aBufferLength := 0.
//        int      anOffset := 0.
//        
//        while (aTotal > 0)
//        [
//            if(aTotal < 64)
//                [ aBlockLength := aTotal ];
//                [ aBlockLength := 64 ].
//                
//            theEncoder convert int:anOffset vint:aBlockLength chararray:anInput bytearray:aBuffer vint:aBufferLength.
//            
//            theStream write bytearray:aBuffer int:aBufferLength.
//            
//            aTotal := aTotal - aBlockLength.
//            anOffset := anOffset + aBlockLength
//        ]
//    ]
//    
//    close
//    [
//        theStream close
//    ]
}

// --- StreamReader ---

public class StreamReader :: TextReader
{
    stream  theStream.
    encoder theEncoder.
    bool    theOpen.

    constructor new(stream aStream, encoder anEncoder)
        <= new;
    [
        theStream := aStream.
        theEncoder := anEncoder.
        theOpen := true.
    ]

    constructor new:aPath
        <= new(FileStream openForRead:aPath, UTF8Encoder).

    constructor new(literal aPath, encoder anEncoder)
        <= new(FileStream openForRead:aPath, anEncoder).

    bool available = theOpen.

//    stream = theStream.
//
//    encoder = theEncoder.

    read(chararray anOutput, ref<int> aLength)
    [
        int        aBufferLength := 64.
        T<byte,64> aBuffer.
        theStream read(T<bytearray>(aBuffer), &aBufferLength).
        if(aBufferLength == 0)
            [ theOpen := false. aLength value := 0 ];
            [
                int aRead := aBufferLength.

                theEncoder convert(0, &aBufferLength, T<bytearray>(aBuffer), anOutput, aLength).
                
                if(aRead > aBufferLength)
                    [ theStream reduceIndex(aRead - aBufferLength) ]
            ].           
    ]
    
    close
    [
        theStream close.
    ]
}

//// --- BinaryFileReader ---
//
//sealed BinaryFileReader :: BinaryReader
//{
//    constructor new path:aPath
//        <= new stream:(FileStream openForRead:aPath).
//}
//
//// --- BinaryFileWriter ---
//
//sealed BinaryFileWriter :: BinaryWriter
//{
//    constructor new:aPath
//        <= new stream:(FileStream openForReWrite:aPath).
//}
//
//// --- fileOp ---
//
//singleton fileControl
//{
//    openForRead : path = FileStream openForRead:path.
//    
//    openForReWrite : path = FileStream openForReWrite:path.
//    
//    openForEdit : path = FileStream openForEdit:path.
//    
//    openForAppend : path = FileStream openForAppend:path.
//        
//    textreader reader : path = StreamReader new:path.
//    
//    textwriter writer : path = StreamWriter new:path.
//    
//    textwriter logger :path = StreamWriter new(path, true).
//    
//    bool isAvailable:path
//    [
//        int ret := system'external'KERNEL32 GetFileAttributesW wide:(path wide).
//        
//        if(ret != 0FFFFFFFFh)
//            [
//                ret := ret && $FILE_ATTRIBUTE_DIRECTORY.
//                
//                ^ ret == 0
//            ].
//            
//        ^ false
//    ]
//    
//    delete : path
//    [
//        int ret := system'external'KERNEL32 DeleteFileW wide:(path wide).
//        
//       ^ ret != 0
//    ]
//}
//
//// --- directoryExtOp
//
//singleton directoryControl
//{
//    bool isAvailable:path
//    [
//        int ret := system'external'KERNEL32 GetFileAttributesW wide:(path wide).
//     
//        if(ret != 0FFFFFFFFh)
//            [
//                ret := ret && $FILE_ATTRIBUTE_DIRECTORY.
//                
//                ^ ret != 0
//            ].
//            
//        ^ false
//    ]    
//    
//    create : path
//    [
//        int ret := system'external'KERNEL32 CreateDirectoryW
//            wide:(path wide)
//            int:0.
//            
//        if(ret == 0)
//            [ IOException new literal:"Cannot create a directory"; raise ]
//    ]
//    
//    delete : path
//    [
//        int ret := system'external'KERNEL32 RemoveDirectoryW wide:(path wide).
//        
//       ^ ret != 0
//    ]    
//}