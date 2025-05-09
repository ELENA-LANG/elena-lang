namespace text
{
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
}