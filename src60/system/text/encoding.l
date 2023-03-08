namespace text
{
   // --- Encoder ---
   public interface Encoder
   {
      abstract convertChar(char ch, byte[] byteArray, int index, ref int length);

      abstract char convertToChar(byte[] byteArray, int index, ref int length);

      convertFrom(byte[] byteArray, int index, ref int len, char[] output, int outputIndex, ref int outputLen)
      {
         int i := 0;
         int j := outputIndex;
         while (i < len) {
            char ch := self.convertToChar(byteArray, index + i, ref int chLen);
            output[j] := ch;

            i += chLen;
            j += 1;
         };

         outputLen := j - i;
      }

      convertTo(char[] chars, int index, int len, byte[] output, int outputIndex, ref int outputLen)
      {
         int j := outputIndex;
         for (int i := 0, i < len, i += 1) {
            char ch := chars[i + index];

            self.convertChar(ch, output, j, ref int chLen);

            j += chLen;
         };

         outputLen := j - outputIndex;
      }

      toByteArray(string s, int index, ref int length, byte[] output, int outputIndex, ref int outputLength)
      {
         char tmp[128];
         int end := length;
         if (end > 128) {
            end := 128;
         };
         end += index;

         int i := index;
         int j := 0;
         while (i < end) {
            char ch := s[i];
            int chLen := ch.Length;

            tmp[j] := ch;
            j += 1;
            i += chLen;
         };

         length := i - index;

         self.convertTo(tmp, 0, j, output, outputIndex, /*ref outputLength*/ref int tmpLen);
         outputLength := tmpLen;
      }

   }

   // --- UTF16Encoding ---
   public singleton UTF8Encoding
   {
      string toString(int index, int size, byte[] buffer)
      {
         ^ String.fromByteArray(index, size, buffer);
      }
   }

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