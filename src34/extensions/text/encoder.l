import system'io.
import system'text.

public extension<Encoder> encoderOpX
{
    T<ByteArray> toByteArray(literal s)
    [
        auto arr := MemoryBuffer new.
        V<byte,256> buffer.
        
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
            self convert(index, &s_len, s, T<ByteArray>(buffer), &b_len).    
            
            arr write(0, b_len, T<ByteArray>(buffer)).
            
            index += s_len.
            len -= s_len
        ].
        
        ^ arr.
    ]
}
