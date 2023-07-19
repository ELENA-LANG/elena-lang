namespace culture
{
   public class Locale : ILocale
   {
      wide _name;

      constructor new(wide name)
      {
         _name := name
      }
        
      constructor()
      {
         _name := nil
      }
      
      string toUppercase(string s)
      {
         string retVal := nil;

         int     i := 0;
         int     len := s.Length;

         while (len > 0)
         {
            int s_len := 32;
            if(s_len > len)
               { s_len := len };
                
            short src_tmp[32];
            int   src_len := 32;
                
            short dst_tmp[32];
            int   dst_len := 64;
    
//            UTF16Encoder.

////                UTF16Encoding.convert(i, ref s_len, self, src_tmp, ref src_len);
////                                        
////                self.strToUpper(src_tmp, src_len, dst_tmp, ref dst_len);
////    
////                if(nil == retVal)
////                { 
////                    retVal := UTF16Encoding.toString(0, dst_len, dst_tmp) 
////                }
////                else
////                { 
////                    retVal := retVal.add(UTF16Encoding.toString(0, dst_len, dst_tmp)) 
////                };
////                    
////                i := i + s_len;
////                len := len - s_len
         };

         ^ retVal
      }
   }

   public static Locale invariantLocale = Locale.Invariant;
    
   public static Locale currentLocale = Locale.Current;
}