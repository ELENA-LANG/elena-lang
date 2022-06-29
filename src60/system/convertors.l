
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
