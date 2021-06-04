// === basic interfaces ===

namespace text
{
    
    // --- Encoder ---
    
    public interface Encoder
    {
        abstract convertFrom(int index, ref int length, byte[] byteArray, char[] output, ref int outputLength);
    
        abstract convertTo(int index, ref int length, char[] charArray, byte[] output, ref int outputLength);
        
        convert(int index, ref int length, string s, byte[] output, ref int outputLength)
        {
            char temp[64];
            int  temp_len := 64;
            if (temp_len > length)
            {
                temp_len := length
            }
            else
            {
                length := temp_len
            };
            
            s.Substring(index, length).save(0, temp, ref temp_len);
            
            self.convert(0, ref temp_len, temp, output, ref outputLength)
        }
        
        convert(int index, ref int length, wide s, byte[] output, ref int outputLength)
        {
            char temp[64];
            int  temp_len := 64;
            if (temp_len > length)
            {
                temp_len := length
            }
            else
            {
                length := temp_len
            };
            
            s.Substring(index, length).save(0, temp, ref temp_len);
            
            self.convert(0, ref temp_len, temp, output, ref outputLength)
        }
        
        toByteArray(int index, int length, string s)
        {
            int    i := index;
            byte[] retVal := nil;
            int    remaining := length;
            
            while (i < length)
            {
                byte tmp[64];
                int  tmp_len := 64;
                int  len := remaining;
                if (len > tmp_len)
                    { len := tmp_len };
                 
                self.convert(i, ref len, s, tmp, ref tmp_len);
                
                if (nil == retVal)
                { 
                    retVal := class Array<byte>.copy(tmp, 0, tmp_len)
                }
                else
                { 
                    retVal := retVal.add(tmp_len, tmp) 
                };
                
                i := i + len;
                remaining := remaining - len
            };
            
            ^ retVal
        }
        
        toByteArray(int index, int length, wide s)
        {
            int    i := index;
            byte[] retVal := nil;
            int    remaining := length;
            
            while (i < length)
            {
                byte tmp[64];
                int  tmp_len := 64;
                int  len := remaining;
                if (len > tmp_len)
                    { len := tmp_len };
                 
                self.convert(i, ref len, s, tmp, ref tmp_len);
                
                if (nil == retVal)
                { 
                    retVal := class Array<byte>.copy(tmp, 0, tmp_len)
                }
                else
                { 
                    retVal := retVal.add(tmp_len, tmp) 
                };
                
                i := i + len;
                remaining := remaining - len
            };
            
            ^ retVal
        }
        
        string toString(int index, int length, byte[] array)
        {
            int    i := index;
            string retVal := nil;
            int    remaining := length;
            
            while (i < length)
            {
                char tmp[64];
                int  tmp_len := 64;
                int  len := remaining;
                if (len > tmp_len)
                    { len := tmp_len };
                 
                self.convert(i, ref len, array, tmp, ref tmp_len);
                
                if (nil == retVal)
                { 
                    retVal := String.copy(0, tmp_len, tmp) 
                }
                else
                { 
                    retVal := retVal.add(String.copy(0, tmp_len, tmp)) 
                };
                
                i := i + len;
                remaining := remaining - len
            };
            
            ^ retVal
        }
        
        wide toWide(int index, int length, byte[] array)
        {
            int  i := index;
            wide retVal := nil;
            int  remaining := length;
            
            while (i < length)
            {
                char tmp[64];
                int  tmp_len := 64;
                int  len := remaining;
                if (len > tmp_len)
                    { len := tmp_len };
                 
                self.convert(i, ref len, array, tmp, ref tmp_len);
                
                if (nil == retVal)
                { 
                    retVal := WideString.copy(0, tmp_len, tmp) 
                }
                else
                { 
                    retVal := retVal.add(WideString.copy(0, tmp_len, tmp)) 
                };
                
                i := i + len;
                remaining := remaining - len
            };
            
            ^ retVal
        }
    }
    
    // --- UTF8Encoder ---
    
    public singleton UTF8Encoder : Encoder
    {
        convertFrom(int index, ref int length, byte[] array, char[] output, ref int outputLength)
            => intern system'core_routines'__utf8encoder;
        
        convertTo(int index, ref int length, char[] array, byte[] output, ref int outputLength)
            => intern system'core_routines'__utf8encoder;
        
        convert(int index, ref int length, string s, byte[] output, ref int outputLength)
        {
            int l := length;
            s.saveToByteArray(index, output, l);
    
            outputLength := length;
        }
        
        string toString(int index, int length, byte[] array)
            = UTF8Encoding.toString(index, length, array);
    }
    
    // --- UTF8Encoding ---
    
    public singleton UTF8Encoding
    {
        Encoder encoder() = UTF8Encoder;
        
        charCount(string s, ref int retVal)
            => intern system'core_routines'__utf8encoder;
        
        convert(int index, int length, string s, byte[] array)
        {
            s.saveToByteArray(index, array, length)
        }
            
        string toString(int index, int length, byte[] array)
            = String.fromByteArray(index, length, array);
            
        convert(int index, ref int length, string s, char[] output, ref int outputLength)
            => intern system'core_routines'__utf8encoder;
    }
    
    // --- UTF16Encoder ---
    
    public singleton UTF16Encoder : Encoder
    {
        convertFrom(int index, ref int length, byte[] byteArray, char[] output, ref int outputLength)
            => intern system'core_routines'__utf16encoder;
        
        convertTo(int index, ref int length, char[] array, byte[] output, ref int outputLength)
            => intern system'core_routines'__utf16encoder;
    }

    // --- UTF16Encoding ---
    
    public singleton UTF16Encoding
    {
        Encoder encoder = UTF16Encoder;
        
        char toChar(short w)
        {
            short buffer[2];
            buffer[0] := w;
            
            charConvertor.convert(0, buffer, ref char retVal);
            
            ^ retVal 
        }
        
        charCount(wide s, ref int retVal)
            => intern system'core_routines'__utf16encoder;
            
        convert(int index, int size, short[] array, char[] output, ref int outputLength)
            => intern system'core_routines'__utf16encoder;
            
        convert(int index, int size, char[] array, short[] output, ref int outputLength)
            => intern system'core_routines'__utf16encoder;
            
        convert(int index, int length, wide s, short[] array)
        {
            s.saveToShortArray(index, array, length)
        }
    
        convert(int index, ref int size, string s, short[] output, ref int outputLength)
        {
            char temp[64];
            int  temp_len := 64;
            if (temp_len < size)
                { size := temp_len };
            
            UTF8Encoding.convert(index, ref size, s, temp, ref temp_len);
            
            self.convert(0, temp_len, temp, output, ref outputLength)
        }
            
        string toString(int index, int length, short[] array)
        {
            char temp[64];
            int  temp_len := 64;
            string s := nil;
            int i := index;
            int l := length;
            while (l > 0)
            {
                temp_len := 64;
                if (temp_len > l)
                {
                    temp_len := l
                };
            
                self.convert(i, temp_len, array, temp, ref temp_len);
                if (s == nil)
                {
                    s := String.copy(0, temp_len, temp)
                }
                else
                {
                    s := s + String.copy(0, temp_len, temp);
                };
                
                l -= temp_len;
            };
            
            ^ s
        }
        
        string toWide(int index, int length, short[] array)
        {
            ^ WideString.fromShortArray(index, length, array)
        }        
    }    
}