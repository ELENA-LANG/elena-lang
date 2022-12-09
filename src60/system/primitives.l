// --- PrimitiveOperations ---

internal singleton PrimitiveOperations
{
   copy(byte[] dest, int index, int size, byte[] sour)
   {
      for (int i := 0, i < size, i := i + 1) {
         dest[i] := sour[index + i];
      }
   }

   int getCharLength(int code)
   {
      if (code < 80h)
      {
         ^ 1
      };
      if (code < 800h)
      {
         ^ 2
      };
      if (code < 10000h)
      {
         ^ 3
      };

      ^ 4
   }

   int getCharWideLength(int code)
   {
      if (code < 0D800h)
      {
         ^ 2
      };

      ^ 4
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

   int copyUTF32(byte[] dest, int index, int value)
   {
      int length := 0;
      int n := value;

      if (value < 080h)
      {
         byte b := n;

         length := 1;
         dest[index] := b;

         ^ length
      };
      if (value < 0800h)
      {
         int n2 := value $shr 6;
         n2 := n2 + 0C0h;

         n := n & 03Fh;
         n := n + 080h;
         
         byte b := n;
         byte b2 := n2;

         length := 2;
         dest[index] := b2;         
         dest[index + 1] := b;

         ^ length
      };
      if (value < 10000h)
      {
         int n3 := value $shr 12;
         n3 := n3 + 0E0h;

         int n2 := value $shr 6;
         n2 := n2 & 03Fh;
         n2 := n2 + 080h;
         
         n := n & 03Fh;
         n := n + 80h;

         length := 3;
         byte b3 := n3;
         byte b2 := n2;
         byte b := n;
         dest[index] := b3;
         dest[index + 1] := b2;
         dest[index + 2] := b;

         ^ length
      };

      int n4 := value $shr 18;
      n4 := n4 & 0Fh;
      n4 := n4 + 0F0h;

      int n3 := value $shr 12;
      n3 := n3 + 0E0h;

      int n2 := value $shr 6;
      n2 := n2 & 03Fh;
      n2 := n2 + 080h;
         
      n := n & 03Fh;
      n := n + 80h;

      length := 4;
      byte b4 := n4;
      byte b3 := n3;
      byte b2 := n2;
      byte b := n;
      dest[index] := b4;
      dest[index + 1] := b3;
      dest[index + 2] := b2;
      dest[index + 3] := b;      
      
      ^ length;
   }
}

internal singleton PrimitiveRealOperations
{
   trunc(real r, ref real retVal) : external<system'core_routines'__realTrunc>;

   int normalizeFloat(ref real value)
   {
      real positiveExpThreshold := 1.0e7;
      real negativeExpThreshold := 1e-5;
      int exponent := 0;
      
      if (value >= positiveExpThreshold) {
         if (value >= 1e256) {
            value /= 1e256;
            exponent += 256;
         };
         if (value >= 1e128) {
            value /= 1e128;
            exponent += 128;
         };
         if (value >= 1e64) {
            value /= 1e64;
            exponent += 64;
         };
         if (value >= 1e32) {
            value /= 1e32;
            exponent += 32;
         };
         if (value >= 1e16) {
            value /= 1e16;
            exponent += 16;
         };
         if (value >= 1e8) {
            value /= 1e8;
            exponent += 8;
         };
         if (value >= 1e4) {
            value /= 1e4;
            exponent += 4;
         };
         if (value >= 1e2) {
            value /= 1e2;
            exponent += 2;
         };
         if (value >= 1e1) {
            value /= 1e1;
            exponent += 1;
         };
      };

      if (value > 0 && value <= negativeExpThreshold) {
         if (value < 1e-255) {
            value *= 1e256;
            exponent -= 256;
         };
         if (value < 1e-127) {
            value *= 1e128;
            exponent -= 128;
         };
         if (value < 1e-63) {
            value *= 1e64;
            exponent -= 64;
         };
         if (value < 1e-31) {
            value *= 1e32;
            exponent -= 32;
         };
         if (value < 1e-15) {
            value *= 1e16;
            exponent -= 16;
         };
         if (value < 1e-7) {
            value *= 1e8;
            exponent -= 8;
         };
         if (value < 1e-3) {
//            value *= 1e4;
  //          exponent -= 4;
         };
//         if (value < 1e-1) {
//            value *= 1e2;
//            exponent -= 2;
//         };
//         if (value < 1e0) {
//            value *= 1e1;
//            exponent -= 1;
//         };
      };

      ^ exponent;
   }

   splitReal(real value, ref int integerPart, ref int decimalPart, ref int exponent)
   {  
      real r := value;

      int e := PrimitiveRealOperations.normalizeFloat(ref r);

      real remainder := value.Integer;
      int intPart := intConvertor.convert(remainder);

      remainder := value - remainder;

      remainder := remainder * 1.0e9;
      
      int decPart := intConvertor.convert(remainder);

      // rounding
      real tmp := decPart;
      remainder := remainder - tmp;
      if (remainder > 0.5) {
         decPart := decPart + 1;
         if (decPart >= 1000000000) {
            decPart := 0;
            intPart := intPart + 1;
            if (e != 0 && intPart >= 10) {
               e := e + 1;
               intPart := 1;
            }
         }
      };

      integerPart := intPart;
      decimalPart := decPart;
      exponent := e;
   }
}