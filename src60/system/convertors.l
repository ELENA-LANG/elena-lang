// --- byteConvertor ---

public singleton byteConvertor
{
   convert(int n, ref byte retVal) : external<system'core_routines'__intToByte>;

   byte convert(int n)
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
}

// --- stringConvertor ---

public singleton stringConvertor
{
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

   string convert(short value, int radix)
   {
      int intValue := value;

      ^ self.convert(intValue, radix);
   }
}