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

//    #method evalSelf &func1:aFunction
//        = aFunction eval:self.
//
//    #method evalSelf:aFunction
//        = aFunction eval:self.
//
//    /// Returns true if the object is nil
//    #method(bool) is &nil
//        = nil == self.
//        
//    /// Returns true if the object is literal or character
//    #method is &literal
//        = self cast:%eval &to:{ literal : anObject = true. wide : anObject = true. char : anObject = true. ! : anObject = false. }.
//        
//    /// Returns true if the object is integer numereric
//    #method is &integer
//        = self cast:%eval &to:{ int : x = true. long : x = true. short : x = true. byte : x = true. ! : anObject = false. }.
//        
//    /// Returns true if the object is numereric
//    #method is &numeric
//        = self cast:%eval &to:{ int : x = true. long : x = true. real : x = true. short : x = true. byte : x = true. ! : anObject = false. }.
        
    #method(bool,w2) is &empty
        = self length == 0.
        
    #method(bool) is &nonempty
        = self length != 0.
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    #method(bool) safeEqual : anObject
        = anObject cast:%eval &to:{ ! : aTarget : anObject = aTarget::$subject equal:anObject \ back:false. } &with:self bool.

//    #method eval &with:action
//        = action eval:self.
//        
//    #method(stacksafe) reallocate &int:aNewLength
//        => system'core_routines'__array.
}

// --- Operations ---

// --- byteOp ---

#class(extension:byte) byteOp
{
    #method(byte,stacksafe) absolute
    [
        #var(byte)n := self.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ByteNumber new &byte:n.
    ]
    
    #method(byte,w2) mod : anOperand
    [
        #var retVal := self / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:byteConvertor.
    ]    
    
    #method(byte,stacksafe)power &byte:y
    [
        #var(byte)x := self.
        
        #var(byte)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ByteNumber new &byte:x.
    ]
    
    #method(byte,stacksafe) sqr
    [
        #var(byte)aRetVal.
        
        aRetVal := self * self.
        
        ^ ByteNumber new &byte:aRetVal.
    ]

    #method power : aPower
        = self::byteOp power &byte:(aPower byte).
                
    #method(bool) is &odd
        = self::byteOp mod:2 != 0.
        
    #method(bool) is &even
        = self::byteOp mod:2 == 0.
        
    #method(bool,stacksafe) is &zero
        = self equal &int:0.
        
    #method(bool,stacksafe) is &positive
        = self > 0.
        
    #method(bool,stacksafe) is &negative
        = self < 0.
        
    #method(bool,stacksafe) is &nonnegative
        = self <= 0.
}

// --- shortOp ---

#class(extension:short) shortOp
{
    #method(short,stacksafe) absolute
    [
        #var(short)n := self.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ShortNumber new &short:n.
    ]
    
    #method(short,w2) mod : anOperand
    [
        #var retVal := self / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:shortConvertor.
    ]
    
    #method(short,stacksafe)power &short:y
    [
        #var(short)x := self.
        
        #var(short)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ShortNumber new &short:x.
    ]
    
    #method(short,stacksafe) sqr
    [
        #var(short)n := self.
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
        
    #method(bool,stacksafe) is &zero
        = self equal &int:0.
        
    #method(bool,stacksafe) is &positive
        = self > 0.
        
    #method(bool,stacksafe) is &negative
        = self < 0.
        
    #method(bool,stacksafe) is &nonnegative
        = self <= 0.
    
    #method(byte,stacksafe) high
    [
        #var(short)aWord := self.
        aWord := aWord && 0FF00h.
        aWord := aWord shift &int:8.
        
        ^ byteConvertor convert &short:aWord.
    ]
        
    #method(byte,stacksafe) low
    [
        #var(short)aWord := self.
        aWord := aWord && 0FFh.
        
        ^ byteConvertor convert &short:aWord.
    ]
}

// --- intOp ---

#class(extension:int) intOp
{
    #method(int,stacksafe) absolute
    [
        #var(int)n := self.
        
        (n < 0)
            ? [ n := n * -1. ].
        
        ^ IntNumber new &int:n.
    ]
    
    #method(int) mod : anOperand
    [
        #var(int)x := self.
        #var(int)y := anOperand int.
        
        #var(int)z := x / y.
        z := z * y.
        
        ^ IntNumber new &int:(x - z).
    ]
    
    #method(int,stacksafe)power &int:y
    [
        #var(int)x := self.
        #var(int)r := 1.
        
        #var(int)i := 0.
        #loop (i < y)?
        [
            r := r * x.
            
            i += 1.
        ].
        
        ^ IntNumber new &int:r.
    ]
    
    #method(int,stacksafe) sqr
    [
        #var(int)n := self.        
        #var(int)aRetVal.
        
        aRetVal := n * n.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(int,stacksafe) sqrt
    [
        #var(int)num := self.
        
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
                
    #method(bool,stacksafe) is &odd
        = self and &int:1 != 0.
        
    #method(bool,stacksafe) is &even
        = self and &int:1 == 0.
        
    #method(bool,stacksafe) is &zero
        = self equal &int:0.
        
    #method(bool,stacksafe) is &positive
        = self > 0.
        
    #method(bool,stacksafe) is &negative
        = self < 0.
        
    #method(bool,stacksafe) is &nonnegative
        = self <= 0.

    #method high
    [
        #var(int)aWord := self.
        aWord := aWord && 0FFFF0000h.
        aWord := aWord >> 16.
        
        ^ shortConvertor convert &int:aWord.
    ]

    #method low
    [
        #var(int)aWord := self.
        aWord := aWord && 0FFFFh.
        
        ^ shortConvertor convert &int:aWord.
    ]
}

// --- longOp ---

#class(extension:long) longOp
{
    #method(long,stacksafe) absolute
    [
        #var(long)n := self.

        (n < 0l)
            ? [ n := n * -1l. ].
                
        ^ LongNumber new &long:n.
    ]
    
    #method(long) mod : anOperand
    [
        #var(long)x := self.
        #var(long)y := anOperand long.
        
        #var(long)z := x / y.
        z := z * y.
        
        ^ LongNumber new &long:(x - z).
    ]
    
    #method(long,stacksafe)power &long:y
    [
        #var(long)x := self.
        
        #var(long)i := 0l.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ LongNumber new &long:x.
    ]
    
    #method(long,stacksafe) sqr
    [
        #var(long)n := self.        
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
        
    #method(bool,stacksafe) is &zero
        = self equal &long:0l.
        
    #method(bool,stacksafe) is &positive
        = self > 0l.
        
    #method(bool,stacksafe) is &negative
        = self < 0l.
        
    #method(bool,stacksafe) is &nonnegative
        = self <= 0l.

    #method(int,stacksafe) high
    [
        #var(long)aWord := self.
        aWord := aWord && -4294967296l.
        aWord := aWord >> 32.
        
        ^ intConvertor convert &long:aWord.
    ]

    #method(int,stacksafe) low
    [
        #var(long)aWord := self.
        aWord := aWord && 4294967295l.
        
        ^ intConvertor convert &long:aWord.
    ]
}

// --- realOp ---

#class(extension:real) realOp
{        
    #method(real,stacksafe) absolute
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    #method(real,stacksafe) exp
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController exp &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(real,stacksafe) ln
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController ln &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(real,stacksafe) sqrt
    [
        #var(real)n := self.      
        #var(real)aRetVal.
        
        realOpController sqrt &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method (real,stacksafe)power &real:y
    [
        #var(real)x := self.
        
        (x == 0.0r)
            ? [ ^ 0.0r. ].
                        
        (y == 0.0r)
            ? [ ^ 1.0r. ].
            
        realOpController ln &real:x &vreal:x.
        
        x := x multiply &real:y.
        
        realOpController exp &real:x &vreal:x.
            
        ^ RealNumber new &real:x.            
            
    ]

    #method(real,stacksafe)power &int:y
    [
        #var(real)x := self.

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

    #method(real,stacksafe) sqr
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        aRetVal := n * n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real,stacksafe) rounded
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController round &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real,stacksafe) integer
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController truncate &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real,stacksafe) frac
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:(n - aRetVal).
    ]

    #method(real,stacksafe)sin
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController sin &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real,stacksafe)cos
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController cos &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(real,stacksafe)tan
    [
        #var(real)n := self.        
        #var(real)aSinVal.
        #var(real)aCosVal.
        
        realOpController sin &real:n &vreal:aSinVal.
        realOpController cos &real:n &vreal:aCosVal.
        
        ^ RealNumber new &real:(aSinVal / aCosVal).
    ]

    #method(real,stacksafe)arctan
    [
        #var(real)n := self.        
        #var(real)aRetVal.
        
        realOpController arctan &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
            
    #method(real,stacksafe)arcsin
    [
        #var(real)n := self.        
        #var(real)arg.
        #var(real)retVal.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt &real:retVal &vreal:arg.
        
        arg := n / arg.        
        realOpController arctan &real:arg &vreal:retVal.
        
        ^ RealNumber new &real:retVal.
    ]
            
    #method(real,stacksafe)arccos
    [
        #var(real)n := self.        
        #var(real)arg.
        #var(real)retVal.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt &real:retVal &vreal:arg.
        
        arg := arg / n.        
        realOpController arctan &real:arg &vreal:retVal.
        
        ^ RealNumber new &real:retVal.
    ]
            
    #method(real,stacksafe) radian
    [
        #var(real)aValue := self.
        
        aValue := aValue * pi_value.
        
        ^ RealNumber new &real:(aValue / 180.0r).
    ]
            
    #method(real,stacksafe) degree
    [
        #var(real)aValue := self.
        
        aValue := aValue * 180.0r.
        
        ^ RealNumber new &real:(aValue / pi_value).
    ]
            
    #method(real,stacksafe) reciprocal
    [
        #var(real)aValue := self.
        
        ^ RealNumber new &real:(1.0r / aValue).
    ]    
            
    #method(bool,stacksafe) is &zero
        = self == 0.0r.
        
    #method(bool,stacksafe) is &positive
        = self > 0.0r.
        
    #method(bool,stacksafe) is &negative
        = self < 0.0r.
        
    #method(bool,stacksafe) is &nonnegative
        = self <= 0.0r.
}

// --- arrayOp ---

#class(extension:intarray)intArrayOp
{
    #method(stacksafe) exchange &int:anIndex1 &int:anIndex2
    [
        #var(int)n1.
        #var(int)n2.

        self read &int:anIndex1 &vint:n1.
        self read &int:anIndex2 &vint:n2.
        
        self write &int:anIndex1 &int:n2.
        self write &int:anIndex2 &int:n1.
    ]

//    #method set &every:anIndexFunc
//        = self::intArrayOp set &nfunc:(anIndexFunc nfunc).
//        
//    #method set &nfunc:anIndexFunc
//    [
//        #var(int)anIndex := 0.
//        #var(int)aLength := self length.
//        
//        #loop (anIndex < aLength) ?
//        [            
//            self write &int:anIndex &int:(anIndexFunc eval &int:anIndex int).
//            
//            anIndex += 1.
//        ].
//        
//        ^ self.
//    ]
}

// --- arrayOp ---

#class(extension:array)arrayOp
{
    #method exchange : anIndex1 : anIndex2
        = self::arrayOp exchange &int:(anIndex1 int) &int:(anIndex2 int).
        
    #method exchange &int:anIndex1 &int:anIndex2
    [
        #var aTemp := self getAt &int:anIndex1.
        
        self setAt &int:anIndex1 &object:(self getAt &int:anIndex2).
        self setAt &int:anIndex2 &object:aTemp.
    ]
    
//    #method(stacksafe) indexOf &int:anIndex &object:anObject &vint:aRetVal
//    [
//        aRetVal := -1.
//        
//        #var(int)aLength := self length.
//        
//        #var(int)i := anIndex.
//        #loop (i < aLength)?
//        [
//            #var anArrayItem := self getAt &int:i.
//            
//            (anObject == anArrayItem)
//                ? [ aRetVal := i. ].
//            
//            i += 1.
//        ].
//    ]        
//        
//    #method(stacksafe) indexOf &int:anIndex &object:anObject
//    [
//        #var(int)aRetVal.
//        
//        self::arrayOp indexOf &int:anIndex &object:anObject &vint:aRetVal.
//        
//        ^ IntNumber new &int:aRetVal.
//    ]
//    
//    #method indexOf : anObject
//        = self::arrayOp indexOf &int:0 &object:anObject.
//        
//    #method(w2) safeSeek : aValue
//    [
//        #var(int)aLength := self length.
//        
//        #var(int)i := 0.
//        #loop (i < aLength)?
//        [
//            #var anArrayItem := self getAt &int:i.
//            
//            (aValue safeEqual:anArrayItem)
//                ? [ ^ anArrayItem. ].
//            
//            i += 1.
//        ].
//        
//        ^ $nil.
//    ]
//        
//    #method deepClone
//    [
//        #var(int)aLength := self length.
//        
//        #var(array)aCopy := Array new &int:aLength.
//                
//        #var(int)anIndex := 0.
//        #loop (anIndex < aLength) ?
//        [
//            aCopy setAt &int:anIndex &object:(self getAt &int:anIndex clone).
//            
//            anIndex += 1.
//        ].
//        
//        ^ self.
//    ]
//    
//    #method set &every:anIndexFunc
//        = self::arrayOp set &nfunc:(anIndexFunc nfunc).
//    
//    #method set &nfunc:anIndexFunc
//    [
//        #var(int)anIndex := 0.
//        #var(int)aLength := self length.
//        
//        #loop (anIndex < aLength) ?
//        [            
//            self setAt &int:anIndex &object:(anIndexFunc eval &int:anIndex).
//            
//            anIndex += 1.
//        ].
//        
//        ^ self.
//    ]
//    
//    #method sort : aCompareFunc
//        = self::arrayOp sort &func2:(aCompareFunc func2).
//    
//    #method(w2) sort &func2:aCompareFunc
//    [
//        #var(int)i := 0.
//        #var(int)j := 0.
//        #var(int)aLength := self length.
//        #var aCurrent := $nil.
//        #var aNext := $nil.
//        
//        #loop (i < aLength) ?
//        [
//            aCurrent := self getAt &int:i.
//            
//            j := i + 1.
//            #loop (j < aLength) ?
//            [
//                aNext := self getAt &int:j.
//                (aCompareFunc eval:aCurrent:aNext)
//                    ! [                        
//                        self setAt &int:i &object:aNext.
//                        self setAt &int:j &object:aCurrent.
//                        
//                        aCurrent := aNext.
//                    ].
//                    
//                j += 1.
//            ].
//            
//            i += 1.
//        ].
//        
//        ^ self.
//    ]
//    
//    #method ascendant
//        = (self clone)::arrayOp sort:ifOrdered.
//    
//    #method(bool) is &empty
//    [
//        #var(int)aLength := self length.
//        
//        ^ aLength == 0.
//    ]
//    
//    #method Subarray : aLength &at:anIndex
//        = self::arrayOp Subarray &int:(anIndex int) &int:(aLength int).
//    
//    /// obsolete
//    #method Subarray &index:anIndex &length:aLength
//        = self::arrayOp Subarray &int:(anIndex int) &int:(aLength int).
//    
//    #method Subarray &int:anIndex &int:aLength
//    [
//        #var(iterator) it := self iterator.
//        
//        ^ Array new &int:aLength set &nfunc: (&int:i) [ it writeIndex &int:(anIndex + i) get ].
//    ]
}

//// --- literalOp ---
//
//#class(extension:literal)literalOp
//{
//    #method indexOf : anObject
//        = self indexOf:anObject &at:0.
//
//    #method indexOf &literal:aLiteral
//        = self indexOf &literal:aLiteral &at:0.
//
//    #method indexOf &char:aChar
//        = self indexOf &char:aChar &at:0.
//        
//    #method(literal) Substring &at:anIndex
//        = self::literalOp Substring &int:(anIndex int).
//        
//    #method(literal,stacksafe) Substring &int:anIndex
//    [
//        #var(int)aLength := self length.
//        
//        aLength -= anIndex.
//        
//        ^ self Substring &int:anIndex &int:aLength.
//    ]        
//    
//    #method trimLeft &char:aChar
//    [
//        #var(int)anIndex := 0.
//        #var(int)aLength := self length.
//        #var(char)aCurrent.
//        #var(int)aCharLen := aChar length.
//        
//        #loop (anIndex < aLength) ?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent == aChar)
//                ! [ 
//                    (anIndex == 0)
//                        ? [ ^ self. ]
//                        ! [^ self delete &int:0 &int:anIndex. ].
//                ].
//            
//            anIndex += aCharLen.
//        ].
//        
//        ^ emptyLiteralValue.
//    ]
//    
//    #method trimLeft
//        = self::literalOp trimLeft &char:#32.    
//       
//    #method(w2) trimRight &char:aChar
//    [
//        #var(int)aLength := self length.
//        #var(int)anIndex := aLength - 1.
//        #var(int)aPrevious := aLength.
//        #var(char)aCurrent.
//        #var(int)aCharLen := aChar length.
//        
//        #loop (anIndex >= 0) ?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent equal &char:aChar)
//                ! [ ^ self delete &int:aPrevious &int:(aLength - aPrevious). ].
//            
//            aPrevious := anIndex.
//            anIndex -= aCharLen.
//        ].
//        
//        ^ emptyLiteralValue.
//    ]
//    
//    #method trimRight
//        = self::literalOp trimRight &char:#32.
//        
//    #method trim &char:aChar
//        = self::literalOp trimLeft &char:aChar trimRight &char:aChar.
//    
//    #method trim
//        = self::literalOp trimLeft &char:#32 trimRight &char:#32.        
//    
//    #method padLeft &char:aChar &int:aLength
//    [
//        #var(int)aCurrentLength := self length.
//        
//        (aCurrentLength < aLength)
//            ? [ ^ LiteralValue new &int:(aLength - aCurrentLength) &char:aChar add &literal:self. ].
//            
//        ^ self.            
//    ]
//    
//    #method padRight &char:aChar &int:aLength
//    [
//        #var(int)aCurrentLength := self length.
//
//        (aCurrentLength < aLength)
//            ? [ ^ self add &literal:(LiteralValue new &int:(aLength - aCurrentLength) &char:aChar). ].
//            
//        ^ self.            
//    ]
//
//    #method startingWith:s
//        = self::literalOp startingWith &literal:(s literal).
//        
//    #method startingWith &literal:s
//    [
//        #var(int)n.
//        self indexOf &int:0 &literal:s &vint:n.
//        
//        ^ 0 == n.
//    ]
//        
//    #method endingWith &literal:s
//    [
//        #var(int)l1 := self length.
//        #var(int)l2 := s length.
//
//        #var(int)n.
//        self indexOf &int:(l1 - l2) &literal:s &vint:n.
//        
//        ^ n != -1.
//    ]
//
//    #method endingWith:s
//        = self::literalOp endingWith &literal:(s literal).
//        
//    #method containing &literal:s
//    [
//        #var(int)l1 := self length.
//        #var(int)l2 := s length.
//
//        #var(int)n.
//        self indexOf &int:0 &literal:s &vint:n.
//        
//        ^ n != -1.
//    ]
//
//    #method containing:s
//        = self::literalOp containing &literal:(s literal).
//    
//    #method replaceFirst &literal:aReplacee &literal:aReplacer
//    [
//        #var(int)aReplaceeLen := aReplacee length.
//        #var(int)anIndex.
//        
//        self indexOf &int:0 &literal:aReplacee &vint:anIndex.
//        
//        (anIndex != -1)
//            ? [ ^ self delete &int:anIndex &int:aReplaceeLen insert &int:anIndex &literal:aReplacee. ]
//            ! [ ^ self. ].
//    ]    
//    
//    #method replace &literal:aReplacee &literal:aReplacer
//    [
//        #var(literal) aTarget := emptyLiteralValue.
//        #var(int)aStart := 0.
//        #var(int)anEnd.
//        #var(int)aReplaceeLen := aReplacee length.
//        
//        self indexOf &int:aStart &literal:aReplacee &vint:anEnd.
//        
//        #loop (anEnd != -1) ?
//        [
//            aTarget := aTarget
//                add &literal:(self Substring &int:aStart &int:(anEnd - aStart))
//                add &literal:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            self indexOf &int:aStart &literal:aReplacee &vint:anEnd.
//        ].
//    
//        ^ (aStart != 0)
//            ? [ aTarget add &literal:(self::literalOp Substring &int:aStart) ]
//            ! [ self ].
//    ]
//    
//    #method(bool) is &empty
//    [
//        #var(int)aLength := self length.
//        
//        ^ aLength == 0.
//    ]    
//}
//
//// --- wideOp ---
//
//#class(extension:wide)wideOp
//{
//    #method indexOf : anObject
//        = self indexOf:anObject &at:0.
//        
//    #method indexOf &wide:aLiteral
//        = self indexOf &int:0 &wide:aLiteral.
//
//    #method indexOf &char:aChar
//        = self indexOf &int:0 &char:aChar.
//                
//    #method(wide) Substring &at:anIndex
//        = self::wideOp Substring &int:(anIndex int).
//        
//    #method(wide,stacksafe) Substring &int:anIndex
//    [
//        #var(int)aLength := self length.
//        
//        aLength -= anIndex.
//        
//        ^ self Substring &int:anIndex &int:aLength.
//    ]
//     
//    #method trimLeft &char:aChar
//    [
//        #var(int)anIndex := 0.
//        #var(int)aLength := self length.
//        #var(char)aCurrent.
//        #var(int)aCharLen := aChar length.
//        
//        #loop (anIndex < aLength) ?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent equal &char:aChar)
//                ! [ ^ self delete &int:0 &int:anIndex. ].
//            
//            anIndex += aCharLen.
//        ].
//        
//        ^ emptyWideLiteralValue.
//    ]
//    
//    #method trimLeft
//        = self::wideOp trimLeft &char:#32.
//       
//    #method trimRight &char:aChar
//    [
//        #var(int)aLength := self length.
//        #var(char)aCurrent.
//        #var(int)aCharLen := aChar length.
//                
//        #var(int)anIndex := aLength - 1.
//        #var(int)aPrevious := aLength.
//        #loop (anIndex >= 0) ?
//        [
//            self read &int:anIndex &vchar:aCurrent.
//            
//            (aCurrent equal &char:aChar)
//                ! [ ^ self delete &int:aPrevious &int:(aLength - aPrevious). ].
//            
//            aPrevious := anIndex.
//            anIndex -= aCharLen.
//        ].
//        
//        ^ emptyWideLiteralValue.
//    ]
//    
//    #method trimRight
//        = self::wideOp trimRight &char:#32.
//       
//    #method trim &char:aChar
//        = self::wideOp trimLeft &char:aChar trimRight &char:aChar.
//    
//    #method trim
//        = self::wideOp trimLeft &char:#32 trimRight &char:#32.
//    
//    #method padLeft &char:aChar &int:aLength
//    [
//        #var(int)aCurrentLength := self length.
//        
//        (aCurrentLength < aLength)
//            ? [ ^ WideLiteralValue new &int:(aLength - aCurrentLength) &char:aChar add &wide:self. ].
//            
//        ^ self.            
//    ]
//    
//    #method padRight &char:aChar &int:aLength
//    [
//        #var(int)aCurrentLength := self length.
//
//        (aCurrentLength < aLength)
//            ? [ ^ self add &wide:(WideLiteralValue new &int:(aLength - aCurrentLength) &char:aChar). ].
//            
//        ^ self.            
//    ]
//
//    #method startingWith &wide:s
//    [
//        #var(int)n.
//        self indexOf &int:0 &wide:s &vint:n.
//        
//        ^ 0 == n.
//    ]
//    
//    #method startingWith:s
//        = self::wideOp startingWith &wide:(s wide).
//
//    #method endingWith &wide:s
//    [
//        #var(int)l1 := self length.
//        #var(int)l2 := s length.
//        #var(int)n.        
//        self indexOf &int:(l1 - l2) &wide:s &vint:n.
//        
//        ^ n != -1.
//    ]
//    
//    #method endingWith:s
//        = self::wideOp endingWith &wide:(s wide).
//
//    #method containing &wide:s
//    [
//        #var(int)l1 := self length.
//        #var(int)l2 := s length.
//        #var(int)n.
//        
//        self indexOf &int:0 &wide:s &vint:n.
//        
//        ^ n != -1.
//    ]
//    
//    #method containing:s
//        = self::wideOp containing &wide:(s wide).
//
//    #method replaceFirst &wide:aReplacee &wide:aReplacer
//    [
//        #var(int) aReplaceeLen := aReplacee length.
//        #var(int)anIndex.
//        
//        self indexOf &int:0 &wide:aReplacee &vint:anIndex.
//        
//        (anIndex != -1)
//            ? [ ^ self delete &int:anIndex &int:aReplaceeLen insert &int:anIndex &wide:aReplacee. ]
//            ! [ ^ self. ].
//    ]
//        
//    #method(w2) replace &wide:aReplacee &wide:aReplacer
//    [
//        #var(wide) aTarget := emptyWideLiteralValue.
//        #var(int)aStart := 0.
//        #var(int)anEnd.
//        #var(int)aReplaceeLen := aReplacee length.
//        
//        self indexOf &int:aStart &wide:aReplacee &vint:anEnd.
//        
//        #loop (anEnd != -1) ?
//        [
//            aTarget := aTarget
//                add &wide:(self Substring &int:aStart &int:(anEnd - aStart))
//                add &wide:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            self indexOf &int:aStart &wide:aReplacee &vint:anEnd.
//        ].
//    
//        ^ (aStart != 0)
//            ? [ aTarget add &wide:(self::wideOp Substring &int:aStart) ]
//            ! [ self ].
//    ]
//
//    #method(bool) is &empty
//    [
//        #var(int)aLength := self length.
//        
//        ^ aLength == 0.
//    ]
//}
//
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
//        #var(int)aNumber := aCounter.
//        
//        #loop (aNumber > 0) ?
//        [
//            self write:aLiteral. 
//            
//            aNumber -= 1.
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
//        #var(int)aWidthInt := aWidth int.
//        #var(int)aLength := aLine length int.        
//        
//        (aLength < aWidthInt)
//            ? [
//                #var(int)aLen := aWidthInt - aLength.
//                #var(int)aLeftLen := aLen / 2.
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
    #method(bool,stacksafe)is &whitespace
    [
        (self == #9)
            ? [ ^ true. ]
            ! [
                (self == #32)
                    ? [ ^ true. ]
                    ! [
                        (self == #10)
                            ? [ ^ true.]
                            ! [
                                (self == #13)
                                    ? [ ^ true. ]
                                    ! [
                                        ^ self == #12.
                                    ]
                            ]
                    ]
            ].
            
        ^ false.            
    ]
    
    #method(stacksafe,bool)is &digit
    [
        #var(int) ch.
        intConvertor convert &char:self &vint:ch.
        
        ^ (ch >= 30h) and: (ch <= 39h).
    ]
    
    #method(bool,stacksafe)is &newLine
    [
        ^ self == #10.
    ]
    
    #method(bool,stacksafe)is &letter
    [
        #var(int) ch.
        intConvertor convert &char:self &vint:ch.
        
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
    
    #method(bool,stacksafe)is &ltoken
    [
        #var(int) ch.
        intConvertor convert &char:self &vint:ch.
        
        ((ch >= 41h) and: (ch <= 5Ah))
            ? [ ^ true. ].
        
        ((ch >= 61h) and: (ch <= 7Ah))
            ? [ ^ true. ].
        
        ((ch >= 3ACh) and: (ch <= 3CEh))
            ? [ ^ true. ].
            
        ((ch >= 400h) and: (ch <= 42Fh))
            ? [ ^ true. ].
            
        ^ ch == 39.
    ]
}

// --- byteArrayHelper ---

#symbol byteArrayHelper =
{
    #method(stacksafe)read &bytearray:array &int:offset &vshort:retVal
    [
        array read &int:offset &vshort:retVal.
    ]
    
    #method(stacksafe)read &bytearray:array &int:offset &vint:retVal
    [
        array read &int:offset &vint:retVal.
    ]
    
    #method(stacksafe)read &bytearray:array &int:offset &vlong:retVal
    [
        array read &int:offset &vlong:retVal.
    ]
    
    #method(stacksafe)read &bytearray:array &int:offset &vreal:retVal
    [
        array read &int:offset &vreal:retVal.
    ]
    
    #method(stacksafe)read &bytearray:array &int:offset &vchar:retVal
    [
        array read &int:offset &vchar:retVal.
    ]
    
    #method(stacksafe)write &bytearray:array &int:offset &short:retVal
    [
        array write &int:offset &short:retVal.
    ]
    
    #method(stacksafe)write &bytearray:array &int:offset &int:retVal
    [
        array write &int:offset &int:retVal.
    ]
    
    #method(stacksafe)write &bytearray:array &int:offset &long:retVal
    [
        array write &int:offset &long:retVal.
    ]
    
    #method(stacksafe)write &bytearray:array &int:offset &real:retVal
    [
        array write &int:offset &real:retVal.
    ]
    
    #method(stacksafe)write &bytearray:array &int:offset &char:retVal
    [
        array write &int:offset &char:retVal.
    ]
}.
