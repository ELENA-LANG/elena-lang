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

      convertChar(char ch, byte[] byteArray, int index, ref int length)
      {
         byte buffer[4];
         short tmp[4];
         int chValue := ch.Value;
   
         int tmpLen := PrimitiveOperations.copyUTF32(tmp, 0, chValue);

         int retVal := extern KERNEL32.WideCharToMultiByte(
            codePage, 0,
            tmp,
            tmpLen, buffer, length, 0, 0);

         for (int i := 0, i < retVal, i += 1) {
            byteArray[index + i] := buffer[i]
         };

         length := retVal;
      }

      char convertToChar(byte[] byteArray, int index, ref int length)
      {
         short tmp[4];
         int byteArrayLen := 1;

         int converted := extern KERNEL32.MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, byteArray, byteArrayLen, tmp, 4);
         
         int ch := PrimitiveOperations.readUTF32(tmp, 0);

         ^ ch
      }
   }

   // --- OEMEncoder ---
   public static Encoder OEMEncoder = new WinEncoder(1);
}