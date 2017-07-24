import system'collections.
import system'routines.
import system'calendar.
import system'text.
import system'dynamic.

// --- convertors ---

class literalExConvertor =
{
    convert enumerable:aList
    [
        textbuffer anOutput := TextBuilder new.

        aList run:anOutput eachPair: (:aParam:anItem)[ aParam write:(anItem~convertorOp toLiteral); write:"," ].
        
        int aLength := anOutput length.

        if (0 < aLength)
            [ anOutput setLength int:(-1 + aLength) ].
        
        ^ anOutput literal
    ]
}.

class wideExConvertor =
{
    convert enumerable:aList
    [
        textbuffer anOutput := TextBuilder new.

        aList run:anOutput eachPair: (:aParam:anItem)[ aParam write:(anItem~convertorOp toWide); write:"," ].
        
        int aLength := anOutput length.

        if (0 < aLength)
            [ anOutput setLength int:(-1 + aLength) ].
        
        ^ anOutput wide
    ]
}.

class arrayExConvertor =
{
    convert enumerable:aList
    [
        array_list aRetVal := ArrayList new.

        aList run each: (:anItem)[ aRetVal append:anItem ].

        ^ aRetVal array
    ]
}.

class intarrayExConvertor =
{
    convert enumerable:aList
    [
        array_list alist := ArrayList new.

        aList run each: (:anItem)[ alist append:anItem ].
        
        ^ IntArray new int:(alist length); set every: (&int:i)[ alist getAt int:i; toInt ]
    ]
}.

class intmatrixExConvertor =
{
    convert array:a
    [
        int n := a length.
        int m := a@0 length.
        
        ^ IntMatrix new int:n int:m; set every_ij: (&int:i&int:j)[ a getAt int:i; array; getAt int:j; toInt ].
    ]
    
    toLiteral intmatrix:matrix
    [
        textbuffer anOutput := TextBuilder new.
        
        int i := 0.
        int j := 0.
        int n := 0.
        int m := 0.
        int l := 0.
        
        matrix readLengthsTo vint:n vint:m.
        
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                int r := 0.
                matrix read int:i int:j vint:r.
                
                anOutput write:r; write:",".
                
                j := j + 1
            ].
            anOutput readLengthTo vint:l.            
            anOutput setLength int:(l - 1).
            
            anOutput write:'newLine.
            
            i := i + 1
        ].
        
        ^ anOutput literal
    ]
}.

// --- convertorOp ---

extension convertorOp
{
    toArray
        = self cast:%eval to:
            {
                literal : aLiteral = arrayExConvertor convert enumerable:aLiteral.
                
                wide : aWide = arrayExConvertor convert enumerable:aWide.
                
                array : anArray
                    = anArray.
                    
                enumerable : aList
                    = arrayExConvertor convert enumerable:aList.
                    
                enumerator : anEnumerator
                    = arrayExConvertor convert enumerable:anEnumerator.
                    
                void : val
                    = Array min.                    
            }.
    
    toIntArray
        = self cast:%eval to:
            {
                intarray : anArray
                    = anArray.
                
                array : anArray
                    = intarrayExConvertor convert enumerable:anArray.
                    
                enumerable : aList
                    = intarrayExConvertor convert enumerable:aList.
                    
                enumerator : anEnumerator
                    = intarrayExConvertor convert enumerable:anEnumerator.
            }.
    
    toByteArray        
        = self cast:%eval to:
        {
            literal : s
                = UTF8Encoder toByteArray int:0 int:(s length) literal:s.
            
            wide : s
                = UTF16Encoder toByteArray int:0 int:(s length) wide:s.
        }.
    
    toIntMatrix
        = self cast:%eval to:
            {
                intmatrix : anArray
                    = anArray.
                
                array : anArray
                    = intmatrixExConvertor convert array:anArray.
            }.
    
    toLiteral
        = self cast:%eval to:
            {
                literal : aLiteral = aLiteral.
                
                wide : aWide = aWide literal.
                
                array : anArray
                    = literalExConvertor convert enumerable:anArray.
                
                enumerable : aList
                    = literalExConvertor convert enumerable:aList.
                    
                int : aValue = aValue literal.
                    
                short : aValue = aValue literal.
                    
                byte : aValue = aValue literal.
                    
                long : aValue = aValue literal.
                    
                real : aValue = aValue literal.
                    
                datetime : aValue = aValue literal.
                    
                timestamp : aValue = aValue literal.
                
                intmatrix : m
                    = intmatrixExConvertor toLiteral intmatrix:m.
                
                generic : anObject = anObject literal.
            }.
    
    toWide
        = self cast:%eval to:
            {
                literal : aLiteral = aLiteral wide.
                
                wide : aWide = aWide.
                
                array : anArray
                    = wideExConvertor convert enumerable:anArray.
                
                enumerable : aList
                    = wideExConvertor convert enumerable:aList.
                    
                int : aValue = aValue wide.
                    
                short : aValue = aValue wide.
                    
                byte : aValue = aValue wide.
                    
                long : aValue = aValue wide.
                    
                real : aValue = aValue wide.
                    
                datetime : aValue = aValue wide.
                    
                timestamp : aValue = aValue wide.
                
                generic : anObject = anObject wide.
            }.

     toLiteral base:aBase
        = self cast:%eval to:
                   { 
                      eval : aBase byte : anInt = literalConvertor convert byte:anInt int:aBase.
                      
                      eval : aBase short : anInt = literalConvertor convert short:anInt int:aBase.
                      
                      eval : aBase int : anInt = literalConvertor convert int:anInt int:aBase.
                      
                      eval : aBase long : anInt = literalConvertor convert long:anInt int:aBase.
                      
                   } with:aBase.

     toWide base:aBase
        = self cast:%eval to:
                   { 
                      eval : aBase byte : anInt = wideConvertor convert byte:anInt int:aBase.
                      
                      eval : aBase short : anInt = wideConvertor convert short:anInt int:aBase.
                      
                      eval : aBase int : anInt = wideConvertor convert int:anInt int:aBase.
                      
                      eval : aBase long : anInt = wideConvertor convert long:anInt int:aBase.
                      
                   } with:aBase.
            
    toByte
        = self cast:%convert to:byteConvertor.

    toShort
        = self cast:%convert to:shortConvertor.

    toInt
        = self cast:%convert to:intConvertor.
        
    toHexInt
        = self cast:%eval to:
            {
                literal : l = intConvertor convert literal:l base:16.
                
                wide : l = intConvertor convert wide:l base:16.
            }.

    toLong
        = self cast:%convert to:longConvertor.
        
    toHexLong
        = self cast:%eval to:
            {
                literal : l = longConvertor convert literal:l base:16.
                
                wide : l = longConvertor convert wide:l base:16.
            }.
    
    toChar
        = self cast:%convert to:charConvertor.
    
    toReal
        = self cast:%convert to:realConvertor.

    convert to : aTarget
        = aTarget set:(aTarget cast:%eval to:{ generic : x : y = (Variant new:x)~$subject get. } with:self).
}

class convertor 
    = { generic : n = Wrap::(n, convertorOp)~$subject get. }.

datetime extension dateFormatterOp 
{
    toShortTimeLiteral
        = self __time_st; toShortTime; literal.
}

/// obsolete
symbol String = extensions'text'StringWriter.
