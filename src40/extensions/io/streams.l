import system'text;

public extension streamExOp
{
    writeString(string s)
    {
        byte buffer[255];
        int b_len := 255;
        int len := s.Length;
        int index := 0;
        while (len > 0) {
            if (len < 255)
            {
                b_len := len
            };
            
            UTF8Encoding.convert(0, b_len, s, buffer);
            self.write(buffer, b_len);
            
            len -= b_len;
            index += b_len;
        }
    }
    
    writeByte(byte n)
    {
        self.write(n)
    }
    writeByte(n)
    {
        self.write(cast byte(n));
    }
}