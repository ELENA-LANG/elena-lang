// ==== ver 3.4.1 ===

// === actions ===

#symbol ifOrdered
    = (:aFormer:aLater) [ aFormer < aLater ].

// --- common methods ---

/// A generic extender
#class(extension)objectOp
{
    /// Returns true if the specified object instances are equal; otherwise, false.
    #method equal &reference:anObject
        => system'internal'object_equal.
        
    /// Returns the parameter        
    #method back : anObject = anObject.

    #method evalSelf &func1:aFunction
        = aFunction eval:self.

    #method evalSelf:aFunction
        = aFunction eval:self.

    /// Returns true if the object is nil
    #method(type:bool) is &nil
        = nil == self.
        
    /// Returns true if the object is literal or character
    #method is &literal
        = self cast:%eval &to:{ literal : anObject = true. wide : anObject = true. char : anObject = true. ! : anObject = false. }.
        
    /// Returns true if the object is integer numereric
    #method is &integer
        = self cast:%eval &to:{ int : x = true. long : x = true. short : x = true. byte : x = true. ! : anObject = false. }.
        
    /// Returns true if the object is numereric
    #method is &numeric
        = self cast:%eval &to:{ int : x = true. long : x = true. real : x = true. short : x = true. byte : x = true. ! : anObject = false. }.
        
    #method(type:bool,suppress:w2) is &empty
        = self length == 0.
        
    #method(type:bool) is &nonempty
        = self length != 0.
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    #method safeEqual : anObject
        = anObject cast:%eval &to:{ ! : aTarget : anObject = aTarget::$subject equal:anObject \ back:false. } &with:self.
}

// --- Operations ---

// --- byteOp ---

#class(extension:byte) byteOp
{
    #method(type:byte) absolute
    [
        #var(type:byte)n := self byte.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ByteNumber new &byte:n.
    ]
    
    #method(type:byte,suppress:w2) mod : anOperand
    [
        #var retVal := self byte / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:byteConvertor.
    ]    
    
    #method(type:byte)power &byte:y
    [
        #var(type:byte)x := self byte.
        
        #var(type:byte)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ByteNumber new &byte:x.
    ]
    
    #method(type:byte) sqr
    [
        #var(type:byte)n := self byte.        
        #var(type:byte)aRetVal.
        
        aRetVal := n * n.
        
        ^ ByteNumber new &byte:aRetVal.
    ]

    #method power : aPower
        = self::byteOp power &byte:(aPower byte).
                
    #method(type:bool) is &odd
        = self::byteOp mod:2 != 0.
        
    #method(type:bool) is &even
        = self::byteOp mod:2 == 0.
        
    #method(type:bool) is &zero
        = self byte equal &int:0.
        
    #method(type:bool) is &positive
        = self byte > 0.
        
    #method(type:bool) is &negative
        = self byte < 0.
        
    #method(type:bool) is &nonnegative
        = self byte <= 0.
}

// --- intOp ---

#class(extension:short) shortOp
{
    #method(type:short) absolute
    [
        #var(type:short)n := self short.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ShortNumber new &short:n.
    ]
    
    #method(type:short,suppress:w2) mod : anOperand
    [
        #var retVal := self short / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:shortConvertor.
    ]
    
    #method(type:short)power &short:y
    [
        #var(type:short)x := self short.
        
        #var(type:short)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ShortNumber new &short:x.
    ]
    
    #method(type:short) sqr
    [
        #var(type:short)n := self short.        
        #var(type:short)aRetVal.
        
        aRetVal := n * n.
        
        ^ ShortNumber new &short:aRetVal.
    ]

    #method power : aPower
        = self::shortOp power &short:(aPower short).
                
    #method(type:bool) is &odd
        = self::shortOp mod:2 != 0.
        
    #method(type:bool) is &even
        = self::shortOp mod:2 == 0.
        
    #method(type:bool) is &zero
        = self short equal &int:0.
        
    #method(type:bool) is &positive
        = self short > 0.
        
    #method(type:bool) is &negative
        = self short < 0.
        
    #method(type:bool) is &nonnegative
        = self short <= 0.
    
    #method(type:byte) high
    [
        #var(type:short)aWord := self short.
        aWord := aWord && 0FF00h.
        aWord := aWord shift &int:8.
        
        ^ byteConvertor convert &short:aWord.
    ]
        
    #method(type:byte) low
    [
        #var(type:short)aWord := self short.
        aWord := aWord && 0FFh.
        
        ^ byteConvertor convert &short:aWord.
    ]
}

// --- intOp ---

#class(extension:int) intOp
{
    #method(type:int) absolute
    [
        #var(type:int)n := self int.
        
        (n < 0)
            ? [ n := n * -1. ].
        
        ^ IntNumber new &int:n.
    ]
    
    #method(type:int) mod : anOperand
    [
        #var(type:int)x := self int.
        #var(type:int)y := anOperand int.
        
        #var(type:int)z := x / y.
        z := z * y.
        
        ^ IntNumber new &int:(x - z).
    ]
    
    #method(type:int)power &int:y
    [
        #var(type:int)x := self int.
        #var(type:int)r := 1.
        
        #var(type:int)i := 0.
        #loop (i < y)?
        [
            r := r * x.
            
            i := i + 1.
        ].
        
        ^ IntNumber new &int:r.
    ]
    
    #method(type:int) sqr
    [
        #var(type:int)n := self int.        
        #var(type:int)aRetVal.
        
        aRetVal := n * n.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method power : aPower
        = self::intOp power &int:(aPower int).
                
    #method(type:bool) is &odd
        = self int and &int:1 != 0.
        
    #method(type:bool) is &even
        = self int and &int:1 == 0.
        
    #method(type:bool) is &zero
        = self int equal &int:0.
        
    #method(type:bool) is &positive
        = self int > 0.
        
    #method(type:bool) is &negative
        = self int < 0.
        
    #method(type:bool) is &nonnegative
        = self int <= 0.

    #method high
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FFFF0000h.
        aWord := aWord shift &int:16.
        
        ^ shortConvertor convert &int:aWord.
    ]

    #method low
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FFFFh.
        
        ^ shortConvertor convert &int:aWord.
    ]
}

// --- longOp ---

#class(extension:long) longOp
{
    #method(type:long) absolute
    [
        #var(type:long)n := self long.

        (n < 0l)
            ? [ n := n * -1l. ].
                
        ^ LongNumber new &long:n.
    ]
    
    #method(type:long) mod : anOperand
    [
        #var(type:long)x := self long.
        #var(type:long)y := anOperand long.
        
        #var(type:long)z := x / y.
        z := z * y.
        
        ^ LongNumber new &long:(x - z).
    ]
    
    #method(type:long)power &long:y
    [
        #var(type:long)x := self long.
        
        #var(type:long)i := 0l.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ LongNumber new &long:x.
    ]
    
    #method(type:long) sqr
    [
        #var(type:long)n := self long.        
        #var(type:long)aRetVal.
        
        aRetVal := n * n.
        
        ^ LongNumber new &long:aRetVal.
    ]

    #method power : aPower
        = self::longOp power &long:(aPower long).
                
    #method(type:bool) is &odd
        = self::longOp mod:2l != 0.
        
    #method(type:bool) is &even
        = self::longOp mod:2l == 0.
        
    #method(type:bool) is &zero
        = self long equal &long:0l.
        
    #method(type:bool) is &positive
        = self long > 0l.
        
    #method(type:bool) is &negative
        = self long < 0l.
        
    #method(type:bool) is &nonnegative
        = self long <= 0l.

    #method(type:int) high
    [
        #var(type:long)aWord := self long.
        aWord := aWord && -4294967296l.
        aWord := aWord shift &int:32.
        
        ^ intConvertor convert &long:aWord.
    ]

    #method(type:int) low
    [
        #var(type:long)aWord := self long.
        aWord := aWord && 4294967295l.
        
        ^ intConvertor convert &long:aWord.
    ]
}

// --- realOp ---

#class(extension:real) realOp
{        
    #method(type:real) absolute
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    #method(type:real) exp
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController exp &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(type:real) ln
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController ln &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(type:real) sqrt
    [
        #var(type:real)n := self real.      
        #var(type:real)aRetVal.
        
        realOpController sqrt &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real)power &int:y
    [
        #var(type:real)x := self real.

        (x == 0.0r)
            ? [ ^ 0.0r. ].
                        
        (y == 0)
            ? [ ^ 1.0r. ].
                        
        (x < 0.0r)
            ? [
                x := x * -1.0r.
                
                realOpController ln &real:x &vreal:x.
                
                x := x multiply &int:y.
                
                realOpController exp &real:x &vreal:x.
                
                (y::intOp is &odd)
                    ? [ x := x * -1.0r. ].
            ]
            ! [
                realOpController ln &real:x &vreal:x.
                
                x := x multiply &int:y.
                
                realOpController exp &real:x &vreal:x.
            ].
        
        ^ RealNumber new &real:x.
    ]

    #method power : aPower
        = self::realOp power &int:(aPower int).

    #method(type:real) sqr
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        aRetVal := n * n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) rounded
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController round &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) integer
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController truncate &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) frac
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:(n - aRetVal).
    ]

    #method(type:real)sin
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController sin &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real)cos
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController cos &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real)tan
    [
        #var(type:real)n := self real.        
        #var(type:real)aSinVal.
        #var(type:real)aCosVal.
        
        realOpController sin &real:n &vreal:aSinVal.
        realOpController cos &real:n &vreal:aCosVal.
        
        ^ RealNumber new &real:(aSinVal / aCosVal).
    ]

    #method(type:real)arctan
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController arctan &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
            
    #method(type:real)arcsin
    [
        #var(type:real)n := self real.        
        #var(type:real)arg.
        #var(type:real)aRetVal.
        
        n := 1.0r - n*n.
        
        realOpController sqrt &real:n &vreal:arg.
        realOpController arctan &real:arg &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
            
    #method(type:real)arccos
    [
        #var(type:real)n := self real.        
        #var(type:real)arg.
        #var(type:real)aRetVal.
        
        n := 1.0r - n*n.
        
        realOpController sqrt &real:n &vreal:arg.
        
        arg := arg / n.

        realOpController arctan &real:arg &vreal:aRetVal.
                
        ^ RealNumber new &real:aRetVal.
    ]
            
    #method(type:real) radian
    [
        #var(type:real)aValue := self real.
        
        aValue := aValue * pi_value.
        
        ^ RealNumber new &real:(aValue / 180.0r).
    ]
            
    #method(type:real) degree
    [
        #var(type:real)aValue := self real.
        
        aValue := aValue * 180.0r.
        
        ^ RealNumber new &real:(aValue / pi_value).
    ]
            
    #method(type:real) reciprocal
    [
        #var(type:real)aValue := self real.
        
        ^ RealNumber new &real:(1.0r / aValue).
    ]    
            
    #method(type:bool) is &zero
        = self real == 0.
        
    #method(type:bool) is &positive
        = self real > 0.
        
    #method(type:bool) is &negative
        = self real < 0.
        
    #method(type:bool) is &nonnegative
        = self real <= 0.
}

// --- arrayOp ---

#class(extension:intarray)intArrayOp
{
    #method(stacksafe) exchange &int:anIndex1 &int:anIndex2
    [
        #var(type:intarray)a := self intarray.

        #var(type:int)n1.
        #var(type:int)n2.

        a read &int:anIndex1 &vint:n1.
        a read &int:anIndex2 &vint:n2.
        
        a write &int:anIndex1 &int:n2.
        a write &int:anIndex2 &int:n1.
    ]

    #method set &every:anIndexFunc
        = self::intArrayOp set &nfunc:(anIndexFunc nfunc).
        
    #method set &nfunc:anIndexFunc
    [
        #var(type:intarray)anArray := self intarray.
        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength := anArray length.
        
        #loop (anIndex < aLength) ?
        [            
            anArray write &int:anIndex &int:(anIndexFunc eval &int:anIndex int).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]
}

// --- arrayOp ---

#class(extension:array)arrayOp
{
    #method(stacksafe) reallocate &int:aNewLength
        => system'internal'arr_realloc.

    #method exchange : anIndex1 : anIndex2
        = self::arrayOp exchange &int:(anIndex1 int) &int:(anIndex2 int).
        
    #method exchange &int:anIndex1 &int:anIndex2
    [
        #var(type:array) anArray := self array.

        #var aTemp := anArray getAt &int:anIndex1.
        
        anArray setAt &int:anIndex1 &object:(anArray getAt &int:anIndex2).
        anArray setAt &int:anIndex2 &object:aTemp.
    ]
    
    #method(stacksafe,suppress:w2) indexOf &int:anIndex &object:anObject &vint:aRetVal
    [
        aRetVal << -1.
        
        #var(type:array)anArray := self array.
        #var(type:int)aLength := anArray length.
        
        #var(type:int)i := anIndex.
        #loop (i < aLength)?
        [
            #var anArrayItem := anArray getAt &int:i.
            
            (anObject == anArrayItem)
                ? [ aRetVal << i. ].
            
            i := i + 1.
        ].
    ]        
        
    #method(stacksafe) indexOf &int:anIndex &object:anObject
    [
        #var(type:int)aRetVal.
        
        self::arrayOp indexOf &int:anIndex &object:anObject &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method indexOf : anObject
        = self::arrayOp indexOf &int:0 &object:anObject.
        
    #method(suppress:w2) safeSeek : aValue
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength := anArray length.
        
        #var(type:int)i := 0.
        #loop (i < aLength)?
        [
            #var anArrayItem := anArray getAt &int:i.
            
            (aValue safeEqual:anArrayItem)
                ? [ ^ anArrayItem. ].
            
            i := i + 1.
        ].
        
        ^ $nil.
    ]
        
    #method deepClone
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
        #var(type:array)aCopy := Array new &int:aLength.
                
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            aCopy setAt &int:anIndex &object:(anArray getAt &int:anIndex clone).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]
    
    #method set &every:anIndexFunc
        = self::arrayOp set &nfunc:(anIndexFunc nfunc).
    
    #method set &nfunc:anIndexFunc
    [
        #var(type:array)anArray := self array.
        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
        #loop (anIndex < aLength) ?
        [            
            anArray setAt &int:anIndex &object:(anIndexFunc eval &int:anIndex).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]
    
    #method sort : aCompareFunc
        = self::arrayOp sort &func2:(aCompareFunc func2).
    
    #method(suppress:w2) sort &func2:aCompareFunc
    [
        #var(type:array)anArray := self array.
        
        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)aLength := anArray length.
        #var aCurrent := $nil.
        #var aNext := $nil.
        
        #loop (i < aLength) ?
        [
            aCurrent := anArray getAt &int:i.
            
            j := i + 1.
            #loop (j < aLength) ?
            [
                aNext := anArray getAt &int:j.
                (aCompareFunc eval:aCurrent:aNext)
                    ! [                        
                        anArray setAt &int:i &object:aNext.
                        anArray setAt &int:j &object:aCurrent.
                        
                        aCurrent := aNext.
                    ].
                    
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ self.
    ]
    
    #method ascendant
        = (self clone)::arrayOp sort:ifOrdered.
    
    #method(type:bool) is &empty
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
        ^ aLength == 0.
    ]
    
    #method Subarray : aLength &at:anIndex
        = self::arrayOp Subarray &int:(anIndex int) &int:(aLength int).
    
    /// obsolete
    #method Subarray &index:anIndex &length:aLength
        = self::arrayOp Subarray &int:(anIndex int) &int:(aLength int).
    
    #method Subarray &int:anIndex &int:aLength
    [
        #var(type:iterator) it := self iterator.
        
        ^ Array new &int:aLength set &nfunc: (&int:i) [ it writeIndex &int:(anIndex + i) get ].
    ]
}

// --- literalOp ---

#class(extension:literal)literalOp
{
    #method indexOf : anObject
        = self literal indexOf:anObject &at:0.

    #method indexOf &literal:aLiteral
        = self literal indexOf &literal:aLiteral &at:0.

    #method indexOf &char:aChar
        = self literal indexOf &char:aChar &at:0.
        
    #method(type:literal) Substring &at:anIndex
        = self::literalOp Substring &int:(anIndex int).
        
    #method(type:literal,stacksafe) Substring &int:anIndex
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &int:anIndex &int:aLength.
    ]        
    
    #method trimLeft &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength := aLiteral length.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen := aChar length.
        
        #loop (anIndex < aLength) ?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            
            (aCurrent == aChar)
                ! [ 
                    (anIndex == 0)
                        ? [ ^ aLiteral. ]
                        ! [^ aLiteral delete &int:0 &int:anIndex. ].
                ].
            
            anIndex := anIndex + aCharLen.
        ].
        
        ^ emptyLiteralValue.
    ]
    
    #method trimLeft
        = self::literalOp trimLeft &char:#32.    
       
    #method(suppress:w2) trimRight &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        #var(type:int)anIndex := aLength - 1.
        #var(type:int)aPrevious := aLength.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen := aChar length.
        
        #loop (anIndex >= 0) ?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            
            (aCurrent equal &char:aChar)
                ! [ ^ aLiteral delete &int:aPrevious &int:(aLength - aPrevious). ].
            
            aPrevious := anIndex.
            anIndex := anIndex - aCharLen.
        ].
        
        ^ emptyLiteralValue.
    ]
    
    #method trimRight
        = self::literalOp trimRight &char:#32.
        
    #method trim &char:aChar
        = self::literalOp trimLeft &char:aChar trimRight &char:aChar.
    
    #method trim
        = self::literalOp trimLeft &char:#32 trimRight &char:#32.        
    
    #method padLeft &char:aChar &int:aLength
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:int)aCurrentLength := aLiteral length.
        
        (aCurrentLength < aLength)
            ? [ ^ LiteralValue new &int:(aLength - aCurrentLength) &char:aChar add &literal:aLiteral. ].
            
        ^ self.            
    ]
    
    #method padRight &char:aChar &int:aLength
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aCurrentLength.
        aLiteral readLength &vint:aCurrentLength.

        (aCurrentLength < aLength)
            ? [ ^ aLiteral add &literal:(LiteralValue new &int:(aLength - aCurrentLength) &char:aChar). ].
            
        ^ self.            
    ]

    #method startingWith:s
        = self::literalOp startingWith &literal:(s literal).
        
    #method startingWith &literal:s
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:vint)n.
        aLiteral indexOf &int:0 &literal:s &vint:n.
        
        ^ 0 == n.
    ]
        
    #method endingWith &literal:s
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:int)l1.
        #var(type:int)l2.
        aLiteral readLength &vint:l1.
        s readLength &vint:l2.

        #var(type:vint)n.
        aLiteral indexOf &int:(l1 - l2) &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method endingWith:s
        = self::literalOp endingWith &literal:(s literal).
        
    #method containing &literal:s
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:int)l1 := aLiteral length.
        #var(type:int)l2 := s length.

        #var(type:vint)n.
        aLiteral indexOf &int:0 &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method containing:s
        = self::literalOp containing &literal:(s literal).
    
    #method replaceFirst &literal:aReplacee &literal:aReplacer
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:vint) aReplaceeLen := aReplacee length.
        #var(type:vint)anIndex.
        
        aLiteral indexOf &int:0 &literal:aReplacee &vint:anIndex.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &int:anIndex &int:aReplaceeLen insert &int:anIndex &literal:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]    
    
    #method replace &literal:aReplacee &literal:aReplacer
    [
        #var(type:literal)aLiteral := self literal.        
        #var aTarget := String new.
        #var(type:int)aStart := 0.
        #var(type:int)anEnd.
        #var(type:int)aReplaceeLen := aReplacee length.
        
        aLiteral indexOf &int:aStart &literal:aReplacee &vint:anEnd.
        
        #loop (anEnd != -1) ?
        [
            aTarget 
                append &literal:(aLiteral Substring &int:aStart &int:(anEnd - aStart))
                append &literal:aReplacer.
            
            aStart := anEnd + aReplaceeLen.
            
            aLiteral indexOf &int:aStart &literal:aReplacee &vint:anEnd.
        ].
    
        ^ (aStart != 0)
            ? [ aTarget append:(aLiteral::literalOp Substring &int:aStart) literal ]
            ! [ self literal ].
    ]
    
    #method(type:bool) is &empty
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        ^ aLength == 0.
    ]    
}

// --- wideOp ---

#class(extension:wide)wideOp
{
    #method indexOf : anObject
        = self wide indexOf:anObject &at:0.
        
    #method indexOf &wide:aLiteral
        = self wide indexOf &int:0 &wide:aLiteral.

    #method indexOf &char:aChar
        = self wide indexOf &int:0 &char:aChar.
                
    #method(type:wide) Substring &at:anIndex
        = self::wideOp Substring &int:(anIndex int).
        
    #method(type:wide,stacksafe) Substring &int:anIndex
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aLength := aLiteral length.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &int:anIndex &int:aLength.
    ]
     
    #method(suppress:w2) trimLeft &char:aChar
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)anIndex := 0.
        #var(type:int)aLength := aLiteral length.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen := aChar length.
        
        #loop (anIndex < aLength) ?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            
            (aCurrent equal &char:aChar)
                ! [ ^ aLiteral delete &int:0 &int:anIndex. ].
            
            anIndex := anIndex + aCharLen.
        ].
        
        ^ emptyWideLiteralValue.
    ]
    
    #method trimLeft
        = self::wideOp trimLeft &char:#32.
       
    #method(suppress:w2) trimRight &char:aChar
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aLength := aLiteral length.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen := aChar length.
                
        #var(type:int)anIndex := aLength - 1.
        #var(type:int)aPrevious := aLength.
        #loop (anIndex >= 0) ?
        [
            aLiteral read &int:anIndex &vchar:aCurrent.
            
            (aCurrent equal &char:aChar)
                ! [ ^ aLiteral delete &int:aPrevious &int:(aLength - aPrevious). ].
            
            aPrevious := anIndex.
            anIndex := anIndex - aCharLen.
        ].
        
        ^ emptyWideLiteralValue.
    ]
    
    #method trimRight
        = self::wideOp trimRight &char:#32.
       
    #method trim &char:aChar
        = self::wideOp trimLeft &char:aChar trimRight &char:aChar.
    
    #method trim
        = self::wideOp trimLeft &char:#32 trimRight &char:#32.
    
    #method padLeft &char:aChar &int:aLength
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)aCurrentLength := aLiteral length.
        
        (aCurrentLength < aLength)
            ? [ ^ WideLiteralValue new &int:(aLength - aCurrentLength) &char:aChar add &wide:aLiteral. ].
            
        ^ self.            
    ]
    
    #method padRight &char:aChar &int:aLength
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aCurrentLength := aLiteral length.

        (aCurrentLength < aLength)
            ? [ ^ aLiteral add &wide:(WideLiteralValue new &int:(aLength - aCurrentLength) &char:aChar). ].
            
        ^ self.            
    ]

    #method startingWith &wide:s
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)n.
        aLiteral indexOf &int:0 &wide:s &vint:n.
        
        ^ 0 == n.
    ]
    
    #method startingWith:s
        = self::wideOp startingWith &wide:(s wide).

    #method endingWith &wide:s
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)l1 := aLiteral length.
        #var(type:int)l2 := s length.
        #var(type:int)n.        
        aLiteral indexOf &int:(l1 - l2) &wide:s &vint:n.
        
        ^ n != -1.
    ]
    
    #method endingWith:s
        = self::wideOp endingWith &wide:(s wide).

    #method containing &wide:s
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)l1 := aLiteral length.
        #var(type:int)l2 := s length.
        #var(type:int)n.
        
        aLiteral indexOf &int:0 &wide:s &vint:n.
        
        ^ n != -1.
    ]
    
    #method containing:s
        = self::wideOp containing &wide:(s wide).

    #method replaceFirst &wide:aReplacee &wide:aReplacer
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int) aReplaceeLen := aReplacee length.
        #var(type:int)anIndex.
        
        aLiteral indexOf &int:0 &wide:aReplacee &vint:anIndex.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &int:anIndex &int:aReplaceeLen insert &int:anIndex &wide:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]
        
    #method(suppress:w2) replace &wide:aReplacee &wide:aReplacer
    [
        #var(type:wide)aLiteral := self wide.
        
        #var aTarget := String new.
        #var(type:int)aStart := 0.
        #var(type:int)anEnd.
        #var(type:int)aReplaceeLen := aReplacee length.
        
        aLiteral indexOf &int:aStart &wide:aReplacee &vint:anEnd.
        
        #loop (anEnd != -1) ?
        [
            aTarget 
                append &wide::(aLiteral Substring &int:aStart &int:(anEnd - aStart))
                append &wide:aReplacer.
            
            aStart := anEnd + aReplaceeLen.
            
            aLiteral indexOf &int:aStart &wide:aReplacee &vint:anEnd.
        ].
    
        ^ (aStart != 0)
            ? [ aTarget append:(aLiteral::wideOp Substring &int:aStart) wide ]
            ! [ aLiteral ].
    ]

    #method(type:bool) is &empty
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aLength := aLiteral length.
        
        ^ aLength == 0.
    ]
}

// --- stringOp ---

#symbol stringOpDispatcher =
{
    eval &literal:aLiteral = literalOp.
    
    eval &wide:aLiteral = wideOp.
}.

#class(extension) stringOp
{
    #method Substring : anIndex
        = self Substring:(self length - anIndex) &at:(anIndex int).
            
    #method Substring : anIndex : aLength
        = self Substring:aLength &at:anIndex.
            
    #method trimLeft &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimLeft &char:aChar.
    ]

    #method trimRight &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimRight &char:aChar.
    ]

    #method trim &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trim &char:aChar.
    ]

    #method trim
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trim.
    ]

    #method trimLeft
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimLeft.
    ]

    #method trimRight
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimRight.
    ]
    
    #method padLeft &char:aChar &length:aLength
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher padLeft &char:aChar &int:(aLength int).
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher padRight &char:aChar &int:(aLength int).
    ]
    
    #method startingWith : s
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher startingWith:s.
    ]
    
    #method endingWith : s
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher endingWith:s.
    ]
    
    #method containing : s
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher containing:s.
    ]
    
    #method replaceFirst &literal:aReplacee &literal:aReplacer
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher replaceFirst &literal:aReplacee &literal:aReplacer.
    ]
    
    #method replace &literal:aReplacee &literal:aReplacer
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher replace &literal:aReplacee &literal:aReplacer.
    ]
    
    #method write : aLiteral &length:aCounter
        = self::stringOp write : aLiteral &int:(aCounter int).
            
    #method(stacksafe) write : aLiteral &int:aCounter
    [
        #var(type:int)aNumber := aCounter.
        
        #loop (aNumber > 0) ?
        [
            self write:aLiteral. 
            
            aNumber := aNumber - 1.
        ].
            
        ^ self.
    ]
    
    #method write : aLine &paddingLeft:aWidth &with:aChar
    [
        #var aPaddedStr := aLine padLeft &char:(aChar char) &length:(aWidth int).

        self write:aPaddedStr.
            
        ^ self.
    ]

    #method write : aLine &paddingRight:aWidth &with:aChar
    [
        #var aPaddedStr := aLine padRight &char:(aChar char) &length:(aWidth int).

        self write:aPaddedStr.
            
        ^ self.
    ]
    
    #method write : aLine &padding:aWidth &with:aChar
    [
        #var(type:int)aWidthInt := aWidth int.
        #var(type:int)aLength := aLine length.        
        
        (aLength < aWidthInt)
            ? [
                #var(type:int)aLen := aWidthInt - aLength.
                #var(type:int)aLeftLen := aLen / 2.
                
                self::stringOp write:aChar &int:aLeftLen.
                self write:aLine.
                self::stringOp write:aChar &int:(aLen - aLeftLen).
            ]
            ! [ self write:aLine. ].
            
        ^ self.
    ]    
}
