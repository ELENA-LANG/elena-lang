namespace io
{

    //import system'routines.
    import system'text;
    
    // --- BinaryReader ---
    
    public interface BinaryReader
    {
        Stream theStream;
        int    theLength;
        
        read(byte[] array, ref int actualLength)
        {
            theStream.read(array, ref actualLength)
        }
    
        Stream Stream = theStream;
    
        close() => theStream;
    
        get bool Available()
        {
            int position := theStream.Index;
            
            ^ position < theLength
        }
                
        read(byte[] array, int length)
        {
            int read := length;
            
            self.read(array, ref read);
             
            if(length > read)
                { IOException.new("Cannot read the specified amount").raise() }
        }
    
        read(ref byte int)
        {
            byte buffer[4];
            int  read := 1;
            
            self.read(buffer, ref read);
            
            if (read < 1)
                { IOException.new("Cannot read the specified amount").raise() };
         
            byte b := buffer[0];
                              
            int := b
        }
    
        read(ref short int)
        {
            byte buffer[4];
            int  read := 2;
            
            self.read(buffer, ref read);
            
            if (read < 2)
                { IOException.new("Cannot read the specified amount").raise() };
                
            buffer.read(0, ref int)
        }
    
        read(ref char ch)
        {
            byte buffer[4];
            int  read := 4;
            
            self.read(buffer, ref read);
            
            if (read < 4)
                { IOException.new("Cannot read the specified amount").raise() };
                
            buffer.read(0, ref ch)
        }
    
        read(ref int int)
        {
            byte buffer[4];
            int  read := 4;
            
            self.read(buffer, ref read);
            
            if (read < 4)
                { IOException.new("Cannot read the specified amount").raise() };
                
            buffer.read(0, ref int)
        }
    
        read(ref long long)
        {
            byte buffer[8];
            int  read := 8;
            
            self.read(buffer, ref read);
            
            if (read < 8)
                { IOException.new("Cannot read the specified amount").raise() };
                
            buffer.read(0, ref long)
        }
    
        read(ref real real)
        {
            byte buffer[8];
            int  read := 8;
            
            self.read(buffer, ref read);
            
            if (read < 8)
                { IOException.new("Cannot read the specified amount").raise() };
                
            buffer.read(0, ref real)
        }
    }
    
    public sealed BinaryStreamReader : BinaryReader
    {
        constructor(Stream stream)
        {
            theStream := stream
        }
    }
    
    // --- TextReader ---
    
    public interface TextReader
    {
        TextBuilder theOutput;
        string      theNewLineConstant;
        int         theNewLineLength;
    
        abstract read(char[] array, ref int actualLength);
    
        get abstract Stream Stream();
    
        get abstract Encoder Encoder();
    
        abstract close();
        
        get abstract bool Available();
                
        string readLine()
        {
            string line := emptyString;
            
            int len := -1;
            while (len == -1)
            {
                len := theOutput.indexOf(0, theNewLineConstant);
                
                if(len == -1)
                {
                    char buffer[64];
                    int  bufferLen := 64;
                    
                    self.read(buffer, ref bufferLen);
                    if(bufferLen == 0)
                    { 
                        len := -2 // !! to indicate the eof 
                    }
                    else
                    {
                        theOutput.write(buffer, bufferLen)
                    }
                }
            };
            
            if(len < 0) 
            {
                line := cast string(theOutput);
                theOutput.clear();
            }
            else
            {
                line := theOutput.Substring(0, len);
                theOutput.delete(0, len + theNewLineLength)
            };
    
            ^ line
        }
        
        wide readWideLine()
        {
            wide line := emptyWideString;
            wide nl := cast wide(theNewLineConstant);
            int  nl_len := nl.Length;
            
            int len := -1;
            while (len == -1)
            {
                len := theOutput.indexOf(0, nl);
                
                if(len == -1)
                {
                    char buffer[64];
                    int  bufferLen := 64;

                    self.read(buffer, ref bufferLen);
                    if(bufferLen == 0)
                    { 
                        len := -2 // !! to indicate the eof 
                    }
                    else
                    {
                        theOutput.write(buffer, bufferLen)
                    }                                          
                }
            };

            if(len < 0) 
            {
                line := cast wide(theOutput);
                theOutput.clear();
            }
            else
            {
                line := theOutput.WideSubstring(0, len);
                theOutput.delete(0, len + nl_len)
            };
    
            ^ line
        }
    }

}