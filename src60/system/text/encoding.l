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

         outputLen := j - outputIndex;
      }

      convertTo(char[] chars, int index, ref int len, byte[] output, int outputIndex, ref int outputLen)
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

         self.convertTo(tmp, 0, j, output, outputIndex, ref outputLength);
      }

      string toString(int index, int length, byte[] buffer)
      {
         char tmp[128];
         string retVal := emptyString;
         int i := index;
         int len := length;
         int subLen := 128;
         while (len > 0) {
            if (len < subLen)
               subLen := len;

            self.convertFrom(buffer, i, ref subLen, tmp, 0, ref int outputLen);
            
            retVal := retVal + String.copy(0, outputLen, tmp);

            i += subLen;
            len -= subLen;
         };

         ^ retVal
      }

      wide toWideString(int index, int size, byte[] buffer, Encoder encoder)
      {
         char tmp[128];
         wide retVal := emptyWideString;
         int len := size;
         int subLen := 128;
         int i := index;
         while (len > 0) {
            if (len < subLen)
               subLen := len;

            self.convertFrom(buffer, i, ref subLen, tmp, 0, ref int outputLen);
            
            retVal := retVal + WideString.copy(0, outputLen, tmp);

            i += subLen;
            len -= subLen;            
         };

         ^ retVal
      }
   }

   // --- UTF8Encoding ---
   public singleton UTF8Encoding
   {
      char toChar(byte b)
      {
         char retVal := charConvertor.convert(b);
           
         ^ retVal 
      }

      string toString(int index, int size, byte[] buffer)
      {
         ^ String.fromByteArray(index, size, buffer);
      }

      int getCharCount(string s)
      {
         int len := 0;
         auto en := s.enumerator();
         while (en.next()) {
            len += 1;
         };

         ^ len
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

      char toChar(short[] shortArray, int index, ref int length)
      {
         int chLen := UTFOperations.convertUTF16_2_32(shortArray, index, length, ref int value);
         char ch := value;

         length := chLen;

         ^ ch
      }

      wide toWideString(int index, int size, short[] buffer)
      {
         ^ WideString.fromShortArray(index, size, buffer);
      }

      toShortArray(wide s, int index, int length, short[] output)
      {
         s.saveToShortArray(index, output, length);
      }

      int getCharCount(wide s)
      {
         int len := 0;
         auto en := s.enumerator();
         while (en.next()) {
            len += 1;
         };

         ^ len
      }
   }

   // --- UTF8Encoder ---
   public sealed singleton UTF8Encoder : Encoder
   {
      convertChar(char ch, byte[] byteArray, int index, ref int length)
      {
         int value := ch;

         int byteLen := byteArray.Length;
         length := UTFOperations.convertUTF32_2_8(value, byteArray, index, byteLen);
      }

      char convertToChar(byte[] byteArray, int index, ref int sourLen)
      {
         sourLen := UTFOperations.convertUTF8_2_32(byteArray, index, sourLen, ref int value);
         char ch := value;

         ^ ch
      }
   }   

   // --- UTF16Encoder ---
   public sealed singleton UTF16Encoder : Encoder
   {
      convertChar(char ch, byte[] byteArray, int index, ref int length)
      {
         int value := ch;
         short tmp[2];

         int tmpLen := UTFOperations.convertUTF32_2_16(value, tmp, 0, 2);

         byte hi := tmp[0].High;
         byte lo := tmp[0].Low;
         byteArray[index] := lo;
         byteArray[index + 1] := hi;

         length := 2;

         if (tmpLen == 2) {
            hi := tmp[1].High;
            lo := tmp[1].Low;
            byteArray[index + 2] := lo;
            byteArray[index + 3] := hi;

            length := 4;   
         };
      }
      
      char convertToChar(byte[] byteArray, int index, ref int length)
      {
         short tmp[2];

         int subLen := 4;
         int len := byteArray.Length;
         if (len - index < 4)
            subLen := 2;
            
         PrimitiveArrayOperations.copyToShortArray(byteArray, index, subLen, tmp, 0);
   
         int length := UTFOperations.convertUTF16_2_32(tmp, 0, 2, ref int value);
         length *= 2;

         char ch := value;

         ^ ch
      }
   }
}