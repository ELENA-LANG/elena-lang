namespace text
{
   import system'io;
   
   // --- Encoder ---
   public interface Encoder
   {
      abstract int convertChar(char ch, byte[] byteArray, int index);

      abstract char convertToChar(byte[] byteArray, int index, ref int length);

      int convertFrom(byte[] byteArray, int index, ref int len, char[] output, int outputIndex, int outputLen)
      {
         int i := 0;
         int j := outputIndex;
         while (i < len) {
            char ch := self.convertToChar(byteArray, index + i, ref int chLen);
            output[j] := ch;

            i += chLen;
            j += 1;
         };

         ^ j - outputIndex;
      }

      int convertTo(char[] chars, int index, ref int len, byte[] output, int outputIndex, int outputLen)
      {
         int j := outputIndex;
         for (int i := 0; i < len; i += 1) {
            char ch := chars[i + index];

            int chLen := self.convertChar(ch, output, j);

            j += chLen;
         };

         ^ j - outputIndex;
      }

      int toByteArray(string s, int index, ref int length, byte[] output, int outputIndex, int outputLength)
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

         ^ self.convertTo(tmp, 0, j, output, outputIndex, outputLength);
      }

      string toString(int index, int length, byte[] buffer)
      {
         char tmp[128];
         string retVal := EmptyString;
         int i := index;
         int len := length;
         int subLen := 128;
         while (len > 0) {
            if (len < subLen)
               subLen := len;

            int outputLen := self.convertFrom(buffer, i, ref subLen, tmp, 0, 128);
            
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

            int outputLen := self.convertFrom(buffer, i, ref subLen, tmp, 0, 128);
            
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
         char retVal := CharConvertor.convert(b);
           
         ^ retVal 
      }

      string toString(int index, int size, byte[] buffer)
      {
         ^ String.fromByteArray(index, size, buffer);
      }

      string toString(MemoryBuffer buffer)
         <= toString(0, buffer.Length, *buffer);

      int fromString(string sour, MemoryBuffer output)
      {
         byte tmp[1024];
         int len := sour.Length;
         int tmp_len := 1024;
         int index := 0;
         while (len > 0) {
            if (len < 1024) {
               tmp_len := len;
            }
            else tmp_len := 1024;
            
            sour.saveToByteArray(index, tmp, tmp_len);
            
            output.write(0, tmp_len, tmp);
            
            len -= tmp_len;
            index += tmp_len;
         };
         
         ^ index
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

      int getSubStringCharCount(string s, int index, int length)
      {
         int i := index;
         int len := 0;
         int end := index + length;
         while (i < end) {
            i += s[index].Length;
            
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
         char retVal := CharConvertor.convert(w);
           
         ^ retVal 
      }

      char toChar(short[] shortArray, int index, out int length)
      {
         int chLen := UTFOperations.convertUTF16_2_32(shortArray, index, length, out int value);
         char ch := value;

         length := chLen;

         ^ ch
      }

      wide toWideString(int index, int size, short[] buffer)
      {
         ^ WideString.fromShortArray(index, size, buffer);
      }

      toShortArray(wide s, int index, int length, short[] output, int outputIndex)
      {
         s.saveToShortArray(index, output, length, outputIndex);
      }

      toShortArray(char ch, short[] output, int outputIndex, ref int len)
      {
         int v := ch.Value;

         len := UTFOperations.convertUTF32_2_16(v, output, outputIndex, 2);
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
      int convertChar(char ch, byte[] byteArray, int index)
      {
         int value := ch;

         int byteLen := byteArray.Length;
         ^ UTFOperations.convertUTF32_2_8(value, byteArray, index, byteLen);
      }

      char convertToChar(byte[] byteArray, int index, out int sourLen)
      {
         sourLen := UTFOperations.convertUTF8_2_32(byteArray, index, sourLen, out int value);
         char ch := value;

         ^ ch
      }
   }   

   // --- UTF16Encoder ---
   public sealed singleton UTF16Encoder : Encoder
   {
      int convertChar(char ch, byte[] byteArray, int index)
      {
         int value := ch;
         short tmp[2];

         int tmpLen := UTFOperations.convertUTF32_2_16(value, tmp, 0, 2);

         byte hi := tmp[0].High;
         byte lo := tmp[0].Low;
         byteArray[index] := lo;
         byteArray[index + 1] := hi;

         int length := 2;

         if (tmpLen == 2) {
            hi := tmp[1].High;
            lo := tmp[1].Low;
            byteArray[index + 2] := lo;
            byteArray[index + 3] := hi;

            length := 4;   
         };

         ^ length;
      }
      
      char convertToChar(byte[] byteArray, int index, out int chLen)
      {
         short tmp[2];

         int subLen := 4;
         int len := byteArray.Length;
         if (len - index < 4)
            subLen := 2;
            
         PrimitiveArrayOperations.copyToShortArray(byteArray, index, subLen, tmp, 0);
   
         int length := UTFOperations.convertUTF16_2_32(tmp, 0, 2, out int value);
         length *= 2;

         chLen := length;

         char ch := value;

         ^ ch
      }
   }

#if (__project["_Win"]) // Windows

   const IntNumber MB_ERR_INVALID_CHARS     = 00000008h;

   // --- WinEncoder ---
   public class WinEncoder : Encoder
   {
      int codePage;

      constructor(int codePage)
      {
         this codePage := codePage;
      }

      int convertChar(char ch, byte[] byteArray, int index)
      {
         byte buffer[4];
         short tmp[4];
         int chValue := ch.Value;
   
         int tmpLen := UTFOperations.convertUTF32_2_16(chValue, tmp, 0, 4);

         int retVal := extern KERNEL32.WideCharToMultiByte(
            codePage, 0,
            tmp,
            tmpLen, buffer, 4, 0, 0);

         for (int i := 0; i < retVal; i += 1) {
            byteArray[index + i] := buffer[i]
         };

         ^ retVal;
      }

      char convertToChar(byte[] byteArray, int index, out int length)
      {
         short tmp[4];
         int byteArrayLen := 1;

         int converted := extern KERNEL32.MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, byteArray, byteArrayLen, tmp, 4);
         
         length := converted;

         UTFOperations.convertUTF16_2_32(tmp, 0, converted, out int value);

         char ch := value;
         ^ ch
      }
   }

   // --- OEMEncoder ---
   public static Encoder OEMEncoder = new WinEncoder(1);
   public static Encoder Win1250Encoder = new WinEncoder(1250);
   public static Encoder Win1251Encoder = new WinEncoder(1251);

#endif

}