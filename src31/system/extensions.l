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

    literal className
    [
        var c := self~objectOp class_ref.
        
        byte aBuffer(256).
        int  aLength := system'external LoadClassName 
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
        
    /// Returns the parameter        
    back : anObject = anObject.

    evalSelf func1:aFunction
        = aFunction eval:self.

    evalSelf:aFunction
        = aFunction eval:self.

    /// Returns true if the object is nil
    bool is nil
        = nil == self.
        
    /// Returns true if the object is literal or character
    is literal
        = self cast:%eval to:{ literal : anObject = true. wide : anObject = true. char : anObject = true. generic eval : anObject = false. }.
        
    /// Returns true if the object is integer numereric
    is integer
        = self cast:%eval to:{ int : x = true. long : x = true. short : x = true. byte : x = true. generic eval : anObject = false. }.
        
    /// Returns true if the object is numereric
    is numeric
        = self cast:%eval to:{ int : x = true. long : x = true. real : x = true. short : x = true. byte : x = true. generic eval : anObject = false. }.
        
    bool is empty
        = self length == 0.
        
    bool is nonempty
        = self length != 0.
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    bool safeEqual : anObject
        = anObject cast:%eval to:{ generic eval:aTarget:anObject = aTarget~$subject equal:anObject \ back:false. } with:self.

    eval with:action
        = action eval:self.
        
    stacksafe reallocate int:aNewLength
        => system'core_routines'__array.
        
    shift : n
    [
        if (n < 0)
            [ ^ self shiftLeft int:(n int; negative) ];
            [ ^ self shiftRight int:(n int) ].
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
        
        ^ retVal cast:%convertor to:byteConvertor
    ]    
    
    stacksafe byte power byte:y
    [
        byte x := self.
        
        byte i := 0.
        while (i < y)
        [
            x := x * x.
        ].
        
        ^ ByteNumber new byte:x
    ]
    
    stacksafe byte sqr
    [
        byte aRetVal := 0.
        
        aRetVal := self * self.
        
        ^ ByteNumber new byte:aRetVal
    ]

    power : aPower
        = self~byteOp power byte:aPower.
                
    bool is odd
        = self~byteOp mod:2 != 0.
        
    bool is even
        = self~byteOp mod:2 == 0.
        
    stacksafe bool is zero
        = self equal int:0.
        
    stacksafe bool is positive
        = self > 0.
        
    stacksafe bool is negative
        = self < 0.
        
    stacksafe bool is nonnegative
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
        
        ^ retVal cast:%convertor to:shortConvertor
    ]
    
    stacksafe short power short:y
    [
        short x := self.
        
        short i := 0.
        while (i < y)
        [
            x := x * x
        ].
        
        ^ ShortNumber new short:x
    ]
    
    stacksafe short sqr
    [
        short n := self.
        short aRetVal := 0.
        
        aRetVal := n * n.
        
        ^ ShortNumber new short:aRetVal
    ]

    power : aPower
        = self~shortOp power short:aPower.
                
    bool is odd
        = self~shortOp mod:2 != 0.
        
    bool is even
        = self~shortOp mod:2 == 0.
        
    stacksafe bool is zero
        = self equal int:0.
        
    stacksafe bool is positive
        = self > 0.
        
    stacksafe bool is negative
        = self < 0.
        
    stacksafe bool is nonnegative
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
    int absolute :: outnprop(absolute).
    
    int mod      :: nfn(mod).
    
    int power    :: nfn(power).
    
    int sqr      :: outnprop(sqr).
    
    int sqrt     :: outnprop(sqrt).
    
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
                
    stacksafe bool is odd
        = self and int:1 != 0.
        
    stacksafe bool is even
        = self and int:1 == 0.
        
    stacksafe bool is zero
        = self equal int:0.
        
    stacksafe bool is positive
        = self > 0.
        
    stacksafe bool is negative
        = self < 0.
        
    stacksafe bool is nonnegative
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
        
        long i := 0l.
        while (i < y)
        [
            x := x * x.
        ].
        
        ^ LongNumber new long:x
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
                
    bool is odd
        = self~longOp mod:2l != 0.
        
    bool is even
        = self~longOp mod:2l == 0.
        
    stacksafe bool is zero
        = self equal long:0l.
        
    stacksafe bool is positive
        = self > 0l.
        
    stacksafe bool is negative
        = self < 0l.
        
    stacksafe bool is nonnegative
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
                
                (y~intOp is odd)
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
            
    stacksafe bool is zero
        = self == 0.0r.
        
    stacksafe bool is positive
        = self > 0.0r.
        
    stacksafe bool is negative
        = self < 0.0r.
        
    stacksafe bool is nonnegative
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

    set every:anIndexFunc
        = self~intArrayOp set nfunc:anIndexFunc.
        
    set nfunc:anIndexFunc
    [
        int anIndex := 0.
        int aLength := self length.
        
        while (anIndex < aLength)
        [            
            self write int:anIndex int:(anIndexFunc eval int:anIndex).
            
            anIndex := anIndex + 1
        ].
        
        ^ self
    ]
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
        
    safeSeek : aValue
    [
        int aLength := self length.
        
        int i := 0.
        while (i < aLength)
        [
            var anArrayItem := self getAt int:i.
            
            if (aValue safeEqual:anArrayItem)
                [ ^ anArrayItem ].
            
            i := i + 1
        ].
        
        ^ $nil
    ]
        
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

// --- intmatrixOp ---

intmatrix extension intmatrixOp
{
    set every_ij:func
        = self set nnfunc:func.
    
    set nnfunc:func
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
        = self indexOf literal:aLiteral at:0.

    indexOf char:aChar
        = self indexOf char:aChar at:0.
        
    literal Substring at:anIndex
        = self~literalOp Substring int:anIndex.
        
    stacksafe literal Substring int:anIndex
    [
        int aLength := self length.
        
        aLength := aLength - anIndex.
        
        ^ self Substring int:anIndex int:aLength
    ]        
    
    trimLeft char:aChar
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
    
    trimLeft
        = self~literalOp trimLeft char:$32.    
       
    trimRight char:aChar
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
    
    trimRight
        = self~literalOp trimRight char:$32.
        
    trim char:aChar
        = self~literalOp trimLeft char:aChar; trimRight char:aChar.
    
    trim
        = self~literalOp trimLeft char:$32; trimRight char:$32.        
    
    padLeft char:aChar int:aLength
    [
        int aCurrentLength := self length.
        
        if (aCurrentLength < aLength)
            [ ^ LiteralValue new int:(aLength - aCurrentLength) char:aChar; add literal:self ].
            
        ^ self
    ]
    
    padRight char:aChar int:aLength
    [
        int aCurrentLength := self length.

        if (aCurrentLength < aLength)
            [ ^ self add literal:(LiteralValue new int:(aLength - aCurrentLength) char:aChar) ].
            
        ^ self
    ]

    startingWith:s
        = self~literalOp startingWith literal:s.
        
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

    endingWith:s
        = self~literalOp endingWith literal:s.
        
    containing literal:s
    [
        int l1 := self length.
        int l2 := s length.

        int n := 0.
        self indexOf int:0 literal:s vint:n.
        
        ^ n != -1
    ]

    containing:s
        = self~literalOp containing literal:s.
    
    replaceFirst literal:aReplacee literal:aReplacer
    [
        int aReplaceeLen := aReplacee length.
        int anIndex := 0.
        
        self indexOf int:0 literal:aReplacee vint:anIndex.
        
        if (anIndex != -1)
            [ ^ self delete int:anIndex int:aReplaceeLen; insert int:anIndex literal:aReplacee ];
            [ ^ self ].
    ]    
    
    replace literal:aReplacee literal:aReplacer
    [
        literal aTarget := emptyLiteralValue.
        int     aStart := 0.
        int     anEnd := 0.
        int     aReplaceeLen := aReplacee length.
        
        self indexOf int:aStart literal:aReplacee vint:anEnd.
        
        while (anEnd != -1)
        [
            aTarget := aTarget
                add literal:(self Substring int:aStart int:(anEnd - aStart));
                add literal:aReplacer.
            
            aStart := anEnd + aReplaceeLen.
            
            self indexOf int:aStart literal:aReplacee vint:anEnd
        ].
    
        (aStart != 0)
            ? [ ^ aTarget add literal:(self~literalOp Substring int:aStart) ]
            ! [ ^ self ]
    ]
    
    bool is empty
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
        = self indexOf int:0 wide:aLiteral.

    indexOf char:aChar
        = self indexOf int:0 char:aChar.
                
    wide Substring at:anIndex
        = self~wideOp Substring int:anIndex.
        
    stacksafe wide Substring int:anIndex
    [
        int aLength := self length.
        
        aLength := aLength - anIndex.
        
        ^ self Substring int:anIndex int:aLength
    ]
     
    trimLeft char:aChar
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
    
    trimLeft
        = self~wideOp trimLeft char:$32.
       
    trimRight char:aChar
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
    
    trimRight
        = self~wideOp trimRight char:$32.
       
    trim char:aChar
        = self~wideOp trimLeft char:aChar; trimRight char:aChar.
    
    trim
        = self~wideOp trimLeft char:$32; trimRight char:$32.
    
    padLeft char:aChar int:aLength
    [
        int aCurrentLength := self length.
        
        if (aCurrentLength < aLength)
            [ ^ WideLiteralValue new int:(aLength - aCurrentLength) char:aChar; add wide:self ].
            
        ^ self
    ]
    
    padRight char:aChar int:aLength
    [
        int aCurrentLength := self length.

        if (aCurrentLength < aLength)
            [ ^ self add wide:(WideLiteralValue new int:(aLength - aCurrentLength) char:aChar) ].
            
        ^ self
    ]

    startingWith wide:s
    [
        int n := 0.
        self indexOf int:0 wide:s vint:n.
        
        ^ 0 == n
    ]
    
    startingWith:s
        = self~wideOp startingWith wide:s.

    endingWith wide:s
    [
        int l1 := self length.
        int l2 := s length.
        int n := 0.
        self indexOf int:(l1 - l2) wide:s vint:n.
        
        ^ n != -1
    ]
    
    endingWith:s
        = self~wideOp endingWith wide:s.

    containing wide:s
    [
        int l1 := self length.
        int l2 := s length.
        int n := 0.
        
        self indexOf int:0 wide:s vint:n.
        
        ^ n != -1
    ]
    
    containing:s
        = self~wideOp containing wide:s.

    replaceFirst wide:aReplacee wide:aReplacer
    [
        int aReplaceeLen := aReplacee length.
        int anIndex := 0.
        
        self indexOf int:0 wide:aReplacee vint:anIndex.
        
        if (anIndex != -1)
            [ ^ self delete int:anIndex int:aReplaceeLen; insert int:anIndex wide:aReplacee ];
            [ ^ self ].
    ]
        
    replace wide:aReplacee wide:aReplacer
    [
        wide aTarget := emptyWideLiteralValue.
        int  aStart := 0.
        int  anEnd := 0.
        int  aReplaceeLen := aReplacee length.
        
        self indexOf int:aStart wide:aReplacee vint:anEnd.
        
        while (anEnd != -1)
        [
            aTarget := aTarget
                add wide:(self Substring int:aStart int:(anEnd - aStart));
                add wide:aReplacer.
            
            aStart := anEnd + aReplaceeLen.
            
            self indexOf int:aStart wide:aReplacee vint:anEnd
        ].
    
        if (aStart != 0)
            [ ^ aTarget add wide:(self~wideOp Substring int:aStart) ];
            [ ^ self ].
    ]

    bool is empty
    [
        int aLength := self length.
        
        ^ aLength == 0
    ]
}

// --- stringOp ---

class stringOpDispatcher =
{
    eval literal:aLiteral = literalOp.
    
    eval wide:aLiteral = wideOp.
}.

extension stringOp
{
    Substring : anIndex
        = self Substring:(self length - anIndex) at:anIndex.
            
    Substring : anIndex : aLength
        = self Substring:aLength at:anIndex.
            
    trimLeft char:aChar
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher trimLeft char:aChar
    ]

    trimRight char:aChar
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher trimRight char:aChar
    ]

    trim char:aChar
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher trim char:aChar
    ]

    trim
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher trim
    ]

    trimLeft
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher trimLeft
    ]

    trimRight
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher trimRight
    ]
    
    padLeft char:aChar length:aLength
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher padLeft char:aChar int:aLength
    ]
    
    padRight char:aChar length:aLength
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher padRight char:aChar int:aLength
    ]
    
    startingWith : s
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher startingWith:s
    ]
    
    endingWith : s
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher endingWith:s
    ]
    
    containing : s
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher containing:s
    ]
    
    replaceFirst literal:aReplacee literal:aReplacer
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher replaceFirst literal:aReplacee literal:aReplacer
    ]
    
    replace literal:aReplacee literal:aReplacer
    [
        var dispatcher := self cast:%eval to:stringOpDispatcher.
        
        ^ self~dispatcher replace literal:aReplacee literal:aReplacer
    ]
    
    write : aLiteral length:aCounter
        = self~stringOp write : aLiteral int:aCounter.
            
    stacksafe write : aLiteral int:aCounter
    [
        int aNumber := aCounter.
        
        while (aNumber > 0)
        [
            self write:aLiteral. 
            
            aNumber := aNumber - 1
        ].
            
        ^ self
    ]
    
    write : aLine paddingLeft:aWidth with:aChar
    [
        var aPaddedStr := aLine padLeft char:aChar length:aWidth.

        self write:aPaddedStr.
            
        ^ self
    ]

    write : aLine paddingRight:aWidth with:aChar
    [
        var aPaddedStr := aLine padRight char:aChar length:aWidth.

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
                
                self~stringOp write:aChar int:aLeftLen.
                self write:aLine.
                self~stringOp write:aChar int:(aLen - aLeftLen)
            ];
            [ self write:aLine ].
            
        ^ self
    ]    
}

// --- charOp ---

char extension charOp
{
    stacksafe bool is whitespace
    [
        if (self == $9)
            [ ^ true ];
            [
                if (self == $32)
                    [ ^ true ];
                    [
                        if (self == $10)
                            [ ^ true ];
                            [
                                if (self == $13)
                                    [ ^ true ];
                                    [
                                        ^ self == $12
                                    ]
                            ]
                    ]
            ].
            
        ^ false
    ]
    
    stacksafe bool is digit
    [
        int ch := 0.
        intConvertor convert char:self vint:ch.
        
        ^ (ch >= 30h) and: (ch <= 39h)
    ]
    
    stacksafe bool is newLine
    [
        ^ self == $10
    ]
    
    stacksafe bool is letter
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
    
    stacksafe bool is ltoken
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
}.
