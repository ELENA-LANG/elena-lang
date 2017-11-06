import system'text.
import system'io.
import extensions.
import extensions'dynamic.

const int INTRODUCTION = 1.
const int SEND         = 2.
const int TRANSLATE    = 3.

class BaseClient
{
    membuffer theBuffer.
    
    constructor new
    [
        theBuffer := MemoryBuffer new.
    ]    

    stacksafe write bytearray:buffer int:length
    [
        theBuffer write int:0 int:length bytearray:buffer.
    ]
    
    readCommand
    [
        int len := theBuffer length.
        if (len > 0)
        [
            int package_len := 0.
            theBuffer read int:0 vint:package_len.
            
            if (package_len + 4 <= len)
            [
                literal literal := UTF8Encoding toLiteral int:4 int:package_len bytearray(theBuffer bytearray).
                
                var llen := literal length.
                
                theBuffer delete int:0 int(package_len + 4).
                
                ^ literal fromJson.
            ].
        ].
        
        ^ $nil.
    ]        
}

class CommandDTO
{
    dto_prop Command :: _command.
    
    dto_prop Value :: _value.
}