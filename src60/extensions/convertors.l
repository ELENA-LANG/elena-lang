import system'text;

singleton stringExConvertor
{
//    string convert(RealMatrix a)
//        = realmatrixExConvertor.toString(a);
//
//    string convert(IntMatrix a)
//        = intmatrixExConvertor.toString(a);

//   string convert(Indexer it)
//   {
//      auto output := new TextBuilder();
//      while (it.Available)
//      {
//         output.write((*it).toString());
//         output.write:",";
//            
//         it.appendIndex(1)
//      };
//        
//      int length := output.Length;
//      if (0 < length)
//         { output.Length := -1 + length };
//        
//      ^ output
//   }

//   string convert(Enumerator en)
//   {
//      auto output := new TextBuilder();
//      while(en.next())
//      {
//         output.write((*en).toString());
//         output.write:",";
//      };
//
//      int length := output.Length;
//      if (0 < length)
//         { output.Length := -1 + length };
//        
//      ^ output
//   }

//    string convert(Date val) = val.toPrintable();
//        
//    string convert(Timespan val) = val.toPrintable();

//    string convert(Enumerable o)
//        <= convert(o.enumerator());
//
//    string convert(Indexable o)
//        <= convert(o.indexer());

    string convert(o)
        = stringConvertor.convert(o);
}

// --- enumerableConvertor ---

singleton enumerableConvertor
{
   Enumerable convert(Enumerable o)
      = o;
    
   Enumerable convert(o)
      = new Enumerable { Enumerator enumerator() = o.enumerator(); };
}

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
      = stringExConvertor.convert(self);
}

// --- intConvertOp ---

public extension intConvertOp
{
   int toInt()
     = intConvertor.convert(self, 10);

   int toInt(int radix)
     = intConvertor.convert(self, radix);
}

// --- realConvertOp ---
public extension realConvertOp
{
   real toReal()
      = realConvertor.convert(self);
}

// --- enumerableExOp ---
public extension enumerableExOp
{
    Enumerable asEnumerable()
        = enumerableConvertor.convert(self);
}

