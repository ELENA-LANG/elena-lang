
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
}