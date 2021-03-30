namespace io
{

    import system'text;

    const int SEEK_SET = 0;
    const int SEEK_CUR = 1;
    const int SEEK_END = 2;

    // --- FileStream ---    
    public struct FileStream : Stream
    {
        handle theHandle;
      
        constructor openForRead(path)
            <= new(path, "r+");
        
        constructor openForReWrite(path)
            <= new(path, "w+");
    
        constructor openForAppend(path)
            <= new(path, "a");
    
        constructor new(string path, string attributes)
        {
            theHandle := extern libc.fopen
                          (path,
                          attributes);
                          
            if(-1 == theHandle)
                { IOException.new("Cannot open the file").raise() }
        }
      
        write(byte[] dump, int length)
        {
            extern libc.fwrite(
                dump,
                1,
                length,
                theHandle)
        }
    
        read(byte[] dump, ref int retVal)
        {
            int n := extern libc.fread(
                dump,
                1,
                retVal,
                theHandle);
    
            retVal := n
        }
    
        int Index
        {    
            get()
            {
                int ret := extern libc.lseek(
                    theHandle,
                    0,
                    SEEK_CUR);
                    
                ^ ret
            } 
            
            set(index)
            {
                int ret := extern libc.lseek(
                    theHandle,
                    index,
                    SEEK_SET);
            }
        }
        
        int Length
        {
            get() 
            { 
                int ret := extern libc.lseek(
                    theHandle,
                    0,
                    SEEK_END);
                    
                ^ ret
            }
            
            set(length)
            {
//                int pos := self.Index;
//                
//                self.Index := length;
//                extern KERNEL32.SetEndOfFile(theHandle);
//                
//                if(length > pos)
//                    { self.Index := pos }
            }
        }
        
        close()
        {
            extern libc.fclose(theHandle)
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
                    
                theEncoder.convert(offset, ref blockLength, input, buffer, ref bufferLength);
                
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
        
//        bool isAvailable(path)
//        {
//            wide s := cast wide(path); // !! temporal
//            
//            int ret := extern KERNEL32.GetFileAttributesW(/*cast wide(path)*/s);
//            
//            if(ret != 0FFFFFFFFh)
//            {
//                ret := ret && FILE_ATTRIBUTE_DIRECTORY;
//                
//                ^ ret == 0
//            };
//                
//            ^ false
//        }
//        
//        bool delete(path)
//        {
//            wide s := cast wide(path); // !! temporal
//            
//            int ret := extern KERNEL32.DeleteFileW(/*cast wide(path)*/s);
//            
//           ^ ret != 0
//        }
    }

    singleton directoryControl
    {
    }
}