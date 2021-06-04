namespace text
{

    const int MB_ERR_INVALID_CHARS     = 00000008h;
    
    // --- WinEncoder ---
    
    public struct WinEncoder : Encoder
    {
        int theCodePage;
        
        constructor(int codePage)
        {
            theCodePage := codePage
        }
    
        convertFrom(int index, ref int length, byte[] array, char[] output, ref int outputLength)
        {
            if (length > 127)
                { InvalidArgumentException.raise() };
            
            short temp_buf[127];
            int   converted := length;
            int   n := length;                
            
            if (index != 0)
            {
                dirty_ptr ptr := /*0*/array;
                
                ptr := ptr + index;
    
                converted := extern KERNEL32.MultiByteToWideChar(theCodePage, MB_ERR_INVALID_CHARS, ptr, n, temp_buf, 127)
            }
            else
            {
                converted := extern KERNEL32.MultiByteToWideChar 
                   (theCodePage,
                    MB_ERR_INVALID_CHARS,
                    array,
                    n,
                    temp_buf,
                    127)
            };
            
            //aConverted *= 2.
            UTF16Encoding.convert(0, converted, temp_buf, output, ref outputLength)
        }
    
        convertTo(int index, ref int length, char[] array, byte[] output, ref int outputLength)
        {
            short temp_buf[127];
            int   temp_len := 127;
            int   chLen := length;
    
            UTF16Encoding.convert(index, chLen, array, temp_buf, ref temp_len);
            
            int retVal := extern KERNEL32.WideCharToMultiByte(
                theCodePage, 0,
                temp_buf,
                temp_len, 0, 0, 0, 0);
    
            if (retVal > chLen)
                { InvalidArgumentException.raise() };
                    
            retVal := extern KERNEL32.WideCharToMultiByte(
                theCodePage, 0, temp_buf, temp_len, output, chLen, 0, 0);
            
            outputLength := retVal
        }
    }
    
    // --- Win1252Encoder ---
    
    public static Encoder WIN1252Encoder = new WinEncoder(1252);
    
    // --- Win1251Encoder ---
    
    public static Encoder WIN1251Encoder = new WinEncoder(1251);
    
    // --- OEMEncoder ---
    
    public static Encoder OEMEncoder = new WinEncoder(1);
    
    // --- AnsiEncoder ---
    
    public static Encoder AnsiEncoder = new WinEncoder(0);

}