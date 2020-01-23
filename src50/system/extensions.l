// --- common methods ---

/// A generic extender
public extension objectOp
{
    __getClass()
        => intern system'core_routines'__object;

//    __getBaseClass()
//        => system'core_routines'__object;
//
//    __getParentClass()
//        => system'core_routines'__object;
//
//    bool instanceOf(type)
//        => system'core_routines'__object;

    string __getClassName()
    {
        dirty_ptr c := self.__getClass();

        byte dump[256];
        int         len := extern elenart.LoadClassName(c, dump, 255);
         
        if (len == 0)
            { ^ "Unknown class" };
        
        ^ String.fromByteArray(0,len,dump)
    }
    
//    /// Returns true if the specified object instances are equal; otherwise, false.
//    bool equalReference(o)
//        => system'core_routines'__object;
//        
////    evalSelf(Func1 aFunction)
////        = aFunction(self).
////
//////    evalSelf:aFunction
//////        = aFunction eval:self.
////
////    /// Returns true if the object is nil
////    bool isNil
////        = nil == self.
////        
//////    /// Returns true if the object is literal or character
//////    isLiteral
//////    [
//////        var o := self literal \ back:$nil.
//////        if ($nil == o)
//////        [
//////            o := self wide \ back:$nil.
//////            if ($nil == o)
//////            [
//////                o := self char \ back:$nil.
//////                
//////                if ($nil == o)
//////                [
//////                    ^ false
//////                ]
//////            ]
//////        ].
//////        
//////        ^ true.
//////    ]
//////    
//////    /// Returns true if the object is integer numereric
//////    isInteger
//////    [
//////        var o := self int \ back:$nil.
//////        if ($nil == o)
//////        [
//////            o := self long \ back:$nil.
//////            if ($nil == o)
//////            [
//////                o := self short \ back:$nil.
//////                
//////                if ($nil == o)
//////                [
//////                    o := self byte \ back:$nil.
//////                    
//////                    if ($nil == o)
//////                    [
//////                        ^ false
//////                    ]
//////                ]
//////            ]
//////        ].
//////        
//////        ^ true.
//////    ]
//////    
//////    /// Returns true if the object is numereric
//////    isNumeric
//////    [
//////        var o := self int \ back:$nil.
//////        if ($nil == o)
//////        [
//////            o := self long \ back:$nil.
//////            if ($nil == o)
//////            [
//////                o := self short \ back:$nil.
//////                
//////                if ($nil == o)
//////                [
//////                    o := self byte \ back:$nil.
//////                    
//////                    if ($nil == o)
//////                    [
//////                        o := self real \ back:$nil.
//////                        
//////                        if ($nil == o)
//////                        [
//////                            ^ false
//////                        ]
//////                    ]
//////                ]
//////            ]
//////        ].
//////        
//////        ^ true.
//////    ]
        
    bool isEmpty()
    {
        int len := self.Length;
        
        ^ len == 0;
    }
        
    bool isNonempty()
    {
        int len := self.Length;
        
        ^ len != 0;
    }
        
//    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
//    bool safeEqual(o)
//        = self.equal:o \ back:false;
//
//    doWith(action)
//        = action(self);
//        
//    reallocate(int newLength)
//        => system'core_routines'__array;
//        
////    shift(int n)
////    [
////        if (n < 0)
////            [ ^ self shiftLeft(n negative) ];
////            [ ^ self shiftRight(n) ].
////    ]
}

//// --- Operations ---
//
//// --- byteOp ---
//
//public extension byteOp : ByteNumber
//{
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
//
//    get byte Absolute()
//    {
//        byte n := self;
//        if (n < 0)
//            { n := n * -1 };
//                
//        ^ new ByteNumber(n)
//    }
//    
//    byte mod(operand)
//    {
//        var retVal := self / operand * operand;
//        
//        ^ byteConvertor.convert(retVal)
//    }    
//    
//    byte power(int y)
//    {
//        byte x := self;
//        byte r := 1;
//        
//        for (int i := 0, i < y, i += 1)
//        {
//            r := r * x
//        };
//        
//        ^ new ByteNumber(r)
//    }
//    
//    byte sqr()
//    {
//        byte retVal := 0;
//        
//        retVal := self * self;
//        
//        ^ new ByteNumber(retVal)
//    }
//
////    power : aPower
////        = self~byteOp power(aPower).
//                
//    bool isOdd()
//        = self.mod:2 != 0;
//        
//    bool isEven()
//        = self.mod:2 == 0;
//        
//    bool isZero()
//        = self.equal(0);
//        
//    bool isPositive()
//        = self > 0;
//        
//    bool isNegative()
//        = self < 0;
//        
//    bool isNonnegative()
//        = self <= 0;
//}
//
//// --- shortOp ---
//
//public extension shortOp : ShortNumber
//{
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
//    
//    get short Absolute()
//    {
//        short n := self;
//        if (n < 0)
//            { n := n * -1 };
//                            
//        ^ new ShortNumber(n)
//    }
//    
//    short mod(operand)
//    {
//        var retVal := self / operand * operand;
//        
//        ^ shortConvertor.convert(retVal)
//    }
//    
//    short power(int y)
//    {
//        short x := self;
//        short r := 1;
//        
//        for (/*int*/IntNumber i := 0, i < y, i += 1)
//        {
//            r := r * x
//        };
//        
//        ^ new ShortNumber(r)
//    }
//    
//    short sqr()
//    {
//        short n := self;
//        short retVal := 0;
//        
//        retVal := n * n;
//        
//        ^ new ShortNumber(retVal)
//    }
//
////    power : aPower
////        = self~shortOp power(aPower).
//                
//    bool isOdd()
//        = self.mod:2 != 0;
//        
//    bool isEven()
//        = self.mod:2 == 0;
//        
//    bool isZero()
//        = self.equal(0);
//        
//    bool isPositive()
//        = self > 0;
//        
//    bool isNegative()
//        = self < 0;
//        
//    bool isNonnegative()
//        = self <= 0;
//    
////    byte high
////    [
////        short aWord := self.
////        aWord := aWord && 0FF00h.
////        aWord := aWord shiftRight(8).
////        
////        ^ byteConvertor convert(aWord)
////    ]
////        
////    byte low
////    [
////        short aWord := self.
////        aWord := aWord && 0FFh.
////        
////        ^ byteConvertor convert(aWord)
////    ]
//}

// --- intOp ---

public extension intOp : IntNumber
{
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
//    
//    get int Absolute()
//    {
//        int n := self;
//    
//        if (n < 0)
//           { n := n * -1 };
//    
//        ^ n
//    }
//
//    int power(int y)
//    {
//        int x := self;
//        int r := 1;
//        if (x == 0)
//            { r := 0 };
//        
//        for(int i := 0, i < y, i += 1)
//        {
//            r := r * x
//        };
//        
//        ^ r
//    }
//  
//    int mod(int y)
//    {
//        int x := self;
//        
//        int z := x / y;
//        z := z * y;
//        
//        int retVal := x - z;
//        
//        ^ retVal
//    }
//
//    int mod(o)
//    {
//        int retVal := self.mod(cast int(o));
//        
//        ^ retVal
//    }
//    
//    real realDiv(n)
//        = new RealNumber(self).divide:n;
//   
//    int sqr()
//    {
//        int n := self;        
//        
//        n := n * n;
//        
//        ^ n
//    }
//    
//    int sqrt()
//    {   
//        int num := self;
//        
//        if (num == 0)
//            { ^ 0 };
//            
//        int n := num / 2 + 1;
//        int n1 := (n + (num / n)) / 2;
//        while (n1 < n)
//        {
//            n := n1;
//            n1 := (n + (num / n)) / 2
//        };
//        
//        ^ n
//    }
//
//    power(n)
//        = self.power(cast int(n));
                
    bool isOdd()
        = (self && 1) != 0;
        
//    bool isEven()
//        = self.and(1) == 0;
//        
//    bool isZero()
//        = self.equal(0);
//        
//    bool isPositive()
//        = self > 0;
//        
//    bool isNegative()
//        = self < 0;
//        
//    bool isNonnegative()
//        = self <= 0;
//
////    high
////    [
////        int aWord := self.
////        aWord := aWord && 0FFFF0000h.
////        aWord := aWord >> 16.
////        
////        ^ shortnConvertor convert(aWord)
////    ]
////
////    low
////    [
////        int aWord := self.
////        aWord := aWord && 0FFFFh.
////        
////        ^ shortConvertor convert(aWord)
////    ]
////    
////    shift : n
////    [
////        if (n < 0)
////            [ ^ self shiftLeft(T<int>(n) negative) ];
////            [ ^ self shiftRight(T<int>(n)) ].
////    ]            
}

//public extension uintOp : UIntNumber
//{
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
//}
//    
//// --- longOp ---
//
//public extension longOp : LongNumber
//{
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
//    
//    get long Absolute()
//    {
//        long n := self;
//
//        if (n < 0l)
//            { n := n * -1l };
//                
//        ^ new LongNumber(n)
//    }
//    
//    long mod(n)
//    {
//        long x := self;
//        long y := cast long(n);
//        
//        long z := x / y;
//        z := z * y;
//        
//        ^ new LongNumber(x - z)
//    }
//    
//    long power(long y)
//    {
//        long x := self;
//        long r := 1;
//        
//        for(long i := 0l, i < y, i += 1l)
//        {
//            r := r * x;
//        };
//        
//        ^ new LongNumber(r)
//    }
//    
////    long power(int y)
////    [
////        long x := self.
////        long r := 1.
////        
////        int i := 0.
////        while (i < y)
////        [
////            r := r * x.
////            
////            i += 1.
////        ].
////        
////        ^ LongNumber new(r)
////    ]
//    
//    long sqr()
//    {
//        long n := self;        
//        long retVal := 0l;
//        
//        retVal := n * n;
//        
//        ^ new LongNumber(retVal)
//    }
//
////    power : aPower
////        = self~longOp power(T<long>(aPower)).
//                
//    bool isOdd()
//        = self.mod:2l != 0;
//        
//    bool isEven()
//        = self.mod:2l == 0;
//        
//    bool isZero()
//        = self.equal(0l);
//        
//    bool isPositive()
//        = self > 0l;
//        
//    bool isNegative()
//        = self < 0l;
//        
//    bool isNonnegative()
//        = self <= 0l;
//
////    int high
////    [
////        long aWord := self.
////        aWord := aWord && -4294967296l.
////        aWord := aWord >> 32.
////        
////        ^ intConvertor convert(aWord)
////    ]
////
////    int low
////    [
////        long aWord := self.
////        aWord := aWord && 4294967295l.
////        
////        ^ intConvertor convert(aWord)
////    ]
//}

// --- realOp ---

public extension realOp : RealNumber
{        
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
//    
//    get real Absolute()
//    {
//        real n := self;        
//        real retVal := realOpController.abs(n);
//        
//        ^ retVal
//    }
//        
//    real exp()
//    {
//        real n := self;        
//        real retVal := realOpController.exp(n);
//        
//        ^ retVal
//    }
//    
//    real ln()
//    {
//        real n := self;
//        real retVal := realOpController.ln(n);
//        
//        ^ retVal
//    }
//    
//    real sqrt()
//    {
//        real n := self;
//        real retVal := realOpController.sqrt(n);
//        
//        ^ retVal
//    }

    real power(real y)
    {
        real x := self;
        
        (x == 0.0r)
            ? { ^ 0.0r };
                        
        (y == 0.0r)
            ? { ^ 1.0r };
            
        x := realOpController.ln(x);
        
        x := x * y;
        
        x := realOpController.exp(x);
            
        ^ x            
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
            
            x := realOpController.ln(x);
            
            x := x * y;
            
            x := realOpController.exp(x);
            
            (y.isOdd())
                ? { x := x * -1.0r }
        }
        else
        {
            x := realOpController.ln(x);
            
            x := x * y;
            
            x := realOpController.exp(x)
        };
        
        ^ x
    }

//    power : aPower
//        = self~realOp power(T<int>(aPower)).

//    real sqr()
//    {
//        real n := self;        
//        real retVal := 0.0r;
//        
//        retVal := n * n;
//        
//        ^ retVal
//    }
//
//    get real Rounded()
//    {
//        real n := self;        
//        real retVal := realOpController.round(n);
//        
//        ^ retVal
//    }
//    
////    real round(int precision)
////    [
////        real a := self.
////        bool neg := false.
////        if(self isNegative)
////        [
////            a := a * -1.0r.
////            neg := true.
////        ].
////           
////        int b := 10 power:precision.
////        real r := a * b + 0.5r.
////        
////        int c := intConvertor convert(r rounded).
////    
////        a := c realDiv:b.
////    
////        ^neg iif(a * -1.0r, a). 
////    ]
//    
//    real truncate(int precision)
//    {
//        real a := self;
//        bool neg := false;
//        if(self.isNegative())
//        {
//            a := a * -1.0r;
//            neg := true
//        };
//       
//        int b := 10.power(precision);
//        real c := (a * b).Integer; // returns the integer part of the real number
//
//        a := c / b;
//
//        ^ neg ? (a * -1.0r) : a 
//    }
//
//    get real Integer()
//    {
//        real n := self;
//        real retVal := realOpController.truncate(n);
//        
//        ^ retVal
//    }
//
//    real frac()
//    {
//        real n := self;
//        real retVal := realOpController.abs(n);
//        
//        retVal := n - retVal;
//        
//        ^ retVal
//    }
//
//    real sin()
//    {
//        real n := self;  
//        real retVal := realOpController.sin(n);
//        
//        ^ retVal
//    }
//
//    real cos()
//    {
//        real n := self;
//        real retVal := realOpController.cos(n);
//        
//        ^ retVal
//    }
//
//    real tan()
//    {
//        real n := self;
//        
//        real sinVal := realOpController.sin(n);
//        real cosVal := realOpController.cos(n);
//        
//        n := sinVal / cosVal;
//        
//        ^ n
//    }
//
//    real arctan()
//    {
//        real n := self;
//        real retVal :=realOpController.arctan(n);
//        
//        ^ retVal
//    }
//            
//    real arcsin()
//    {
//        real n := self;
//        real retVal := 0.0r;
//        
//        retVal := n * n;    
//        retVal := 1.0r - retVal;
//        real arg := realOpController.sqrt(retVal);
//        
//        arg := n / arg;
//        retVal := realOpController.arctan(arg);
//        
//        ^ retVal
//    }
//            
//    real arccos()
//    {
//        real n := self;        
//        real retVal := 0.0r;
//        
//        retVal := n * n;    
//        retVal := 1.0r - retVal;        
//        real arg := realOpController.sqrt(retVal);
//        
//        arg := arg / n;
//        retVal := realOpController.arctan(arg);
//        
//        ^ retVal
//    }
//            
//    get real Radian()
//    {
//        real value := self;
//        
//        value := value * Pi_value;
//        value := value / 180.0r;
//        
//        ^ value
//    }
//            
//    get real Degree()
//    {
//        real value := self;
//        
//        value := value * 180.0r;
//        value := value / Pi_value;
//        
//        ^ value
//    }
//            
//    get real Reciprocal()
//    {
//        real value := self;
//        
//        value := 1.0r / value;
//        
//        ^ value
//    }    
//            
//    bool isZero()
//        = self == 0.0r;
//        
//    bool isPositive()
//        = self > 0.0r;
//        
//    bool isNegative()
//        = self < 0.0r;
//        
//    bool isNonnegative()
//        = self <= 0.0r;
}

//// --- intArrayOp ---
//
//public extension intArrayOp : Array<int>
//{
//    /// Copies the short array at the specified position
//    write(int index, int length, int[] array)
//        => system'core_routines'__intarray;
//
////    exchange(int anIndex1, int anIndex2)
////    [
////        int n1 := 0.
////        int n2 := 0.
////
////        self read(anIndex1, &n1).
////        self read(anIndex2, &n2).
////        
////        self write(anIndex1, n2).
////        self write(anIndex2, n1).
////    ]
////
////    populate(Func<int,int> f)
////    [
////        int anIndex := 0.
////        int aLength := self length.
////        
////        while (anIndex < aLength)
////        [            
////            self write(anIndex, f(anIndex)).
////            
////            anIndex := anIndex + 1
////        ].
////        
////        ^ self
////    ]
////    
////    populate(Func1 f)
////        = self~intArrayOp populate(:n)<int,int>[ ^f(n) ].    
//}
//
//// --- arrayOp ---
//
//public extension weakArrayOp
//{
//    Array Subarray(int index, int length)
//    {
//        auto it := self.indexer();
//        it.Index := index;
//        
//        auto a := Array.allocate(length);
//        for (int i := 0, i < length, i += 1)
//        {
//            a[i] := it.get();
//            
//            it.appendIndex(1)
//        };
//        
//        ^ a;
//    }
//}
//
////public extension<Array> arrayOp
////{
//////    exchange : anIndex1 : anIndex2
//////        = self~arrayOp exchange int:anIndex1 int:anIndex2.
////        
////    exchange(int anIndex1, int anIndex2)
////    [
////        var aTemp := self getAt(anIndex1).
////        
////        self setAt(anIndex1, self getAt(anIndex2)).
////        self setAt(anIndex2, aTemp).
////    ]
////    
////    indexOf(int anIndex, object anObject, ref<int> aRetVal)
////    [
////        aRetVal value := -1.
////        
////        int aLength := self length.
////        
////        int i := anIndex.
////        while(i < aLength)
////        [
////            var anArrayItem := self getAt(i).
////            
////            if (anObject == anArrayItem)
////                [ aRetVal value := i ].
////            
////            i := i + 1
////        ]
////    ]        
////        
////    int indexOf(int anIndex, object anObject)
////    [
////        int aRetVal := 0.
////        
////        self~arrayOp indexOf(anIndex, anObject, &aRetVal).
////        
////        ^ aRetVal
////    ]
////    
////    int indexOf : anObject
////        = self~arrayOp indexOf(0, anObject).
////        
////////    safeSeek : aValue
////////    [
////////        int aLength := self length.
////////        
////////        int i := 0.
////////        while (i < aLength)
////////        [
////////            var anArrayItem := self getAt int:i.
////////            
////////            if (aValue safeEqual:anArrayItem)
////////                [ ^ anArrayItem ].
////////            
////////            i := i + 1
////////        ].
////////        
////////        ^ $nil
////////    ]
//////        
//////    deepClone
//////    [
//////        int aLength := self length.
//////        
//////        array aCopy := Array new int:aLength.
//////                
//////        int anIndex := 0.
//////        while(anIndex < aLength)
//////        [
//////            aCopy setAt int:anIndex object:(self getAt int:anIndex; clone).
//////            
//////            anIndex := anIndex + 1.
//////        ].
//////        
//////        ^ self.
//////    ]
////    
////    populate(Func<int,object> anIndexFunc)
////    [
////        int anIndex := 0.
////        int aLength := self length.
////        
////        while(anIndex < aLength)
////        [            
////            self setAt(anIndex, anIndexFunc(anIndex)).
////            
////            anIndex := anIndex + 1.
////        ].
////        
////        ^ self
////    ]
////    
////    populate(Func1 f)
////        = self~arrayOp populate(:n)<int,object>[ ^f(T<int>(n)) ].
////
//////    multi populate : anAction
//////        = self~arrayOp populate(:n)<int>[ ^anAction eval:n ].    
////    
////    sort(Func2 aCompareFunc)
////    [
////        int i := 0.
////        int j := 0.
////        int aLength := self length.
////        var aCurrent := nil.
////        var aNext := nil.
////        
////        while(i < aLength)
////        [
////            aCurrent := self getAt(i).
////            
////            j := i + 1.
////            while (j < aLength)
////            [
////                aNext := self getAt(j).
////                ifnot(aCompareFunc(aCurrent,aNext))
////                    [                        
////                        self setAt(i, aNext).
////                        self setAt(j, aCurrent).
////                        
////                        aCurrent := aNext
////                    ].
////                    
////                j := j + 1
////            ].
////            
////            i := i + 1
////        ].
////        
////        ^ self
////    ]
////    
////    ascendant
////        = (self clone)~arrayOp sort(:aFormer:aLater)(aFormer < aLater).
////    
////    bool isEmpty
////    [
////        int aLength := self length.
////        
////        ^ aLength == 0.
////    ]
////    
////    T<Array> Subarray : aLength at:anIndex
////        = self~arrayOp Subarray(anIndex, aLength).
////    
////    T<Array> Subarray(int anIndex, int aLength)
////    [
////        auto it := T<Indexer>(self).
////        
////        ^ Array new(aLength); populate(:i)<int>(it writeIndex(anIndex + i); get).
////    ]
////}
//
//public extension arrayOp<T> : Array<T>
//{
//    T[] Subarray(int index, int length)
//    {
//        ^ new Array<T>(self, index, length)
//    }
//}
//
////// --- intmatrixOp ---
////
////public extension<IntMatrix> intMatrixOp
////{
////    populate(Func<int,int,int> fn)
////    [
////        int n := 0.
////        int m := 0.
////        self readLengthsTo(&n, &m).
////        
////        int i := 0.
////        int j := 0.
////        while (i < n)
////        [
////            j := 0.
////            while (j < m)
////            [
////                int v := fn(i, j).
////                
////                self write(i, j, v).
////                
////                j := j + 1
////            ].
////            
////            i := i + 1
////        ].
////        
////        ^ self
////    ]
////}
//
//// --- literalOp ---
//
//public extension stringOp : String
//{
//    HashCode
//    {
//        /// Reads the object hash code into the output parameter
//        int get()
//        {
//            int len := self.Length;
//            
//            int h := extern elenart.core_utf8hashcode(self, len);
//            
//            ^ h
//        }    
//    }
//
////    indexOf : anObject
////        = self indexOf:anObject at:0.
////
//    int indexOf(string s)
//        = self.indexOf(0, s);
//
//    int indexOf(char ch)
//        = self.indexOf(0, ch);
//        
//    string Substring(index)
//        = self.Substring(cast int(index));
//        
//    string Substring(int index)
//    {
//        int len := self.Length;
//        if (len > index)
//        {
//            len := len - index;
//            
//            ^ self.Substring(index, len)
//        }
//        else
//        {
//            ^ emptyString
//        }        
//    }        
//    
//    string trimLeft(char ch)
//    {
//        int  index := 0;
//        int  len := self.Length;
//        char current := $0;
//        int  chLen := ch.Length;
//        
//        while(index < len)
//        {
//            self.read(index, ref current);
//            
//            ifnot(current == ch)
//            { 
//                if (index == 0)
//                { 
//                    ^ self 
//                }
//                else
//                { 
//                    ^ self.delete(0, index) 
//                }
//            };
//            
//            index += chLen
//        };
//        
//        ^ emptyString
//    }
//    
//    string trimLeft()
//        = self.trimLeft($32);
//       
//    string trimRight(char ch)
//    {
//        int  len := self.Length;
//        int  index := len - 1;
//        int  prev := len;
//        char current := $0;
//        int  chLen := ch.Length;
//        
//        while (index >= 0)
//        {
//            self.read(index, ref current);
//            
//            ifnot (current == ch)
//                { ^ self.delete(prev,len - prev) };
//            
//            prev := index;
//            index -= chLen
//        };
//        
//        ^ emptyString
//    }
//    
//    string trimRight()
//        = self.trimRight($32);
//        
//    string trim(char ch)
//        = self.trimLeft(ch).trimRight(ch);
//    
//    string trim()
//        = self.trimLeft($32).trimRight($32);
//    
//    string padLeft(int length)
//        = self.padLeft($32, length);
//    
//    string padLeft(char ch, int length)
//    {
//        int currentLength := self.Length;
//        
//        if (currentLength < length)
//            { ^ String.fill(length - currentLength, ch).add(self) };
//            
//        ^ self
//    }
//    
//    string padRight(int length)
//        = self.padRight($32, length);
//    
//    string padRight(char ch, int length)
//    {
//        int currentLength := self.Length;
//
//        if (currentLength < length)
//            { ^ self.add(String.fill(length - currentLength, ch)) };
//            
//        ^ self
//    }
//
//    bool startingWith(string s)
//    {
//        int n := 0;
//        self.indexOf(0, s, ref n);
//        
//        ^ 0 == n
//    }
//        
//    bool endingWith(string s)
//    {
//        int l1 := self.Length;
//        int l2 := s.Length;
//
//        int n := 0;
//        self.indexOf(l1 - l2, s, ref n);
//        
//        ^ n != -1
//    }
//        
//    bool containing(string s)
//    {
//        int l1 := self.Length;
//        int l2 := s.Length;
//
//        self.indexOf(0, s, ref int n);
//        
//        ^ n != -1
//    }
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
//    string replace(string replacee, string replacer)
//    {
//        string target := emptyString;
//        int    start := 0;
//        int    end := 0;
//        int    replaceeLen := replacee.Length;
//        
//        self.indexOf(start, replacee, ref end);
//        
//        while (end != -1)
//        {
//            target := target
//                .add(self.Substring(start, end - start))
//                .add(replacer);
//            
//            start := end + replaceeLen;
//            
//            self.indexOf(start, replacee, ref end)
//        };
//    
//        if(start != 0)
//        {
//            ^ target.add(self.Substring(start))
//        }
//        else
//        {
//            ^ self
//        }
//    }
//    
//    bool isEmpty()
//    {
//        int length := self.Length;
//        
//        ^ length == 0
//    }
//}
//
//// --- wideOp ---
//
//public extension wideOp : WideString
//{
//    HashCode
//    {
//        int get()
//        {
//            int len := self.Length;
//        
//            int h := extern elenart.core_utf16hashcode(self, len);
//        
//            ^ h
//        }
//    }
//    
////    indexOf : anObject
////        = self indexOf:anObject at:0.
//        
//    indexOf(wide s)
//        = self.indexOf(0, s);
//
//    indexOf(char ch)
//        = self.indexOf(0, ch);
//                
////    wide Substring(index)
////        = self~wideOp Substring(T<int>(anIndex)).
//        
//    wide Substring(int index)
//    {
//        int length := self.Length;
//        
//        length := length - index;
//        
//        ^ self.Substring(index, length)
//    }
//     
//    wide trimLeft(char ch)
//    {
//        int  index := 0;
//        int  len := self.Length;
//        char current := $0;
//        int  chLen := ch.Length;
//        
//        while (index < len)
//        {
//            self.read(index, ref current);
//            
//            ifnot(current == ch)
//                { ^ self.delete(0, index) };
//            
//            index += chLen
//        };
//        
//        ^ emptyWideString
//    }
//    
//    wide trimLeft()
//        = self.trimLeft($32);
//       
//    wide trimRight(char ch)
//    {
//        int  len := self.Length;
//        char current := $0;
//        int  chLen := ch.Length;
//                
//        int  index := len - 1;
//        int  prev := len;
//        while (index >= 0)
//        {
//            self.read(index, ref current);
//            
//            ifnot (current == ch)
//                { ^ self.delete(prev, len - prev) };
//            
//            prev := index;
//            index -= chLen
//        };
//        
//        ^ emptyWideString
//    }
//    
//    wide trimRight()
//        = self.trimRight($32);
//       
//    wide trim(char ch)
//        = self.trimLeft(ch).trimRight(ch);
//    
//    wide trim()
//        = self.trimLeft($32).trimRight($32);
//    
//    wide padLeft(char ch, int length)
//    {
//        int currentLength := self.Length;
//        
//        if (currentLength < length)
//            { ^ WideString.fill(length - currentLength, ch).add(self) };
//            
//        ^ self
//    }
//    
//    wide padRight(char ch, int length)
//    {
//        int currentLength := self.Length;
//
//        if (currentLength < length)
//            { ^ self.add(WideString.fill(length - currentLength, ch)) };
//            
//        ^ self
//    }
//
//    bool startingWith(wide s)
//    {
//        int n := 0;
//        self.indexOf(0, s, ref n);
//        
//        ^ 0 == n
//    }
//    
//    bool endingWith(wide s)
//    {
//        int l1 := self.Length;
//        int l2 := s.Length;
//        int n := 0;
//        self.indexOf(l1 - l2, s, ref n);
//        
//        ^ n != -1
//    }
//    
//    bool containing(wide s)
//    {
//        int l1 := self.Length;
//        int l2 := s.Length;
//        int n := 0;
//        
//        self.indexOf(0, s, ref n);
//        
//        ^ n != -1
//    }
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
//        
//    replace(wide replacee, wide replacer)
//    {
//        wide target := emptyWideString;
//        int  start := 0;
//        int  end := 0;
//        int  replaceeLen := replacee.Length;
//        
//        self.indexOf(start, replacee, ref end);
//        
//        while (end != -1)
//        {
//            target := target
//                .add(self.Substring(start, end - start))
//                .add:replacer;
//            
//            start := end + replaceeLen;
//            
//            self.indexOf(start, replacee, ref end)
//        };
//    
//        if (start != 0)
//        { 
//            ^ target.add(self.Substring(start)) 
//        }
//        else
//        { 
//            ^ self 
//        }
//    }
//
//    bool isEmpty()
//    {
//        int length := self.Length;
//        
//        ^ length == 0
//    }
//}
//
//// --- stringOp ---
//
//singleton stringOpDispatcher
//{
//    padRight(string s, char ch, int len)
//        = s.padRight(ch, len);
//    
//    padRight(wide s, char ch, int len)
//        = s.padRight(ch, len);
//        
//    padLeft(string s, char ch, int len)
//        = s.padLeft(ch, len);
//    
//    padLeft(wide s, char ch, int len)
//        = s.padLeft(ch, len);
//        
//    bool startingWith(string s, string subs)
//        = s.startingWith(subs);
//
//    bool startingWith(wide s, wide subs)
//        = s.startingWith(subs);
//
//    bool endingWith(string s, string subs)
//        = s.endingWith(subs);
//
//    bool endingWith(wide s, wide subs)
//        = s.endingWith(subs);
//        
//    bool containing(string s, string subs)
//        = s.containing(subs);
//
//    bool containing(wide s, wide subs)
//        = s.containing(subs);
//        
//    trimLeft(string s, char ch)
//        =s.trimLeft(ch);       
//        
//    trimRight(string s, char ch)
//        =s.trimRight(ch);
//        
//    trim(string s, char ch)
//        =s.trim(ch);
//        
//    trimLeft(wide s, char ch)
//        =s.trimLeft(ch);        
//        
//    trimRight(wide s, char ch)
//        =s.trimRight(ch);        
//        
//    trim(wide s, char ch)
//        =s.trim(ch);
//        
//    replace(string s, src, dst)
//        = s.replace(cast string(src), cast string(dst));
//        
//    replace(string s, string src, string dst)
//        = s.replace(src, dst);
//        
//    replace(wide s, wide src, wide dst)
//        = s.replace(src, dst);
//}
//
//public extension stringWeakOp
//{
//    Substring(index)
//        = self.Substring(index, self.Length - index);
//            
////    Substring(object anIndex, object aLength)
////        = self Substring:aLength at:anIndex.
//    
//    trimLeft(char ch)
//        = stringOpDispatcher.trimLeft(self, ch);
//
//    trimRight(char ch)
//        = stringOpDispatcher.trimRight(self, ch);
//
//    trim(char ch)
//        = stringOpDispatcher.trim(self, ch);
//    
//    trimLeft()
//        = stringOpDispatcher.trimLeft(self, $32);
//
//    trimRight()
//        = stringOpDispatcher.trimRight(self, $32);
//
//    trim()
//        = stringOpDispatcher.trim(self, $32);
//
//    padLeft(padSymbol, len)
//        = stringOpDispatcher.padLeft(self, padSymbol, len);
//        
//    padRight(padSymbol, len)
//        = stringOpDispatcher.padRight(self, padSymbol, len);
//
//    startingWith(s)
//        = stringOpDispatcher.startingWith(self, s);
//
//    endingWith(s)
//        = stringOpDispatcher.endingWith(self, s);
//
//    containing(s)
//        = stringOpDispatcher.containing(self, s);
//
//////    replaceFirst literal:aReplacee literal:aReplacer
//////    [
//////        var dispatcher := self cast:%eval to:stringOpDispatcher.
//////        
//////        ^ self~dispatcher replaceFirst literal:aReplacee literal:aReplacer
//////    ]
//    
//    replace(replacee, replacer)
//        = stringOpDispatcher.replace(self, replacee, replacer);
//    
//    fillString(s, int counter)
//    {
//        for (int i := 0, i < counter, i += 1)
//        {
//            self.write(s)
//        };
//            
//        ^ self
//    }
//
//    writeCopies(line, counter)
//        = self.fillString(line, cast int(counter));
//
//    writePaddingLeft(line, char ch, int width)
//    {
//        var paddedStr := line.padLeft(ch, width);
//
//        self.write(paddedStr);
//            
//        ^ self
//    }
//    
//    writePaddingRight(line, char ch, int width)
//    {
//        var paddedStr := line.padRight(ch, width);
//
//        self.write(paddedStr);
//            
//        ^ self
//    }
//    
//    writePaddingLeft(line, int width)
//    {
//        var paddedStr := line.padLeft($32, width);
//
//        self.write(paddedStr);
//            
//        ^ self
//    }
//    
//    writePaddingRight(line, int width)
//    {
//        var paddedStr := line.padRight($32, width);
//
//        self.write(paddedStr);
//            
//        ^ self
//    }
//    
//    writePadding(line, char ch, int width)
//    {
//        int length := line.Length;        
//        
//        if (length < width)
//        {
//            int len := width - length;
//            int leftLen := len / 2;
//            
//            self.fillString(ch, leftLen);
//            self.write(line);
//            self.fillString(ch, len - leftLen)
//        }
//        else
//        { 
//            self.write(line)
//        };
//            
//        ^ self
//    }    
//}

// --- charOp ---

public extension charOp : CharValue
{
//    HashCode
//    {
//        // Reads the object hash code into the output parameter
//        int get()
//        {
//            intern system'core_routines'__int.readHashCodeTo(self, ref int retVal);
//            
//            ^ retVal
//        }
//    }
    
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
    
    bool isDigit()
    {
        int ch := intConvertor.convert(self);
        
        ^ (ch >= 30h) && (ch <= 39h)
    }
    
//    bool isNewLine()
//    {
//        ^ self == $10
//    }
//    
//    bool isLetter()
//    {
//        int ch := intConvertor.convert(self);
//        
//        if (ch >= 41h && ch <= 5Ah)
//            { ^ true };
//        
//        if (ch >= 61h && ch <= 7Ah)
//            { ^ true };
//        
//        if (ch >= 3ACh && ch <= 3CEh)
//            { ^ true };
//            
//        if (ch >= 400h && ch <= 42Fh)
//            { ^ true };
//            
//        ^ false
//    }
    
    bool isLToken()
    {
        int ch := intConvertor.convert(self);
        
        if (ch >= 41h && ch <= 5Ah)
            { ^ true };
        
        if (ch >= 61h && ch <= 7Ah)
            { ^ true };
        
        if (ch >= 3ACh && ch <= 3CEh)
            { ^ true };
            
        if (ch >= 400h && ch <= 42Fh)
            { ^ true };
            
        ^ ch == 39
    }
}

// --- byteArrayHelper ---

public extension byteArrayHelper : Array<byte>
{
//    /// Reads a byte value at the specified position and saves it into an output variable
//    read(int index, ref byte retVal)
//        => system'core_routines'__bytearray;

    /// Reads a short integer value at the specified position and saves it into an output variable
    read(int index, ref short retVal)
        => intern system'core_routines'__bytearray;

    /// Reads an integer value at the specified position and saves it into an output variable
    read(int index, ref int retVal)
        => intern system'core_routines'__bytearray;

    /// Reads a character value at the specified position and saves it into an output variable
    read(int index, ref char retVal)
        => intern system'core_routines'__bytearray;                
        
//    move(int index, int length, int offset)
//        => system'core_routines'__bytearray;
//                
//    /// Copies a sub array to the provided one at the specified position
//    read(int index, int length, byte[] dump)
//        => system'core_routines'__bytearray;
//        
//    /// Copies the byte array at the specified position
//    write(int index, int length, byte[] dump)
//        => system'core_routines'__bytearray;
//        
//    /// Copies the byte array at the specified position
//    /// Copies the byte array at the specified position
//    write(int index, short value)
//        => system'core_routines'__bytearray;
//        
//    /// Copies the byte array at the specified position
//    write(int index, int value)
//        => system'core_routines'__bytearray;
//        
//    /// Copies the byte array at the specified position
//    write(int index, char value)
//        => system'core_routines'__bytearray;
//        
//    /// Copies the byte array at the specified position
//    write(int index, long value)
//        => system'core_routines'__bytearray;
//        
//    /// Copies the byte array at the specified position
//    write(int index, real value)
//        => system'core_routines'__bytearray;
//
//    /// Fills the sub array with a specified value
//    fill(int offset, int length, byte val)
//    {
//        int totalLength := self.Length;
//        
//        int i := offset;
//        int m := offset + length;
//        
//        if (i > totalLength)
//            { system'InvalidArgumentException.new().raise() };
//        
//        if (m > totalLength)
//            { system'InvalidArgumentException.new().raise() };
//            
//        while(i < m)
//        {
//            self[i] :=  val;
//            
//            i := i + 1
//        }
//    }
}

// --- charArrayHelper ---

public extension charArrayHelper : Array<char>
{
//    move(int index, int length, int offset)
//        => system'core_routines'__chararray;

    /// Copies the array at the specified position    
    write(int index, int length, char[] array)
        => intern system'core_routines'__chararray;
}

////public singleton byteArrayHelper
////{
//////    read(ByteArray array, int offset, ref<short> retVal)
//////    [
//////        array read(offset, retVal)
//////    ]
//////    
//////    read(ByteArray array, int offset, ref<int> retVal)
//////    [
//////        array read(offset, retVal)
//////    ]
//////    
//////    read(ByteArray array, int offset, ref<long> retVal)
//////    [
//////        array read(offset, retVal)
//////    ]
//////    
//////    read(ByteArray array, int offset, ref<real> retVal)
//////    [
//////        array read(offset, retVal)
//////    ]
//////    
//////    read(ByteArray array, int offset, ref<char> retVal)
//////    [
//////        array read(offset, retVal)
//////    ]
//////    
//////    write(ByteArray array, int offset, short retVal)
//////    [
//////        array write(offset, retVal)
//////    ]
//////    
//////    write(ByteArray array, int offset, int retVal)
//////    [
//////        array write(offset, retVal)
//////    ]
//////    
//////    write(ByteArray array, int offset, long retVal)
//////    [
//////        array write(offset, retVal).
//////    ]
//////    
//////    write(ByteArray array, int offset, real retVal)
//////    [
//////        array write(offset, retVal)
//////    ]
//////    
//////    write(ByteArray array, int offset, char retVal)
//////    [
//////        array write(offset, retVal)
//////    ]
//////    
////}
////// --- intArrayHelper ---
//////
//////singleton intArrayHelper
//////{
//////    stacksafe fill intarray:array int:offset int:length int:val
//////    [
//////        array fill int:offset int:length int:val.
//////    ]
//////}