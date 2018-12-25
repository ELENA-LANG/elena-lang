//import system'collections.
//import system'routines.
//import system'calendar.
import system'text;
//import system'dynamic.

//// --- convertors ---
//
//singleton charArrayExConvertor
//{
//    T<CharArray> convert(literal s)
//    [
//        int len := 0.
//        UTF8Encoding charCount(s, &len).
//        
//        auto retVal := CharArray new(len).
//        s save(0, retVal, &len).
//        
//        ^ retVal.
//    ]
//    
//    T<CharArray> convert(wide s)
//    [
//        int len := 0.
//        UTF16Encoding charCount(s, &len).
//        
//        auto retVal := CharArray new(len).
//        s save(0, retVal, &len).
//        
//        ^ retVal.
//    ]
//} 

singleton stringExConvertor
{
//    literal convert(RealMatrix a)
//        = realmatrixExConvertor toLiteral(a).
//
//    literal convert(IntMatrix a)
//        = intmatrixExConvertor toLiteral(a).

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

//    literal convert(Enumerator en)
//    [
//        auto anOutput := TextBuilder new.
//
//        en forEachPair(anOutput,(:aParam:anItem)[ aParam write:(anItem~convertorOp toLiteral); write:"," ]).
//        
//        int aLength := anOutput length.
//
//        if (0 < aLength)
//            [ anOutput setLength(-1 + aLength) ].
//        
//        ^ anOutput literal
//    ]
//
////    literal convert datetime:aValue = aValue literal.
////        
////    literal convert timestamp:aValue = aValue literal.
//
//    literal convert(Enumerable o)
//        <= convert(o enumerator).

    string convert(Indexable o)
        //<= convert(o.indexer());
        = self.convert(o.indexer());

    string convert(o)
        = stringConvertor.convert(o);
}

//class wideExConvertor =
//{
//    wide convert(Indexer it)
//    [
//        auto anOutput := TextBuilder new.
//        while (it available)
//        [
//            anOutput write(it get; toWide).
//            anOutput write:",".
//            
//            it appendIndex:1
//        ].
//        
//        int aLength := anOutput length.
//        if (0 < aLength)
//            [ anOutput setLength(-1 + aLength) ].
//        
//        ^ anOutput
//    ]
//
//    wide convert(Enumerator en)
//    [
//        auto anOutput := TextBuilder new.
//
//        en forEachPair(anOutput,(:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ]).
//        
//        int aLength := anOutput length.
//
//        if (0 < aLength)
//            [ anOutput setLength(-1 + aLength) ].
//        
//        ^ anOutput
//    ]
////        
////    wide convert datetime:aValue = aValue literal; wide.
////        
////    wide convert timestamp:aValue = aValue literal; wide.
////                
//////                intmatrix : m
//////                    = intmatrixExConvertor toLiteral intmatrix:m.
////
//    wide convert(Enumerable o)
//        <= convert(o enumerator).
//
//    wide convert(Indexable o)
//        <= convert(o indexer).
//
//    wide convert:o
//        = wideConvertor convert:o.
//}.
//
//////class wideExConvertor =
//////{
//////    convert enumerable:aList
//////    [
//////        textbuffer anOutput := TextBuilder new.
//////
//////        aList run:anOutput eachPair: (:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ].
//////        
//////        int aLength := anOutput length.
//////
//////        if (0 < aLength)
//////            [ anOutput setLength int:(-1 + aLength) ].
//////        
//////        ^ anOutput wide
//////    ]
//////}.
//
//public singleton class byteArrayExConvertor
//{
//    T<ByteArray> convert(literal s)
//        = UTF8Encoder toByteArray(0, s length, s).
//    
//    T<ByteArray> convert(wide s)
//        = UTF16Encoder toByteArray(0, s length, s).
//}
//
//public singleton intarrayConvertorEx
//{
//    T<IntArray> convert(Enumerator<int> e)
//    [
//        List<int> list := List<int>().
//
//        while (e next)
//        [
//            list append(e get)
//        ].
//
//        ^ IntArray new(list indexer).
//    ]
//    
//    T<IntArray> convert(Enumerator e)
//    [
//        List<int> list := List<int>().
//
//        while (e next)
//        [
//            list append(e get)
//        ].
//
//        ^ IntArray new(list indexer).        
//    ]
//    
//    T<IntArray> convert(o)
//        <= convert(o enumerator).
//    
////    convert enumerable:aList
////    [
////        array_list alist := ArrayList new.
////
////        aList run each: (:anItem)[ alist append:anItem ].
////        
////        ^ IntArray new int:(alist length); set every: (&int:i)[ alist getAt int:i; toInt ]
////    ]
//}
//
//public singleton intmatrixExConvertor
//{
//    convert(Array a)
//    [
//        int n := a length.
//        int m := a[0] length.
//        
//        ^ IntMatrix new(n, m); populate(:i:j)<int,int,int>( a getAt(i); toArray; getAt(j); toInt ).
//    ]
//    
//    toLiteral(IntMatrix matrix)
//    [
//        auto anOutput := TextBuilder new.
//        
//        int i := 0.
//        int j := 0.
//        int n := 0.
//        int m := 0.
//        int l := 0.
//        
//        matrix readLengthsTo(&n, &m).
//        
//        while (i < n)
//        [
//            j := 0.
//            while (j < m)
//            [
//                int r := 0.
//                matrix read(i, j, &r).
//                
//                anOutput write:r; write:",".
//                
//                j := j + 1
//            ].
//            anOutput readLengthTo(&l).            
//            anOutput setLength(l - 1).
//            
//            anOutput write:forwards'newLine.
//            
//            i := i + 1
//        ].
//        
//        ^ anOutput literal
//    ]
//}
//
//public singleton realmatrixExConvertor
//{
//    convert(Array a)
//    [
//        int n := a length.
//        int m := a[0] length.
//        
//        ^ RealMatrix new(n, m); populate(:i:j)<int,int,real>( a getAt(i); toArray; getAt(j); toReal ).
//    ]
//    
//    toLiteral(RealMatrix matrix)
//    [
//        auto anOutput := TextBuilder new.
//        
//        int i := 0.
//        int j := 0.
//        int n := 0.
//        int m := 0.
//        int l := 0.
//        
//        matrix readLengthsTo(&n, &m).
//        
//        while (i < n)
//        [
//            j := 0.
//            while (j < m)
//            [
//                real r := 0.0r.
//                matrix read(i, j, &r).
//                
//                anOutput write:r; write:",".
//                
//                j := j + 1
//            ].
//            anOutput readLengthTo(&l).            
//            anOutput setLength(l - 1).
//            
//            anOutput write:forwards'newLine.
//            
//            i := i + 1
//        ].
//        
//        ^ anOutput literal
//    ]
//}
//
//// --- arrayConvertorEx ---
//
//public singleton arrayConvertorEx
//{
//    T<Array> convert : o
//    [
//        T<Array> arr := T<Array>(o) ?? nil.
//        if (arr == nil)
//        [
//            var it := o indexer.
//            int len := it length.
//            arr := Array new(len).
//            int i := 0.
//            while (i < len)
//            [
//                arr[i] := it get.
//                it appendIndex(1).
//                
//                i += 1.
//            ]
//        ].
//        
//        ^ arr.
//    ]
//    
//    T<Array> convert(Array anArray)
//        = anArray.
//
//    T<Array> convert(literal l)
//        <= convert(T<Enumerator>(l enumerator)).
//
//    T<Array> convert(wide l)
//        <= convert(T<Enumerator>(l enumerator)).
//
//    T<Array> convert(Enumerator anEnumerator)
//    [
//        T<ArrayList> aRetVal := ArrayList new.
//
//        anEnumerator forEach(:anItem)[ aRetVal append:anItem ].
//
//        ^ T<Array>(aRetVal)
//    ]
//
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
//        
////    convert void:val
////        = Array min.                        
//}

// --- convertorOp ---

public extension convertorOp
{
//    T<Array> toArray
//        = arrayConvertorEx convert:self.
//
//    T<IntArray> toIntArray
//        = intarrayConvertorEx convert:self.
//    
//    T<ByteArray> toByteArray        
//        = byteArrayExConvertor convert:self.
//    
//    T<IntMatrix> toIntMatrix
//        = intmatrixExConvertor convert:self.
    
    string toString()
        = stringExConvertor.convert(self);
    
//    T<literal> toLiteral(int radix)
//        = literalConvertor convert(self, radix).
//        
//    T<CharArray> toCharArray
//        = charArrayExConvertor convert:self.
//    
//    T<wide> toWide
//        = wideExConvertor convert:self.
//
//    toWide(int radix)
//        = wideConvertor convert(self, radix).
//            
//    T<byte> toByte
//        = byteConvertor convert:self.
//
//    T<short> toShort
//        = shortConvertor convert:self.
//
//    T<uint> toUInt
//        = uintConvertor convert:self.
//        
//    T<int> toInt
//        = intConvertor convert:self.
//        
//    T<int> toInt(IntNumber radix)
//        = intConvertor convert(self, radix).
//        
//    T<int> toHexInt
//        = intConvertor convert(self, 16).
//
//    T<long> toLong
//        = longConvertor convert:self.
//
//    T<long> toHexLong
//        = longConvertor convert(self, 16).
//    
//    T<char> toChar
//        = charConvertor convert:self.
//
//    toReal
//        = realConvertor convert:self.
//
//    convertTo : aTarget
//        = Variant new:self; saveTo:aTarget.
}

//public convertor 
//      = { generic : n = Wrap::(n, convertorOp)~received get. }.
//
//public extension<Date> dateFormatterOp 
//{
//    toShortTimeLiteral
//        = T<TimeStruct>(self) toShortTime; literal.
//}
