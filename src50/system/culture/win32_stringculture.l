namespace culture
{
    import system'text;
    
    // --- literalCultureOp ---
    
    public extension stringCultureOp : String
    {
        string toUpper(Locale locale)
        {
            string retVal := nil;
            
            int     i := 0;
            int     len := self.Length;
            
            while (len > 0)
            {
                int s_len := 32;
                if(s_len > len)
                    { s_len := len };
                
                short src_tmp[32];
                int   src_len := 32;
                
                short dst_tmp[32];
                int   dst_len := 64;
    
                UTF16Encoding.convert(i, ref s_len, self, src_tmp, ref src_len);
                                        
                locale.strToUpper(src_tmp, src_len, dst_tmp, ref dst_len);
    
                if(nil == retVal)
                { 
                    retVal := UTF16Encoding.toString(0, dst_len, dst_tmp) 
                }
                else
                { 
                    retVal := retVal.add(UTF16Encoding.toString(0, dst_len, dst_tmp)) 
                };
                    
                i := i + s_len;
                len := len - s_len
            };
            
            ^ retVal
        }
        
        string toLower(Locale locale)
        {
            string retVal := nil;
            
            int i := 0;
            int len := self.Length;
            
            while (len > 0)
            {
                int s_len := 32;
                if (s_len > len)
                    { s_len := len };
                
                short src_tmp[32];
                int   src_len := 32;
                
                short dst_tmp[32];
                int   dst_len := 64;
    
                UTF16Encoding.convert(i, ref s_len, self, src_tmp, ref src_len);
                                        
                locale.strToLower(src_tmp, src_len, dst_tmp, ref dst_len);
    
                if(nil == retVal)
                { 
                    retVal := UTF16Encoding.toString(0, dst_len, dst_tmp) 
                }
                else
                {
                    retVal := retVal.add(UTF16Encoding.toString(0, dst_len, dst_tmp)) 
                };
                    
                i := i + s_len;
                len := len - s_len
            };
            
            ^ retVal
        }
    }
    
    // --- wideCultureOp ---
    
    public extension wideCultureOp : WideString
    {
        wide toUpper(Locale locale)
        {
            wide retVal := nil;
            
            int i := 0;
            int len := self.Length;
            
            while (len > 0)
            {
                short src_tmp[32];
                int   src_len := 32;
                
                if(src_len > len)
                    { src_len := len };
                
                short dst_tmp[32];
                int   dst_len := 64;
                
                self.saveToShortArray(i, src_tmp, src_len);
                
                locale.strToUpper(src_tmp, src_len, dst_tmp, ref dst_len);
                
                if(nil == retVal)
                { 
                    retVal := WideString.fromShortArray(0, dst_len, dst_tmp) 
                }
                else
                { 
                    retVal := retVal.add(WideString.fromShortArray(0, dst_len, dst_tmp)) 
                };
                    
                i := i + src_len;
                len := len - src_len
            };
            
            ^ retVal
        }
        
        wide toLower(Locale locale)
        {
            wide retVal := nil;
            
            int i := 0;
            int len := self.Length;
            
            while (len > 0)
            {
                short src_tmp[32];
                int   src_len := 32;
                
                if(src_len > len)
                    { src_len := len };
                
                short dst_tmp[32];
                int   dst_len := 64;
                
                self.saveToShortArray(i, src_tmp, src_len);
                
                locale.strToLower(src_tmp, src_len, dst_tmp, ref dst_len);
                
                if(nil == retVal)
                { 
                    retVal := WideString.fromShortArray(0, dst_len, dst_tmp) 
                }
                else
                { 
                    retVal := retVal.add(WideString.fromShortArray(0, dst_len, dst_tmp)) 
                };
                    
                i := i + src_len;
                len := len - src_len
            };
            
            ^ retVal
        }
    }
}