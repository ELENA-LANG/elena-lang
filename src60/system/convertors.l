// --- byteConvertor ---

public singleton byteConvertor
{
   convert(int n, ref byte retVal) : external(system'core_routines'__intToByte);

   convert(uint n, ref byte retVal) : external(system'core_routines'__uintToByte);

   convert(short n, ref byte retVal) : external(system'core_routines'__shortToByte);

   convert(long n, ref byte retVal) : external(system'core_routines'__longToByte);

   byte convert(o)
      = cast byte(o);

   byte convert(BaseVariable v)
      <= convert(*v);

   byte convert(byte b)
      = b;

   byte convert(short s)
   {
      self.convert(s, ref byte b);

      ^ b;      
   }

   byte convert(char ch)
   {
      int n := ch;

      ^ self.convert(n);
   }

   byte convert(int n)
   {
      if (n > 255 || n < 0) {
         OutOfRangeException.raise();
      };

      self.convert(n, ref byte b);

      ^ b;
   }

   byte convert(uint n)
   {
      self.convert(n, ref byte b);

      ^ b;
   }

   byte convert(long n)
   {
      self.convert(n, ref byte b);

      ^ b;
   }

   byte convert(real r)
   {
      int n := intConvertor.convert(r);

      ^ self.convert(n);
   }

   byte convert(string s, int radix)
   {
      int n := intConvertor.convert(s, radix);

      ^ self.convert(n);
   }

   byte convert(wide s, int radix)
   {
      int n := intConvertor.convert(s, radix);

      ^ self.convert(n);
   }
}

// --- sbyteConvertor ---

public singleton sbyteConvertor
{
   convert(int n, ref sbyte retVal) : external(system'core_routines'__intToSByte);

   sbyte convert(int n)
   {
      if (n > 127 || n < -128) {
         OutOfRangeException.raise();
      };

      self.convert(n, ref sbyte b);

      ^ b;
   }
}

// --- shortConvertor ---

public singleton shortConvertor
{
   private convertInt(int n, ref short retVal) : external(system'core_routines'__intToShort);

   private convertUInt(uint n, ref short retVal) : external(system'core_routines'__uintToShort);

   private convertByte(byte b, ref short retVal) : external(system'core_routines'__byteToShort);

   short convert(o)
      = cast short(o);

   short convert(BaseVariable v)
      <= convert(*v);

   short convert(byte b)
   {
      self.convertByte(b, ref short s);

      ^ s;
   }   

   short convert(short s)
      = s;

   short convert(int n)
   {
      self.convertInt(n, ref short s);

      ^ s;
   }

   short convert(uint n)
   {
      self.convertUInt(n, ref short s);

      ^ s;
   }

   short convert(long l)
   {
      int n := intConvertor.convert(l);

      ^ self.convert(n)
   }

   short convert(real r)
   {
      int n := intConvertor.convert(r);

      ^ self.convert(n)
   }

   short convert(char ch)
   {
      int n := intConvertor.convert(ch);

      ^ self.convert(n)
   }

   short convert(string s, int radix)
   {
      int n := intConvertor.convert(s, radix);

      ^ self.convert(n)
   }

   short convert(wide s, int radix)
   {
      int n := intConvertor.convert(s, radix);

      ^ self.convert(n)
   }
}

// --- ushortConvertor ---

public singleton ushortConvertor
{
   ushort convert(ushort s)
      = s;

   ushort convert(short s)
   {
      int n := s;

      ^ n
   }
}

// --- intConvertor ---

public singleton intConvertor
{
   private convertChar(char ch, ref int retVal) : external(system'core_routines'__charToInt);
   private convertLong(long l, ref int retVal) : external(system'core_routines'__longToInt);
   private convertReal(real l, ref int retVal) : external(system'core_routines'__realToInt);
   private convertUInt(uint l, ref int retVal) : external(system'core_routines'__uintToInt);

   int convert(o)
      = cast int(o);

   int convert(BaseVariable v)
      <= convert(*v);

   int convert(int n)
      = n;

   int convert(byte b)
   {
      int n := b;

      ^ n;
   }

   int convert(short s)
   {
      int n := s;

      ^ n;
   }

   int convert(uint n)
   {
      self.convertUInt(n, ref int retVal);

      ^ retVal
   }

   int convert(char ch)
   {
      self.convertChar(ch, ref int retVal);

      ^ retVal
   }

   int convert(long l)
   {
      self.convertLong(l, ref int n);

      ^ n;
   }

   int convert(real r)
   {
      self.convertReal(r, ref int n);

      ^ n
   }   

   int convert(string s, int radix)
   {
      int len := s.Length;
      int val := 0;
      int start := 0;
      bool inverted := false;
      if (s[0]==$45) {
         start := 1;
         inverted := true;
      };

      for (int i := start; i < len; i := i + 1) {
         byte c := s[i];
         int ch := c;
         ch := ch - 48;
         if (ch >= radix) {
            ch := ch - 7;
         };
         val := val * radix;
         val := val + ch;
      };

      if (inverted) {
         val := -val;
      };

      ^ val
   }

   int convert(string s)
      <= convert(s, 10);

   int convert(wide s, int radix)
   {
      int len := s.Length;
      int val := 0;
      for (int i := 0; i < len; i := i + 1) {
         short c := s[i];
         int ch := c;
         ch := ch - 48;
         if (ch >= radix) {
            ch := ch - 7;
         };
         val := val * radix;
         val := val + ch;
      };

      ^ val
   }

   int convert(wide s)
      <= convert(s, 10);
}

// --- uintConvertor ---

public singleton uintConvertor
{
   private copyDirectly(int n, ref uint retVal) : external(system'core_routines'__intToUInt);

   uint convert(o)
      = cast uint(o);

   uint convert(uint n)
      = n;

   uint convert(int n)
   {
//      if (n < 0) {
//         InvalidArgumentException.raise();
//      };

      self.copyDirectly(n, ref uint retVal);

      ^ retVal
   }

   uint convert(byte b)
   {
      int n := b;

      ^ uintConvertor.convert(n);
   }

   uint convert(short s)
   {
      int n := s;

      ^ uintConvertor.convert(n);
   }
   uint convert(long l)
   {
      int n := intConvertor.convert(l);

      ^ self.convert(n);
   }

   uint convert(string s)
      <= convert(s, 10);

   uint convert(string s, int radix)
   {
      int len := s.Length;
      uint val := 0;
      uint r := radix;
      for (int i := 0; i < len; i := i + 1) {
         byte c := s[i];
         uint ch := c;
         ch := ch - 48;
         if (ch >= r) {            
            ch := ch - 7;
         };
         val := val * r;
         val := val + ch;
      };

      ^ val
   }

   uint convert(wide s, int radix)
   {
      int len := s.Length;
      uint val := 0;
      for (int i := 0; i < len; i := i + 1) {
         short c := s[i];
         int ch := c;
         ch := ch - 48;
         if (ch >= radix) {
            ch := ch - 49;
         };
         val := val * radix;
         val := val + ch;
      };

      ^ val
   }
}

// --- longConvertor ---

public singleton longConvertor
{
   long convert(o)
      = cast long(o);

   long convert(BaseVariable v)
      <= convert(*v);

   long convert(long l)
      = l;

   long convert(byte b)
   {
      long l := b;

      ^ l
   }

   long convert(short s)
   {
      long l := s;

      ^ l
   }

   long convert(int n)
   {
      long l := n;

      ^ l
   }

   long convert(uint n)
   {
      long l := n;

      ^ l
   }

   long convert(real r)
   {
      // !! temporal implementation
      int n := intConvertor.convert(r);

      long l := n;

      ^ l
   }

   long convert(char ch)
   {
      int n := intConvertor.convert(ch);

      long l := n;

      ^ l
   }

   long convert(string s, int radix)
   {
      int len := s.Length;
      long val := 0;
      int start := 0;
      bool inverted := false;
      if (s[0]==$45) {
         start := 1;
         inverted := true;
      };

      for (int i := start; i < len; i := i + 1) {
         byte c := s[i];
         int ch := c;
         ch := ch - 48;
         if (ch >= radix) {
            ch := ch - 49;
         };
         val := val * radix;
         val := val + ch;
      };

      if (inverted) {
         val := -val;
      };

      ^ val
   }

   long convert(string s)
      <= convert(s, 10);

   long convert(wide s, int radix)
   {
      int len := s.Length;
      long val := 0;
      int start := 0;
      bool inverted := false;
      if (s[0]==$45) {
         start := 1;
         inverted := true;
      };
      for (int i := start; i < len; i := i + 1) {
         short c := s[i];
         int ch := c;
         ch := ch - 48;
         if (ch >= radix) {
            ch := ch - 49;
         };
         val := val * radix;
         val := val + ch;
      };

      if (inverted) {
         val := -val;
      };

      ^ val
   }

   long convert(wide s)
      <= convert(s, 10);
}

// --- realConvertor ---

public singleton realConvertor
{
   real convert(o)
      = cast real(o);

   real convert(real r)
      = r;

   real convert(BaseVariable v)
      <= convert(*v);

   real convert(string s)
   {
      string intPartStr := nil;
      string decimalPartStr := nil;

      int index := s.indexOf(0, $46);
      if (index != -1) {
         intPartStr := s.Substring(0, index);
         decimalPartStr := s.Substring(index + 1);
      }
      else {
         intPartStr := s;
      };

      bool inverted := false;
      int intPart := intConvertor.convert(intPartStr, 10);
      if (intPart < 0) {
         intPart := -intPart;
         inverted := true;
      };

      real r := intPart;
      if (decimalPartStr != nil) {
         real frac := intConvertor.convert(decimalPartStr, 10);
         for (int i := 0; i < decimalPartStr.Length; i += 1) {
            frac := frac * 0.1;
         };

         r += frac;
      };

      if (inverted)
         r := r * -1.0;

      ^ r
   }

   real convert(wide w)
      <= convert(stringConvertor.convert(w));

   real convert(byte b)
   {
      real r := b;

      ^ r;
   }

   real convert(short s)
   {
      real r := s;

      ^ r
   }

   real convert(int n)
   {
      real r := n;

      ^ r
   }

   real convert(uint n)
   {
      real r := n;

      ^ r
   }

   real convert(long l)
   {
      int n := intConvertor.convert(l);

      real r := n;

      ^ r
   }
}

// --- charConvertor ---

public singleton charConvertor
{
   convert(int n, ref char retVal) : external(system'core_routines'__intToChar);

   char convert(int n)
   {
      self.convert(n, ref char ch);

      ^ ch;
   }

   char convert(string s)
      : info("Converts a literal string into a character")
   {
      char ch := s[0];
      if (ch.Length != s.Length)
         InvalidArgumentException.raise();

      ^ ch
   }        

   char convert(wide s)
      : info("Converts a wide string into a character")
   {
      char ch := s[0];
      if (ch.WideLength != s.Length)
         InvalidArgumentException.raise();

      ^ ch
   }      

   char convert(short w)
   {
      int n := w;

      ^ self.convert(n);
   }

   char convert(byte b)
   {
      int n := b;

      ^ self.convert(n);
   }

   char convert(o)
      = cast char(o);
}

// --- stringConvertor ---

public singleton stringConvertor
{
   string convert(byte b, int radix)
   {
      int n := b;

      ^ self.convert(n, radix);
   }

   string convert(int value, int radix)
   {
      int n := value;
      if (n < 0)
      {
         n := 0 - n;
      };   

      byte buffer[36];
      byte tmp;
      int len := 0;
      int index := 35;
      while (n >= radix) {
         int r := n.mod(radix);
         n := n / radix;

         tmp := 48 + r;
         if (r > 9) {
            tmp :=tmp + 7;
         };

         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
      if (n > 9) {
         tmp :=tmp + 7;
      };

      buffer[index] := tmp;
      len := len + 1;

      if (value < 0) {
        index := index - 1;
        tmp := 45;
        buffer[index] := tmp;
        len := len + 1;
      };

      ^ String.fromByteArray(index, len, buffer);
   }

   string convert(int value)
      <= convert(value, 10);

   string convert(long value, int radix)
   {
      long r := radix;
      long n := value;
      if (n < 0)
      {
         n := 0 - n;
      };   

      byte buffer[24];
      byte tmp;
      int len := 0;
      int index := 23;
      while (n >= r) {
         long rest := n.mod(r);
         n := n / r;

         tmp := rest;
         tmp := tmp + 48;
         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := n;
      tmp := tmp + 48;
      buffer[index] := tmp;
      len := len + 1;

      if (value < 0) {
        index := index - 1;
        tmp := 45;
        buffer[index] := tmp;
        len := len + 1;
      };

      ^ String.fromByteArray(index, len, buffer);
   }

   string convert(uint value, int radix)
   {
      uint n := value;
      uint uradix := radix;

      byte buffer[12];
      byte tmp;
      int len := 0;
      int index := 11;
      while (n >= uradix) {
         uint r := n.mod(uradix);
         n := n / uradix;

         tmp := 48 + r;
         if (r > 9) {
            tmp :=tmp + 7;
         };

         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
      if (n > 9) {
         tmp := tmp + 7;
      };

      buffer[index] := tmp;
      len := len + 1;

      ^ String.fromByteArray(index, len, buffer);
   }

   string convert(short value, int radix)
   {
      int intValue := value;

      ^ self.convert(intValue, radix);
   }

   private writeInteger(byte[] buffer, int value, ref int index, ref int length)
   {
      int i := index;
      int len := length;

      int decimalPart := value;
      byte tmp := 0;
      while (decimalPart >= 10) {
         int r := decimalPart.mod(10);
         decimalPart := decimalPart / 10;

         tmp := 48 + r;
         buffer[i] := tmp;
         i := i - 1;
         len := len + 1;
      };
      tmp := 48 + decimalPart;
      buffer[i] := tmp;
      i := i - 1;
      len := len + 1;

      index := i;
      length := len;
   }

   private writeByte(byte[] buffer, byte b, ref int index, ref int length)
   {
      int i := index;
      int len := length;

      buffer[i] := b;
      i := i - 1;
      len := len + 1;

      index := i;
      length := len;
   }
   
   string convert(real value)
   {
      real r := value;

      bool negative := false;
      if (r < 0.0) {
         negative := true;
         r := 0.0 - r;
      };

      PrimitiveRealOperations.splitReal(r, ref int integerPart, ref int decimalPart, ref int decimalPartLen, ref int exponent);

      byte buffer[40];
      int length := 0;
      int index := 39;
      byte tmp := 0;

      // write exponent part
      if (exponent < 0) {
         exponent := -exponent;

         self.writeInteger(buffer, exponent, ref index, ref length);

         // -
         tmp := 45;
         self.writeByte(buffer, tmp, ref index, ref length);

         // e
         tmp := 101;
         self.writeByte(buffer, tmp, ref index, ref length);
      };
      if (exponent > 0) {
         self.writeInteger(buffer, exponent, ref index, ref length);

         // e
         tmp := 101;
         self.writeByte(buffer, tmp, ref index, ref length);
      };

      // write decimal part
      self.writeInteger(buffer, decimalPart, ref index, ref length);

      // write leading zeros
      decimalPartLen -= length;

      for (int i := 0; i < decimalPartLen; i += 1) {
         buffer[index] := 30h;
         index -= 1;
         length += 1;
      };

      int dotIndex := index;

      // decimal sign    
      tmp := 46;  
      buffer[index] := tmp;
      index := index - 1;
      length := length + 1;

      // write integer part
      self.writeInteger(buffer, integerPart, ref index, ref length);

      // sign
      if (negative) {
         tmp := 45;
         self.writeByte(buffer, tmp, ref index, ref length); 
      };

      int last := 39;
      while (buffer[last] == 30h) {
         last -= 1;
         length -= 1;
      };
      if (last == dotIndex) {
         length += 1;
      };

      ^ String.fromByteArray(index + 1, length, buffer);      
   }

   string convert(char ch)
   {
      int value := ch.Value;

      byte buffer[4];
      int length := UTFOperations.convertUTF32_2_8(value, buffer, 0, 4);

      ^ String.fromByteArray(0, length, buffer);      
   }

   string convert(BaseVariable v)
      <= convert(*v);
    
   string convert(BaseVariable v, int radix)
      <= convert(*v, radix);

   string convert(wide w)
      = w;

   string convert(string s)
      = s;

   string convert(o)
      = o.toPrintable();
}

// --- wideConvertor ---

public singleton wideConvertor
{
   wide convert(char ch)
   {
      int value := ch.Value;

      short buffer[2];
      int length := UTFOperations.convertUTF32_2_16(value, buffer, 0, 2);

      ^ WideString.fromShortArray(0, length, buffer);      
   }  

   wide convert(byte b, int radix)
   {
      int n := b;

      ^ self.convert(n, radix);
   }

   wide convert(int value, int radix)
   {
      int n := value;
      if (n < 0)
      {
         n := 0 - n;
      };   

      short buffer[36];
      short tmp;
      int len := 0;
      int index := 35;
      while (n >= radix) {
         int r := n.mod(radix);
         n := n / radix;

         tmp := 48 + r;
         if (r > 9) {
            tmp :=tmp + 7;
         };

         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
      if (n > 9) {
         tmp :=tmp + 7;
      };

      buffer[index] := tmp;
      len := len + 1;

      if (value < 0) {
        index := index - 1;
        tmp := 45;
        buffer[index] := tmp;
        len := len + 1;
      };

      ^ WideString.fromShortArray(index, len, buffer);
   }

   wide convert(int value)
      <= convert(value, 10);

   wide convert(long value, int radix)
   {
      long r := radix;
      long n := value;
      if (n < 0)
      {
         n := 0 - n;
      };   

      short buffer[24];
      short tmp;
      int len := 0;
      int index := 23;
      while (n >= r) {
         long rest := n.mod(r);
         n := n / r;

         tmp := rest;
         tmp := tmp + 48;
         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := n;
      tmp := tmp + 48;
      buffer[index] := tmp;
      len := len + 1;

      if (value < 0) {
        index := index - 1;
        tmp := 45;
        buffer[index] := tmp;
        len := len + 1;
      };

      ^ WideString.fromShortArray(index, len, buffer);
   }

   wide convert(uint value, int radix)
   {
      uint n := value;
      uint uradix := radix;

      short buffer[12];
      short tmp;
      int len := 0;
      int index := 11;
      while (n >= uradix) {
         uint r := n.mod(uradix);
         n := n / uradix;

         tmp := 48 + r;
         if (r > 9) {
            tmp :=tmp + 7;
         };

         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
      if (n > 9) {
         tmp := tmp + 7;
      };

      buffer[index] := tmp;
      len := len + 1;

      ^ WideString.fromShortArray(index, len, buffer);
   }

   wide convert(short value, int radix)
   {
      int intValue := value;

      ^ self.convert(intValue, radix);
   }

   private writeInteger(short[] buffer, int value, ref int index, ref int length)
   {
      int i := index;
      int len := length;

      int decimalPart := value;
      short tmp := 0;
      while (decimalPart >= 10) {
         int r := decimalPart.mod(10);
         decimalPart := decimalPart / 10;

         tmp := 48 + r;
         buffer[i] := tmp;
         i := i - 1;
         len := len + 1;
      };
      tmp := 48 + decimalPart;
      buffer[i] := tmp;
      i := i - 1;
      len := len + 1;

      index := i;
      length := len;
   }

   private writeByte(short[] buffer, short b, ref int index, ref int length)
   {
      int i := index;
      int len := length;

      buffer[i] := b;
      i := i - 1;
      len := len + 1;

      index := i;
      length := len;
   }

   private int skipTrailingZerros(int value)
   {
      int n := value;
      while (n != 0) {
         int r := n.mod(10);
         if (r != 0) {
            ^ n;
         };
         n := n / 10;            
      };

      ^ n;
   }
   
   wide convert(real value)
   {
      real r := value;

      bool negative := false;
      if (r < 0.0) {
         negative := true;
         r := 0.0 - r;
      };

      PrimitiveRealOperations.splitReal(r, ref int integerPart, ref int decimalPart, ref int decimalPartLen, ref int exponent);

      short buffer[40];
      int length := 0;
      int index := 39;
      short tmp := 0;

      // write exponent part
      if (exponent < 0) {
         exponent := -exponent;

         self.writeInteger(buffer, exponent, ref index, ref length);

         // -
         tmp := 45;
         self.writeByte(buffer, tmp, ref index, ref length);

         // e
         tmp := 101;
         self.writeByte(buffer, tmp, ref index, ref length);
      };
      if (exponent > 0) {
         self.writeInteger(buffer, exponent, ref index, ref length);

         // e
         tmp := 101;
         self.writeByte(buffer, tmp, ref index, ref length);
      };

      // write decimal part
      self.writeInteger(buffer, decimalPart, ref index, ref length);

      // write leading zeros
      decimalPartLen -= length;

      for (int i := 0; i < decimalPartLen; i += 1) {
         buffer[index] := 30h;
         index -= 1;
         length += 1;
      };

      int dotIndex := index;

      // decimal sign    
      tmp := 46;  
      buffer[index] := tmp;
      index := index - 1;
      length := length + 1;

      // write integer part
      self.writeInteger(buffer, integerPart, ref index, ref length);

      // sign
      if (negative) {
         tmp := 45;
         self.writeByte(buffer, tmp, ref index, ref length); 
      };

      int last := 39;
      while (buffer[last] == 30h) {
         last -= 1;
         length -= 1;
      };
      if (last == dotIndex) {
         length += 1;
      };

      ^ WideString.fromShortArray(index + 1, length, buffer);      
   }

   wide convert(BaseVariable v)
      <= convert(*v);
    
   wide convert(BaseVariable v, int radix)
      <= convert(*v, radix);

   wide convert(o)
      = o.toPrintable();
}

// --- byteArrayConvertor ---

public singleton byteArrayConvertor
{
   int convert(wide sour, int sourLen, byte[] dest, int destIndex, int destLen)
   {
      int i := 0;
      int j := 0;
      while (i < sourLen) {
         char ch := sour[i];
         int chVal := ch.Value;
         
         int length := UTFOperations.convertUTF32_2_8(chVal, dest, destIndex + j, destLen);
         
         j += length;
         i += ch.WideLength;
      };

      ^ j
   }   
}

// --- shortArrayConvertor ---

public singleton shortArrayConvertor
{
   int convert(string sour, int sourLen, short[] dest, int destIndex, int destLen)
   {
      int i := 0;
      int j := 0;
      while (i < sourLen) {
         char ch := sour[i];
         int chVal := ch.Value;
         
         int length := UTFOperations.convertUTF32_2_16(chVal, dest, destIndex + j, destLen);
         
         j += length;
         i += ch.Length;
      };

      ^ j
   }   
}

// --- mssgConvertor ---

public singleton mssgConvertor
{
   convert(int n, ref Message retVal) : external(system'core_routines'__intToMssg);

   Message convert(int n)
      : info("Converting int value to the string<br/>NOTE : unsafe operation")
   {
      self.convert(n, ref Message m);

      ^ m;
   }
}
