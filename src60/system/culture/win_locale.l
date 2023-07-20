import system'text;

namespace culture
{
   // --- Constants ---
   const int LCMAP_LOWERCASE = 00000100h;  // lower case letters
   const int LCMAP_UPPERCASE = 00000200h;  // UPPER CASE LETTERS
    
   public sealed class Locale : ILocale
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
      
      private strToUpper(short[] src, int length, short[] dest, ref int destLength)
      {
         int retVal := extern KERNEL32.LCMapStringEx(
                _name,
                LCMAP_UPPERCASE,
                src,
                length,
                dest,
                destLength,
                0,
                0,
                0);
                
         destLength := retVal
      }
    
      private strToLower(short[] src, int length, short[] dest, ref int destLength)
      {
         int retVal := extern KERNEL32.LCMapStringEx(
                _name,
                LCMAP_LOWERCASE,
                src,
                length,
                dest,
                destLength,
                0,
                0,
                0);
                
         destLength := retVal            
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
            int   dst_len := 32;
    
            UTF16Encoder.toShortArray(s, i, s_len, src_tmp);

            self.strToUpper(src_tmp, src_len, dst_tmp, ref dst_len);
    
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

      string toLowercase(string s)
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
            int   dst_len := 32;
    
            UTF16Encoder.toShortArray(s, i, s_len, src_tmp);

            self.strToLower(src_tmp, src_len, dst_tmp, ref dst_len);
    
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

   public static Locale invariantLocale = Locale.Invariant;
    
   public static Locale currentLocale = Locale.Current;
}