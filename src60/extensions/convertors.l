// --- convertOp ---

public extension convertOp
{
   convertTo(target)
      = new Variant(self).saveTo(target);   
}

// --- stringConvertor ---

public extension stringConvertOp
{
   string toString()
      = self.toPrintable();
}

// --- intConvertOp ---

public extension intConvertOp
{
   int toInt()
     = intConvertor.convert(self, 10);

   int toInt(int radix)
     = intConvertor.convert(self, radix);
}

