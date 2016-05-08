// === actions ===

#symbol ifOrdered
    = (:aFormer:aLater) [ aFormer < aLater ].

#symbol ifDecrescent
    = (:aFormer:aLater) [ aFormer > aLater ].

// --- common methods ---

/// A generic extender
#class(extension)objectOp
{
    /// Returns true if the specified object instances are equal; otherwise, false.
    #method(bool) equal &reference:anObject
        => system'core_routines'__object.
        
    /// Returns the parameter        
    #method back : anObject = anObject.

    #method evalSelf &func1:aFunction
        = aFunction eval:self.

    #method evalSelf:aFunction
        = aFunction eval:self.

    /// Returns true if the object is nil
    #method(bool) is &nil
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
        
    #method(bool,suppress:w2) is &empty
        = self length == 0.
        
    #method(bool) is &nonempty
        = self length != 0.
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    #method safeEqual : anObject
        = anObject cast:%eval &to:{ ! : aTarget : anObject = aTarget::$subject equal:anObject \ back:false. } &with:self.

    #method eval &with:action
        = action eval:self.
}

// --- Operations ---

// --- byteOp ---

#class(extension:byte) byteOp
{
    #method(byte) absolute
    [
        #var(byte)n := self byte.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ByteNumber new &byte:n.
    ]
    
    #method(byte,suppress:w2) mod : anOperand
    [
        #var retVal := self byte / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:byteConvertor.
    ]    
    
    #method(byte)power &byte:y
    [
        #var(byte)x := self byte.
        
        #var(byte)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ByteNumber new &byte:x.
    ]
    
    #method(byte) sqr
    [
        #var(byte)n := self byte.        
        #var(byte)aRetVal.
        
        aRetVal := n * n.
        
        ^ ByteNumber new &byte:aRetVal.
    ]

    #method power : aPower
        = self::byteOp power &byte:(aPower byte).
                
    #method(bool) is &odd
        = self::byteOp mod:2 != 0.
        
    #method(bool) is &even
        = self::byteOp mod:2 == 0.
        
    #method(bool) is &zero
        = self byte equal &int:0.
        
    #method(bool) is &positive
        = self byte > 0.
        
    #method(bool) is &negative
        = self byte < 0.
        
    #method(bool) is &nonnegative
        = self byte <= 0.
}

// --- shortOp ---

#class(extension:short) shortOp
{
    #method(short) absolute
    [
        #var(short)n := self short.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ShortNumber new &short:n.
    ]
    
    #method(short,suppress:w2) mod : anOperand
    [
        #var retVal := self short / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:shortConvertor.
    ]
    
    #method(short)power &short:y
    [
        #var(short)x := self short.
        
        #var(short)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ShortNumber new &short:x.
    ]
    
    #method(short) sqr
    [
        #var(short)n := self short.        
        #var(short)aRetVal.
        
        aRetVal := n * n.
        
        ^ ShortNumber new &short:aRetVal.
    ]

    #method power : aPower
        = self::shortOp power &short:(aPower short).
                
    #method(bool) is &odd
        = self::shortOp mod:2 != 0.
        
    #method(bool) is &even
        = self::shortOp mod:2 == 0.
        
    #method(bool) is &zero
        = self short equal &int:0.
        
    #method(bool) is &positive
        = self short > 0.
        
    #method(bool) is &negative
        = self short < 0.
        
    #method(bool) is &nonnegative
        = self short <= 0.
    
    #method(byte) high
    [
        #var(short)aWord := self short.
        aWord := aWord && 0FF00h.
        aWord := aWord shift &int:8.
        
        ^ byteConvertor convert &short:aWord.
    ]
        
    #method(byte) low
    [
        #var(short)aWord := self short.
        aWord := aWord && 0FFh.
        
        ^ byteConvertor convert &short:aWord.
    ]
}

// --- intOp ---

#class(extension:int) intOp
{
    #method(int) absolute
    [
        #var(int)n := self int.
        
        (n < 0)
            ? [ n := n * -1. ].
        
        ^ IntNumber new &int:n.
    ]
    
    #method(int) mod : anOperand
    [
        #var(int)x := self int.
        #var(int)y := anOperand int.
        
        #var(int)z := x / y.
        z := z * y.
        
        ^ IntNumber new &int:(x - z).
    ]
    
    #method(int)power &int:y
    [
        #var(int)x := self int.
        #var(int)r := 1.
        
        #var(int)i := 0.
        #loop (i < y)?
        [
            r := r * x.
            
            i += 1.
        ].
        
        ^ IntNumber new &int:r.
    ]
    
    #method(int) sqr
    [
        #var(int)n := self int.        
        #var(int)aRetVal.
        
        aRetVal := n * n.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(int) sqrt
    [
        #var(int)num := self int.
        
        (num == 0)
            ? [ ^ 0. ].
            
        #var(int)n := num / 2 + 1.
        #var(int)n1 := (n + (num / n)) / 2.
        #loop (n1 < n)?
        [
            n := n1.
            n1 := (n + (num / n)) / 2.
        ].
        
        ^ IntNumber new &int:n.
    ]

    #method power : aPower
        = self::intOp power &int:(aPower int).
                
    #method(bool) is &odd
        = self int and &int:1 != 0.
        
    #method(bool) is &even
        = self int and &int:1 == 0.
        
    #method(bool) is &zero
        = self int equal &int:0.
        
    #method(bool) is &positive
        = self int > 0.
        
    #method(bool) is &negative
        = self int < 0.
        
    #method(bool) is &nonnegative
        = self int <= 0.

    #method high
    [
        #var(int)aWord := self int.
        aWord := aWord && 0FFFF0000h.
        aWord := aWord >> 16.
        
        ^ shortConvertor convert &int:aWord.
    ]

    #method low
    [
        #var(int)aWord := self int.
        aWord := aWord && 0FFFFh.
        
        ^ shortConvertor convert &int:aWord.
    ]
}

// --- longOp ---

#class(extension:long) longOp
{
    #method(long) absolute
    [
        #var(long)n := self long.

        (n < 0l)
            ? [ n := n * -1l. ].
                
        ^ LongNumber new &long:n.
    ]
    
    #method(long) mod : anOperand
    [
        #var(long)x := self long.
        #var(long)y := anOperand long.
        
        #var(long)z := x / y.
        z := z * y.
        
        ^ LongNumber new &long:(x - z).
    ]
    
    #method(long)power &long:y
    [
        #var(long)x := self long.
        
        #var(long)i := 0l.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ LongNumber new &long:x.
    ]
    
    #method(long) sqr
    [
        #var(long)n := self long.        
        #var(long)aRetVal.
        
        aRetVal := n * n.
        
        ^ LongNumber new &long:aRetVal.
    ]

    #method power : aPower
        = self::longOp power &long:(aPower long).
                
    #method(bool) is &odd
        = self::longOp mod:2l != 0.
        
    #method(bool) is &even
        = self::longOp mod:2l == 0.
        
    #method(bool) is &zero
        = self long equal &long:0l.
        
    #method(bool) is &positive
        = self long > 0l.
        
    #method(bool) is &negative
        = self long < 0l.
        
    #method(bool) is &nonnegative
        = self long <= 0l.

    #method(int) high
    [
        #var(long)aWord := self long.
        aWord := aWord && -4294967296l.
        aWord := aWord >> 32.
        
        ^ intConvertor convert &long:aWord.
    ]

    #method(int) low
    [
        #var(long)aWord := self long.
        aWord := aWord && 4294967295l.
        
        ^ intConvertor convert &long:aWord.
    ]
}

// --- realOp ---

#class(extension:real) realOp
{        
    #method(real) absolute
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    #method(real) exp
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController exp &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(real) ln
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController ln &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(real) sqrt
    [
        #var(real)n := self real.      
        #var(real)aRetVal.
        
        realOpController sqrt &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method (real)power &real:y
    [
        #var(real)x := self real.
        
        (x == 0.0r)
            ? [ ^ 0.0r. ].
                        
        (y == 0.0r)
            ? [ ^ 1.0r. ].
            
        realOpController ln &real:x &vreal:x.
        
        x := x multiply &real:y.
        
        realOpController exp &real:x &vreal:x.
            
        ^ RealNumber new &real:x.            
            
    ]

    #method(real)power &int:y
    [
        #var(real)x := self real.

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

    #method(real) sqr
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        aRetVal := n * n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real) rounded
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController round &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real) integer
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController truncate &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real) frac
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:(n - aRetVal).
    ]

    #method(real)sin
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController sin &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real)cos
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController cos &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real)tan
    [
        #var(real)n := self real.        
        #var(real)aSinVal.
        #var(real)aCosVal.
        
        realOpController sin &real:n &vreal:aSinVal.
        realOpController cos &real:n &vreal:aCosVal.
        
        ^ RealNumber new &real:(aSinVal / aCosVal).
    ]

    #method(real)arctan
    [
        #var(real)n := self real.        
        #var(real)aRetVal.
        
        realOpController arctan &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
            
    #method(real)arcsin
    [
        #var(real)n := self real.        
        #var(real)arg.
        #var(real)retVal.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt &real:retVal &vreal:arg.
        
        arg := n / arg.        
        realOpController arctan &real:arg &vreal:retVal.
        
        ^ RealNumber new &real:retVal.
    ]
            
    #method(real)arccos
    [
        #var(real)n := self real.        
        #var(real)arg.
        #var(real)retVal.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt &real:retVal &vreal:arg.
        
        arg := arg / n.        
        realOpController arctan &real:arg &vreal:retVal.
        
        ^ RealNumber new &real:retVal.
    ]
            
    #method(real) radian
    [
        #var(real)aValue := self real.
        
        aValue := aValue * pi_value.
        
        ^ RealNumber new &real:(aValue / 180.0r).
    ]
            
    #method(real) degree
    [
        #var(real)aValue := self real.
        
        aValue := aValue * 180.0r.
        
        ^ RealNumber new &real:(aValue / pi_value).
    ]
            
    #method(real) reciprocal
    [
        #var(real)aValue := self real.
        
        ^ RealNumber new &real:(1.0r / aValue).
    ]    
            
    #method(bool) is &zero
        = self real == 0.
        
    #method(bool) is &positive
        = self real > 0.
        
    #method(bool) is &negative
        = self real < 0.
        
    #method(bool) is &nonnegative
        = self real <= 0.
}

// --- arrayOp ---

#class(extension:intarray)intArrayOp
{
    #method(stacksafe) exchange &int:anIndex1 &int:anIndex2
    [
        #var(intarray)a := self intarray.

        #var(int)n1.
        #var(int)n2.

        a read &int:anIndex1 &vint:n1.
        a read &int:anIndex2 &vint:n2.
        
        a write &int:anIndex1 &int:n2.
        a write &int:anIndex2 &int:n1.
    ]

    #method set &every:anIndexFunc
        = self::intArrayOp set &nfunc:(anIndexFunc nfunc).
        
    #method set &nfunc:anIndexFunc
    [
        #var(intarray)anArray := self intarray.
        
        #var(int)anIndex := 0.
        #var(int)aLength := anArray length.
        
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
        => system'core_routines'__array.

    #method exchange : anIndex1 : anIndex2
        = self::arrayOp exchange &int:(anIndex1 int) &int:(anIndex2 int).
        
    #method exchange &int:anIndex1 &int:anIndex2
    [
        #var(array) anArray := self array.

        #var aTemp := anArray getAt &int:anIndex1.
        
        anArray setAt &int:anIndex1 &object:(anArray getAt &int:anIndex2).
        anArray setAt &int:anIndex2 &object:aTemp.
    ]
    
    #method(stacksafe,suppress:w2) indexOf &int:anIndex &object:anObject &vint:aRetVal
    [
        aRetVal << -1.
        
        #var(array)anArray := self array.
        #var(int)aLength := anArray length.
        
        #var(int)i := anIndex.
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
        #var(int)aRetVal.
        
        self::arrayOp indexOf &int:anIndex &object:anObject &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]
    
    #method indexOf : anObject
        = self::arrayOp indexOf &int:0 &object:anObject.
        
    #method(suppress:w2) safeSeek : aValue
    [
        #var(array)anArray := self array.
        #var(int)aLength := anArray length.
        
        #var(int)i := 0.
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
        #var(array)anArray := self array.
        #var(int)aLength := anArray length.
        
        #var(array)aCopy := Array new &int:aLength.
                
        #var(int)anIndex := 0.
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
        #var(array)anArray := self array.
        
        #var(int)anIndex := 0.
        #var(int)aLength := anArray length.
        
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
        #var(array)anArray := self array.
        
        #var(int)i := 0.
        #var(int)j := 0.
        #var(int)aLength := anArray length.
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
    
    #method(bool) is &empty
    [
        #var(array)anArray := self array.
        #var(int)aLength := anArray length.
        
        ^ aLength == 0.
    ]
    
    #method Subarray : aLength &at:anIndex
        = self::arrayOp Subarray &int:(anIndex int) &int:(aLength int).
    
    /// obsolete
    #method Subarray &index:anIndex &length:aLength
        = self::arrayOp Subarray &int:(anIndex int) &int:(aLength int).
    
    #method Subarray &int:anIndex &int:aLength
    [
        #var(iterator) it := self iterator.
        
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
        
    #method(literal) Substring &at:anIndex
        = self::literalOp Substring &int:(anIndex int).
        
    #method(literal,stacksafe) Substring &int:anIndex
    [
        #var(literal)aLiteral := self literal.        
        #var(int)aLength := aLiteral length.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &int:anIndex &int:aLength.
    ]        
    
    #method trimLeft &char:aChar
    [
        #var(literal)aLiteral := self literal.        
        #var(int)anIndex := 0.
        #var(int)aLength := aLiteral length.
        #var(char)aCurrent.
        #var(int)aCharLen := aChar length.
        
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
        #var(literal)aLiteral := self literal.        
        #var(int)aLength := aLiteral length.
        #var(int)anIndex := aLength - 1.
        #var(int)aPrevious := aLength.
        #var(char)aCurrent.
        #var(int)aCharLen := aChar length.
        
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
    
//    #method padLeft &char:aChar &int:aLength
//    [
//        #var(literal)aLiteral := self literal.
//        #var(int)aCurrentLength := aLiteral length.
//        
//        (aCurrentLength < aLength)
//            ? [ ^ LiteralValue new &int:(aLength - aCurrentLength) &char:aChar add &literal:aLiteral. ].
//            
//        ^ self.            
//    ]
//    
//    #method padRight &char:aChar &int:aLength
//    [
//        #var(literal)aLiteral := self literal.        
//        #var(int)aCurrentLength := aLiteral length.
//
//        (aCurrentLength < aLength)
//            ? [ ^ aLiteral add &literal:(LiteralValue new &int:(aLength - aCurrentLength) &char:aChar). ].
//            
//        ^ self.            
//    ]

    #method startingWith:s
        = self::literalOp startingWith &literal:(s literal).
        
    #method startingWith &literal:s
    [
        #var(literal)aLiteral := self literal.        
        #var(int)n.
        aLiteral indexOf &int:0 &literal:s &vint:n.
        
        ^ 0 == n.
    ]
        
    #method endingWith &literal:s
    [
        #var(literal)aLiteral := self literal.
        #var(int)l1 := aLiteral length.
        #var(int)l2 := s length.

        #var(int)n.
        aLiteral indexOf &int:(l1 - l2) &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method endingWith:s
        = self::literalOp endingWith &literal:(s literal).
        
    #method containing &literal:s
    [
        #var(literal)aLiteral := self literal.
        #var(int)l1 := aLiteral length.
        #var(int)l2 := s length.

        #var(int)n.
        aLiteral indexOf &int:0 &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method containing:s
        = self::literalOp containing &literal:(s literal).
    
    #method replaceFirst &literal:aReplacee &literal:aReplacer
    [
        #var(literal)aLiteral := self literal.
        #var(int)aReplaceeLen := aReplacee length.
        #var(int)anIndex.
        
        aLiteral indexOf &int:0 &literal:aReplacee &vint:anIndex.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &int:anIndex &int:aReplaceeLen insert &int:anIndex &literal:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]    
    
//    #method replace &literal:aReplacee &literal:aReplacer
//    [
//        #var(literal)aLiteral := self literal.        
//        #var aTarget := String new.
//        #var(int)aStart := 0.
//        #var(int)anEnd.
//        #var(int)aReplaceeLen := aReplacee length.
//        
//        aLiteral indexOf &int:aStart &literal:aReplacee &vint:anEnd.
//        
//        #loop (anEnd != -1) ?
//        [
//            aTarget 
//                append &literal:(aLiteral Substring &int:aStart &int:(anEnd - aStart))
//                append &literal:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            aLiteral indexOf &int:aStart &literal:aReplacee &vint:anEnd.
//        ].
//    
//        ^ (aStart != 0)
//            ? [ aTarget append:(aLiteral::literalOp Substring &int:aStart) literal ]
//            ! [ self literal ].
//    ]
    
    #method(bool) is &empty
    [
        #var(literal)aLiteral := self literal.        
        #var(int)aLength := aLiteral length.
        
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
                
    #method(wide) Substring &at:anIndex
        = self::wideOp Substring &int:(anIndex int).
        
    #method(wide,stacksafe) Substring &int:anIndex
    [
        #var(wide)aLiteral := self wide.        
        #var(int)aLength := aLiteral length.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &int:anIndex &int:aLength.
    ]
     
    #method(suppress:w2) trimLeft &char:aChar
    [
        #var(wide)aLiteral := self wide.
        #var(int)anIndex := 0.
        #var(int)aLength := aLiteral length.
        #var(char)aCurrent.
        #var(int)aCharLen := aChar length.
        
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
        #var(wide)aLiteral := self wide.        
        #var(int)aLength := aLiteral length.
        #var(char)aCurrent.
        #var(int)aCharLen := aChar length.
                
        #var(int)anIndex := aLength - 1.
        #var(int)aPrevious := aLength.
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
    
//    #method padLeft &char:aChar &int:aLength
//    [
//        #var(wide)aLiteral := self wide.
//        #var(int)aCurrentLength := aLiteral length.
//        
//        (aCurrentLength < aLength)
//            ? [ ^ WideLiteralValue new &int:(aLength - aCurrentLength) &char:aChar add &wide:aLiteral. ].
//            
//        ^ self.            
//    ]
//    
//    #method padRight &char:aChar &int:aLength
//    [
//        #var(wide)aLiteral := self wide.        
//        #var(int)aCurrentLength := aLiteral length.
//
//        (aCurrentLength < aLength)
//            ? [ ^ aLiteral add &wide:(WideLiteralValue new &int:(aLength - aCurrentLength) &char:aChar). ].
//            
//        ^ self.            
//    ]

    #method startingWith &wide:s
    [
        #var(wide)aLiteral := self wide.        
        #var(int)n.
        aLiteral indexOf &int:0 &wide:s &vint:n.
        
        ^ 0 == n.
    ]
    
    #method startingWith:s
        = self::wideOp startingWith &wide:(s wide).

    #method endingWith &wide:s
    [
        #var(wide)aLiteral := self wide.
        #var(int)l1 := aLiteral length.
        #var(int)l2 := s length.
        #var(int)n.        
        aLiteral indexOf &int:(l1 - l2) &wide:s &vint:n.
        
        ^ n != -1.
    ]
    
    #method endingWith:s
        = self::wideOp endingWith &wide:(s wide).

    #method containing &wide:s
    [
        #var(wide)aLiteral := self wide.
        #var(int)l1 := aLiteral length.
        #var(int)l2 := s length.
        #var(int)n.
        
        aLiteral indexOf &int:0 &wide:s &vint:n.
        
        ^ n != -1.
    ]
    
    #method containing:s
        = self::wideOp containing &wide:(s wide).

    #method replaceFirst &wide:aReplacee &wide:aReplacer
    [
        #var(wide)aLiteral := self wide.
        #var(int) aReplaceeLen := aReplacee length.
        #var(int)anIndex.
        
        aLiteral indexOf &int:0 &wide:aReplacee &vint:anIndex.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &int:anIndex &int:aReplaceeLen insert &int:anIndex &wide:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]
        
//    #method(suppress:w2) replace &wide:aReplacee &wide:aReplacer
//    [
//        #var(wide)aLiteral := self wide.
//        
//        #var aTarget := String new.
//        #var(int)aStart := 0.
//        #var(int)anEnd.
//        #var(int)aReplaceeLen := aReplacee length.
//        
//        aLiteral indexOf &int:aStart &wide:aReplacee &vint:anEnd.
//        
//        #loop (anEnd != -1) ?
//        [
//            aTarget 
//                append &wide::(aLiteral Substring &int:aStart &int:(anEnd - aStart))
//                append &wide:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            aLiteral indexOf &int:aStart &wide:aReplacee &vint:anEnd.
//        ].
//    
//        ^ (aStart != 0)
//            ? [ aTarget append:(aLiteral::wideOp Substring &int:aStart) wide ]
//            ! [ aLiteral ].
//    ]

    #method(bool) is &empty
    [
        #var(wide)aLiteral := self wide.        
        #var(int)aLength := aLiteral length.
        
        ^ aLength == 0.
    ]
}

//// --- stringOp ---
//
//#symbol stringOpDispatcher =
//{
//    eval &literal:aLiteral = literalOp.
//    
//    eval &wide:aLiteral = wideOp.
//}.
//
//#class(extension) stringOp
//{
//    #method Substring : anIndex
//        = self Substring:(self length - anIndex) &at:(anIndex int).
//            
//    #method Substring : anIndex : aLength
//        = self Substring:aLength &at:anIndex.
//            
//    #method trimLeft &char:aChar
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher trimLeft &char:aChar.
//    ]
//
//    #method trimRight &char:aChar
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher trimRight &char:aChar.
//    ]
//
//    #method trim &char:aChar
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher trim &char:aChar.
//    ]
//
//    #method trim
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher trim.
//    ]
//
//    #method trimLeft
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher trimLeft.
//    ]
//
//    #method trimRight
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher trimRight.
//    ]
//    
//    #method padLeft &char:aChar &length:aLength
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher padLeft &char:aChar &int:(aLength int).
//    ]
//    
//    #method padRight &char:aChar &length:aLength
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher padRight &char:aChar &int:(aLength int).
//    ]
//    
//    #method startingWith : s
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher startingWith:s.
//    ]
//    
//    #method endingWith : s
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher endingWith:s.
//    ]
//    
//    #method containing : s
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher containing:s.
//    ]
//    
//    #method replaceFirst &literal:aReplacee &literal:aReplacer
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher replaceFirst &literal:aReplacee &literal:aReplacer.
//    ]
//    
//    #method replace &literal:aReplacee &literal:aReplacer
//    [
//        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
//        
//        ^ self::dispatcher replace &literal:aReplacee &literal:aReplacer.
//    ]
//    
//    #method write : aLiteral &length:aCounter
//        = self::stringOp write : aLiteral &int:(aCounter int).
//            
//    #method(stacksafe) write : aLiteral &int:aCounter
//    [
//        #var(type:int)aNumber := aCounter.
//        
//        #loop (aNumber > 0) ?
//        [
//            self write:aLiteral. 
//            
//            aNumber := aNumber - 1.
//        ].
//            
//        ^ self.
//    ]
//    
//    #method write : aLine &paddingLeft:aWidth &with:aChar
//    [
//        #var aPaddedStr := aLine padLeft &char:(aChar char) &length:(aWidth int).
//
//        self write:aPaddedStr.
//            
//        ^ self.
//    ]
//
//    #method write : aLine &paddingRight:aWidth &with:aChar
//    [
//        #var aPaddedStr := aLine padRight &char:(aChar char) &length:(aWidth int).
//
//        self write:aPaddedStr.
//            
//        ^ self.
//    ]
//    
//    #method write : aLine &padding:aWidth &with:aChar
//    [
//        #var(type:int)aWidthInt := aWidth int.
//        #var(type:int)aLength := aLine length.        
//        
//        (aLength < aWidthInt)
//            ? [
//                #var(type:int)aLen := aWidthInt - aLength.
//                #var(type:int)aLeftLen := aLen / 2.
//                
//                self::stringOp write:aChar &int:aLeftLen.
//                self write:aLine.
//                self::stringOp write:aChar &int:(aLen - aLeftLen).
//            ]
//            ! [ self write:aLine. ].
//            
//        ^ self.
//    ]    
//}

// --- charOp ---

#class(extension:char)charOp
{
    #method(bool)is &whitespace
    [
        #var(int)aCode := intConvertor convert &char:self.
        #var(bool)ret := false.
        
        aCode =>
            0 ? [ ret := true. ]
            9 ? [ ret := true. ]
            10 ? [ ret := true. ]
            11 ? [ ret := true. ]
            12 ? [ ret := true. ]
            13 ? [ ret := true. ]
            32 ? [ ret := true. ].
        
        ^ ret.
    ]
    
    #method(bool)is &digit
    [
        #var(int) ch := intConvertor convert &char:(self char).
        
        ^ (ch >= 30h) and: (ch <= 39h).
    ]
    
    #method(bool)is &newLine
    [
        ^ self == #10.
    ]
    
    #method(bool)is &letter
    [
        #var(int) ch := intConvertor convert &char:(self char).
        
        ((ch >= 41h) and: (ch <= 5Ah))
            ? [ ^ true. ].
        
        ((ch >= 61h) and: (ch <= 7Ah))
            ? [ ^ true. ].
        
        ((ch >= 3ACh) and: (ch <= 3CEh))
            ? [ ^ true. ].
            
        ((ch >= 400h) and: (ch <= 42Fh))
            ? [ ^ true. ].
            
        ^ false.            
    ]
}