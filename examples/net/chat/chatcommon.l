import system'text;
import system'io;
////import extensions.
import extensions'dynamic;

const int INTRODUCTION = 1;
const int SEND         = 2;
const int TRANSLATE    = 3;

const int DEFAULT_PORT = 6881;

class BaseClient
{
    MemoryBuffer theBuffer;
    
    protected constructor new()
    {
        theBuffer := MemoryBuffer.allocate()
    }

    write(byte[] buffer, length)
    {
        theBuffer.write(0, length, buffer)
    }
    
    readCommand()
    {
        int len := theBuffer.Length;
        if (len > 0)
        {
            int package_len := theBuffer.readInt32(0);
            
            if (package_len + 4 <= len)
            {
                string s := UTF8Encoding.toString(4, package_len, theBuffer.Value); // !! should be optimized
                
                int llen := s.Length;
                
                theBuffer.delete(0, package_len + 4);
                
                //^ s.fromJson()
                
                var o := s.fromJson();
                ^ o
            }
        };
        
        ^ nil
    }
}

class CommandDTO : DTO
{
    dto_prop int Command;
    
    dto_prop string Value;
    
    dto_prop string User;
}