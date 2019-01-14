// --- common methods ---

/// A generic extender
public extension objectOp
{
    __getClass()
        => system'core_routines'__object;

//    __parent
//        => system'core_routines'__object.

    bool instanceOf(type)
        => system'core_routines'__object;

    string __getClassName()
    {
        dirty_ptr c := self.__getClass();

        byte dump[256];
        int         len := extern elenart.LoadClassName(c, dump, 255);
         
        if (len == 0)
            { ^ "Unknown class" };
        
        ^ String.fromByteArray(0,len,dump)
    }
    
    /// Returns true if the specified object instances are equal; otherwise, false.
    bool equalReference(o)
        => system'core_routines'__object;
        
//    evalSelf(Func1 aFunction)
//        = aFunction(self).
//
////    evalSelf:aFunction
////        = aFunction eval:self.
//
//    /// Returns true if the object is nil
//    bool isNil
//        = nil == self.
//        
////    /// Returns true if the object is literal or character
////    isLiteral
////    [
////        var o := self literal \ back:$nil.
////        if ($nil == o)
////        [
////            o := self wide \ back:$nil.
////            if ($nil == o)
////            [
////                o := self char \ back:$nil.
////                
////                if ($nil == o)
////                [
////                    ^ false
////                ]
////            ]
////        ].
////        
////        ^ true.
////    ]
////    
////    /// Returns true if the object is integer numereric
////    isInteger
////    [
////        var o := self int \ back:$nil.
////        if ($nil == o)
////        [
////            o := self long \ back:$nil.
////            if ($nil == o)
////            [
////                o := self short \ back:$nil.
////                
////                if ($nil == o)
////                [
////                    o := self byte \ back:$nil.
////                    
////                    if ($nil == o)
////                    [
////                        ^ false
////                    ]
////                ]
////            ]
////        ].
////        
////        ^ true.
////    ]
////    
////    /// Returns true if the object is numereric
////    isNumeric
////    [
////        var o := self int \ back:$nil.
////        if ($nil == o)
////        [
////            o := self long \ back:$nil.
////            if ($nil == o)
////            [
////                o := self short \ back:$nil.
////                
////                if ($nil == o)
////                [
////                    o := self byte \ back:$nil.
////                    
////                    if ($nil == o)
////                    [
////                        o := self real \ back:$nil.
////                        
////                        if ($nil == o)
////                        [
////                            ^ false
////                        ]
////                    ]
////                ]
////            ]
////        ].
////        
////        ^ true.
////    ]
        
    bool isEmpty()
        = self.Length == 0;
        
    bool isNonempty()
        = self.Length != 0;
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    bool safeEqual(o)
        = self.equal:o \ back:false;

//    doWith:action
//        = action(self).
        
    reallocate(int newLength)
        => system'core_routines'__array;
        
//    shift(int n)
//    [
//        if (n < 0)
//            [ ^ self shiftLeft(n negative) ];
//            [ ^ self shiftRight(n) ].
//    ]
}

// --- Operations ---

// --- byteOp ---

public extension byteOp : ByteNumber
{
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }

//    byte absolute
//    [
//        byte n := self.
//        if (n < 0)
//            [ n := n * -1 ].
//                
//        ^ ByteNumber new(n)
//    ]
//    
//    byte mod : anOperand
//    [
//        var retVal := self / anOperand * anOperand.
//        
//        ^ byteConvertor convert(retVal)
//    ]    
    
    byte power(int y)
    {
        byte x := self;
        byte r := 1;
        
        for (int i := 0, i < y, i += 1)
        {
            r := r * x
        };
        
        ^ new ByteNumber(r)
    }
    
//    byte sqr
//    [
//        byte aRetVal := 0.
//        
//        aRetVal := self * self.
//        
//        ^ ByteNumber new(aRetVal)
//    ]
//
//    power : aPower
//        = self~byteOp power(aPower).
//                
//    bool isOdd
//        = self~byteOp mod:2 != 0.
//        
//    bool isEven
//        = self~byteOp mod:2 == 0.
//        
//    bool isZero
//        = self equal(0).
//        
//    bool isPositive
//        = self > 0.
//        
//    bool isNegative
//        = self < 0.
//        
//    bool isNonnegative
//        = self <= 0.
}

// --- shortOp ---

public extension shortOp : ShortNumber
{
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }
    
//    short absolute
//    [
//        short n := self.
//        if (n < 0)
//            [ n := n * -1 ].
//                
//        ^ ShortNumber new(n)
//    ]
//    
//    short mod : anOperand
//    [
//        var retVal := self / anOperand * anOperand.
//        
//        ^ shortConvertor convert(retVal)
//    ]
    
    short power(int y)
    {
        short x := self;
        short r := 1;
        
        for (/*int*/IntNumber i := 0, i < y, i += 1)
        {
            r := r * x
        };
        
        ^ new ShortNumber(r)
    }
    
//    short sqr
//    [
//        short n := self.
//        short aRetVal := 0.
//        
//        aRetVal := n * n.
//        
//        ^ ShortNumber new(aRetVal)
//    ]
//
//    power : aPower
//        = self~shortOp power(aPower).
//                
//    bool isOdd
//        = self~shortOp mod:2 != 0.
//        
//    bool isEven
//        = self~shortOp mod:2 == 0.
//        
//    bool isZero
//        = self equal(0).
//        
//    bool isPositive
//        = self > 0.
//        
//    bool isNegative
//        = self < 0.
//        
//    bool isNonnegative
//        = self <= 0.
//    
//    byte high
//    [
//        short aWord := self.
//        aWord := aWord && 0FF00h.
//        aWord := aWord shiftRight(8).
//        
//        ^ byteConvertor convert(aWord)
//    ]
//        
//    byte low
//    [
//        short aWord := self.
//        aWord := aWord && 0FFh.
//        
//        ^ byteConvertor convert(aWord)
//    ]
}

// --- intOp ---

public extension intOp : IntNumber
{
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }
    
//    int rvprop absolute :: absolute.
//    
//    int rvprop sqr      :: sqr.
//    
//    int rvprop sqrt     :: sqrt.
//    
//    absolute(ref<int> retVal)
//    [
//        int n := self.
//
//        if (n < 0)
//            [ n := n * -1 ].
//
//        retVal value := n
//    ]
//    
    fnprop int power
    {
        read(int y, ref int retVal)
        {
            int x := self;
            int r := 1;
            if (x == 0)
                { r := 0 };
            
            for(int i := 0, i < y, i += 1)
            {
                r := r * x
            };
            
            retVal := r
        }
    }
  
    fnprop int mod
    {
        read(int y, ref int retVal)
        {
            int x := self;
            
            int z := x / y;
            z := z * y;
            
            retVal := (x - z)
        }
    }

//    int mod : anOperand
//    [
//        int retVal := self mod(T<int>(anOperand)).
//        
//        ^ retVal
//    ]
    
    real realDiv(n)
        = new RealNumber(self).divide:n;
   
//    sqr(ref<int> retVal)
//    [
//        int n := self.        
//        
//        retVal value := n * n.
//    ]
//    
//    sqrt(ref<int> retVal)
//    [
//        int num := self.
//        
//        if (num == 0)
//            [ ^ 0 ].
//            
//        int n := num / 2 + 1.
//        int n1 := (n + (num / n)) / 2.
//        while (n1 < n)
//        [
//            n := n1.
//            n1 := (n + (num / n)) / 2
//        ].
//        
//        retVal value := n
//    ]

    power(n)
        = self.power(cast int(n));
                
    bool isOdd()
        = (self && 1) != 0;
        
//    bool isEven
//        = self and(1) == 0.
//        
//    bool isZero
//        = self equal(0).
//        
//    bool isPositive
//        = self > 0.
//        
//    bool isNegative
//        = self < 0.
//        
//    bool isNonnegative
//        = self <= 0.
//
//    high
//    [
//        int aWord := self.
//        aWord := aWord && 0FFFF0000h.
//        aWord := aWord >> 16.
//        
//        ^ shortnConvertor convert(aWord)
//    ]
//
//    low
//    [
//        int aWord := self.
//        aWord := aWord && 0FFFFh.
//        
//        ^ shortConvertor convert(aWord)
//    ]
//    
//    shift : n
//    [
//        if (n < 0)
//            [ ^ self shiftLeft(T<int>(n) negative) ];
//            [ ^ self shiftRight(T<int>(n)) ].
//    ]            
}

public extension uintOp : UIntNumber
{
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }
}
    
// --- longOp ---

public extension longOp : LongNumber
{
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }
    
//    long absolute
//    [
//        long n := self.
//
//        if (n < 0l)
//            [ n := n * -1l ].
//                
//        ^ LongNumber new(n)
//    ]
    
    long mod(n)
    {
        long x := self;
        long y := cast long(n);
        
        long z := x / y;
        z := z * y;
        
        ^ new LongNumber(x - z)
    }
    
//    long power(long y)
//    [
//        long x := self.
//        long r := 1.
//        
//        long i := 0l.
//        while (i < y)
//        [
//            r := r * x.
//            
//            i += 1l.
//        ].
//        
//        ^ LongNumber new(r)
//    ]
//    
//    long power(int y)
//    [
//        long x := self.
//        long r := 1.
//        
//        int i := 0.
//        while (i < y)
//        [
//            r := r * x.
//            
//            i += 1.
//        ].
//        
//        ^ LongNumber new(r)
//    ]
//    
//    long sqr
//    [
//        long n := self.        
//        long aRetVal := 0l.
//        
//        aRetVal := n * n.
//        
//        ^ LongNumber new(aRetVal)
//    ]
//
//    power : aPower
//        = self~longOp power(T<long>(aPower)).
//                
//    bool isOdd
//        = self~longOp mod:2l != 0.
//        
//    bool isEven
//        = self~longOp mod:2l == 0.
//        
//    bool isZero
//        = self equal(0l).
//        
//    bool isPositive
//        = self > 0l.
//        
//    bool isNegative
//        = self < 0l.
//        
//    bool isNonnegative
//        = self <= 0l.
//
//    int high
//    [
//        long aWord := self.
//        aWord := aWord && -4294967296l.
//        aWord := aWord >> 32.
//        
//        ^ intConvertor convert(aWord)
//    ]
//
//    int low
//    [
//        long aWord := self.
//        aWord := aWord && 4294967295l.
//        
//        ^ intConvertor convert(aWord)
//    ]
}

// --- realOp ---

public extension realOp : RealNumber
{        
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }
    
//    real absolute
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController abs(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//        
//    real exp
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController exp(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal).
//    ]
    
    real ln()
    {
        real n := self;
        real retVal := 0.0r;
        
        realOpController.ln(n, ref retVal);
        
        ^ new RealNumber(retVal)
    }
    
//    real sqrt
//    [
//        real n := self.      
//        real aRetVal := 0.0r.
//        
//        realOpController sqrt(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]

    real power(real y)
    {
        real x := self;
        
        (x == 0.0r)
            ? { ^ 0.0r };
                        
        (y == 0.0r)
            ? { ^ 1.0r };
            
        realOpController.ln(x, ref x);
        
        x := x * y;
        
        realOpController.exp(x, ref x);
            
        ^ new RealNumber(x)
            
    }

    real power(int y)
    {
        real x := self;

        (x == 0.0r)
            ? { ^ 0.0r };
                        
        (y == 0)
            ? { ^ 1.0r };
                        
        if (x < 0.0r)
        {
            x := x * -1.0r;
            
            realOpController.ln(x, ref x);
            
            x := x * y;
            
            realOpController.exp(x, ref x);
            
            (y.isOdd())
                ? { x := x * -1.0r }
        }
        else
        {
            realOpController.ln(x, ref x);
            
            x := x * y;
            
            realOpController.exp(x, ref x)
        };
        
        ^ new RealNumber(x)
    }

//    power : aPower
//        = self~realOp power(T<int>(aPower)).
//
//    real sqr
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        aRetVal := n * n.
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    real rounded
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController round(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//    
//    real round(int precision)
//    [
//        real a := self.
//        bool neg := false.
//        if(self isNegative)
//        [
//            a := a * -1.0r.
//            neg := true.
//        ].
//           
//        int b := 10 power:precision.
//        real r := a * b + 0.5r.
//        
//        int c := intConvertor convert(r rounded).
//    
//        a := c realDiv:b.
//    
//        ^neg iif(a * -1.0r, a). 
//    ]
//    
//    real truncate(int precision)
//    [
//        real a := self.
//        bool neg := false.
//        if(self isNegative)
//        [
//            a := a * -1.0r.
//            neg := true.
//        ].
//       
//        int b := 10 power:precision.
//        real c := (a * b) integer. // returns the integer part of the real number
//
//        a := c / b.
//
//        ^(neg) iif(a * -1.0r, a). 
//    ]
//
//    real integer
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//
//        realOpController truncate(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    real frac
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController abs(n, &aRetVal).
//        
//        ^ RealNumber new(n - aRetVal)
//    ]
//
//    real sin
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController sin(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    real cos
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController cos(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//
//    real tan
//    [
//        real n := self.        
//        real aSinVal := 0.0r.
//        real aCosVal := 0.0r.
//        
//        realOpController sin(n, &aSinVal).
//        realOpController cos(n, &aCosVal).
//        
//        ^ RealNumber new(aSinVal / aCosVal)
//    ]
//
//    real arctan
//    [
//        real n := self.        
//        real aRetVal := 0.0r.
//        
//        realOpController arctan(n, &aRetVal).
//        
//        ^ RealNumber new(aRetVal)
//    ]
//            
//    real arcsin
//    [
//        real n := self.        
//        real arg := 0.0r.
//        real retVal := 0.0r.
//        
//        retVal := n * n.        
//        retVal := 1.0r - retVal.        
//        realOpController sqrt(retVal, &arg).
//        
//        arg := n / arg.        
//        realOpController arctan(arg, &retVal).
//        
//        ^ RealNumber new(retVal)
//    ]
//            
//    real arccos
//    [
//        real n := self.        
//        real arg := 0.0r.
//        real retVal := 0.0r.
//        
//        retVal := n * n.        
//        retVal := 1.0r - retVal.        
//        realOpController sqrt(retVal, &arg).
//        
//        arg := arg / n.        
//        realOpController arctan(arg, &retVal).
//        
//        ^ RealNumber new(retVal)
//    ]
//            
//    real radian
//    [
//        real aValue := self.
//        
//        aValue := aValue * pi_value.
//        
//        ^ RealNumber new(aValue / 180.0r)
//    ]
//            
//    real degree
//    [
//        real aValue := self.
//        
//        aValue := aValue * 180.0r.
//        
//        ^ RealNumber new(aValue / pi_value)
//    ]
//            
//    real reciprocal
//    [
//        real aValue := self.
//        
//        ^ RealNumber new(1.0r / aValue)
//    ]    
//            
//    bool isZero
//        = self == 0.0r.
//        
//    bool isPositive
//        = self > 0.0r.
//        
//    bool isNegative
//        = self < 0.0r.
//        
//    bool isNonnegative
//        = self <= 0.0r.
}

// --- intArrayOp ---

public extension intArrayOp : Array<int>
{
    /// Copies the short array at the specified position
    write(int index, int length, int[] array)
        => system'core_routines'__intarray;

//    exchange(int anIndex1, int anIndex2)
//    [
//        int n1 := 0.
//        int n2 := 0.
//
//        self read(anIndex1, &n1).
//        self read(anIndex2, &n2).
//        
//        self write(anIndex1, n2).
//        self write(anIndex2, n1).
//    ]
//
//    populate(Func<int,int> f)
//    [
//        int anIndex := 0.
//        int aLength := self length.
//        
//        while (anIndex < aLength)
//        [            
//            self write(anIndex, f(anIndex)).
//            
//            anIndex := anIndex + 1
//        ].
//        
//        ^ self
//    ]
//    
//    populate(Func1 f)
//        = self~intArrayOp populate(:n)<int,int>[ ^f(n) ].    
}

//// --- arrayOp ---
//
//public extension<Array> arrayOp
//{
////    exchange : anIndex1 : anIndex2
////        = self~arrayOp exchange int:anIndex1 int:anIndex2.
//        
//    exchange(int anIndex1, int anIndex2)
//    [
//        var aTemp := self getAt(anIndex1).
//        
//        self setAt(anIndex1, self getAt(anIndex2)).
//        self setAt(anIndex2, aTemp).
//    ]
//    
//    indexOf(int anIndex, object anObject, ref<int> aRetVal)
//    [
//        aRetVal value := -1.
//        
//        int aLength := self length.
//        
//        int i := anIndex.
//        while(i < aLength)
//        [
//            var anArrayItem := self getAt(i).
//            
//            if (anObject == anArrayItem)
//                [ aRetVal value := i ].
//            
//            i := i + 1
//        ]
//    ]        
//        
//    int indexOf(int anIndex, object anObject)
//    [
//        int aRetVal := 0.
//        
//        self~arrayOp indexOf(anIndex, anObject, &aRetVal).
//        
//        ^ aRetVal
//    ]
//    
//    int indexOf : anObject
//        = self~arrayOp indexOf(0, anObject).
//        
//////    safeSeek : aValue
//////    [
//////        int aLength := self length.
//////        
//////        int i := 0.
//////        while (i < aLength)
//////        [
//////            var anArrayItem := self getAt int:i.
//////            
//////            if (aValue safeEqual:anArrayItem)
//////                [ ^ anArrayItem ].
//////            
//////            i := i + 1
//////        ].
//////        
//////        ^ $nil
//////    ]
////        
////    deepClone
////    [
////        int aLength := self length.
////        
////        array aCopy := Array new int:aLength.
////                
////        int anIndex := 0.
////        while(anIndex < aLength)
////        [
////            aCopy setAt int:anIndex object:(self getAt int:anIndex; clone).
////            
////            anIndex := anIndex + 1.
////        ].
////        
////        ^ self.
////    ]
//    
//    populate(Func<int,object> anIndexFunc)
//    [
//        int anIndex := 0.
//        int aLength := self length.
//        
//        while(anIndex < aLength)
//        [            
//            self setAt(anIndex, anIndexFunc(anIndex)).
//            
//            anIndex := anIndex + 1.
//        ].
//        
//        ^ self
//    ]
//    
//    populate(Func1 f)
//        = self~arrayOp populate(:n)<int,object>[ ^f(T<int>(n)) ].
//
////    multi populate : anAction
////        = self~arrayOp populate(:n)<int>[ ^anAction eval:n ].    
//    
//    sort(Func2 aCompareFunc)
//    [
//        int i := 0.
//        int j := 0.
//        int aLength := self length.
//        var aCurrent := nil.
//        var aNext := nil.
//        
//        while(i < aLength)
//        [
//            aCurrent := self getAt(i).
//            
//            j := i + 1.
//            while (j < aLength)
//            [
//                aNext := self getAt(j).
//                ifnot(aCompareFunc(aCurrent,aNext))
//                    [                        
//                        self setAt(i, aNext).
//                        self setAt(j, aCurrent).
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
//        = (self clone)~arrayOp sort(:aFormer:aLater)(aFormer < aLater).
//    
//    bool isEmpty
//    [
//        int aLength := self length.
//        
//        ^ aLength == 0.
//    ]
//    
//    T<Array> Subarray : aLength at:anIndex
//        = self~arrayOp Subarray(anIndex, aLength).
//    
//    T<Array> Subarray(int anIndex, int aLength)
//    [
//        auto it := T<Indexer>(self).
//        
//        ^ Array new(aLength); populate(:i)<int>(it writeIndex(anIndex + i); get).
//    ]
//}
//
//// --- intmatrixOp ---
//
//public extension<IntMatrix> intMatrixOp
//{
//    populate(Func<int,int,int> fn)
//    [
//        int n := 0.
//        int m := 0.
//        self readLengthsTo(&n, &m).
//        
//        int i := 0.
//        int j := 0.
//        while (i < n)
//        [
//            j := 0.
//            while (j < m)
//            [
//                int v := fn(i, j).
//                
//                self write(i, j, v).
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

public extension stringOp : String
{
    vprop int HashCode
    {
        /// Reads the object hash code into the output parameter
        readHashCodeTo(ref int retVal)
        {
            int len := self.Length;
            
            int h := extern elenart.core_utf8hashcode(self, len);
            
            retVal := h
        }    
    }

//    indexOf : anObject
//        = self indexOf:anObject at:0.
//
//    indexOf(literal aLiteral)
//        = self indexOf:aLiteral at:0.
//
//    indexOf(char aChar)
//        = self indexOf:aChar at:0.
//        
//    literal Substring : anIndex
//        = self~literalOp Substring(T<int>(anIndex)).
        
    string Substring(int index)
    {
        int len := self.Length;
        if (len > index)
        {
            len := len - index;
            
            ^ self.Substring(index, len)
        }
        else
        {
            ^ emptyString
        }        
    }        
    
    string trimLeft(char ch)
    {
        int  index := 0;
        int  len := self.Length;
        char current := $0;
        int  chLen := ch.Length;
        
        while(index < len)
        {
            self.read(index, ref current);
            
            ifnot(current == ch)
            { 
                if (index == 0)
                { 
                    ^ self 
                }
                else
                { 
                    ^ self.delete(0, index) 
                }
            };
            
            index += chLen
        };
        
        ^ emptyString
    }
    
    string trimLeft()
        = self.trimLeft($32);
       
    string trimRight(char ch)
    {
        int  len := self.Length;
        int  index := len - 1;
        int  prev := len;
        char current := $0;
        int  chLen := ch.Length;
        
        while (index >= 0)
        {
            self.read(index, ref current);
            
            ifnot (current == ch)
                { ^ self.delete(prev,len - prev) };
            
            prev := index;
            index -= chLen
        };
        
        ^ emptyString
    }
    
    string trimRight()
        = self.trimRight($32);
        
    string trim(char ch)
        = self.trimLeft(ch).trimRight(ch);
    
    string trim()
        = self.trimLeft($32).trimRight($32);
    
//    padLeft(int aLength)
//        = self padLeft($32, aLength).
//    
//    padLeft(char aChar, int aLength)
//    [
//        int aCurrentLength := self length.
//        
//        if (aCurrentLength < aLength)
//            [ ^ LiteralValue new(aLength - aCurrentLength, aChar); add(self) ].
//            
//        ^ self
//    ]
//    
//    padRight(int aLength)
//        = self padRight($32, aLength).
//    
//    padRight(char aChar, int aLength)
//    [
//        int aCurrentLength := self length.
//
//        if (aCurrentLength < aLength)
//            [ ^ self add(LiteralValue new(aLength - aCurrentLength, aChar)) ].
//            
//        ^ self
//    ]
//
//    bool startingWith(literal s)
//    [
//        int n := 0.
//        self indexOf(0, s, &n).
//        
//        ^ 0 == n
//    ]
//        
//    bool endingWith(literal s)
//    [
//        int l1 := self length.
//        int l2 := s length.
//
//        int n := 0.
//        self indexOf(l1 - l2, s, &n).
//        
//        ^ n != -1
//    ]
//        
//    bool containing(literal s)
//    [
//        int l1 := self length.
//        int l2 := s length.
//
//        int n := 0.
//        self indexOf(0, s, &n).
//        
//        ^ n != -1
//    ]
//
////    replaceFirst literal:aReplacee literal:aReplacer
////    [
////        int aReplaceeLen := aReplacee length.
////        int anIndex := 0.
////        
////        self indexOf int:0 literal:aReplacee vint:anIndex.
////        
////        if (anIndex != -1)
////            [ ^ self delete int:anIndex int:aReplaceeLen; insert int:anIndex literal:aReplacee ];
////            [ ^ self ].
////    ]    
//    
//    replace(literal aReplacee, literal aReplacer)
//    [
//        literal aTarget := emptyLiteral.
//        int     aStart := 0.
//        int     anEnd := 0.
//        int     aReplaceeLen := aReplacee length.
//        
//        self indexOf(aStart, aReplacee, &anEnd).
//        
//        while (anEnd != -1)
//        [
//            aTarget := aTarget
//                add(self Substring(aStart, anEnd - aStart));
//                add(aReplacer).
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            self indexOf(aStart, aReplacee, &anEnd)
//        ].
//    
//        (aStart != 0)
//            ? [ ^ aTarget add(self~literalOp Substring(aStart)) ]
//            ! [ ^ self ]
//    ]
//    
//    bool isEmpty
//    [
//        int aLength := self length.
//        
//        ^ aLength == 0
//    ]    
}

// --- wideOp ---

public extension wideOp : WideString
{
    vprop int HashCode
    {
        read(ref int retVal)
        {
            int len := self.Length;
        
            int h := extern elenart.core_utf16hashcode(self, len);
        
            retVal := h
        }
    }
    
//    indexOf : anObject
//        = self indexOf:anObject at:0.
//        
//    indexOf(wide aLiteral)
//        = self indexOf:aLiteral at:0.
//
//    indexOf(char aChar)
//        = self indexOf:aChar at:0.
//                
//    wide Substring:anIndex
//        = self~wideOp Substring(T<int>(anIndex)).
//        
//    wide Substring(int anIndex)
//    [
//        int aLength := self length.
//        
//        aLength := aLength - anIndex.
//        
//        ^ self Substring(anIndex, aLength)
//    ]
     
    wide trimLeft(char ch)
    {
        int  index := 0;
        int  len := self.Length;
        char current := $0;
        int  chLen := ch.Length;
        
        while (index < len)
        {
            self.read(index, ref current);
            
            ifnot(current == ch)
                { ^ self.delete(0, index) };
            
            index += chLen
        };
        
        ^ emptyWideString
    }
    
    wide trimLeft()
        = self.trimLeft($32);
       
    wide trimRight(char ch)
    {
        int  len := self.Length;
        char current := $0;
        int  chLen := ch.Length;
                
        int  index := len - 1;
        int  prev := len;
        while (index >= 0)
        {
            self.read(index, ref current);
            
            ifnot (current == ch)
                { ^ self.delete(prev, len - prev) };
            
            prev := index;
            index -= chLen
        };
        
        ^ emptyWideString
    }
    
    wide trimRight()
        = self.trimRight($32);
       
    wide trim(char ch)
        = self.trimLeft(ch).trimRight(ch);
    
    wide trim()
        = self.trimLeft($32).trimRight($32);
    
//    padLeft(char aChar, int aLength)
//    [
//        int aCurrentLength := self length.
//        
//        if (aCurrentLength < aLength)
//            [ ^ WideLiteralValue new(aLength - aCurrentLength, aChar); add(self) ].
//            
//        ^ self
//    ]
//    
//    padRight(char aChar, int aLength)
//    [
//        int aCurrentLength := self length.
//
//        if (aCurrentLength < aLength)
//            [ ^ self add(WideLiteralValue new(aLength - aCurrentLength, aChar)) ].
//            
//        ^ self
//    ]
//
//    bool startingWith(wide s)
//    [
//        int n := 0.
//        self indexOf(0, s, &n).
//        
//        ^ 0 == n
//    ]
//    
//    bool endingWith(wide s)
//    [
//        int l1 := self length.
//        int l2 := s length.
//        int n := 0.
//        self indexOf(l1 - l2, s, &n).
//        
//        ^ n != -1
//    ]
//    
//    bool containing(wide s)
//    [
//        int l1 := self length.
//        int l2 := s length.
//        int n := 0.
//        
//        self indexOf(0, s, &n).
//        
//        ^ n != -1
//    ]
//    
////    replaceFirst wide:aReplacee wide:aReplacer
////    [
////        int aReplaceeLen := aReplacee length.
////        int anIndex := 0.
////        
////        self indexOf int:0 wide:aReplacee vint:anIndex.
////        
////        if (anIndex != -1)
////            [ ^ self delete int:anIndex int:aReplaceeLen; insert int:anIndex wide:aReplacee ];
////            [ ^ self ].
////    ]
////        
//    replace(wide aReplacee, wide aReplacer)
//    [
//        wide aTarget := emptyWideLiteral.
//        int  aStart := 0.
//        int  anEnd := 0.
//        int  aReplaceeLen := aReplacee length.
//        
//        self indexOf(aStart, aReplacee, &anEnd).
//        
//        while (anEnd != -1)
//        [
//            aTarget := aTarget
//                add(self Substring(aStart, anEnd - aStart));
//                add:aReplacer.
//            
//            aStart := anEnd + aReplaceeLen.
//            
//            self indexOf(aStart, aReplacee, &anEnd)
//        ].
//    
//        if (aStart != 0)
//            [ ^ aTarget add(self~wideOp Substring(aStart)) ];
//            [ ^ self ].
//    ]
//
//    bool isEmpty
//    [
//        int aLength := self length.
//        
//        ^ aLength == 0
//    ]
}

// --- stringOp ---

singleton stringOpDispatcher
{
//    padRight(literal s, char ch, int len)
//        = s~literalOp padRight(ch, len).
//    
//    padRight(wide s, char ch, int len)
//        = s~wideOp padRight(ch, len).
//        
//    padLeft(literal s, char ch, int len)
//        = s~literalOp padLeft(ch, len).
//    
//    padLeft(wide s, char ch, int len)
//        = s~wideOp padLeft(ch, len).
//        
//    bool startingWith(literal s, literal subs)
//        = s~literalOp startingWith(subs).
//
//    bool startingWith(wide s, wide subs)
//        = s~wideOp startingWith(subs).
//
//    bool endingWith(literal s, literal subs)
//        = s~literalOp endingWith(subs).
//
//    bool endingWith(wide s, wide subs)
//        = s~wideOp endingWith(subs).
//        
//    bool containing(literal s, literal subs)
//        = s~literalOp containing(subs).
//
//    bool containing(wide s, wide subs)
//        = s~wideOp containing(subs).
        
    trimLeft(string s, char ch)
        =s.trimLeft(ch);       
        
    trimRight(string s, char ch)
        =s.trimRight(ch);
        
    trim(string s, char ch)
        =s.trim(ch);
        
    trimLeft(wide s, char ch)
        =s.trimLeft(ch);        
        
    trimRight(wide s, char ch)
        =s.trimRight(ch);        
        
    trim(wide s, char ch)
        =s.trim(ch);
        
//    replace(literal s, object src, object dst)
//        = s~literalOp replace(T<literal>(src), T<literal>(dst)).
//        
//    replace(literal s, literal src, literal dst)
//        = s~literalOp replace(src, dst).
//        
//    replace(wide s, wide src, wide dst)
//        = s~wideOp replace(src, dst).
}

public extension stringWeakOp
{
//    Substring : anIndex
//        = self Substring:(self length - anIndex) at:anIndex.
//            
//    Substring(object anIndex, object aLength)
//        = self Substring:aLength at:anIndex.
    
    trimLeft(ch)
        = stringOpDispatcher.trimLeft(self, ch);

    trimRight(ch)
        = stringOpDispatcher.trimRight(self, ch);

    trim(ch)
        = stringOpDispatcher.trim(self, ch);
    
    trimLeft()
        = stringOpDispatcher.trimLeft(self, $32);

    trimRight()
        = stringOpDispatcher.trimRight(self, $32);

    trim()
        = stringOpDispatcher.trim(self, $32);

//    padLeft(object padSymbol, object len)
//        = stringOpDispatcher padLeft(self, padSymbol, len).
//        
//    padRight(object padSymbol, object len)
//        = stringOpDispatcher padRight(self, padSymbol, len).
//
//    startingWith : s
//        = stringOpDispatcher startingWith(self, s).
//
//    endingWith : s
//        = stringOpDispatcher endingWith(self, s).
//
//    containing : s
//        = stringOpDispatcher containing(self, s).
//
////    replaceFirst literal:aReplacee literal:aReplacer
////    [
////        var dispatcher := self cast:%eval to:stringOpDispatcher.
////        
////        ^ self~dispatcher replaceFirst literal:aReplacee literal:aReplacer
////    ]
//    
//    replace(aReplacee, aReplacer)
//        = stringOpDispatcher replace(self, aReplacee, aReplacer).
//    
//    fillString(object aLiteral, int aCounter)
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
//        = self~stringOp fillString(aLine, T<int>(aCounter)).
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
//        int aWidthInt := aWidth.
//        int aLength := aLine length.        
//        
//        if (aLength < aWidthInt)
//            [
//                int aLen := aWidthInt - aLength.
//                int aLeftLen := aLen / 2.
//                
//                self~stringOp fillString(aChar, aLeftLen).
//                self write:aLine.
//                self~stringOp fillString(aChar, aLen - aLeftLen)
//            ];
//            [ self write:aLine ].
//            
//        ^ self
//    ]    
}

// --- charOp ---

public extension charOp : CharValue
{
    vprop int HashCode
    {
        // Reads the object hash code into the output parameter
        read(ref int retVal)
            => system'core_routines'__int;        
    }
    
    bool isWhitespace()
    {
        self =>
            $9  { ^ true }
            $32 { ^ true }
            $10 { ^ true }
            $13 { ^ true }
            $12 { ^ true };
            
        ^ false
    }
    
//    bool isDigit
//    [
//        int ch := 0.
//        intConvertor convert(self, &ch).
//        
//        ^ (ch >= 30h) and: (ch <= 39h)
//    ]
//    
//    bool isNewLine
//    [
//        ^ self == $10
//    ]
//    
//    bool isLetter
//    [
//        int ch := 0.
//        intConvertor convert(self, &ch).
//        
//        if ((ch >= 41h) && (ch <= 5Ah))
//            [ ^ true ].
//        
//        if ((ch >= 61h) && (ch <= 7Ah))
//            [ ^ true ].
//        
//        if ((ch >= 3ACh) && (ch <= 3CEh))
//            [ ^ true ].
//            
//        if ((ch >= 400h) && (ch <= 42Fh))
//            [ ^ true ].
//            
//        ^ false
//    ]
//    
//    bool isLToken
//    [
//        int ch := 0.
//        intConvertor convert(self, &ch).
//        
//        if ((ch >= 41h) && (ch <= 5Ah))
//            [ ^ true ].
//        
//        if ((ch >= 61h) && (ch <= 7Ah))
//            [ ^ true ].
//        
//        if ((ch >= 3ACh) && (ch <= 3CEh))
//            [ ^ true ].
//            
//        if ((ch >= 400h) && (ch <= 42Fh))
//            [ ^ true ].
//            
//        ^ ch == 39
//    ]
}

// --- byteArrayHelper ---

public extension byteArrayHelper : Array<byte>
{
    /// Reads a byte value at the specified position and saves it into an output variable
    read(int index, ref byte retVal)
        => system'core_routines'__bytearray;

    /// Reads a short integer value at the specified position and saves it into an output variable
    read(int index, ref short retVal)
        => system'core_routines'__bytearray;

    /// Reads an integer value at the specified position and saves it into an output variable
    read(int index, ref int retVal)
        => system'core_routines'__bytearray;

    /// Reads a character value at the specified position and saves it into an output variable
    read(int index, ref char retVal)
        => system'core_routines'__bytearray;                
}

// --- charArrayHelper ---

public extension charArrayHelper : Array<char>
{
    move(int index, int length, int offset)
        => system'core_routines'__chararray;

    /// Copies the array at the specified position    
    write(int index, int length, char[] array)
        => system'core_routines'__chararray;
}

//public singleton byteArrayHelper
//{
////    read(ByteArray array, int offset, ref<short> retVal)
////    [
////        array read(offset, retVal)
////    ]
////    
////    read(ByteArray array, int offset, ref<int> retVal)
////    [
////        array read(offset, retVal)
////    ]
////    
////    read(ByteArray array, int offset, ref<long> retVal)
////    [
////        array read(offset, retVal)
////    ]
////    
////    read(ByteArray array, int offset, ref<real> retVal)
////    [
////        array read(offset, retVal)
////    ]
////    
////    read(ByteArray array, int offset, ref<char> retVal)
////    [
////        array read(offset, retVal)
////    ]
////    
////    write(ByteArray array, int offset, short retVal)
////    [
////        array write(offset, retVal)
////    ]
////    
////    write(ByteArray array, int offset, int retVal)
////    [
////        array write(offset, retVal)
////    ]
////    
////    write(ByteArray array, int offset, long retVal)
////    [
////        array write(offset, retVal).
////    ]
////    
////    write(ByteArray array, int offset, real retVal)
////    [
////        array write(offset, retVal)
////    ]
////    
////    write(ByteArray array, int offset, char retVal)
////    [
////        array write(offset, retVal)
////    ]
////    
////    fill(ByteArray array, int offset, int length, byte val)
////    [
////        array fill(offset, length, val).
////    ]
//}
//// --- intArrayHelper ---
////
////singleton intArrayHelper
////{
////    stacksafe fill intarray:array int:offset int:length int:val
////    [
////        array fill int:offset int:length int:val.
////    ]
////}