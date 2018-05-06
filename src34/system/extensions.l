// --- common methods ---

/// A generic extender
public extension objectOp
{
    __class
        => system'core_routines'__object.

    __parent
        => system'core_routines'__object.

    bool instanceOf:type
        => system'core_routines'__object.        

    literal __className
    [
        dirty_ptr c := self~objectOp __class.
        
        T<byte,256> dump.
        int         len := system'external LoadClassName(c, T<bytearray>(dump), 255).
         
        if (len == 0)
            [ ^ "Unknown class" ].
        
        ^ LiteralValue fromByteArray(0,len,T<ByteArray>(dump)).
    ]    
    
    /// Returns true if the specified object instances are equal; otherwise, false.
    bool equalReference:anObject
        => system'core_routines'__object.
        
//    evalSelf func1:aFunction
//        = aFunction eval:self.
//
//    evalSelf:aFunction
//        = aFunction eval:self.
//
//    /// Returns true if the object is nil
//    bool isNil
//        = nil == self.
//        
//    /// Returns true if the object is literal or character
//    isLiteral
//    [
//        var o := self literal \ back:$nil.
//        if ($nil == o)
//        [
//            o := self wide \ back:$nil.
//            if ($nil == o)
//            [
//                o := self char \ back:$nil.
//                
//                if ($nil == o)
//                [
//                    ^ false
//                ]
//            ]
//        ].
//        
//        ^ true.
//    ]
//    
//    /// Returns true if the object is integer numereric
//    isInteger
//    [
//        var o := self int \ back:$nil.
//        if ($nil == o)
//        [
//            o := self long \ back:$nil.
//            if ($nil == o)
//            [
//                o := self short \ back:$nil.
//                
//                if ($nil == o)
//                [
//                    o := self byte \ back:$nil.
//                    
//                    if ($nil == o)
//                    [
//                        ^ false
//                    ]
//                ]
//            ]
//        ].
//        
//        ^ true.
//    ]
//    
//    /// Returns true if the object is numereric
//    isNumeric
//    [
//        var o := self int \ back:$nil.
//        if ($nil == o)
//        [
//            o := self long \ back:$nil.
//            if ($nil == o)
//            [
//                o := self short \ back:$nil.
//                
//                if ($nil == o)
//                [
//                    o := self byte \ back:$nil.
//                    
//                    if ($nil == o)
//                    [
//                        o := self real \ back:$nil.
//                        
//                        if ($nil == o)
//                        [
//                            ^ false
//                        ]
//                    ]
//                ]
//            ]
//        ].
//        
//        ^ true.
//    ]
        
    bool isEmpty
        = self length == 0.
        
    bool isNonempty
        = self length != 0.
        
//    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
////    bool safeEqual : anObject
////        = anObject cast:%eval to:{ generic eval:aTarget:anObject = aTarget~$subject equal:anObject \ back:false. } with:self.
//
//    doWith:action
//        = action eval:self.
//        
//    stacksafe reallocate int:aNewLength
//        => system'core_routines'__array.
        
    shift(int n)
    [
        if (n < 0)
            [ ^ self shiftLeft(n negative) ];
            [ ^ self shiftRight(n) ].
    ]
}

// --- Operations ---

// --- byteOp ---

extension<byte> byteOp
{
    stacksafe byte absolute
    [
        byte n := self.
        if (n < 0)
            [ n := n * -1 ].
                
        ^ ByteNumber new(n)
    ]
    
    byte mod : anOperand
    [
        var retVal := self / anOperand * anOperand.
        
        ^ byteConvertor convert(retVal)
    ]    
    
    stacksafe byte power(int y)
    [
        byte x := self.
        byte r := 1.
        
        int i := 0.
        while (i < y)
        [
            r := r * x.
            
            i += 1
        ].
        
        ^ ByteNumber new(r)
    ]
    
    stacksafe byte sqr
    [
        byte aRetVal := 0.
        
        aRetVal := self * self.
        
        ^ ByteNumber new(aRetVal)
    ]

    power : aPower
        = self~byteOp power(aPower).
                
    bool isOdd
        = self~byteOp mod:2 != 0.
        
    bool isEven
        = self~byteOp mod:2 == 0.
        
    stacksafe bool isZero
        = self equal(0).
        
    stacksafe bool isPositive
        = self > 0.
        
    stacksafe bool isNegative
        = self < 0.
        
    stacksafe bool isNonnegative
        = self <= 0.
}

// --- shortOp ---

extension<short> shortOp
{
    stacksafe short absolute
    [
        short n := self.
        if (n < 0)
            [ n := n * -1 ].
                
        ^ ShortNumber new(n)
    ]
    
    short mod : anOperand
    [
        var retVal := self / anOperand * anOperand.
        
        ^ shortConvertor convert(retVal)
    ]
    
    stacksafe short power(int y)
    [
        short x := self.
        short r := 1.
        
        int i := 0.
        while (i < y)
        [
            r := r * x.
            
            i += 1.
        ].
        
        ^ ShortNumber new(r)
    ]
    
    stacksafe short sqr
    [
        short n := self.
        short aRetVal := 0.
        
        aRetVal := n * n.
        
        ^ ShortNumber new(aRetVal)
    ]

    power : aPower
        = self~shortOp power(aPower).
                
    bool isOdd
        = self~shortOp mod:2 != 0.
        
    bool isEven
        = self~shortOp mod:2 == 0.
        
    stacksafe bool isZero
        = self equal(0).
        
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
        aWord := aWord shiftRight(8).
        
        ^ byteConvertor convert(aWord)
    ]
        
    stacksafe byte low
    [
        short aWord := self.
        aWord := aWord && 0FFh.
        
        ^ byteConvertor convert(aWord)
    ]
}

// --- intOp ---

extension<int> intOp
{
    int rvprop absolute :: absolute.
    
    int fnprop  mod      :: mod.
    
    int fnprop  power    :: power.
    
    int rvprop sqr      :: sqr.
    
    int rvprop sqrt     :: sqrt.
    
    stacksafe absolute(ref<int> retVal)
    [
        int n := self.

        if (n < 0)
            [ n := n * -1 ].

        retVal value := n
    ]
    
    stacksafe mod(int y, ref<int> retVal)
    [
        int x := self.
        
        int z := x / y.
        z := z * y.
        
        retVal value := (x - z)
    ]
    
    int mod : anOperand
    [
        int retVal := self mod(T<int>(anOperand)).
        
        ^ retVal
    ]
    
    real realDiv : anOperand
        = RealNumber new(T<int>(self)); divide:anOperand.
    
    stacksafe power(int y, ref<int> retVal)
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
        
        retVal value := r
    ]
    
    stacksafe sqr(ref<int> retVal)
    [
        int n := self.        
        
        retVal value := n * n.
    ]
    
    stacksafe sqrt(ref<int> retVal)
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
        
        retVal value := n
    ]

    power : aPower
        = self~intOp power(T<int>(aPower)).
                
    stacksafe bool isOdd
        = self and(1) != 0.
        
    stacksafe bool isEven
        = self and(1) == 0.
        
    stacksafe bool isZero
        = self equal(0).
        
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
        
        ^ shortConvertor convert(aWord)
    ]

    low
    [
        int aWord := self.
        aWord := aWord && 0FFFFh.
        
        ^ shortConvertor convert(aWord)
    ]
    
    shift : n
    [
        if (n < 0)
            [ ^ self shiftLeft(T<int>(n) negative) ];
            [ ^ self shiftRight(T<int>(n)) ].
    ]            
}

// --- longOp ---

extension<long> longOp
{
    stacksafe long absolute
    [
        long n := self.

        if (n < 0l)
            [ n := n * -1l ].
                
        ^ LongNumber new(n)
    ]
    
    long mod : anOperand
    [
        long x := self.
        long y := T<long>(anOperand).
        
        long z := x / y.
        z := z * y.
        
        ^ LongNumber new(x - z)
    ]
    
    stacksafe long power(long y)
    [
        long x := self.
        long r := 1.
        
        long i := 0l.
        while (i < y)
        [
            r := r * x.
            
            i += 1l.
        ].
        
        ^ LongNumber new(r)
    ]
    
    stacksafe long power(int y)
    [
        long x := self.
        long r := 1.
        
        int i := 0.
        while (i < y)
        [
            r := r * x.
            
            i += 1.
        ].
        
        ^ LongNumber new(r)
    ]
    
    stacksafe long sqr
    [
        long n := self.        
        long aRetVal := 0l.
        
        aRetVal := n * n.
        
        ^ LongNumber new(aRetVal)
    ]

    power : aPower
        = self~longOp power(T<long>(aPower)).
                
    bool isOdd
        = self~longOp mod:2l != 0.
        
    bool isEven
        = self~longOp mod:2l == 0.
        
    stacksafe bool isZero
        = self equal(0l).
        
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
        
        ^ intConvertor convert(aWord)
    ]

    stacksafe int low
    [
        long aWord := self.
        aWord := aWord && 4294967295l.
        
        ^ intConvertor convert(aWord)
    ]
}

// --- realOp ---

extension<real> realOp
{        
    stacksafe real absolute
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController abs(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]
        
    stacksafe real exp
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController exp(n, &aRetVal).
        
        ^ RealNumber new(aRetVal).
    ]
    
    stacksafe real ln
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController ln(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]
    
    stacksafe real sqrt
    [
        real n := self.      
        real aRetVal := 0.0r.
        
        realOpController sqrt(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]

    stacksafe real power(real y)
    [
        real x := self.
        
        (x == 0.0r)
            ? [ ^ 0.0r ].
                        
        (y == 0.0r)
            ? [ ^ 1.0r ].
            
        realOpController ln(x, &x).
        
        x := x multiply(y).
        
        realOpController exp(x, &x).
            
        ^ RealNumber new(x)
            
    ]

    stacksafe real power(int y)
    [
        real x := self.

        (x == 0.0r)
            ? [ ^ 0.0r ].
                        
        (y == 0)
            ? [ ^ 1.0r ].
                        
        if (x < 0.0r)
            [
                x := x * -1.0r.
                
                realOpController ln(x, &x).
                
                x := x multiply(y).
                
                realOpController exp(x, &x).
                
                (y~intOp isOdd)
                    ? [ x := x * -1.0r ].
            ];
            [
                realOpController ln(x, &x).
                
                x := x multiply(y).
                
                realOpController exp(x, &x)
            ].
        
        ^ RealNumber new(x)
    ]

    power : aPower
        = self~realOp power(T<int>(aPower)).

    stacksafe real sqr
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        aRetVal := n * n.
        
        ^ RealNumber new(aRetVal)
    ]

    stacksafe real rounded
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController round(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]
    
    stacksafe real round(int precision)
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
    
        ^neg iif(a * -1.0r, a). 
    ]
    
    stacksafe real truncate(int precision)
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

        realOpController truncate(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]

    stacksafe real frac
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController abs(n, &aRetVal).
        
        ^ RealNumber new(n - aRetVal)
    ]

    stacksafe real sin
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController sin(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]

    stacksafe real cos
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController cos(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]

    stacksafe real tan
    [
        real n := self.        
        real aSinVal := 0.0r.
        real aCosVal := 0.0r.
        
        realOpController sin(n, &aSinVal).
        realOpController cos(n, &aCosVal).
        
        ^ RealNumber new(aSinVal / aCosVal)
    ]

    stacksafe real arctan
    [
        real n := self.        
        real aRetVal := 0.0r.
        
        realOpController arctan(n, &aRetVal).
        
        ^ RealNumber new(aRetVal)
    ]
            
    stacksafe real arcsin
    [
        real n := self.        
        real arg := 0.0r.
        real retVal := 0.0r.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt(retVal, &arg).
        
        arg := n / arg.        
        realOpController arctan(arg, &retVal).
        
        ^ RealNumber new(retVal)
    ]
            
    stacksafe real arccos
    [
        real n := self.        
        real arg := 0.0r.
        real retVal := 0.0r.
        
        retVal := n * n.        
        retVal := 1.0r - retVal.        
        realOpController sqrt(retVal, &arg).
        
        arg := arg / n.        
        realOpController arctan(arg, &retVal).
        
        ^ RealNumber new(retVal)
    ]
            
    stacksafe real radian
    [
        real aValue := self.
        
        aValue := aValue * pi_value.
        
        ^ RealNumber new(aValue / 180.0r)
    ]
            
    stacksafe real degree
    [
        real aValue := self.
        
        aValue := aValue * 180.0r.
        
        ^ RealNumber new(aValue / pi_value)
    ]
            
    stacksafe real reciprocal
    [
        real aValue := self.
        
        ^ RealNumber new(1.0r / aValue)
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

//// --- intArrayOp ---
//
//intarray extension intArrayOp
//{
//    stacksafe exchange int:anIndex1 int:anIndex2
//    [
//        int n1 := 0.
//        int n2 := 0.
//
//        self read int:anIndex1 vint:n1.
//        self read int:anIndex2 vint:n2.
//        
//        self write int:anIndex1 int:n2.
//        self write int:anIndex2 int:n1.
//    ]
//
//    populate nfunc:anIndexFunc
//    [
//        int anIndex := 0.
//        int aLength := self length.
//        
//        while (anIndex < aLength)
//        [            
//            self write int:anIndex int(anIndexFunc eval int:anIndex).
//            
//            anIndex := anIndex + 1
//        ].
//        
//        ^ self
//    ]
//    
//    populate(BaseFunction1 aFunction)
//        = self~intArrayOp populate(:n)<int>[ ^aFunction eval:n ].    
//}

// --- arrayOp ---

extension<array> arrayOp
{
//    exchange : anIndex1 : anIndex2
//        = self~arrayOp exchange int:anIndex1 int:anIndex2.
        
    exchange(int anIndex1, int anIndex2)
    [
        var aTemp := self getAt(anIndex1).
        
        self setAt(anIndex1, self getAt(anIndex2)).
        self setAt(anIndex2, aTemp).
    ]
    
//    stacksafe indexOf int:anIndex object:anObject vint:aRetVal
//    [
//        aRetVal int := -1.
//        
//        int aLength := self length.
//        
//        int i := anIndex.
//        while(i < aLength)
//        [
//            var anArrayItem := self getAt int:i.
//            
//            if (anObject == anArrayItem)
//                [ aRetVal int := i ].
//            
//            i := i + 1
//        ]
//    ]        
//        
//    stacksafe indexOf int:anIndex object:anObject
//    [
//        int aRetVal := 0.
//        
//        self~arrayOp indexOf int:anIndex object:anObject vint:aRetVal.
//        
//        ^ IntNumber new int:aRetVal
//    ]
//    
//    indexOf : anObject
//        = self~arrayOp indexOf int:0 object:anObject.
//        
////    safeSeek : aValue
////    [
////        int aLength := self length.
////        
////        int i := 0.
////        while (i < aLength)
////        [
////            var anArrayItem := self getAt int:i.
////            
////            if (aValue safeEqual:anArrayItem)
////                [ ^ anArrayItem ].
////            
////            i := i + 1
////        ].
////        
////        ^ $nil
////    ]
//        
//    deepClone
//    [
//        int aLength := self length.
//        
//        array aCopy := Array new int:aLength.
//                
//        int anIndex := 0.
//        while(anIndex < aLength)
//        [
//            aCopy setAt int:anIndex object:(self getAt int:anIndex; clone).
//            
//            anIndex := anIndex + 1.
//        ].
//        
//        ^ self.
//    ]
    
    populate(Func<int,object> anIndexFunc)
    [
        int anIndex := 0.
        int aLength := self length.
        
        while(anIndex < aLength)
        [            
            self setAt(anIndex, anIndexFunc(anIndex)).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self
    ]
    
//    populate(BaseFunction1 aFunction)
//        = self~arrayOp populate(:n)<int>[ ^aFunction eval:n ].
//
//    multi populate : anAction
//        = self~arrayOp populate(:n)<int>[ ^anAction eval:n ].    
//    
//    sort func2:aCompareFunc
//    [
//        int i := 0.
//        int j := 0.
//        int aLength := self length.
//        var aCurrent := $nil.
//        var aNext := $nil.
//        
//        while(i < aLength)
//        [
//            aCurrent := self getAt int:i.
//            
//            j := i + 1.
//            while (j < aLength)
//            [
//                aNext := self getAt int:j.
//                ifnot(aCompareFunc eval(aCurrent,aNext))
//                    [                        
//                        self setAt int:i object:aNext.
//                        self setAt int:j object:aCurrent.
//                        
//                        aCurrent := aNext
//                    ].
//                    
//                j := j + 1
//            ].
//            
//            i := i + 1
//        ].
//        
//        ^ self
//    ]
//    
//    ascendant
//        = (self clone)~arrayOp sort:ifOrdered.
//    
//    bool isEmpty
//    [
//        int aLength := self length.
//        
//        ^ aLength == 0.
//    ]
//    
//    array Subarray : aLength at:anIndex
//        = self~arrayOp Subarray int:anIndex int:aLength.
//    
//    array Subarray int:anIndex int:aLength
//    [
//        indexer it := self indexer.
//        
//        ^ Array new int:aLength; populate(:i)<int>(it writeIndex int(anIndex + i); get).
//    ]
}

//// --- intmatrixOp ---
//
//intmatrix extension intmatrixOp
//{
//    populate nnfunc:func
//    [
//        int n := 0.
//        int m := 0.
//        self readLengthsTo vint:n vint:m.
//        
//        int i := 0.
//        int j := 0.
//        while (i < n)
//        [
//            j := 0.
//            while (j < m)
//            [
//                int v := func eval int:i int:j.
//                
//                self write int:i int:j int:v.
//                
//                j := j + 1
//            ].
//            
//            i := i + 1
//        ].
//        
//        ^ self
//    ]
//}

// --- literalOp ---

extension<literal> literalOp
{
    indexOf : anObject
        = self indexOf:anObject at:0.

    indexOf(literal aLiteral)
        = self indexOf:aLiteral at:0.

    indexOf(char aChar)
        = self indexOf:aChar at:0.
        
    literal Substring : anIndex
        = self~literalOp Substring(T<int>(anIndex)).
        
    stacksafe literal Substring(int anIndex)
    [
        int aLength := self length.
        if (aLength > anIndex)
        [
            aLength := aLength - anIndex.
            
            ^ self Substring(anIndex, aLength)
        ];
        [
            ^ emptyLiteralValue
        ]        
    ]        
    
    literal trimLeft(char aChar)
    [
        int  anIndex := 0.
        int  aLength := self length.
        char aCurrent := $0.
        int  aCharLen := aChar length.
        
        while(anIndex < aLength)
        [
            self read(anIndex, &aCurrent).
            
            ifnot(aCurrent == aChar)
                [ 
                    if (anIndex == 0)
                        [ ^ self ];
                        [ ^ self delete(0, anIndex) ]
                ].
            
            anIndex := anIndex + aCharLen
        ].
        
        ^ emptyLiteralValue
    ]
    
    literal trimLeft
        = self~literalOp trimLeft($32).
       
    literal trimRight(char aChar)
    [
        int  aLength := self length.
        int  anIndex := aLength - 1.
        int  aPrevious := aLength.
        char aCurrent := $0.
        int  aCharLen := aChar length.
        
        while (anIndex >= 0)
        [
            self read(anIndex, &aCurrent).
            
            ifnot (aCurrent equal(aChar))
                [ ^ self delete(aPrevious,aLength - aPrevious) ].
            
            aPrevious := anIndex.
            anIndex := anIndex - aCharLen
        ].
        
        ^ emptyLiteralValue
    ]
    
    literal trimRight
        = self~literalOp trimRight($32).
        
    literal trim(char aChar)
        = self~literalOp trimLeft(aChar); trimRight(aChar).
    
    literal trim
        = self~literalOp trimLeft($32); trimRight($32).
    
    padLeft(int aLength)
        = self padLeft($32, aLength).
    
    padLeft(char aChar, int aLength)
    [
        int aCurrentLength := self length.
        
        if (aCurrentLength < aLength)
            [ ^ LiteralValue new(aLength - aCurrentLength, aChar); add(self) ].
            
        ^ self
    ]
    
    padRight(int aLength)
        = self padRight($32, aLength).
    
    padRight(char aChar, int aLength)
    [
        int aCurrentLength := self length.

        if (aCurrentLength < aLength)
            [ ^ self add(LiteralValue new(aLength - aCurrentLength, aChar)) ].
            
        ^ self
    ]

    bool startingWith(literal s)
    [
        int n := 0.
        self indexOf(0, s, &n).
        
        ^ 0 == n
    ]
        
    bool endingWith(literal s)
    [
        int l1 := self length.
        int l2 := s length.

        int n := 0.
        self indexOf(l1 - l2, s, &n).
        
        ^ n != -1
    ]
        
    bool containing(literal s)
    [
        int l1 := self length.
        int l2 := s length.

        int n := 0.
        self indexOf(0, s, &n).
        
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
    
    replace(literal aReplacee, literal aReplacer)
    [
        literal aTarget := emptyLiteralValue.
        int     aStart := 0.
        int     anEnd := 0.
        int     aReplaceeLen := aReplacee length.
        
        self indexOf(aStart, aReplacee, &anEnd).
        
        while (anEnd != -1)
        [
            aTarget := aTarget
                add(self Substring(aStart, anEnd - aStart));
                add(aReplacer).
            
            aStart := anEnd + aReplaceeLen.
            
            self indexOf(aStart, aReplacee, &anEnd)
        ].
    
        (aStart != 0)
            ? [ ^ aTarget add(self~literalOp Substring(aStart)) ]
            ! [ ^ self ]
    ]
    
    bool isEmpty
    [
        int aLength := self length.
        
        ^ aLength == 0
    ]    
}

// --- wideOp ---

public extension<wide> wideOp
{
    indexOf : anObject
        = self indexOf:anObject at:0.
        
    indexOf(wide aLiteral)
        = self indexOf:aLiteral at:0.

    indexOf(char aChar)
        = self indexOf:aChar at:0.
                
    wide Substring:anIndex
        = self~wideOp Substring(T<int>(anIndex)).
        
    stacksafe wide Substring(int anIndex)
    [
        int aLength := self length.
        
        aLength := aLength - anIndex.
        
        ^ self Substring(anIndex, aLength)
    ]
     
    wide trimLeft(char aChar)
    [
        int  anIndex := 0.
        int  aLength := self length.
        char aCurrent := $0.
        int  aCharLen := aChar length.
        
        while (anIndex < aLength)
        [
            self read(anIndex, &aCurrent).
            
            ifnot(aCurrent equal(aChar))
                [ ^ self delete(0, anIndex) ].
            
            anIndex := anIndex + aCharLen
        ].
        
        ^ emptyWideLiteralValue
    ]
    
    wide trimLeft
        = self~wideOp trimLeft($32).
       
    wide trimRight(char aChar)
    [
        int  aLength := self length.
        char aCurrent := $0.
        int  aCharLen := aChar length.
                
        int  anIndex := aLength - 1.
        int  aPrevious := aLength.
        while (anIndex >= 0)
        [
            self read(anIndex, &aCurrent).
            
            ifnot (aCurrent equal(aChar))
                [ ^ self delete(aPrevious, aLength - aPrevious) ].
            
            aPrevious := anIndex.
            anIndex := anIndex - aCharLen
        ].
        
        ^ emptyWideLiteralValue
    ]
    
    wide trimRight
        = self~wideOp trimRight($32).
       
    wide trim(char aChar)
        = self~wideOp trimLeft(aChar); trimRight(aChar).
    
    wide trim
        = self~wideOp trimLeft($32); trimRight($32).
    
    padLeft(char aChar, int aLength)
    [
        int aCurrentLength := self length.
        
        if (aCurrentLength < aLength)
            [ ^ WideLiteralValue new(aLength - aCurrentLength, aChar); add(self) ].
            
        ^ self
    ]
    
    padRight(char aChar, int aLength)
    [
        int aCurrentLength := self length.

        if (aCurrentLength < aLength)
            [ ^ self add(WideLiteralValue new(aLength - aCurrentLength, aChar)) ].
            
        ^ self
    ]

    bool startingWith(wide s)
    [
        int n := 0.
        self indexOf(0, s, &n).
        
        ^ 0 == n
    ]
    
    bool endingWith(wide s)
    [
        int l1 := self length.
        int l2 := s length.
        int n := 0.
        self indexOf(l1 - l2, s, &n).
        
        ^ n != -1
    ]
    
    bool containing(wide s)
    [
        int l1 := self length.
        int l2 := s length.
        int n := 0.
        
        self indexOf(0, s, &n).
        
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
    replace(wide aReplacee, wide aReplacer)
    [
        wide aTarget := emptyWideLiteralValue.
        int  aStart := 0.
        int  anEnd := 0.
        int  aReplaceeLen := aReplacee length.
        
        self indexOf(aStart, aReplacee, &anEnd).
        
        while (anEnd != -1)
        [
            aTarget := aTarget
                add(self Substring(aStart, anEnd - aStart));
                add:aReplacer.
            
            aStart := anEnd + aReplaceeLen.
            
            self indexOf(aStart, aReplacee, &anEnd)
        ].
    
        if (aStart != 0)
            [ ^ aTarget add(self~wideOp Substring(aStart)) ];
            [ ^ self ].
    ]

    bool isEmpty
    [
        int aLength := self length.
        
        ^ aLength == 0
    ]
}

// --- stringOp ---

singleton stringOpDispatcher
{
    padRight(literal s, char ch, int len)
        = s~literalOp padRight(ch, len).
    
    padRight(wide s, char ch, int len)
        = s~wideOp padRight(ch, len).
        
    padLeft(literal s, char ch, int len)
        = s~literalOp padLeft(ch, len).
    
    padLeft(wide s, char ch, int len)
        = s~wideOp padLeft(ch, len).
        
    bool startingWith(literal s, literal subs)
        = s~literalOp startingWith(subs).

    bool startingWith(wide s, wide subs)
        = s~wideOp startingWith(subs).

    bool endingWith(literal s, literal subs)
        = s~literalOp endingWith(subs).

    bool endingWith(wide s, wide subs)
        = s~wideOp endingWith(subs).
        
    bool containing(literal s, literal subs)
        = s~literalOp containing(subs).

    bool containing(wide s, wide subs)
        = s~wideOp containing(subs).
        
    trimLeft(literal s, char ch)
        =s~literalOp trimLeft(ch).        
        
    trimRight(literal s, char ch)
        =s~literalOp trimRight(ch).
        
    trim(literal s, char ch)
        =s~literalOp trim(ch).
        
    trimLeft(wide s, char ch)
        =s~wideOp trimLeft(ch).        
        
    trimRight(wide s, char ch)
        =s~wideOp trimRight(ch).        
        
    trim(wide s, char ch)
        =s~wideOp trim(ch).
}

extension stringOp
{
    Substring : anIndex
        = self Substring:(self length - anIndex) at:anIndex.
            
    Substring(object anIndex, object aLength)
        = self Substring:aLength at:anIndex.
    
    trimLeft :ch
        = stringOpDispatcher trimLeft(self, ch).

    trimRight :ch
        = stringOpDispatcher trimRight(self, ch).

    trim:ch
        = stringOpDispatcher trim(self, ch).
    
    trimLeft
        = stringOpDispatcher trimLeft(self, $32).

    trimRight
        = stringOpDispatcher trimRight(self, $32).

    trim
        = stringOpDispatcher trim(self, $32).

//    padLeft : padSymbol : len
//        = $stringOpDispatcher padLeft(self, padSymbol, len).
//        
//    padRight : padSymbol : len
//        = $stringOpDispatcher padRight(self, padSymbol, len).
//
//    startingWith : s
//        = $stringOpDispatcher startingWith(self, s).
//
//    endingWith : s
//        = $stringOpDispatcher endingWith(self, s).
//
//    containing : s
//        = $stringOpDispatcher containing(self, s).
//
////    replaceFirst literal:aReplacee literal:aReplacer
////    [
////        var dispatcher := self cast:%eval to:stringOpDispatcher.
////        
////        ^ self~dispatcher replaceFirst literal:aReplacee literal:aReplacer
////    ]
////    
////    replace literal:aReplacee literal:aReplacer
////    [
////        var dispatcher := self cast:%eval to:stringOpDispatcher.
////        
////        ^ self~dispatcher replace literal:aReplacee literal:aReplacer
////    ]
//    
//    stacksafe write object:aLiteral int:aCounter
//    [
//        int aNumber := aCounter.
//        
//        while (aNumber > 0)
//        [
//            self write:aLiteral. 
//            
//            aNumber := aNumber - 1
//        ].
//            
//        ^ self
//    ]
//
//    write : aLine length:aCounter
//        = self~stringOp write object:aLine int:aCounter.
//        
//    write : aLine paddingLeft:aWidth
//        = self~stringOp write:aLine paddingLeft:aWidth with:$32.
//
//    write : aLine paddingLeft:aWidth with:aChar
//    [
//        var aPaddedStr := aLine padLeft(aChar, aWidth).
//
//        self write:aPaddedStr.
//            
//        ^ self
//    ]
//
//    write : aLine paddingRight:aWidth
//        = self~stringOp write:aLine paddingRight:aWidth with:$32.
//
//    write : aLine paddingRight:aWidth with:aChar
//    [
//        var aPaddedStr := aLine padRight(aChar, aWidth).
//
//        self write:aPaddedStr.
//            
//        ^ self
//    ]
//    
//    write : aLine padding:aWidth with:aChar
//    [
//        int aWidthInt := aWidth int.
//        int aLength := aLine length.        
//        
//        if (aLength < aWidthInt)
//            [
//                int aLen := aWidthInt - aLength.
//                int aLeftLen := aLen / 2.
//                
//                self~stringOp write object:aChar int:aLeftLen.
//                self write:aLine.
//                self~stringOp write object:aChar int:(aLen - aLeftLen)
//            ];
//            [ self write:aLine ].
//            
//        ^ self
//    ]    
}

// --- charOp ---

public extension<char> charOp
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
        intConvertor convert(self, &ch).
        
        ^ (ch >= 30h) and: (ch <= 39h)
    ]
    
    stacksafe bool isNewLine
    [
        ^ self == $10
    ]
    
    stacksafe bool isLetter
    [
        int ch := 0.
        intConvertor convert(self, &ch).
        
        if ((ch >= 41h) && (ch <= 5Ah))
            [ ^ true ].
        
        if ((ch >= 61h) && (ch <= 7Ah))
            [ ^ true ].
        
        if ((ch >= 3ACh) && (ch <= 3CEh))
            [ ^ true ].
            
        if ((ch >= 400h) && (ch <= 42Fh))
            [ ^ true ].
            
        ^ false
    ]
    
    stacksafe bool isLToken
    [
        int ch := 0.
        intConvertor convert(self, &ch).
        
        if ((ch >= 41h) && (ch <= 5Ah))
            [ ^ true ].
        
        if ((ch >= 61h) && (ch <= 7Ah))
            [ ^ true ].
        
        if ((ch >= 3ACh) && (ch <= 3CEh))
            [ ^ true ].
            
        if ((ch >= 400h) && (ch <= 42Fh))
            [ ^ true ].
            
        ^ ch == 39
    ]
}

// --- byteArrayHelper ---

singleton byteArrayHelper
{
    stacksafe read(bytearray array, int offset, ref<short> retVal)
    [
        array read(offset, retVal)
    ]
    
    stacksafe read(bytearray array, int offset, ref<int> retVal)
    [
        array read(offset, retVal)
    ]
    
    stacksafe read(bytearray array, int offset, ref<long> retVal)
    [
        array read(offset, retVal)
    ]
    
    stacksafe read(bytearray array, int offset, ref<real> retVal)
    [
        array read(offset, retVal)
    ]
    
    stacksafe read(bytearray array, int offset, ref<char> retVal)
    [
        array read(offset, retVal)
    ]
    
    stacksafe write(bytearray array, int offset, short retVal)
    [
        array write(offset, retVal)
    ]
    
    stacksafe write(bytearray array, int offset, int retVal)
    [
        array write(offset, retVal)
    ]
    
    stacksafe write(bytearray array, int offset, long retVal)
    [
        array write(offset, retVal).
    ]
    
    stacksafe write(bytearray array, int offset, real retVal)
    [
        array write(offset, retVal)
    ]
    
    stacksafe write(bytearray array, int offset, char retVal)
    [
        array write(offset, retVal)
    ]
    
    stacksafe fill(bytearray array, int offset, int length, byte val)
    [
        array fill(offset, length, val).
    ]
}

//// --- intArrayHelper ---
//
//singleton intArrayHelper
//{
//    stacksafe fill intarray:array int:offset int:length int:val
//    [
//        array fill int:offset int:length int:val.
//    ]
//}