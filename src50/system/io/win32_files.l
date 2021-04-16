namespace io
{

    import system'text;
    
    // --- File Access Modes ---
    const int FILE_READ_DATA     = 00000001h;
    const int FILE_APPEND_DATA   = 00000004h;
    const int FILE_GENERIC_READ  = 80000000h;
    const int FILE_GENERIC_WRITE = 40000000h;
    
    const int OPEN_EXISTING      = 0003h;
    const int CREATE_ALWAYS      = 0002h;
    const int OPEN_ALWAYS        = 0004h;
    
    const int FILE_SHARE_READ    = 00000001h;
    const int FILE_SHARE_WRITE   = 00000002h;
    
    const int FILE_BEGIN = 0;
    const int FILE_CURRENT = 1;
    const int FILE_END = 2;
    
    const int FILE_ATTRIBUTE_DIRECTORY = 10h;
    
    // --- FileStream ---
    
    public struct FileStream : Stream
    {
        handle theHandle;
        
        constructor openForRead(path)
            <= new(cast wide(path), FILE_GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, 0);
        
        constructor openForReWrite(path)
            <= new(cast wide(path), FILE_GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, 0);
        
        constructor openForAppend(path)
            <= new(cast wide(path), FILE_APPEND_DATA, FILE_SHARE_READ, OPEN_ALWAYS, 0);
        
        constructor openForEdit(path)
            <= new(cast wide(path), FILE_GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING, 0);
        
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
        
        write(byte[] dump, int length)
        {
            int written := 0;
            extern KERNEL32.WriteFile(
                theHandle,
                dump,
                length,
                ref written,
                0)
        }
    
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
    
        int Index
        {    
            get()
            {
                int ret := extern KERNEL32.SetFilePointer(
                    theHandle,
                    0,
                    0,
                    FILE_CURRENT);
                    
                ^ ret
            } 
            
            set(index)
            {
                int ret := extern KERNEL32.SetFilePointer(
                    theHandle,
                    index,
                    0,
                    FILE_BEGIN)
            }
        }
        
        int Length
        {
            get() 
            { 
                int n := extern KERNEL32.GetFileSize(
                    theHandle,
                    0);
            
                ^ n 
            }
            
            set(length)
            {
                int pos := self.Index;
                
                self.Index := length;
                extern KERNEL32.SetEndOfFile(theHandle);
                
                if(length > pos)
                    { self.Index := pos }
            }
        }

        close()
        {
            extern KERNEL32.CloseHandle(theHandle)
        }
    }
    
    // --- fileStreamHelper ---
    
    fileStreamHelper = new
    {
        Stream openForWrite(string path, bool appendMode)
        {
            if(appendMode) { ^ FileStream.openForAppend(path) };
            
            ^ FileStream.openForReWrite(path)
        }
    };
    
    // --- StreamWriter ---
    
    public class StreamWriter : TextWriter
    {
        Stream  theStream;
        Encoder theEncoder;
    
        constructor new(Stream stream, Encoder encoder)
        {
            theBuffer := new TextBuilder();
            theStream := stream;
            theEncoder := encoder;
    
            theNewLineConstant := forward newLine;                
            theNewLineLength := theNewLineConstant.Length;
        }
    
        constructor new(path)
            <= new(FileStream.openForReWrite(path), UTF8Encoder);
            
        constructor new(string path, bool appendMode)
            <= new(fileStreamHelper.openForWrite(path, appendMode), UTF8Encoder);
            
        Stream Stream = theStream;        
            
        Encoder Encoder = theEncoder;
            
        write(char[] input, int length)
        {
            int  total := length;
            byte buffer[64];
            int  blockLength := 0;
            int  bufferLength := 0;
            int  offset := 0;
            
            while (total > 0)
            {
                if(total < 64)
                { 
                    blockLength := total 
                }
                else
                { 
                    blockLength := 64 
                };
                    
                theEncoder.convertTo(offset, ref blockLength, input, buffer, ref bufferLength);
                
                theStream.write(buffer, bufferLength);
                
                total := total - blockLength;
                offset := offset + blockLength
            }
        }
        
        close()
        {
            theStream.close()
        }
    }
    
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
            
        constructor new(string path, Encoder encoder)
            <= new(FileStream.openForRead:path, encoder);
    
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
    
                theEncoder.convertFrom(0, ref bufferLength, buffer, output, ref length);
                
                if(read > bufferLength)
                    { theStream.reduceIndex(read - bufferLength) }
            }           
        }
        
        close()
        {
            theStream.close()
        }
    }
    
    // --- BinaryFileReader ---
    
    public sealed BinaryFileReader : BinaryReader
    {
        constructor new(Stream stream)
        {
            theStream := stream;
            
            theLength := stream.Length;
        }
        
        constructor new(path)
            <= new(FileStream.openForRead(path));
    }
    
    // --- BinaryFileWriter ---
    
    public sealed BinaryFileWriter : BinaryWriter
    {
        constructor new(Stream stream)
        {
            theStream := stream
        }
        
        constructor new(path)
            <= new(FileStream.openForReWrite:path);
    }
    
    // --- fileOp ---
    
    singleton fileControl
    {
        BinaryReader openForRead(path) = new BinaryStreamReader(FileStream.openForRead(path));
        
        BinaryReader openForReWrite(path) = new BinaryStreamReader(FileStream.openForReWrite(path));
        
        BinaryReader openForEdit(path) = new BinaryStreamReader(FileStream.openForEdit(path));
        
        BinaryReader openForAppend(path) = new BinaryStreamReader(FileStream.openForAppend(path));
            
        TextReader newReader(path) = StreamReader.new:path;
        
        TextWriter writer(path) = StreamWriter.new:path;
        
        TextWriter logger(path) = StreamWriter.new(path, true);
        
        bool isAvailable(path)
        {
            wide s := cast wide(path); // !! temporal
            
            int ret := extern KERNEL32.GetFileAttributesW(/*cast wide(path)*/s);
            
            if(ret != 0FFFFFFFFh)
            {
                ret := ret & FILE_ATTRIBUTE_DIRECTORY;
                
                ^ ret == 0
            };
                
            ^ false
        }
        
        bool delete(path)
        {
            wide s := cast wide(path); // !! temporal
            
            int ret := extern KERNEL32.DeleteFileW(/*cast wide(path)*/s);
            
           ^ ret != 0
        }
    }
    
    // --- directoryExtOp
    
    singleton directoryControl
    {
        bool isAvailable(path)
        {
            wide s := cast wide(path); // !! temporal
            
            int ret := extern KERNEL32.GetFileAttributesW(/*cast wide(path)*/s);
         
            if(ret != 0FFFFFFFFh)
                {
                    ret := ret & FILE_ATTRIBUTE_DIRECTORY;
                    
                    ^ ret != 0
                };
                
            ^ false
        }    
        
        create(path)
        {
            wide s := cast wide(path); // !! temporal
            
            int ret := extern KERNEL32.CreateDirectoryW(
                /*cast wide(path)*/s,
                0);
                
            if(ret == 0)
                { IOException.new("Cannot create a directory").raise() }
        }
        
        delete(path)
        {
            wide s := cast wide(path); // !! temporal
            
            int ret := extern KERNEL32.RemoveDirectoryW(/*cast wide(path)*/s);
            
           ^ ret != 0
        }    
    }

}