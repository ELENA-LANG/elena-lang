namespace text
{

  // --- UTF16Encoding ---
   public singleton UTF16Encoding
   {
       char toChar(short w)
       {
          char retVal := charConvertor.convert(w);
           
          ^ retVal 
       }
   }

}