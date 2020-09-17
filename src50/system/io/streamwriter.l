namespace io
{

    import system'text;
    
    // --- BinaryWriter ---
    
    public interface BinaryWriter
    {
        Stream theStream;
        
        Stream Stream = theStream;
    
        close() => theStream;
        
        write(byte[] array, int length)
        {
            theStream.write(array, length)
        }
    
        write(byte int)
        {
            byte buffer[4];
            
            buffer[0] := int;
                    
            self.write(buffer, 1)
        }
    
        write(short int)
        {
            byte buffer[4];
            
            buffer.write(0, int);
                    
            self.write(buffer, 2)
        }
    
        write(char ch)
        {
            byte buffer[4];
            
            buffer.write(0, ch);
                    
            self.write(buffer, 4)
        }
    
        write(int int)
        {
            byte buffer[4];
            
            buffer.write(0, int);
                    
            self.write(buffer, 4)
        }
    
        write(long long)
        {
            byte buffer[8];
            
            buffer.write(0, long);
                    
            self.write(buffer, 8)
        }
    
        write(real long)
        {
            byte buffer[8];
            
            buffer.write(0, long);
                    
            self.write(buffer, 8)
        }
    }
    
    public sealed BinaryStreamWriter : BinaryWriter
    {
        constructor(Stream stream)
        {
            theStream := stream
        }
    }
    
    // --- TextWriter ---
    
    public interface TextWriter
    {
        TextBuilder theBuffer;
        string      theNewLineConstant;
        int         theNewLineLength;
    
        get abstract Stream Stream();
    
        get abstract Encoder Encoder();
    
        abstract close();
        
        abstract write(char[] array, int length);
        
        writeLine(line)
        {
            self.write:line.write:theNewLineConstant
        }
        
        write(object)
            <= write(object.Printable);
        
        write(string line)
        {
            theBuffer.clear().write(line);
            
            int length := theBuffer.Length;
            
            self.write(cast char[](theBuffer), length)
        }
        
        write(wide line)
        {
            theBuffer.clear().write(line);
            
            int length := theBuffer.Length;
            
            self.write(cast char[](theBuffer), length)
        }
        
        writeLine()
            <= write(theNewLineConstant);
    }

}