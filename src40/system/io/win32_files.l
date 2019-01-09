import system'text;

// --- File Access Modes ---
//const int FILE_READ_DATA     = 00000001h.
//const int FILE_APPEND_DATA   = 00000004h.
const int FILE_GENERIC_READ  = 80000000h;
//const int FILE_GENERIC_WRITE = 40000000h.

const int OPEN_EXISTING      = 0003h;
//const int CREATE_ALWAYS      = 0002h.
//const int OPEN_ALWAYS        = 0004h.

const int FILE_SHARE_READ    = 00000001h;
//const int FILE_SHARE_WRITE   = 00000002h.

const int FILE_BEGIN = 0;
const int FILE_CURRENT = 1;
//const int FILE_END = 2.
//
//const int FILE_ATTRIBUTE_DIRECTORY = 10h.

// --- FileStream ---

public struct FileStream : Stream
{
    handle theHandle;
    
    constructor openForRead(path)
        <= new(cast wide(path), FILE_GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
    
//    constructor openForReWrite:aPath
//        <= new(T<wide>(aPath), FILE_GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, 0).
//    
//    constructor openForAppend:aPath
//        <= new(T<wide>(aPath), FILE_APPEND_DATA, FILE_SHARE_READ, OPEN_ALWAYS, 0).
//    
//    constructor openForEdit:aPath
//        <= new(T<wide>(aPath), FILE_GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING, 0).
    
    constructor new(wide path, int dwDesiredAccess, int dwShareMode, int dwCreationDisposition, int dwFlagsAndAttributes)
    {
        theHandle := extern KERNEL32.CreateFileW(
                      path,
                      dwDesiredAccess,
                      dwShareMode,
                      0,
                      dwCreationDisposition,
                      dwFlagsAndAttributes,
                      0);
                      
        if(-1 == theHandle)
            { IOException.new("Cannot open the file").raise() }
    }
    
//    write(ByteArray aDump, int aLength)
//    [
//        int aWritten := 0.
//        system'external'KERNEL32 WriteFile(
//            theHandle,
//            aDump,
//            aLength,
//            &aWritten,
//            0).
//    ]

    read(byte[] dump, ref int retVal)
    {
        int toRead := retVal;
        
        extern KERNEL32.ReadFile(
            theHandle,
            dump,
            toRead,
            ref retVal,
            0)
    }
    
    readIndexTo(ref int retVal)
    {
        int ret := extern KERNEL32.SetFilePointer(
            theHandle,
            0,
            0,
            FILE_CURRENT);
            
        retVal := ret
    } 
    
    writeIndex(int index)
    {
        int ret := extern KERNEL32.SetFilePointer(
            theHandle,
            index,
            0,
            FILE_BEGIN)
    }
    
//    readLengthTo(ref<int> aLength) 
//    [ 
//        int n := system'external'KERNEL32 GetFileSize(
//            theHandle,
//            0).
//    
//        aLength value := n. 
//    ]
//    
//    writeLength(int length)
//    [
//        int pos := self index.
//        
//        self writeIndex(length).
//        system'external'KERNEL32 SetEndOfFile(theHandle).
//        
//        if(length > pos)
//            [ self writeIndex(pos) ]
//    ]
    
    close()
    {
        extern KERNEL32.CloseHandle(theHandle)
    }
}

//// --- fileStreamHelper ---
//
//class fileStreamHelper =
//{
//    T<Stream> openForWrite(literal aPath, bool anAppendMode)
//    [
//        if(anAppendMode) [ ^ FileStream openForAppend:aPath ].
//        
//        ^ FileStream openForReWrite:aPath
//    ]
//}.
//
//// --- StreamWriter ---
//
//public class StreamWriter :: TextWriter
//{
//    T<Stream>  theStream.
//    T<Encoder> theEncoder.
//
//    constructor new(Stream aStream, Encoder anEncoder)
//    [
//        theBuffer := TextBuilder new.
//        theStream := aStream.
//        theEncoder := anEncoder.
//
//        theNewLineConstant := forwards'newLine literal.
//    ]
//
//    constructor new:aPath
//        <= new(FileStream openForReWrite:aPath, UTF8Encoder).
//        
//    constructor new(literal aPath, bool anAppendMode)
//        <= new(fileStreamHelper openForWrite(aPath, anAppendMode), UTF8Encoder).
//        
//    T<Stream> stream = theStream.        
//        
//    T<Encoder> encoder = theEncoder.        
//        
//    write(CharArray anInput, int aLength)
//    [
//        int        aTotal := aLength.
//        V<byte,64> aBuffer.
//        int        aBlockLength := 0.
//        int        aBufferLength := 0.
//        int        anOffset := 0.
//        
//        while (aTotal > 0)
//        [
//            if(aTotal < 64)
//                [ aBlockLength := aTotal ];
//                [ aBlockLength := 64 ].
//                
//            theEncoder convert(anOffset, &aBlockLength, anInput, T<ByteArray>(aBuffer), &aBufferLength).
//            
//            theStream write(T<ByteArray>(aBuffer), aBufferLength).
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
//}

// --- StreamReader ---

public class StreamReader : TextReader
{
    Stream  theStream;
    Encoder theEncoder;
    bool    theOpen;

    constructor new(Stream stream, Encoder encoder)
    {
        theOutput := new TextBuilder();
        theNewLineConstant := forward newLine;                
        theNewLineLength := theNewLineConstant.Length;
        
        theStream := stream;
        theEncoder := encoder;
        theOpen := true;
    }

    constructor new(path)
        <= new(FileStream.openForRead(path), UTF8Encoder);
        
//    constructor new(literal aPath, Encoder anEncoder)
//        <= new(FileStream openForRead:aPath, anEncoder).

    bool Available = theOpen;

    Stream Stream = theStream;

    Encoder Encoder = theEncoder;

    read(char[] output, ref int length)
    {
        int  bufferLength := 64;
        byte buffer[64];
        theStream.read(buffer, ref bufferLength);
        if(bufferLength == 0)
        {
            theOpen := false; 
            length := 0;
        }
        else
        {
            int read := bufferLength;

            theEncoder.convert(0, ref bufferLength, buffer, output, ref length);
            
            if(read > bufferLength)
                { theStream.reduceIndex(read - bufferLength) }
        }           
    }
    
    close()
    {
        theStream.close()
    }
}

//// --- BinaryFileReader ---
//
//sealed BinaryFileReader :: BinaryReader
//{
//    constructor new(Stream aStream)
//    [
//        theStream := aStream.
//        
//        theLength := aStream length.
//    ]
//    
//    constructor new:aPath
//        <= new(FileStream openForRead:aPath).
//}
//
//// --- BinaryFileWriter ---
//
//sealed BinaryFileWriter :: BinaryWriter
//{
//    constructor new(Stream aStream)
//    [
//        theStream := aStream.
//    ]
//    
//    constructor new:aPath
//        <= new(FileStream openForReWrite:aPath).
//}

// --- fileOp ---

singleton fileControl
{
//    openForRead : path = FileStream openForRead:path.
//    
//    openForReWrite : path = FileStream openForReWrite:path.
//    
//    openForEdit : path = FileStream openForEdit:path.
//    
//    openForAppend : path = FileStream openForAppend:path.
        
    TextReader newReader(path) = StreamReader.new:path;
    
//    T<TextWriter> writer : path = StreamWriter new:path.
//    
//    T<TextWriter> logger :path = StreamWriter new(path, true).
//    
//    bool isAvailable:path
//    [
//        int ret := system'external'KERNEL32 GetFileAttributesW(T<wide>(path)).
//        
//        if(ret != 0FFFFFFFFh)
//            [
//                ret := ret && FILE_ATTRIBUTE_DIRECTORY.
//                
//                ^ ret == 0
//            ].
//            
//        ^ false
//    ]
//    
//    delete : path
//    [
//        int ret := system'external'KERNEL32 DeleteFileW(T<wide>(path)).
//        
//       ^ ret != 0
//    ]
}

//// --- directoryExtOp
//
//singleton directoryControl
//{
//    bool isAvailable:path
//    [
//        int ret := system'external'KERNEL32 GetFileAttributesW(T<wide>(path)).
//     
//        if(ret != 0FFFFFFFFh)
//            [
//                ret := ret && FILE_ATTRIBUTE_DIRECTORY.
//                
//                ^ ret != 0
//            ].
//            
//        ^ false
//    ]    
//    
//    create : path
//    [
//        int ret := system'external'KERNEL32 CreateDirectoryW(
//            T<wide>(path),
//            0).
//            
//        if(ret == 0)
//            [ IOException new("Cannot create a directory"); raise ]
//    ]
//    
//    delete : path
//    [
//        int ret := system'external'KERNEL32 RemoveDirectoryW(T<wide>(path)).
//        
//       ^ ret != 0
//    ]    
//}