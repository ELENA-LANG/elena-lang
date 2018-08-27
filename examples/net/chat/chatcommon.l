import system'text.
import system'io.
//import extensions.
//import extensions'dynamic.

const int INTRODUCTION = 1.
const int SEND         = 2.
const int TRANSLATE    = 3.

class BaseClient
{
    T<MemoryBuffer> theBuffer.
    
    constructor new
    [
        theBuffer := MemoryBuffer new.
    ]    

    stacksafe write(T<ByteArray> buffer, length)
    [
        theBuffer write(0, length, buffer)
    ]
    
    readCommand
    [
        int len := theBuffer length.
        if (len > 0)
        [
            int package_len := 0.
            theBuffer read(0, &package_len).
            
            if (package_len + 4 <= len)
            [
                literal literal := UTF8Encoding toLiteral(4, package_len, T<ByteArray>(theBuffer)). // !! should be optimized
                
                int llen := literal length.
                
                theBuffer delete(0, package_len + 4).
                
                ^ literal fromJson.
            ].
        ].
        
        ^ nil
    ]        
}

class CommandDTO
{
    dto_prop Command :: _command.
    
    dto_prop Value :: _value.
    
    dto_prop User :: _user.
}