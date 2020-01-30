import system;
import system'collections;
import system'routines;
//import system'calendar;
import system'text;
////import system'dynamic.
//
////// --- convertors ---
////
////singleton charArrayExConvertor
////{
////    T<CharArray> convert(literal s)
////    [
////        int len := 0.
////        UTF8Encoding charCount(s, &len).
////        
////        auto retVal := CharArray new(len).
////        s save(0, retVal, &len).
////        
////        ^ retVal.
////    ]
////    
////    T<CharArray> convert(wide s)
////    [
////        int len := 0.
////        UTF16Encoding charCount(s, &len).
////        
////        auto retVal := CharArray new(len).
////        s save(0, retVal, &len).
////        
////        ^ retVal.
////    ]
////} 

singleton stringExConvertor
{
////    literal convert(RealMatrix a)
////        = realmatrixExConvertor toLiteral(a).
//
//    string convert(IntMatrix a)
//        = intmatrixExConvertor.toString(a);

    string convert(Indexer it)
    {
        auto output := new TextBuilder();
        while (it.Available)
        {
            output.write(it.get().toString());
            output.write:",";
            
            it.appendIndex(1)
        };
        
        int length := output.Length;
        if (0 < length)
            { output.Length := -1 + length };
        
        ^ output
    }

//    string convert(Enumerator en)
//    {
//        auto output := new TextBuilder();
//        while(en.next())
//        {
//            output.write(en.get().toString());
//            output.write:",";
//        };
//
//        int length := output.Length;
//        if (0 < length)
//            { output.Length := -1 + length };
//        
//        ^ output
//    }
//
////    literal convert datetime:aValue = aValue literal.
////        
////    literal convert timestamp:aValue = aValue literal.
//
//    string convert(Enumerable o)
//        <= convert(o.enumerator());

    string convert(Indexable o)
        <= convert(o.indexer());

    string convert(o)
        = stringConvertor.convert(o);
}

////class wideExConvertor =
////{
////    wide convert(Indexer it)
////    [
////        auto anOutput := TextBuilder new.
////        while (it available)
////        [
////            anOutput write(it get; toWide).
////            anOutput write:",".
////            
////            it appendIndex:1
////        ].
////        
////        int aLength := anOutput length.
////        if (0 < aLength)
////            [ anOutput setLength(-1 + aLength) ].
////        
////        ^ anOutput
////    ]
////
////    wide convert(Enumerator en)
////    [
////        auto anOutput := TextBuilder new.
////
////        en forEachPair(anOutput,(:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ]).
////        
////        int aLength := anOutput length.
////
////        if (0 < aLength)
////            [ anOutput setLength(-1 + aLength) ].
////        
////        ^ anOutput
////    ]
//////        
//////    wide convert datetime:aValue = aValue literal; wide.
//////        
//////    wide convert timestamp:aValue = aValue literal; wide.
//////                
////////                intmatrix : m
////////                    = intmatrixExConvertor toLiteral intmatrix:m.
//////
////    wide convert(Enumerable o)
////        <= convert(o enumerator).
////
////    wide convert(Indexable o)
////        <= convert(o indexer).
////
////    wide convert:o
////        = wideConvertor convert:o.
////}.
////
////////class wideExConvertor =
////////{
////////    convert enumerable:aList
////////    [
////////        textbuffer anOutput := TextBuilder new.
////////
////////        aList run:anOutput eachPair: (:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ].
////////        
////////        int aLength := anOutput length.
////////
////////        if (0 < aLength)
////////            [ anOutput setLength int:(-1 + aLength) ].
////////        
////////        ^ anOutput wide
////////    ]
////////}.
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
////public singleton intarrayConvertorEx
////{
////    T<IntArray> convert(Enumerator<int> e)
////    [
////        List<int> list := List<int>().
////
////        while (e next)
////        [
////            list append(e get)
////        ].
////
////        ^ IntArray new(list indexer).
////    ]
////    
////    T<IntArray> convert(Enumerator e)
////    [
////        List<int> list := List<int>().
////
////        while (e next)
////        [
////            list append(e get)
////        ].
////
////        ^ IntArray new(list indexer).        
////    ]
////    
////    T<IntArray> convert(o)
////        <= convert(o enumerator).
////    
//////    convert enumerable:aList
//////    [
//////        array_list alist := ArrayList new.
//////
//////        aList run each: (:anItem)[ alist append:anItem ].
//////        
//////        ^ IntArray new int:(alist length); set every: (&int:i)[ alist getAt int:i; toInt ]
//////    ]
////}
//
//public singleton intmatrixExConvertor
//{
////    convert(Array a)
////    [
////        int n := a length.
////        int m := a[0] length.
////        
////        ^ IntMatrix new(n, m); populate(:i:j)<int,int,int>( a getAt(i); toArray; getAt(j); toInt ).
////    ]
//    
//    toString(IntMatrix matrix)
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
//
////public singleton realmatrixExConvertor
////{
////    convert(Array a)
////    [
////        int n := a length.
////        int m := a[0] length.
////        
////        ^ RealMatrix new(n, m); populate(:i:j)<int,int,real>( a getAt(i); toArray; getAt(j); toReal ).
////    ]
////    
////    toLiteral(RealMatrix matrix)
////    [
////        auto anOutput := TextBuilder new.
////        
////        int i := 0.
////        int j := 0.
////        int n := 0.
////        int m := 0.
////        int l := 0.
////        
////        matrix readLengthsTo(&n, &m).
////        
////        while (i < n)
////        [
////            j := 0.
////            while (j < m)
////            [
////                real r := 0.0r.
////                matrix read(i, j, &r).
////                
////                anOutput write:r; write:",".
////                
////                j := j + 1
////            ].
////            anOutput readLengthTo(&l).            
////            anOutput setLength(l - 1).
////            
////            anOutput write:forwards'newLine.
////            
////            i := i + 1
////        ].
////        
////        ^ anOutput literal
////    ]
////}

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
                arr[i] := it.get();
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

//    T<Array> convert(Indexer it)
//    [
//        T<ArrayList> aRetVal := ArrayList new.
//        while (it available)
//        [
//            aRetVal append(it get).
//            
//            it appendIndex(1)
//        ].
//
//        ^ T<Array>(aRetVal)
//    ]
//
////    convert(Enumerable v)
////        <= convert(v enumerator).
//        
//    T<Array> convert(Indexable v)
//        <= convert(v indexer).
        
//    convert void:val
//        = Array min.                        
}

// --- convertorOp ---

public extension convertorOp
{
    Array toArray()
        = arrayConvertorEx.convert(self);

////    T<IntArray> toIntArray
////        = intarrayConvertorEx convert:self.
//    
//    byte[] toByteArray()
//        = byteArrayExConvertor.convert:self;
//    
////    T<IntMatrix> toIntMatrix
////        = intmatrixExConvertor convert:self.
    
    string toString()
        = stringExConvertor.convert(self);
    
    string toString(int radix)
        = stringConvertor.convert(self, radix);
        
////    T<CharArray> toCharArray
////        = charArrayExConvertor convert:self.
////    
////    T<wide> toWide
////        = wideExConvertor convert:self.
////
////    toWide(int radix)
////        = wideConvertor convert(self, radix).
//            
//    byte toByte()
//        = byteConvertor.convert(self);
//
//    short toShort()
//        = shortConvertor.convert(self);
//
//    uint toUInt()
//        = uintConvertor.convert(self);
//        
//    int toInt()
//        = intConvertor.convert(self);
//        
//    int toInt(IntNumber radix)
//        = intConvertor.convert(self, radix);
//        
//    int toHexInt()
//        = intConvertor.convert(self, 16);
//
//    long toLong()
//        = longConvertor.convert:self;
//
//    long toHexLong()
//        = longConvertor.convert(self, 16);
//    
//    long toLong(int radix)
//        = longConvertor.convert(self, radix);
//    
//    char toChar()
//        = charConvertor.convert:self;
//
//    real toReal()
//        = realConvertor.convert:self;

    convertTo(target)
        = new Variant(self).saveTo(target);
}

//public convertor 
//    = new::{ generic(n) = __received(mixin extensions'convertorOp(n)); };
//
//public extension dateFormatterOp : Date
//{
//    string toShortTimeString()
//        = cast TimeStruct(self).toShortTime();
//}
//
//// --- enumerableConvertor ---
//
//singleton enumerableConvertor
//{
//    Enumerable convert(Enumerable o)
//        = o;
//    
//    Enumerable convert(o)
//        = new Enumerable::{ Enumerator enumerator() = o.enumerator(); };
//}
//
//// --- enumerableExOp ---
//
//public extension enumerableExOp
//{
//    Enumerable asEnumerable()
//        = enumerableConvertor.convert(self);
//}

// --- fileExOp ---

public extension fileExOp
{
    system'io'File asFileName()
        = system'io'File.assign(self);
}