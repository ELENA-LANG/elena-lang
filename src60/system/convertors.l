// --- byteConvertor ---

public singleton byteConvertor
{
   convert(int n, ref byte retVal) : external<system'core_routines'__intToByte>;

   convert(long n, ref byte retVal) : external<system'core_routines'__longToByte>;

   byte convert(int n)
   {
      byte b;
      self.convert(n, ref b);

      ^ b;
   }

   byte convert(long n)
   {
      byte b;
      self.convert(n, ref b);

      ^ b;
   }
}

// --- shortConvertor ---

public singleton shortConvertor
{
   convert(int n, ref short retVal) : external<system'core_routines'__intToShort>;

   short convert(int n)
   {
      short b;
      self.convert(n, ref b);

      ^ b;
   }
}

// --- intConvertor ---

public singleton intConvertor
{
   private convertChar(char ch, ref int retVal) : external<system'core_routines'__charToInt>;
   private convertLong(long l, ref int retVal) : external<system'core_routines'__longToInt>;
   private convertReal(real l, ref int retVal) : external<system'core_routines'__realToInt>;

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

   int convert(char ch)
   {
      self.convertChar(ch, ref int retVal);

      ^ retVal
   }

   int convert(long l)
   {
      int n;
      self.convertLong(l, ref n);

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
      for (int i := 0, i < len, i := i + 1) {
         byte c := s[i];
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

// --- uintConvertor ---

public singleton uintConvertor
{
   uint convert(byte b)
   {
      uint n := b;

      ^ n;
   }

   uint convert(string s, int radix)
   {
      int len := s.Length;
      uint val := 0;
      for (int i := 0, i < len, i := i + 1) {
         byte c := s[i];
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
   long convert(byte b)
   {
      long l := b;

      ^ l
   }
}

// --- realConvertor ---

public singleton realConvertor
{
   real convert(byte b)
   {
      real r := b
   }

   real convert(int n)
   {
      real r := n;

      ^ r
   }
}

// --- charConvertor ---

public singleton charConvertor
{
   convert(int n, ref char retVal) : external<system'core_routines'__intToChar>;

   char convert(int n)
   {
      char ch;
      self.convert(n, ref ch);

      ^ ch;
   }

   char convert(short w)
   {
      int n := w;

      ^ self.convert(n);
   }
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

      byte buffer[12];
      byte tmp;
      int len := 0;
      int index := 11;
      while (n >= radix) {
         int r := n.mod(radix);
         n := n / radix;

         tmp := 48 + r;
         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
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

      byte buffer[12];
      byte tmp;
      int len := 0;
      int index := 11;
      while (n >= radix) {
         int r := n.mod(radix);
         n := n / radix;

         tmp := 48 + r;
         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + n;
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
         buffer[index] := tmp;
         index := index - 1;
         len := len + 1;
      };
      tmp := 48 + decimalPart;
      buffer[index] := tmp;
      index := index - 1;
      len := len + 1;

      index := i;
      length := len;
   }

   private writeByte(byte[] buffer, byte b, ref int index, ref int length)
   {
      int i := index;
      int len := length;

      buffer[index] := b;
      index := index - 1;
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

      PrimitiveRealOperations.splitReal(r, ref int integerPart, ref int decimalPart, ref int exponent);

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

      ^ String.fromByteArray(index, length, buffer);      
   }

   string convert(char ch)
   {
      int value := ch.Value;

      byte buffer[4];
      int length := PrimitiveOperations.copyUTF32(buffer,0,value);

      ^ String.fromByteArray(0, length, buffer);      
   }
}