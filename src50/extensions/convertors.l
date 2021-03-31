import system'collections;
import system'routines;
import system'calendar;
import system'text;

// --- convertors ---

//singleton charArrayExConvertor
//{
//    char[] convert(string s)
//    {
//        int len := 0;
//        UTF8Encoding.charCount(s, ref len);
//        
//        auto retVal := new char[](len);
//        s.save(0, retVal, ref len);
//        
//        ^ retVal
//    }
//    
//    char[] convert(wide s)
//    {
//        int len := 0;
//        UTF16Encoding.charCount(s, ref len);
//        
//        auto retVal := new char[](len);
//        s.save(0, retVal, ref len);
//        
//        ^ retVal
//    }
//} 

singleton stringExConvertor
{
//    string convert(RealMatrix a)
//        = realmatrixExConvertor.toString(a);
//
//    string convert(IntMatrix a)
//        = intmatrixExConvertor.toString(a);

    string convert(Indexer it)
    {
        auto output := new TextBuilder();
        while (it.Available)
        {
            output.write((*it).toString());
            output.write:",";
            
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
            output.write:",";
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

//singleton wideExConvertor
//{
//    wide convert(RealMatrix a)
//        = realmatrixExConvertor.toWide(a);
//
//    wide convert(IntMatrix a)
//        = intmatrixExConvertor.toWide(a);
//        
//    wide convert(Indexer it)
//    {
//        auto output := new TextBuilder();
//        while (it.Available)
//        {
//            output.write(it.get().toWide());
//            output.write:","w;
//            
//            it.appendIndex(1)
//        };
//        
//        int length := output.Length;
//        if (0 < length)
//            { output.Length := -1 + length };
//        
//        ^ output
//    }
//    
//    wide convert(Enumerator en)
//    {
//        auto output := new TextBuilder();
//        while(en.next())
//        {
//            output.write(en.get().toWide());
//            output.write:","w;
//        };
//
//        int length := output.Length;
//        if (0 < length)
//            { output.Length := -1 + length };
//        
//        ^ output
//    }
//        
//    wide convert(Date val) = val.toPrintable();
//        
//    wide convert(Timespan val) = val.toPrintable();
//    
//    wide convert(Enumerable o)
//        <= convert(o.enumerator());
//
//    wide convert(Indexable o)
//        <= convert(o.indexer());
//
//    wide convert(o)
//        = wideConvertor.convert(o);
//}
//
//public singleton byteArrayExConvertor
//{
//    byte[] convert(string s)
//        = UTF8Encoder.toByteArray(0, s.Length, s);
//    
//    byte[] convert(wide s)
//        = UTF16Encoder.toByteArray(0, s.Length, s);
//}
//
//public singleton intArrayExConvertor
//{
//    int[] convert(Enumerator<int> e)
//    {
//        List<int> list := new List<int>();
//
//        while (e.next())
//        {
//            list.append(e.get())
//        };
//
//        ^ list.Value
//    }
//    
//    int[] convert(Enumerator e)
//    {
//        List<int> list := new List<int>();
//
//        while (e.next())
//        {
//            list.append(e.get())
//        };
//
//        ^ list.Value
//    }
//    
//    int[] convert(o)
//        <= convert(o.enumerator());
//}
//
//public singleton intmatrixExConvertor
//{
//    convert(Array a)
//    {
//        int n := a.Length;
//        int m := a[0].length;
//        
//        ^ IntMatrix.allocate(n, m).populate:(int i, int j => a.at(i).at(j).toInt() )
//    }
//    
//    string toString(IntMatrix matrix)
//    {
//        auto output := new TextBuilder();
//        
//        int l := 0;
//        matrix.readLengthsTo(ref int n, ref int m);
//        
//        for (int i := 0, i < n, i += 1)
//        {
//            for (int j := 0, j < m, j += 1)
//            {
//                matrix.read(i, j, ref int r);
//                
//                output.write:r.write:","
//            };
//            l := output.Length;
//            l -= 1;
//            output.Length := l;
//            
//            output.write(forward newLine)
//        };
//        
//        ^ output.Value
//    }
//    
//    wide toWide(IntMatrix matrix)
//    {
//        auto output := new TextBuilder();
//        
//        int l := 0;
//        matrix.readLengthsTo(ref int n, ref int m);
//        
//        for (int i := 0, i < n, i += 1)
//        {
//            for (int j := 0, j < m, j += 1)
//            {
//                matrix.read(i, j, ref int r);
//                
//                output.write:r.write:","
//            };
//            l := output.Length;
//            l -= 1;
//            output.Length := l;
//            
//            output.write(forward newLine)
//        };
//        
//        wide ret := output.Value;
//        
//        ^ ret
//    }
//}
//
//public singleton realmatrixExConvertor
//{
//    convert(Array a)
//    {
//        int n := a.Length;
//        int m := a[0].length;
//        
//        ^ RealMatrix.allocate(n, m).populate:(int i, int j => a.at(i).at(j).toReal() )
//    }
//    
//    string toString(RealMatrix matrix)
//    {
//        auto output := new TextBuilder();
//        
//        int l := 0;
//        matrix.readLengthsTo(ref int n, ref int m);
//        
//        for (int i := 0, i < n, i += 1)
//        {
//            for (int j := 0, j < m, j += 1)
//            {
//                matrix.read(i, j, ref int r);
//                
//                output.write:r.write:","
//            };
//            l := output.Length;
//            l -= 1;
//            output.Length := l;
//            
//            output.write(forward newLine)
//        };
//        
//        ^ output.Value
//    }
//    
//    wide toWide(RealMatrix matrix)
//    {
//        auto output := new TextBuilder();
//        
//        int l := 0;
//        matrix.readLengthsTo(ref int n, ref int m);
//        
//        for (int i := 0, i < n, i += 1)
//        {
//            for (int j := 0, j < m, j += 1)
//            {
//                matrix.read(i, j, ref int r);
//                
//                output.write:r.write:","
//            };
//            l := output.Length;
//            l -= 1;
//            output.Length := l;
//            
//            output.write(forward newLine)
//        };
//        
//        ^ output.Value
//    }
//}

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
            for (int i := 0, i < len, i += 1)
            {
                arr[i] := *it;
                it.appendIndex(1)
            }
        };
        
        ^ arr
    }
    
    Array convert(Array arr)
        = arr;

    Array convert(string s)
        <= convert(cast Enumerator(s.enumerator()));

    Array convert(wide s)
        <= convert(cast Enumerator(s.enumerator()));

    Array convert(Enumerator en)
    {
        ArrayList retVal := new ArrayList();

        en.forEach:(x){ retVal.append:x };

        ^ retVal
    }

    Array convert(Indexer it)
    {
        ArrayList retVal := new ArrayList();
        while (it.Available)
        {
            retVal.append(*it);
            
            it.appendIndex(1)
        };

        ^ retVal
    }

    Array convert(Enumerable v)
        <= convert(v.enumerator());
        
    Array convert(Indexable v)
        <= convert(v.indexer());
}

// --- convertorOp ---

public extension convertorOp
{
    Array toArray()
        = arrayConvertorEx.convert(self);

//    int[] toIntArray()
//        = intArrayExConvertor.convert:self;
//    
//    byte[] toByteArray()
//        = byteArrayExConvertor.convert:self;
//    
//    IntMatrix toIntMatrix()
//        = intmatrixExConvertor.convert:self;
    
    string toString()
        = stringExConvertor.convert(self);
    
    string toString(int radix)
        = stringConvertor.convert(self, radix);
        
//    char[] toCharArray()
//        = charArrayExConvertor.convert:self;
//    
//    wide toWide()
//        = wideExConvertor.convert(self);
//
//    toWide(int radix)
//        = wideConvertor.convert(self, radix);
//            
//    byte toByte()
//        = byteConvertor.convert(self);
//
//    short toShort()
//        = shortConvertor.convert(self);
//
//    uint toUInt()
//        = uintConvertor.convert(self);
        
    int toInt()
        = intConvertor.convert(self);
        
//    int toInt(IntNumber radix)
//        = intConvertor.convert(self, radix);
//        
//    int toHexInt()
//        = intConvertor.convert(self, 16);

    long toLong()
        = longConvertor.convert:self;

//    long toHexLong()
//        = longConvertor.convert(self, 16);
//    
//    long toLong(int radix)
//        = longConvertor.convert(self, radix);
//    
//    char toChar()
//        = charConvertor.convert:self;

    real toReal()
        = realConvertor.convert:self;

    convertTo(target)
        = new Variant(self).saveTo(target);
}

//public convertor 
//    = new 
//    { 
//        generic(n)
//        {
//            MessageName mssg := __received.MessageName;
//            
//            ^ mssg(new ExtensionVariable(extensions'convertorOp, n))
//        }
//    };

public extension dateFormatterOp : Date
{
    string toShortTimeString()
        = cast TimeStruct(self).toShortTime();
}

//// --- enumerableConvertor ---
//
//singleton enumerableConvertor
//{
//    Enumerable convert(Enumerable o)
//        = o;
//    
//    Enumerable convert(o)
//        = new Enumerable { Enumerator enumerator() = o.enumerator(); };
//}
//
//// --- enumerableExOp ---
//
//public extension enumerableExOp
//{
//    Enumerable asEnumerable()
//        = enumerableConvertor.convert(self);
//}
//
//// --- fileExOp ---
//
//public extension fileExOp
//{
//    system'io'File asFileName()
//        = system'io'File.assign(self);
//}