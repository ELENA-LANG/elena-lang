import system'collections.
import system'routines.
import system'calendar.
import system'text.
import system'dynamic.

// --- convertors ---

class literalExConvertor =
{
//    literal convert intmatrix:a
//        = intmatrixExConvertor toLiteral intmatrix:a.

    literal convert(Indexer it)
    [
        auto anOutput := TextBuilder new.
        while (it available)
        [
            anOutput write(it get; toLiteral).
            anOutput write:",".
            
            it appendIndex:1
        ].
        
        int aLength := anOutput length.
        if (0 < aLength)
            [ anOutput setLength(-1 + aLength) ].
        
        ^ anOutput literal
    ]

    literal convert(Enumerator en)
    [
        auto anOutput := TextBuilder new.

        en forEachPair(anOutput,(:aParam:anItem)[ aParam write:(anItem~convertorOp toLiteral); write:"," ]).
        
        int aLength := anOutput length.

        if (0 < aLength)
            [ anOutput setLength(-1 + aLength) ].
        
        ^ anOutput literal
    ]

//    literal convert datetime:aValue = aValue literal.
//        
//    literal convert timestamp:aValue = aValue literal.

    literal convert(Enumerable o)
        <= convert(o enumerator).

    literal convert(Indexable o)
        <= convert(o indexer).

    literal convert:o
        = literalConvertor convert:o.
}.

class wideExConvertor =
{
    wide convert(Indexer it)
    [
        auto anOutput := TextBuilder new.
        while (it available)
        [
            anOutput write(it get; toWide).
            anOutput write:",".
            
            it appendIndex:1
        ].
        
        int aLength := anOutput length.
        if (0 < aLength)
            [ anOutput setLength(-1 + aLength) ].
        
        ^ anOutput
    ]

    wide convert(Enumerator en)
    [
        auto anOutput := TextBuilder new.

        en forEachPair(anOutput,(:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ]).
        
        int aLength := anOutput length.

        if (0 < aLength)
            [ anOutput setLength(-1 + aLength) ].
        
        ^ anOutput
    ]
//        
//    wide convert datetime:aValue = aValue literal; wide.
//        
//    wide convert timestamp:aValue = aValue literal; wide.
//                
////                intmatrix : m
////                    = intmatrixExConvertor toLiteral intmatrix:m.
//
    wide convert(Enumerable o)
        <= convert(o enumerator).

    wide convert(Indexable o)
        <= convert(o indexer).

    wide convert:o
        = wideConvertor convert:o.
}.

////class wideExConvertor =
////{
////    convert enumerable:aList
////    [
////        textbuffer anOutput := TextBuilder new.
////
////        aList run:anOutput eachPair: (:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ].
////        
////        int aLength := anOutput length.
////
////        if (0 < aLength)
////            [ anOutput setLength int:(-1 + aLength) ].
////        
////        ^ anOutput wide
////    ]
////}.
//
//class byteArrayExConvertor =
//{
//    convert literal : s
//        = UTF8Encoder toByteArray int:0 int:(s length) literal:s.
//    
//    convert wide : s
//        = UTF16Encoder toByteArray int:0 int:(s length) wide:s.
//}.

public singleton intarrayConvertorEx
{
    T<IntArray> convert(Enumerator<int> e)
    [
        List<int> list := List<int>().

        while (e next)
        [
            list append(e get)
        ].

        ^ IntArray new(list indexer).
    ]
    
    T<IntArray> convert(o)
        <= convert(Enumerator<int>(o enumerator)).
    
//    convert enumerable:aList
//    [
//        array_list alist := ArrayList new.
//
//        aList run each: (:anItem)[ alist append:anItem ].
//        
//        ^ IntArray new int:(alist length); set every: (&int:i)[ alist getAt int:i; toInt ]
//    ]
}

public singleton intmatrixExConvertor
{
    convert(Array a)
    [
        int n := a length.
        int m := a[0] length.
        
        ^ IntMatrix new(n, m); populate(:i:j)<int,int,int>( a getAt(i); toArray; getAt(j); toInt ).
    ]
    
    toLiteral(IntMatrix matrix)
    [
        auto anOutput := TextBuilder new.
        
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        int l := 0.
        
        matrix readLengthsTo(&n, &m).
        
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                int r := 0.
                matrix read(i, j, &r).
                
                anOutput write:r; write:",".
                
                j := j + 1
            ].
            anOutput readLengthTo(&l).            
            anOutput setLength(l - 1).
            
            anOutput write:forwards'newLine.
            
            i := i + 1
        ].
        
        ^ anOutput literal
    ]
}

// --- arrayConvertorEx ---

public singleton arrayConvertorEx
{
    T<Array> convert : anObject
        = T<Array>(anObject).
    
    T<Array> convert(Array anArray)
        = anArray.

    T<Array> convert(literal l)
        <= convert(T<Enumerator>(l enumerator)).

    T<Array> convert(wide l)
        <= convert(T<Enumerator>(l enumerator)).

    T<Array> convert(Enumerator anEnumerator)
    [
        T<ArrayList> aRetVal := ArrayList new.

        anEnumerator forEach(:anItem)[ aRetVal append:anItem ].

        ^ T<Array>(aRetVal)
    ]

    T<Array> convert(Indexer it)
    [
        T<ArrayList> aRetVal := ArrayList new.
        while (it available)
        [
            aRetVal append(it get).
            
            it appendIndex(1)
        ].

        ^ T<Array>(aRetVal)
    ]

//    convert(Enumerable v)
//        <= convert(v enumerator).
        
    T<Array> convert(Indexable v)
        <= convert(v indexer).
        
//    convert void:val
//        = Array min.                        
}

// --- convertorOp ---

public extension convertorOp
{
    T<Array> toArray
        = arrayConvertorEx convert:self.

    T<IntArray> toIntArray
        = intarrayConvertorEx convert:self.
    
//    toByteArray        
//        = byteArrayExConvertor convert:self.
//    
    T<IntMatrix> toIntMatrix
        = intmatrixExConvertor convert:self.
    
    T<literal> toLiteral
        = literalExConvertor convert:self.
    
    T<literal> toLiteral(int radix)
        = literalConvertor convert(self, radix).
    
    T<wide> toWide
        = wideExConvertor convert:self.

    toWide(int radix)
        = wideConvertor convert(self, radix).
            
    T<byte> toByte
        = byteConvertor convert:self.

    T<short> toShort
        = shortConvertor convert:self.

    T<int> toInt
        = intConvertor convert:self.
        
    T<int> toInt(IntNumber radix)
        = intConvertor convert(self, radix).
        
    T<int> toHexInt
        = intConvertor convert(self, 16).

    T<long> toLong
        = longConvertor convert:self.

    T<long> toHexLong
        = longConvertor convert(self, 16).
    
    T<char> toChar
        = charConvertor convert:self.

    toReal
        = realConvertor convert:self.

    convertTo : aTarget
        = Variant new:self; saveTo:aTarget.
}

public convertor 
    = { generic : n = Wrap::(n, convertorOp)~received get. }.

public extension<Date> dateFormatterOp 
{
    toShortTimeLiteral
        = T<TimeStruct>(self) toShortTime; literal.
}
