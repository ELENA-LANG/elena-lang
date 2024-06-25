// --- PrimitiveOperations ---

internal singleton PrimitiveArrayOperations
{
   copy(byte[] sour, int index, int size, byte[] dest)
   {
      for (int i := 0; i < size; i := i + 1) {
         dest[i] := sour[index + i];
      }
   }

   copyTo(byte[] sour, int size, byte[] dest, int destIndex)
   {
      for (int i := 0; i < size; i := i + 1) {
         dest[destIndex + i] := sour[i];
      }
   }

   copy(short[] sour, int index, int size, short[] dest)
   {
      for (int i := 0; i < size; i := i + 1) {
         dest[i] := sour[index + i];
      }
   }

   int copyCharArray(char[] sour, int index, int size, byte[] dest)
   {
      int destLen := dest.Length;

      int j := 0;
      for (int i := 0; i < size; i := i + 1) {
         char ch := sour[index + i];
         int chVal := ch.Value;
         int chLen := ch.Length;
         
         UTFOperations.convertUTF32_2_8(chVal, dest, j, destLen);
         
         j += chLen;
      };

      ^ j
   }

   int copyCharArray(char[] sour, int index, int size, short[] dest)
   {
      int destLen := dest.Length;

      int j := 0;
      for (int i := 0; i < size; i := i + 1) {
         char ch := sour[index + i];
         int chVal := ch.Value;
         int chLen := ch.Length;
         
         UTFOperations.convertUTF32_2_16(chVal, dest, j, destLen);
         
         j += chLen;
      };

      ^ j
   }

   int copyToShortArray(byte[] byteArray, int index, int length, short[] output, outputIndex)
   {
      int outputLen := 0;
      int len := length;
      int i := index;
      int j := outputIndex;
      while (len > 1) {
         short tmp := byteArray[i + 1];
         tmp *= 100h;
         tmp += byteArray[i];

         output[j] := tmp;

         i += 2;
         len -= 2;

         outputLen += 1;
         j += 1;
      };

      ^ outputLen;
   }
}

internal singleton PrimitiveRealOperations
{
   trunc(real r, ref real retVal) : external(system'core_routines'__realTrunc);

   round(real r, ref real retVal) : external(system'core_routines'__realRound);

   abs(real r, ref real retVal) : external(system'core_routines'__realAbs);

   sqrt(real r, ref real retVal) : external(system'core_routines'__realSqrt);

   exp(real r, ref real retVal) : external(system'core_routines'__realExp);

   ln(real r, ref real retVal) : external(system'core_routines'__realLn);

   sin(real r, ref real retVal) : external(system'core_routines'__realSin);

   cos(real r, ref real retVal) : external(system'core_routines'__realCos);

   arctan(real r, ref real retVal) : external(system'core_routines'__realArctan);

   getPi(ref real retVal) : external(system'core_routines'__realPi);

   real getPi()
   {
      self.getPi(out real retVal);
      
      ^ retVal
   }

   real getE()
   {
      real retVal := self.exp(1.0r);
        
      ^ retVal
   }

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

   splitReal(real value, ref int integerPart, ref int decimalPart, ref int decimalPartLen, ref int exponent)
   {  
      real r := value;

      int e := PrimitiveRealOperations.normalizeFloat(ref r);

      real remainder := value.Integer;
      int intPart := intConvertor.convert(remainder);

      remainder := value - remainder;

      decimalPartLen := 9;
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

   private hash(real r, ref int retVal) : external(system'core_routines'__realHash);

   int getHashcode(real r)
   {
      self.hash(r, out int retVal);

      ^ retVal
   }

   saveToByteArray(real r, byte[] dest, int index)
   {
      pointer ptr := r;
      if (index == 0) {
         ptr.copyTo(dest, 8);
      }
      else {
         byte tmp[8];
         ptr.copyTo(tmp, 8);

         PrimitiveArrayOperations.copyTo(tmp, 8, dest, index);
      }
   }

   loadFromByteArray(byte[] dest, int index, ref real retVal)
   {
      PrimitiveLongOperations.loadFromByteArray(dest, index, out long temp);

      pointer ptr := temp;
      
      real v := ptr;

      retVal := v
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
      int i := index;
      while (i <= sLen) {
         bool isEqual := true;
         int j := 0;         
         while (j < length) {
            char ch1 := s[i + j];
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
         };
         i += 1;
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
            ^ index + i
         };

         int len1 := ch2.Length;
         i += len1;
      };

      ^ -1
   }

   int save(char[] dest, string s, int sourIndex, int sourLength, int destIndex)
   {
      int i := 0;
      int j := destIndex;
      while (i < sourLength) {
         char ch := s[sourIndex + i];

         dest[j] := ch;

         int len := ch.Length;
         i += len;
         j += 1;
      };
      
      j := j - destIndex;

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

   private hash(string s, out int retVal) : external(system'core_routines'__strHash);

   int getHashcode(string s)
   {
      self.hash(s, out int retVal);

      ^ retVal
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
      int i := index;
      while (i < sLen) {
         bool isEqual := true;
         int j := 0;         
         while (j < length) {
            char ch1 := s[i + j];
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
         };
         i += 1;
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
            ^ i + index
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

   int save(char[] dest, wide s, int sourIndex, int sourLength, int destIndex)
   {
      int i := 0;
      int j := destIndex;
      while (i < sourLength) {
         char ch := s[sourIndex + i];

         dest[j] := ch;

         int len := ch.Length;
         i += len;
         j += 1;
      };
      
      j := j - destIndex;

      ^ j
   }

   save(short[] dest, int destIndex, short[] sour, int index, int length)
   {
      int i := 0;
      while (i < length) {
         short b := sour[index + i];

         dest[destIndex + i] := b;

         i += 1;
      };
   }

   private hash(wide s, ref int retVal) : external(system'core_routines'__wstrHash);

   int getHashcode(wide s)
   {
      self.hash(s, out int retVal);

      ^ retVal
   }
}

// --- PrimitiveIntOperations ---

internal singleton PrimitiveIntOperations
{
   private hash(int n, out int retVal) : external(system'core_routines'__intHash);

   int getHashcode(int n)
   {
      self.hash(n, out int retVal);

      ^ retVal
   }

   int getHashcode(uint n)
   {
      int v := n;

      self.hash(v, out int retVal);

      ^ retVal
   }

   saveToByteArray(int n, byte[] dest, int index)
   {
      byte b := n & 0FFh;
      dest[index] := b;
      
      n := n $shr 8;
      b := n & 0FFh;
      dest[index + 1] := b;

      n := n $shr 8;
      b := n & 0FFh;
      dest[index + 2] := b;

      n := n $shr 8;
      b := n & 0FFh;
      dest[index + 3] := b;
   }

   loadFromByteArray(byte[] dest, int index, ref int retVal)
   {
      int val := dest[index + 3];
      val := val $shl 8;

      int temp := dest[index + 2];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index + 1];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index];
      val := val | temp;

      retVal := val
   }
}

// --- PrimitiveLongOperations ---

internal singleton PrimitiveLongOperations
{
   private hash(long n, out int retVal) : external(system'core_routines'__longHash);

   int getHashcode(long n)
   {
      self.hash(n, out int retVal);

      ^ retVal
   }

   saveToByteArray(long n, byte[] dest, int index)
   {
      int lo := n.Low;
      PrimitiveIntOperations.saveToByteArray(lo, dest, index);

      int hi := n.High;
      PrimitiveIntOperations.saveToByteArray(hi, dest, index + 4);
   }

   loadFromByteArray(byte[] dest, int index, out long retVal)
   {
      long val := dest[index + 7];
      val := val $shl 8;

      long temp := dest[index + 6];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index + 5];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index + 4];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index + 3];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index + 2];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index + 1];
      val := val | temp;
      val := val $shl 8;

      temp := dest[index];
      val := val | temp;

      retVal := val
   }
}

// --- UTFOperations ---

internal singleton UTFOperations
{
   int convertUTF8_2_32(byte[] sour, int index, int length, ref int retVal)
   {
      int ch := sour[index];
      int mask := ch & 080h;
      if (mask == 0)
      {
         retVal := ch;

         ^ 1
      };

      int ch2 := sour[index + 1];

      mask := ch & 0E0h;
      if (mask == 0C0h) {
         ch := ch & 1Fh;
         ch2 := ch2 & 3Fh;

         ch := ch $shl 6;
      
         ch += ch2;
         retVal := ch;            

         ^ 2
      };

      int ch3 := sour[index + 2];

      mask := ch & 0F0h;
      if (mask == 0E0h) {
         ch := ch & 0Fh;
         ch2 := ch2 & 03Fh;
         ch3 := ch3 & 03Fh;

         ch := ch $shl 12;
         ch2 := ch2 $shl 6;
         
         ch += ch2;
         ch += ch3;
         retVal := ch;            

         ^ 3
      };
     
      int ch4 := sour[index + 3];

      ch := ch & 7;
      ch2 := ch2 & 03Fh;
      ch3 := ch3 & 03Fh;
      ch4 := ch4 & 03Fh;

      ch := ch $shl 18;
      ch2 := ch2 $shl 12;
      ch3 := ch3 $shl 6;
      
      ch += ch2;
      ch += ch3;
      retVal := ch;            

      ^ 4
   }

   int convertUTF16_2_32(short[] sour, int index, int length, ref int retVal)
   {
      int ch := sour[index];
      
      if (ch < 0D800h) {
         retVal := ch;

         ^ 1;
      };

      int checkVal := ch - 0D800h;
      if (checkVal < 2048) 
         InvalidOperationException.raise();

      int ch2 := sour[index];

      int high_surrogate := ch & 0FFFFFC00h;
      int low_surrogate := ch2 & 0FFFFFC00h;
      
      if (high_surrogate == 0D800h && low_surrogate == 0DC00h) {
         high_surrogate := high_surrogate $shl 10;

         high_surrogate += low_surrogate;
         high_surrogate -= 35FDC00h;

         retVal := high_surrogate;
         ^ 2;
      };

      InvalidOperationException.raise();

      ^ 0;   
   }

   int convertUTF32_2_8(int value, byte[] dest, int index, int length)
   {
      if (value < 80h) {
         byte b := intConvertor.convert(value);

         dest[index] := b;

         ^ 1;
      };

      if (value < 800h) {
         int n1 := value $shr 6;
         n1 := n1 | 0C0h;

         int n2 := value & 03Fh;
         n2 := n2 | 080h;  

         byte b1 := intConvertor.convert(n1);
         byte b2 := intConvertor.convert(n2);

         dest[index] := b1;
         dest[index + 1] := b2;

         ^ 2;
      };
      
      if (value < 10000h) {
         int n1 := value $shr 12;
         n1 := n1 | 0E0h;

         int n2 := value $shr 6;
         n2 := n2 & 03Fh;
         n2 := n2 | 080h;  

         int n3 := value & 03Fh;
         n3 := n3 | 080h;  

         byte b1 := intConvertor.convert(n1);
         byte b2 := intConvertor.convert(n2);
         byte b3 := intConvertor.convert(n3);

         dest[index] := b1;
         dest[index + 1] := b2;
         dest[index + 2] := b3;

         ^ 3;
      };

      if (value < 200000h) {
         int n1 := value $shr 18;
         n1 := n1 | 0F0h;

         int n2 := value $shr 12;
         n2 := n2 & 03Fh;
         n2 := n2 | 080h;  

         int n3 := value $shr 6;
         n3 := n3 & 03Fh;
         n3 := n3 | 080h;  

         int n4 := value & 03Fh;
         n4 := n4 | 080h;  

         byte b1 := intConvertor.convert(n1);
         byte b2 := intConvertor.convert(n2);
         byte b3 := intConvertor.convert(n3);
         byte b4 := intConvertor.convert(n4);

         dest[index] := b1;
         dest[index + 1] := b2;
         dest[index + 2] := b3;
         dest[index + 3] := b4;

         ^ 4;
      };

      InvalidOperationException.raise();

      ^ 0;   
   }
   
   int convertUTF32_2_16(int value, short[] dest, int index, int length)  
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
      dest[index + 1] := tmp;

      ^ 2;        
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
         ^ 1
      };

      ^ 2
   }

   int calcSubStringLength(int index, int length, char[] array)
   {
      int subStrLength := 0;
      for(int i := 0; i < length; i += 1)
      {
         subStrLength += array[index + i].Length;
      };

      ^ subStrLength;
   }

   int calcSubWideStringLength(int index, int length, char[] array)
   {
      int subStrLength := 0;
      for(int i := 0; i < length; i += 1)
      {
         subStrLength += array[index + i].WideLength;
      };

      ^ subStrLength;
   }
}
