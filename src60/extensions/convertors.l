import system'text;
import system'collections;
import system'routines;
import system'calendar;

singleton stringExConvertor
{
//    string convert(RealMatrix a)
//        = realmatrixExConvertor.toString(a);

    string convert(IntMatrix a)
        = intmatrixExConvertor.toString(a);

   string convert(Indexer it)
   {
      auto output := new TextBuilder();
      while (it.Available)
      {
         output.write((*it).toString());
         output.write(",");
            
         it.appendIndex(1)
      };
        
      int length := output.Length;
      if (0 < length)
         { output.Length := -1 + length };
        
      ^ output
   }

   string convert(Enumerator en)
   {
      auto output := new TextBuilder();
      while(en.next())
      {
         output.write((*en).toString());
         output.write(",");
      };

      int length := output.Length;
      if (0 < length)
         { output.Length := -1 + length };
        
      ^ output
   }

//    string convert(Date val) = val.toPrintable();
//        
//    string convert(Timespan val) = val.toPrintable();

   string convert(Enumerable o)
      <= convert(o.enumerator());

   string convert(Indexable o)
      <= convert(o.indexer());

   string convert(o)
      = stringConvertor.convert(o);
}

// --- arrayConvertorEx ---

public singleton arrayConvertorEx
{
   Array convert(o)
   {
      var arr := cast Array(o) ?? nil;
      if (arr == nil)
      {
         var it := o.indexer();
         int len := it.Length;
         arr := Array.allocate(len);
         for (int i := 0; i < len; i += 1)
         {
            arr[i] := *it;
            it.appendIndex(1)
         }
      };
        
      ^ arr
   }
    
   Array convert(Enumerator en)
   {
      ArrayList retVal := new ArrayList();

      en.forEach::(x){ retVal.append(x) };

      ^ retVal
   }

   Array convert(Array arr)
      = arr;
}

// --- intmatrixExConvertor ---

public singleton intmatrixExConvertor
{
   IntMatrix convert(Array a)
   {
      int n := a.Length;
      int m := a[0].length;

      ^ IntMatrix.allocate(n, m).populate::(int i, int j => a.at(i).at(j).toInt() )
   }   
    
   string toString(IntMatrix matrix)
   {
      auto output := new TextBuilder();
        
      int l := 0;
      matrix.readLengthsTo(ref int n, ref int m);
        
      for (int i := 0; i < n; i += 1)
      {
         for (int j := 0; j < m; j += 1)
         {
            int r := matrix.at(i, j);
                
            output.write(r).write(",")
         };
         l := output.Length;
         l -= 1;
         output.Length := l;
            
         output.write(newLineConstant)
      };
        
      ^ output.Value
   }
    
   wide toWide(IntMatrix matrix)
   {
      auto output := new TextBuilder();
        
      int l := 0;
      matrix.readLengthsTo(ref int n, ref int m);
        
      for (int i := 0; i < n; i += 1)
      {
         for (int j := 0; j < m; j += 1)
         {
            int r := matrix.at(i, j);
                
            output.write(r).write(",")
         };
         l := output.Length;
         l -= 1;
         output.Length := l;
            
         output.write(newLineConstant)
      };
        
      wide ret := output.Value;
        
      ^ ret
   }
}

// --- enumerableConvertor ---

singleton enumerableConvertor
{
   Enumerable convert(Enumerable o)
      = o;
    
   Enumerable convert(o)
      = new Enumerable { Enumerator enumerator() = o.enumerator(); };
}

public singleton byteArrayExConvertor
{
   byte[] convert(string s)
   {
      int    i := 0;
      byte[] retVal := nil;
      int    length := s.Length; 
      int    remaining := length;
            
      while (i < length)
      {
         byte tmp[64];
         int  tmp_len := 64;
         int  len := remaining;
         if (len > tmp_len)
            { len := tmp_len };
              
         UTF8Encoder.toByteArray(s, i, ref len, tmp, 0, ref tmp_len);
                
         if (nil == retVal)
         { 
            retVal := class Array<byte>.copy(tmp, 0, tmp_len)
         }
         else
         { 
            retVal := retVal.add(tmp, tmp_len) 
         };
                
         i := i + len;
         remaining := remaining - len
      };
            
      ^ retVal
   }  
}

// --- stringConvertor ---

public extension stringConvertOp
{
   string toString()
      = stringExConvertor.convert(self);

   string toString(int radix)
      = stringConvertor.convert(self, radix);
}

// --- intConvertOp ---

public extension intConvertOp
{
   int toInt()
     = intConvertor.convert(self);

   int toInt(int radix)
     = intConvertor.convert(self, radix);
}

// --- byteConvertOp ---

public extension byteConvertOp
{
   byte toByte()
     = byteConvertor.convert(self);
}

// --- shortConvertOp ---

public extension shortConvertOp
{
   short toShort()
     = shortConvertor.convert(self);
}

// --- longConvertOp ---
public extension longConvertOp
{
   long toLong()
      = longConvertor.convert(self);
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

// --- arrayConvertOp ---
public extension arrayConvertOp
{
   Array toArray()
      = arrayConvertorEx.convert(self);

}

// --- intArrayConvertOp ---
public extension intArrayConvertOp
{
   IntMatrix toIntMatrix()
      = intmatrixExConvertor.convert(self);
}

// --- convertorOp ---

public extension convertorOp
{
   convertTo(target)
      = new Variant(self).saveTo(target);

   char toChar()
      = charConvertor.convert(self);

   byte[] toByteArray()
      = byteArrayExConvertor.convert(self);
}

// --- convertor ---

public singleton intConvertExt
{ 
   generic(n)
   {
      /*MessageName*/var mssg := __received.MessageName;

      ^ mssg(new ExtensionVariable(n, extensions'intConvertOp))
   }
}

public extension dateFormatterOp : Date
{
   string toShortTimeString()
      = cast TimeStruct(self).toShortTime();
}
