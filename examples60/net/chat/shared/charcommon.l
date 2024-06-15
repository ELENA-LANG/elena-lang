import system'io;
import system'text;
import extensions'dynamic;

import extensions;

public const int INTRODUCTION = 1;
public const int SEND         = 2;
public const int TRANSLATE    = 3;

public const int DEFAULT_PORT = 6881;

public class BaseClient
{
   MemoryBuffer _buffer;

   protected constructor new()
   {
      _buffer := MemoryBuffer.allocate()
   }

   write(byte[] buffer, length)
   {
      _buffer.write(0, length, buffer)
   }

   readCommand()
   {
      int len := _buffer.Length;
      if (len > 0)
      {
            int package_len := _buffer.readInt32(0);
            
            if (package_len + 4 <= len)
            {
                string s := UTF8Encoding.toString(4, package_len, _buffer.Value); // !! should be optimized

                _buffer.delete(0, package_len + 4);
                
                //^ class Serializer<CommandDTO>.fromJson(s)

                ^ s.fromJson()
            }
      };
        
      ^ nil
   }

}

public class CommandDTO
{
    int Command  : dto_prop;
    
    string Value : dto_prop;
    
    string User  : dto_prop;
}