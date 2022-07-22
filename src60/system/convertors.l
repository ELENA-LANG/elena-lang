
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

// --- stringConvertor ---

public singleton stringConvertor
{
   string convert(int value, int radix)
   {
      int n := value;

      byte buffer[12];
      byte tmp;
      int len := 0;
      while (n < radix) {
         int r := n.mod(radix);
         n := n / radix;

         tmp := 30 + r;
         buffer[len] := tmp;
         len := len + 1;
      };
      tmp := 30 + n;
      buffer[len] := tmp;
      len := len + 1;

      ^ String.fromByteArray(0, len, buffer);
   }
}