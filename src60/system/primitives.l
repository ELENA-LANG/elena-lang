// --- PrimitiveOperations ---

internal singleton PrimitiveOperations
{
   copy(byte[] dest, int index, int size, byte[] sour)
   {
      for (int i := 0, i < size, i := i + 1) {
         dest[i] := sour[index + i];
      }
   }

   char readChar(byte[] sour, int index)
   {
      int ch := sour[index];
      if (ch < 80h)
      {
         ^ ch
      };
      if (ch < 0C0h)
      {
         //InvalidOperaionException.raise()
      };
      if (ch < 0E0h)
      {
         int ch2 := sour[index + 1];
         int mask := ch2 & 0C0h;
         if (mask != 80h) {
            //InvalidOperaionException.raise()
         };
         ch := ch $shl 6;
         ch2 := ch2 + ch;
         ch2 := ch2 - 3080h;
       
         ^ ch2
      };
      if (ch < 0F0h) {
         int ch2 := sour[index + 1];
         int ch3 := sour[index + 2];

         int mask := ch2 & 0C0h;
         if (mask != 80h) {
            //InvalidOperaionException.raise()
         };

         ch := ch $shl 12;
         ch2 := ch2 $shl 6;
         ch := ch + ch2;

         int mask2 := ch3 & 0C0h;
         if (mask2 != 80h) {
            //InvalidOperaionException.raise()
         };
         ch3 := ch3 + ch;
         ch3 := ch3 - 0E2080h;
         ^ ch3
      };
      if (ch < 0F8h) {
         int ch2 := sour[index + 1];
         int ch3 := sour[index + 2];
         int ch4 := sour[index + 3];

         int mask := ch2 & 0C0h;
         if (mask != 80h) {
            //InvalidOperaionException.raise()
         };

         ch := ch $shl 18;
         ch2 := ch2 $shl 12;
         ch := ch + ch2;

         int mask2 := ch3 & 0C0h;
         if (mask2 != 80h) {
            //InvalidOperaionException.raise()
         };

         ch3 := ch3 $shl 6;
         ch := ch + ch3;

         int mask3 := ch4 & 0C0h;
         if (mask3 != 80h) {
            //InvalidOperaionException.raise()
         };
         
         ch4 := ch4 + ch3;
         ch4 := ch4 - 3C82080h;
         ^ ch4
      }

      //InvalidOperaionException.raise()    
   }
}
