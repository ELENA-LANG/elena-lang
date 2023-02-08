// --- PrimitiveOperations ---

internal singleton PrimitiveOperations
{
   copy(byte[] dest, int index, int size, byte[] sour)
   {
      for (int i := 0, i < size, i := i + 1) {
         dest[i] := sour[index + i];
      }
   }

   copy(byte[] dest, int index, int size, char[] sour)
   {
      int j := 0;
      for (int i := 0, i < size, i := i + 1) {
         char ch := sour[index + 1];   
         int chVal := ch.Value;
         int chLen := ch.Length;
         
         PrimitiveOperations.copyUTF32(dest, j, chVal);
         
         j += chLen;
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

   int readUTF32(byte[] sour, int index)
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

   int readUTF32(short[] sour, int index)
   {
      int ch := sour[index];
      if (ch > 0DBFFh)
      {
         InvalidOperationException.raise()
      };

      if (ch < 0D800h)
      {
         ^ ch
      };

      int ch2 := ch $shl 10;
      int ch3 := sour[index + 1];
      if (ch3 > 0DBFFh)
      {
         InvalidOperationException.raise()
      };
      ch += ch3;

      ch -= 35FDC00h;

      ^ ch
   }

   int calcSubStringLength(int index, int length, char[] array)
   {
      int subStrLength := 0;
      for(int i := 0, i < length, i += 1)
      {
         subStrLength += array[i].Length;
      };

      ^ subStrLength;
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

   int copyUTF32(short[] dest, int index, int value)
   {
      int length := 0;
      int n := value;
      if (n < 010000h) {
         short s := shortConvertor.convert(n); 
         
         dest[index] := s;

         ^ 1
      };
      
      int n2 := n $shr 10;
      n2 += 0D7C0h;
      short tmp := shortConvertor.convert(n2); 
      dest[index] := tmp;
      
      n := n & 03FFh;
      n += 0DC00h;
      tmp := shortConvertor.convert(n); 
      dest[index] := tmp;

      ^ 2;   
   }
}

internal singleton PrimitiveRealOperations
{
   trunc(real r, ref real retVal) : external(system'core_routines'__realTrunc);

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

internal singleton PrimitiveStringOperations
{
   copy(byte[] dest, int index, int size, string sour) : external(system'core_routines'__copySubStr);

   int indexOf(string s, string subs, int index, int length)
   {
      int sLen := s.Length;
      if (sLen < length)
      {
         ^ -1
      };

      sLen -= length;
      int i := 0;
      while (i < sLen) {
         bool isEqual := true;
         int j := 0;         
         while (j < length) {
            char ch1 := subs[i + j];
            char ch2 := subs[j];
            int len1 := ch2.Length;
            int len2 := ch2.Length;
            if (len1 != len2 || ch1 != ch2) {
               isEqual := false;
               $break
            };
            j += len2;
         };
         if (isEqual) {
            ^ i
         }
      };

      ^ -1
   }

   int indexOf(string s, char ch, int index, int length)
   {
      int i := 0;
      while (i < length) {
         char ch2 := s[index + i];
         if (ch == ch2)
         {
            ^ i
         };

         int len1 := ch2.Length;
         i += len1;
      };

      ^ -1
   }

   int save(char[] dest, string s, int index, int length)
   {
      int i := 0;
      int j := 0;
      while (i < length) {
         char ch := s[index + i];

         dest[j] := ch;

         int len := ch.Length;
         i += len;
      };
      
      ^ j
   }

   save(byte[] dest, byte[] sour, int index, int length)
   {
      int i := 0;
      while (i < length) {
         byte b := sour[index + i];

         dest[i] := b;

         i += 1;
      };
   }
}

internal singleton PrimitiveWideStringOperations
{
   copy(short[] dest, int index, int size, wide sour) : external(system'core_routines'__copySubWStr);

   int indexOf(wide s, wide subs, int index, int length)
   {
      int sLen := s.Length;
      if (sLen < length)
      {
         ^ -1
      };

      sLen -= length;
      int i := 0;
      while (i < sLen) {
         bool isEqual := true;
         int j := 0;         
         while (j < length) {
            char ch1 := subs[i + j];
            char ch2 := subs[j];
            int len1 := ch2.Length;
            int len2 := ch2.Length;
            if (len1 != len2 || ch1 != ch2) {
               isEqual := false;
               $break
            };
            j += len2;
         };
         if (isEqual) {
            ^ i
         }
      };

      ^ -1
   }

   int indexOf(wide s, char ch, int index, int length)
   {
      int i := 0;
      while (i < length) {
         char ch2 := s[index + i];
         if (ch == ch2)
         {
            ^ i
         };

         int len1 := ch2.Length;
         i += len1;
      };

      ^ -1
   }

   int save(char[] dest, wide s, int index, int length)
   {
      int i := 0;
      int j := 0;
      while (i < length) {
         char ch := s[index + i];

         dest[j] := ch;

         int len := ch.Length;
         i += len;
      };
      
      ^ j
   }

   save(short[] dest, short[] sour, int index, int length)
   {
      int i := 0;
      while (i < length) {
         short b := sour[index + i];

         dest[i] := b;

         i += 1;
      };
   }
}
