import system'io.
import system'text.

encoder extension encoderOpX
{
    bytearray toByteArray(literal s)
    [
        membuffer arr := MemoryBuffer new.
        byte<256> buffer.
        
        int len := s length.
        int b_len := 0.
        int index := 0.
        
        while (len > 0)
        [
            int s_len := 64.
            if (s_len > len)
            [
                s_len := len
            ].
        
            b_len := 256.
            self convert int:index vint:s_len literal:s bytearray:buffer vint:b_len.    
            
            arr write int:0 int:b_len bytearray:buffer.
            
            index += s_len.
            len -= s_len
        ].
        
        ^ arr bytearray.
    ]
}
