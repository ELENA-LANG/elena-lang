// === actions ===

symbol ifOrdered
    = (:aFormer:aLater) [ ^aFormer < aLater ].

symbol ifDecrescent
    = (:aFormer:aLater) [ ^aFormer > aLater ].

// --- common methods ---

/// A generic extender
extension objectOp
{
    class_ref
        => system'core_routines'__object.

    parent_ref
        => system'core_routines'__object.

    bool instanceOf:type
        => system'core_routines'__object.        

    literal className
    [
        var c := self~objectOp class_ref.
        
        byte<256> aBuffer.
        int       aLength := system'external LoadClassName 
                                ref:c
                                bytearray:aBuffer
                                int:255.
         
        if (aLength == 0)
            [ ^ "Unknown class" ].
        
        ^ LiteralValue $new int:0 int:aLength bytearray:aBuffer
    ]    
    
    /// Returns true if the specified object instances are equal; otherwise, false.
    bool equal reference:anObject
        => system'core_routines'__object.
        
    evalSelf func1:aFunction
        = aFunction eval:self.

    evalSelf:aFunction
        = aFunction eval:self.

    /// Returns true if the object is nil
    bool isNil
        = nil == self.
        
    /// Returns true if the object is literal or character
    isLiteral
    [
        var o := self literal \ back:$nil.
        if ($nil == o)
        [
            o := self wide \ back:$nil.
            if ($nil == o)
            [
                o := self char \ back:$nil.
                
                if ($nil == o)
                [
                    ^ false
                ]
            ]
        ].
        
        ^ true.
    ]
    
    /// Returns true if the object is integer numereric
    isInteger
    [
        var o := self int \ back:$nil.
        if ($nil == o)
        [
            o := self long \ back:$nil.
            if ($nil == o)
            [
                o := self short \ back:$nil.
                
                if ($nil == o)
                [
                    o := self byte \ back:$nil.
                    
                    if ($nil == o)
                    [
                        ^ false
                    ]
                ]
            ]
        ].
        
        ^ true.
    ]
    
    /// Returns true if the object is numereric
    isNumeric
    [
        var o := self int \ back:$nil.
        if ($nil == o)
        [
            o := self long \ back:$nil.
            if ($nil == o)
            [
                o := self short \ back:$nil.
                
                if ($nil == o)
                [
                    o := self byte \ back:$nil.
                    
                    if ($nil == o)
                    [
                        o := self real \ back:$nil.
                        
                        if ($nil == o)
                        [
                            ^ false
                        ]
                    ]
                ]
            ]
        ].
        
        ^ true.
    ]
        
    bool isEmpty
        = self length == 0.
        
    bool isNonempty
        = self length != 0.
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
//    bool safeEqual : anObject
//        = anObject cast:%eval to:{ generic eval:aTarget:anObject = aTarget~$subject equal:anObject \ back:false. } with:self.

    doWith:action
        = action eval:self.
        
    stacksafe reallocate int:aNewLength
        => system'core_routines'__array.
        
    shift : n
    [
        if (n < 0)
            [ ^ self shiftLeft int(n int; negative) ];
            [ ^ self shiftRight int:n ].
    ]
}

// --- Operations ---

// --- byteOp ---

byte extension byteOp
{
    stacksafe byte absolute
    [
        byte n := self.
        if (n < 0)
            [ n := n * -1 ].
                
        ^ ByteNumber new byte:n
    ]
    
    byte mod : anOperand
    [
        var retVal := self / anOperand * anOperand.
        
        ^ byteConvertor convert(retVal)
    ]    
    
    stacksafe byte power int:y
    [
        byte x := self.
        byte r := 1.
        
        int i := 0.
        while (i < y)
        [
            r := r * x.
            
            i += 1
        ].
        
        ^ ByteNumber new byte:r
    ]
    
    stacksafe byte sqr
    [
        byte aRetVal := 0.
        
        aRetVal := self * self.
        
        ^ ByteNumber new byte:aRetVal
    ]

    power : aPower
        = self~byteOp power int:aPower.
                
    bool isOdd
        = self~byteOp mod:2 != 0.
        
    bool isEven
        = self~byteOp mod:2 == 0.
        
    stacksafe bool isZero
        = self equal int:0.
        
    stacksafe bool isPositive
        = self > 0.
        
    stacksafe bool isNegative
        = self < 0.
        
    stacksafe bool isNonnegative
        = self <= 0.
}

// --- shortOp ---

short extension shortOp
{
    stacksafe short absolute
    [
        short n := self.
        if (n < 0)
            [ n := n * -1 ].
                
        ^ ShortNumber new short:n
    ]
    
    short mod : anOperand
    [
        var retVal := self / anOperand * anOperand.
        
        ^ shortConvertor convert(retVal)
    ]
    
    stacksafe short power int:y
    [
        short x := self.
        short r := 1.
        
        int i := 0.
        while (i < y)
        [
            r := r * x.
            
            i += 1.
        ].
        
        ^ ShortNumber new short:r
    ]
    
    stacksafe short sqr
    [
        short n := self.
        short aRetVal := 0.
        
        aRetVal := n * n.
        
        ^ ShortNumber new short:aRetVal
    ]

    power : aPower
        = self~shortOp power int:aPower.
                
    bool isOdd
        = self~shortOp mod:2 != 0.
        
    bool isEven
        = self~shortOp mod:2 == 0.
        
    stacksafe bool isZero
        = self equal int:0.
        
    stacksafe bool isPositive
        = self > 0.
        
    stacksafe bool isNegative
        = self < 0.
        
    stacksafe bool isNonnegative
        = self <= 0.
    
    stacksafe byte high
    [
        short aWord := self.
        aWord := aWord && 0FF00h.
        aWord := aWord shiftRight int:8.
        
        ^ byteConvertor convert short:aWord
    ]
        
    stacksafe byte low
    [
        short aWord := self.
        aWord := aWord && 0FFh.
        
        ^ byteConvertor convert short:aWord
    ]
}

// --- intOp ---

int extension intOp
{
    int rvprop absolute :: absolute.
    
    int fnprop  mod      :: mod.
    
    int fnprop  power    :: power.
    
    int rvprop sqr      :: sqr.
    
    int rvprop sqrt     :: sqrt.
    
    stacksafe absolute vint:retVal
    [
        int n := self.

        if (n < 0)
            [ n := n * -1 ].

        retVal int := n
    ]
    
    stacksafe mod int:y vint:retVal
    [
        int x := self.
        
        int z := x / y.
        z := z * y.
        
        retVal int := (x - z)
    ]
    
    int mod : anOperand
    [
        int retVal := self mod int:anOperand.
        
        ^ retVal
    ]
    
    real realDiv : anOperand
        = RealNumber new int:self; divide:anOperand.
    
    stacksafe power int:y vint:retVal
    [
        int x := self.
        int r := 1.
        if (x == 0)
            [ r := 0 ].
        
        int i := 0.
        while(i < y)
        [
            r := r * x.
            
            i := i + 1
        ].
        
        retVal int := r
    ]
    
    stacksafe sqr vint:retVal
    [
        int n := self.        
        
        retVal int := n * n.
    ]
    
    stacksafe sqrt vint:retVal
    [
        int num := self.
        
        if (num == 0)
            [ ^ 0 ].
            
        int n := num / 2 + 1.
        int n1 := (n + (num / n)) / 2.
        while (n1 < n)
        [
            n := n1.
            n1 := (n + (num / n)) / 2
        ].
        
        retVal int := n
    ]

    power : aPower
        = self~intOp power int:aPower.
                
    stacksafe bool isOdd
        = self and int:1 != 0.
        
    stacksafe bool isEven
        = self and int:1 == 0.
        
    stacksafe bool isZero
        = self equal int:0.
        
    stacksafe bool isPositive
        = self > 0.
        
    stacksafe bool isNegative
        = self < 0.
        
    stacksafe bool isNonnegative
        = self <= 0.

    high
    [
        int aWord := self.
        aWord := aWord && 0FFFF0000h.
        aWord := aWord >> 16.
        
        ^ shortConvertor convert int:aWord
    ]

    low
    [
        int aWord := self.
        aWord := aWord && 0FFFFh.
        
        ^ shortConvertor convert int:aWord
    ]
    
    shift : n
    [
        if (n < 0)
            [ ^ self shiftLeft int:(n int; negative) ];
            [ ^ self shiftRight int:(n int) ].
    ]            
}

// --- longOp ---

long extension longOp
{
    stacksafe long absolute
    [
        long n := self.

        if (n < 0l)
            [ n := n * -1l ].
                
        ^ LongNumber new long:n
    ]
    
    long mod : anOperand
    [
        long x := self.
        long y := anOperand long.
        
        long z := x / y.
        z := z * y.
        
        ^ LongNumber new long:(x - z)
    ]
    
    stacksafe long power long:y
    [
        long x := self.
        long r := 1.
        
        long i := 0l.
        while (i < y)
        [
            r := r * x.
            
            i += 1l.
        ].
        
        ^ LongNumber new long:r
    ]
    
    stacksafe long power int:y
    [
        long x := self.
        long r := 1.
        
        int i := 0.
        while (i < y)
        [
            r := r * x.
            
            i += 1.
        ].
        
        ^ LongNumber new long:r
    ]
    
    stacksafe long sqr
    [
        long n := self.        
        long aRetVal := 0l.
        
        aRetVal := n * n.
        
        ^ LongNumber new long:aRetVal
    ]

    power : aPower
        = self~longOp power long:aPower.
                
    bool isOdd
        = self~longOp mod:2l != 0.
        
    bool isEven
        = self~longOp mod:2l == 0.
        
    stacksafe bool isZero
        = self equal long:0l.
        
    stacksafe bool isPositive
        = self > 0l.
        
    stacksafe bool isNegative
        = self < 0l.
        
    stacksafe bool isNonnegative
        = self <= 0l.

    stacksafe int high
    [
        long aWord := self.
        aWord := aWord && -4294967296l.
        aWord := aWord >> 32.
        
        ^ intConvertor convert long:aWord
    ]

    stacksafe int low
    [
        long aWord := self.
        aWord := aWord && 4294967295l.
        
        ^ intConvertor convert long:aWord
    ]
}

// --- realOp ---

real extension realOp
{        
    stacksafe real absolute
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController abs real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]
        
    stacksafe real exp
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController exp real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal.
    ]
    
    stacksafe real ln
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController ln real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]
    
    stacksafe real sqrt
    [
        real n := self.      
        real aRetVal := 0.0r.
        
        realOpController sqrt real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    stacksafe real power real:y
    [
        real x := self.
        
        (x == 0.0r)
            ? [ ^ 0.0r ].
                        
        (y == 0.0r)
            ? [ ^ 1.0r ].
            
        realOpController ln real:x vreal:x.
        
        x := x multiply real:y.
        
        realOpController exp real:x vreal:x.
            
        ^ RealNumber new real:x         
            
    ]

    stacksafe real power int:y
    [
        real x := self.

        (x == 0.0r)
            ? [ ^ 0.0r ].
                        
        (y == 0)
            ? [ ^ 1.0r ].
                        
        if (x < 0.0r)
            [
                x := x * -1.0r.
                
                realOpController ln real:x vreal:x.
                
                x := x multiply int:y.
                
                realOpController exp real:x vreal:x.
                
                (y~intOp isOdd)
                    ? [ x := x * -1.0r ].
            ];
            [
                realOpController ln real:x vreal:x.
                
                x := x multiply int:y.
                
                realOpController exp real:x vreal:x
            ].
        
        ^ RealNumber new real:x
    ]

    power : aPower
        = self~realOp power int:aPower.

    stacksafe real sqr
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        aRetVal := n * n.
        
        ^ RealNumber new real:aRetVal
    ]

    stacksafe real rounded
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController round real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]
    
    stacksafe real round int:precision
    [
        real a := self.
        bool neg := false.
        if(self isNegative)
        [
            a := a * -1.0r.
            neg := true.
        ].
           
        int b := 10 power:precision.
        int c := (a * b + 0.5r) roundedInt.
    
        a := c realDiv:b.
    
        ^(neg) iif(a * -1.0r, a). 
    ]
    
    stacksafe real truncate int:precision
    [
        real a := self.
        bool neg := false.
        if(self isNegative)
        [
            a := a * -1.0r.
            neg := true.
        ].
       
        int b := 10 power:precision.
        real c := (a * b) integer. // returns the integer part of the real number

        a := c / b.

        ^(neg) iif(a * -1.0r, a). 
    ]

    stacksafe real integer
    [
        real n := self.        
        real aRetVal := 0.0r.

        realOpController truncate real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    stacksafe real frac
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController abs real:n vreal:aRetVal.
        
        ^ RealNumber new real:(n - aRetVal)
    ]

    stacksafe real sin
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController sin real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    stacksafe real cos
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController cos real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]

    stacksafe real tan
    [
        real n := self.        
        real aSinVal := 0.0r.
        real aCosVal := 0.0r.
        
        realOpController sin real:n vreal:aSinVal.
        realOpController cos real:n vreal:aCosVal.
        
        ^ RealNumber new real:(aSinVal / aCosVal)
    ]

    stacksafe real arctan
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController arctan real:n vreal:aRetVal.
        
        ^ RealNumber new real:aRetVal
    ]
            
    stacksafe real arcsin
    [
        real n := self.        
        real arg := 0.0r.
        real retVal := 0.0r.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt real:retVal vreal:arg.
        
        arg := n / arg.        
        realOpController arctan real:arg vreal:retVal.
        
        ^ RealNumber new real:retVal
    ]
            
    stacksafe real arccos
    [
        real n := self.        
        real arg := 0.0r.
        real retVal := 0.0r.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt real:retVal vreal:arg.
        
        arg := arg / n.        
        realOpController arctan real:arg vreal:retVal.
        
        ^ RealNumber new real:retVal
    ]
            
    stacksafe real radian
    [
        real aValue := self.
        
        aValue := aValue * pi_value.
        
        ^ RealNumber new real:(aValue / 180.0r)
    ]
            
    stacksafe real degree
    [
        real aValue := self.
        
        aValue := aValue * 180.0r.
        
        ^ RealNumber new real:(aValue / pi_value)
    ]
            
    stacksafe real reciprocal
    [
        real aValue := self.
        
        ^ RealNumber new real:(1.0r / aValue)
    ]    
            
    stacksafe bool isZero
        = self == 0.0r.
        
    stacksafe bool isPositive
        = self > 0.0r.
        
    stacksafe bool isNegative
        = self < 0.0r.
        
    stacksafe bool isNonnegative
        = self <= 0.0r.
}

// --- intArrayOp ---

intarray extension intArrayOp
{
    stacksafe exchange int:anIndex1 int:anIndex2
    [
        int n1 := 0.
        int n2 := 0.

        self read int:anIndex1 vint:n1.
        self read int:anIndex2 vint:n2.
        
        self write int:anIndex1 int:n2.
        self write int:anIndex2 int:n1.
    ]

    populate nfunc:anIndexFunc
    [
        int anIndex := 0.
        int aLength := self length.
        
        while (anIndex < aLength)
        [            
            self write int:anIndex int(anIndexFunc eval int:anIndex).
            
            anIndex := anIndex + 1
        ].
        
        ^ self
    ]
    
    populate(BaseFunction1 aFunction)
        = self~intArrayOp populate(:n)<int>[ ^aFunction eval:n ].    
}

// --- arrayOp ---

array extension arrayOp
{
    exchange : anIndex1 : anIndex2
        = self~arrayOp exchange int:anIndex1 int:anIndex2.
        
    exchange int:anIndex1 int:anIndex2
    [
        var aTemp := self getAt int:anIndex1.
        
        self setAt int:anIndex1 object:(self getAt int:anIndex2).
        self setAt int:anIndex2 object:aTemp.
    ]
    
    stacksafe indexOf int:anIndex object:anObject vint:aRetVal
    [
        aRetVal int := -1.
        
        int aLength := self length.
        
        int i := anIndex.
        while(i < aLength)
        [
            var anArrayItem := self getAt int:i.
            
            if (anObject == anArrayItem)
                [ aRetVal int := i ].
            
            i := i + 1
        ]
    ]        
        
    stacksafe indexOf int:anIndex object:anObject
    [
        int aRetVal := 0.
        
        self~arrayOp indexOf int:anIndex object:anObject vint:aRetVal.
        
        ^ IntNumber new int:aRetVal
    ]
    
    indexOf : anObject
        = self~arrayOp indexOf int:0 object:anObject.
        
//    safeSeek : aValue
//    [
//        int aLength := self length.
//        
//        int i := 0.
//        while (i < aLength)
//        [
//            var anArrayItem := self getAt int:i.
//            
//            if (aValue safeEqual:anArrayItem)
//                [ ^ anArrayItem ].
//            
//            i := i + 1
//        ].
//        
//        ^ $nil
//    ]
        
    deepClone
    [
        int aLength := self length.
        
        array aCopy := Array new int:aLength.
                
        int anIndex := 0.
        while(anIndex < aLength)
        [
            aCopy setAt int:anIndex object:(self getAt int:anIndex; clone).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]
    
    populate nfunc:anIndexFunc
    [
        int anIndex := 0.
        int aLength := self length.
        
        while(anIndex < aLength)
        [            
            self setAt int:anIndex object:(anIndexFunc eval int:anIndex).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self
    ]
    
    populate(BaseFunction1 aFunction)
        = self~arrayOp populate(:n)<int>[ ^aFunction eval:n ].

    multi populate : anAction
        = self~arrayOp populate(:n)<int>[ ^anAction eval:n ].    
    
    sort func2:aCompareFunc
    [
        int i := 0.
        int j := 0.
        int aLength := self length.
        var aCurrent := $nil.
        var aNext := $nil.
        
        while(i < aLength)
        [
            aCurrent := self getAt int:i.
            
            j := i + 1.
            while (j < aLength)
            [
                aNext := self getAt int:j.
                ifnot(aCompareFunc eval(aCurrent,aNext))
                    [                        
                        self setAt int:i object:aNext.
                        self setAt int:j object:aCurrent.
                        
                        aCurrent := aNext
                    ].
                    
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ self
    ]
    
    ascendant
        = (self clone)~arrayOp sort:ifOrdered.
    
    bool isEmpty
    [
        int aLength := self length.
        
        ^ aLength == 0.
    ]
    
    array Subarray : aLength at:anIndex
        = self~arrayOp Subarray int:anIndex int:aLength.
    
    array Subarray int:anIndex int:aLength
    [
        indexer it := self indexer.
        
        ^ Array new int:aLength; populate(:i)<int>(it writeIndex int(anIndex + i); get).
    ]
}

// --- intmatrixOp ---

intmatrix extension intmatrixOp
{
    populate nnfunc:func
    [
        int n := 0.
        int m := 0.
        self readLengthsTo vint:n vint:m.
        
        int i := 0.
        int j := 0.
        while (i < n)
        [
            j := 0.
            while (j < m)
            [
                int v := func eval int:i int:j.
                
                self write int:i int:j int:v.
                
                j := j + 1
            ].
            
            i := i + 1
        ].
        
        ^ self
    ]
}

// --- literalOp ---

literal extension literalOp
{
    indexOf : anObject
        = self indexOf:anObject at:0.

    indexOf literal:aLiteral
        = self indexOf:aLiteral at:0.

    indexOf char:aChar
        = self indexOf:aChar at:0.
        
    literal Substring at:anIndex
        = self~literalOp Substring int:anIndex.
        
    stacksafe literal Substring int:anIndex
    [
        int aLength := self length.
        
        aLength := aLength - anIndex.
        
        ^ self Substring int:anIndex int:aLength
    ]        
    
    literal trimLeft char:aChar
    [
        int  anIndex := 0.
        int  aLength := self length.
        char aCurrent := $0.
        int  aCharLen := aChar length.
        
        while(anIndex < aLength)
        [
            self read int:anIndex vchar:aCurrent.
            
            ifnot(aCurrent == aChar)
                [ 
                    if (anIndex == 0)
                        [ ^ self ];
                        [ ^ self delete int:0 int:anIndex ]
                ].
            
            anIndex := anIndex + aCharLen
        ].
        
        ^ emptyLiteralValue
    ]
    
    literal trimLeft
        = self~literalOp trimLeft char:$32.    
       
    literal trimRight char:aChar
    [
        int  aLength := self length.
        int  anIndex := aLength - 1.
        int  aPrevious := aLength.
        char aCurrent := $0.
        int  aCharLen := aChar length.
        
        while (anIndex >= 0)
        [
            self read int:anIndex vchar:aCurrent.
            
            ifnot (aCurrent equal char:aChar)
                [ ^ self delete int:aPrevious int:(aLength - aPrevious) ].
            
            aPrevious := anIndex.
            anIndex := anIndex - aCharLen
        ].
        
        ^ emptyLiteralValue
    ]
    
    literal trimRight
        = self~literalOp trimRight char:$32.
        
    literal trim char:aChar
        = self~literalOp trimLeft char:aChar; trimRight char:aChar.
    
    literal trim
        = self~literalOp trimLeft char:$32; trimRight char:$32.        
    
    padLeft int:aLength
        = self padLeft($32, aLength).
    
    padLeft char:aChar int:aLength
    [
        int aCurrentLength := self length.
        
        if (aCurrentLength < aLength)
            [ ^ LiteralValue new int(aLength - aCurrentLength) char(aChar); add literal:self ].
            
        ^ self
    ]
    
    padRight int:aLength
        = self padRight($32, aLength).
    
    padRight char:aChar int:aLength
    [
        int aCurrentLength := self length.

        if (aCurrentLength < aLength)
            [ ^ self add literal(LiteralValue new int(aLength - aCurrentLength) char(aChar)) ].
            
        ^ self
    ]

    startingWith literal:s
    [
        int n := 0.
        self indexOf int:0 literal:s vint:n.
        
        ^ 0 == n
    ]
        
    endingWith literal:s
    [
        int l1 := self length.
        int l2 := s length.

        int n := 0.
        self indexOf int:(l1 - l2) literal:s vint:n.
        
        ^ n != -1
    ]
        
    containing literal:s
    [
        int l1 := self length.
        int l2 := s length.

        int n := 0.
        self indexOf int:0 literal:s vint:n.
        
        ^ n != -1
    ]

//    replaceFirst literal:aReplacee literal:aReplacer
//    [
//        int aReplaceeLen := aReplacee length.
//        int anIndex := 0.
//        
//        self indexOf int:0 literal:aReplacee vint:anIndex.
//        
//        if (anIndex != -1)
//            [ ^ self delete int:anIndex int:aReplaceeLen; insert int:anIndex literal:aReplacee ];
//            [ ^ self ].
//    ]    
//    
//    replace literal:aReplacee literal:aReplacer
//    [
//        literal aTarget := emptyLiteralValue.
//        int     aStart := 0.
//        int     anEnd := 0.
//        int     aReplaceeLen := aReplacee length.
//        
//        self indexOf int:aStart literal:aReplacee vint:anEnd.
//        
//        while (anEnd != -1)
//        [
//            aTarget := aTarget
//                add literal:(self Substring int:aStart int:(anEnd - aStart));
//                add literal:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            self indexOf int:aStart literal:aReplacee vint:anEnd
//        ].
//    
//        (aStart != 0)
//            ? [ ^ aTarget add literal:(self~literalOp Substring int:aStart) ]
//            ! [ ^ self ]
//    ]
    
    bool isEmpty
    [
        int aLength := self length.
        
        ^ aLength == 0
    ]    
}

// --- wideOp ---

wide extension wideOp
{
    indexOf : anObject
        = self indexOf:anObject at:0.
        
    indexOf wide:aLiteral
        = self indexOf:aLiteral at:0.

    indexOf char:aChar
        = self indexOf:aChar at:0.
                
    wide Substring at:anIndex
        = self~wideOp Substring int:anIndex.
        
    stacksafe wide Substring int:anIndex
    [
        int aLength := self length.
        
        aLength := aLength - anIndex.
        
        ^ self Substring int:anIndex int:aLength
    ]
     
    wide trimLeft char:aChar
    [
        int  anIndex := 0.
        int  aLength := self length.
        char aCurrent := $0.
        int  aCharLen := aChar length.
        
        while (anIndex < aLength)
        [
            self read int:anIndex vchar:aCurrent.
            
            ifnot(aCurrent equal char:aChar)
                [ ^ self delete int:0 int:anIndex ].
            
            anIndex := anIndex + aCharLen
        ].
        
        ^ emptyWideLiteralValue
    ]
    
    wide trimLeft
        = self~wideOp trimLeft char:$32.
       
    wide trimRight char:aChar
    [
        int  aLength := self length.
        char aCurrent := $0.
        int  aCharLen := aChar length.
                
        int  anIndex := aLength - 1.
        int  aPrevious := aLength.
        while (anIndex >= 0)
        [
            self read int:anIndex vchar:aCurrent.
            
            ifnot (aCurrent equal char:aChar)
                [ ^ self delete int:aPrevious int:(aLength - aPrevious) ].
            
            aPrevious := anIndex.
            anIndex := anIndex - aCharLen
        ].
        
        ^ emptyWideLiteralValue
    ]
    
    wide trimRight
        = self~wideOp trimRight char:$32.
       
    wide trim char:aChar
        = self~wideOp trimLeft char:aChar; trimRight char:aChar.
    
    wide trim
        = self~wideOp trimLeft char:$32; trimRight char:$32.
    
    padLeft char:aChar int:aLength
    [
        int aCurrentLength := self length.
        
        if (aCurrentLength < aLength)
            [ ^ WideLiteralValue new int(aLength - aCurrentLength) char(aChar); add wide:self ].
            
        ^ self
    ]
    
    padRight char:aChar int:aLength
    [
        int aCurrentLength := self length.

        if (aCurrentLength < aLength)
            [ ^ self add wide(WideLiteralValue new int(aLength - aCurrentLength) char(aChar)) ].
            
        ^ self
    ]

    startingWith wide:s
    [
        int n := 0.
        self indexOf int:0 wide:s vint:n.
        
        ^ 0 == n
    ]
    
    endingWith wide:s
    [
        int l1 := self length.
        int l2 := s length.
        int n := 0.
        self indexOf int:(l1 - l2) wide:s vint:n.
        
        ^ n != -1
    ]
    
    containing wide:s
    [
        int l1 := self length.
        int l2 := s length.
        int n := 0.
        
        self indexOf int:0 wide:s vint:n.
        
        ^ n != -1
    ]
    
//    replaceFirst wide:aReplacee wide:aReplacer
//    [
//        int aReplaceeLen := aReplacee length.
//        int anIndex := 0.
//        
//        self indexOf int:0 wide:aReplacee vint:anIndex.
//        
//        if (anIndex != -1)
//            [ ^ self delete int:anIndex int:aReplaceeLen; insert int:anIndex wide:aReplacee ];
//            [ ^ self ].
//    ]
//        
//    replace wide:aReplacee wide:aReplacer
//    [
//        wide aTarget := emptyWideLiteralValue.
//        int  aStart := 0.
//        int  anEnd := 0.
//        int  aReplaceeLen := aReplacee length.
//        
//        self indexOf int:aStart wide:aReplacee vint:anEnd.
//        
//        while (anEnd != -1)
//        [
//            aTarget := aTarget
//                add wide:(self Substring int:aStart int:(anEnd - aStart));
//                add wide:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            self indexOf int:aStart wide:aReplacee vint:anEnd
//        ].
//    
//        if (aStart != 0)
//            [ ^ aTarget add wide:(self~wideOp Substring int:aStart) ];
//            [ ^ self ].
//    ]

    bool isEmpty
    [
        int aLength := self length.
        
        ^ aLength == 0
    ]
}

// --- stringOp ---

class $stringOpDispatcher =
{
    padRight literal:s char:ch int:len
        = s~literalOp padRight char:ch int:len.
    
    padRight wide:s char:ch int:len
        = s~wideOp padRight char:ch int:len.
        
    padLeft literal:s char:ch int:len
        = s~literalOp padLeft char:ch int:len.
    
    padLeft wide:s char:ch int:len
        = s~wideOp padLeft char:ch int:len.
        
    startingWith literal:s literal:subs
        = s~literalOp startingWith literal:subs.

    startingWith wide:s wide:subs
        = s~wideOp startingWith wide:subs.

    endingWith literal:s literal:subs
        = s~literalOp endingWith literal:subs.

    endingWith wide:s wide:subs
        = s~wideOp endingWith wide:subs.
        
    containing literal:s literal:subs
        = s~literalOp containing literal:subs.

    containing wide:s wide:subs
        = s~wideOp containing wide:subs.
        
    trimLeft literal:s char:ch
        =s~literalOp trimLeft char:ch.        
        
    trimRight literal:s char:ch
        =s~literalOp trimRight char:ch.        
        
    trim literal:s char:ch
        =s~literalOp trim char:ch.
        
    trimLeft wide:s char:ch
        =s~wideOp trimLeft char:ch.        
        
    trimRight wide:s char:ch
        =s~wideOp trimRight char:ch.        
        
    trim wide:s char:ch
        =s~wideOp trim char:ch.}.

extension stringOp
{
    Substring : anIndex
        = self Substring:(self length - anIndex) at:anIndex.
            
    Substring : anIndex : aLength
        = self Substring:aLength at:anIndex.
    
    trimLeft :ch
        = $stringOpDispatcher trimLeft(self, ch).

    trimRight :ch
        = $stringOpDispatcher trimRight(self, ch).

    trim:ch
        = $stringOpDispatcher trim(self, ch).
    
    trimLeft
        = $stringOpDispatcher trimLeft(self, $32).

    trimRight
        = $stringOpDispatcher trimRight(self, $32).

    trim
        = $stringOpDispatcher trim(self, $32).

    padLeft : padSymbol : len
        = $stringOpDispatcher padLeft(self, padSymbol, len).
        
    padRight : padSymbol : len
        = $stringOpDispatcher padRight(self, padSymbol, len).

    startingWith : s
        = $stringOpDispatcher startingWith(self, s).

    endingWith : s
        = $stringOpDispatcher endingWith(self, s).

    containing : s
        = $stringOpDispatcher containing(self, s).

//    replaceFirst literal:aReplacee literal:aReplacer
//    [
//        var dispatcher := self cast:%eval to:stringOpDispatcher.
//        
//        ^ self~dispatcher replaceFirst literal:aReplacee literal:aReplacer
//    ]
//    
//    replace literal:aReplacee literal:aReplacer
//    [
//        var dispatcher := self cast:%eval to:stringOpDispatcher.
//        
//        ^ self~dispatcher replace literal:aReplacee literal:aReplacer
//    ]
    
    stacksafe write object:aLiteral int:aCounter
    [
        int aNumber := aCounter.
        
        while (aNumber > 0)
        [
            self write:aLiteral. 
            
            aNumber := aNumber - 1
        ].
            
        ^ self
    ]

    write : aLine length:aCounter
        = self~stringOp write object:aLine int:aCounter.
        
    write : aLine paddingLeft:aWidth
        = self~stringOp write:aLine paddingLeft:aWidth with:$32.

    write : aLine paddingLeft:aWidth with:aChar
    [
        var aPaddedStr := aLine padLeft(aChar, aWidth).

        self write:aPaddedStr.
            
        ^ self
    ]

    write : aLine paddingRight:aWidth
        = self~stringOp write:aLine paddingRight:aWidth with:$32.

    write : aLine paddingRight:aWidth with:aChar
    [
        var aPaddedStr := aLine padRight(aChar, aWidth).

        self write:aPaddedStr.
            
        ^ self
    ]
    
    write : aLine padding:aWidth with:aChar
    [
        int aWidthInt := aWidth int.
        int aLength := aLine length.        
        
        if (aLength < aWidthInt)
            [
                int aLen := aWidthInt - aLength.
                int aLeftLen := aLen / 2.
                
                self~stringOp write object:aChar int:aLeftLen.
                self write:aLine.
                self~stringOp write object:aChar int:(aLen - aLeftLen)
            ];
            [ self write:aLine ].
            
        ^ self
    ]    
}

// --- charOp ---

char extension charOp
{
    stacksafe bool isWhitespace
    [
        self =>
            $9  [ ^ true ];
            $32 [ ^ true ];
            $10 [ ^ true ];
            $13 [ ^ true ];
            $12 [ ^ true ].
            
        ^ false
    ]
    
    stacksafe bool isDigit
    [
        int ch := 0.
        intConvertor convert char:self vint:ch.
        
        ^ (ch >= 30h) and: (ch <= 39h)
    ]
    
    stacksafe bool isNewLine
    [
        ^ self == $10
    ]
    
    stacksafe bool isLetter
    [
        int ch := 0.
        intConvertor convert char:self vint:ch.
        
        if ((ch >= 41h) and: (ch <= 5Ah))
            [ ^ true ].
        
        if ((ch >= 61h) and: (ch <= 7Ah))
            [ ^ true ].
        
        if ((ch >= 3ACh) and: (ch <= 3CEh))
            [ ^ true ].
            
        if ((ch >= 400h) and: (ch <= 42Fh))
            [ ^ true ].
            
        ^ false
    ]
    
    stacksafe bool isLToken
    [
        int ch := 0.
        intConvertor convert char:self vint:ch.
        
        if ((ch >= 41h) and: (ch <= 5Ah))
            [ ^ true ].
        
        if ((ch >= 61h) and: (ch <= 7Ah))
            [ ^ true ].
        
        if ((ch >= 3ACh) and: (ch <= 3CEh))
            [ ^ true ].
            
        if ((ch >= 400h) and: (ch <= 42Fh))
            [ ^ true ].
            
        ^ ch == 39
    ]
}

// --- byteArrayHelper ---

class byteArrayHelper =
{
    stacksafe read bytearray:array int:offset vshort:retVal
    [
        array read int:offset vshort:retVal
    ]
    
    stacksafe read bytearray:array int:offset vint:retVal
    [
        array read int:offset vint:retVal
    ]
    
    stacksafe read bytearray:array int:offset vlong:retVal
    [
        array read int:offset vlong:retVal
    ]
    
    stacksafe read bytearray:array int:offset vreal:retVal
    [
        array read int:offset vreal:retVal
    ]
    
    stacksafe read bytearray:array int:offset vchar:retVal
    [
        array read int:offset vchar:retVal
    ]
    
    stacksafe write bytearray:array int:offset short:retVal
    [
        array write int:offset short:retVal
    ]
    
    stacksafe write bytearray:array int:offset int:retVal
    [
        array write int:offset int:retVal
    ]
    
    stacksafe write bytearray:array int:offset long:retVal
    [
        array write int:offset long:retVal.
    ]
    
    stacksafe write bytearray:array int:offset real:retVal
    [
        array write int:offset real:retVal
    ]
    
    stacksafe write bytearray:array int:offset char:retVal
    [
        array write int:offset char:retVal
    ]
    
    stacksafe fill bytearray:array int:offset int:length byte:val
    [
        array fill int:offset int:length byte:val.
    ]
}.

// --- intArrayHelper ---

singleton intArrayHelper
{
    stacksafe fill intarray:array int:offset int:length int:val
    [
        array fill int:offset int:length int:val.
    ]
}