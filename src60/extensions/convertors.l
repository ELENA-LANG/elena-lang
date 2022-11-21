// --- stringConvertor ---

public extension stringConvertOp
{
   string toString()
      = self.toPrintable();
}

public extension intConvertOp
{
   int toInt()
     = intConvertor.convert(self, 10);

   int toInt(int radix)
     = intConvertor.convert(self, radix);
}