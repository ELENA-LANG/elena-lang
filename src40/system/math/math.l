// --- mathController ---

singleton mathController
{
////    absolute value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    absolute(byte n)
//        = n~byteOp absolute.
//        
//    absolute(short n)
//        = n~shortOp absolute.
//        
//    absolute(int n)
//        = n~intOp absolute.
//        
//    absolute(long n)
//        = n~longOp absolute.
//        
//    absolute(real n)
//        = n~realOp absolute.
//        
//    absolute:n
//        <= absolute(n value).
//        
//    mod(byte n, object m)
//        = n~byteOp mod:m.
//        
//    mod(short n, object m)
//        = n~shortOp mod:m.
        
    mod(int n, object m)
        = n.mod:m;
//        = mixin intOp(n).mod:m;
        
//    mod(long n, object m)
//        = n~longOp mod:m.

    mod(n, m)
        <= mod(n.Value, m);
                
////    isOdd value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    bool isOdd(byte n)
//        = n~byteOp isOdd.
//        
//    bool isOdd(short n)
//        = n~shortOp isOdd.
//        
//    bool isOdd(int n)
//        = n~intOp isOdd.
//        
//    bool isOdd(long n)
//        = n~longOp isOdd.
//
//    bool isOdd:n
//        <= isOdd(n value).
//                        
////    isEven value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    bool isEven(byte n)
//        = n~byteOp isEven.
//        
//    bool isEven(short n)
//        = n~shortOp isEven.
//        
//    bool isEven(int n)
//        = n~intOp isEven.
//        
//    bool isEven(long n)
//        = n~longOp isEven.
//
//    bool isEven:n
//        <= isEven(n value).
//                
////    isZero value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    bool isZero(byte n)
//        = n~byteOp isZero.
//        
//    bool isZero(short n)
//        = n~shortOp isZero.
//        
//    bool isZero(int n)
//        = n~intOp isZero.
//        
//    bool isZero(long n)
//        = n~longOp isZero.
//        
//    bool isZero(real n)
//        = n~realOp isZero.
//        
//    bool isZero:n
//        <= isZero(n value).
//        
////    isPositive value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    bool isPositive(byte n)
//        = n~byteOp isPositive.
//        
//    bool isPositive(short n)
//        = n~shortOp isPositive.
//        
//    bool isPositive(int n)
//        = n~intOp isPositive.
//        
//    bool isPositive(long n)
//        = n~longOp isPositive.
//        
//    bool isPositive(real n)
//        = n~realOp isPositive.
//        
//    bool isPositive:n
//        <= isPositive(n value).
//        
////    isNegative value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    bool isNegative(byte n)
//        = n~byteOp isNegative.
//        
//    bool isNegative(short n)
//        = n~shortOp isNegative.
//        
//    bool isNegative(int n)
//        = n~intOp isNegative.
//        
//    bool isNegative(long n)
//        = n~longOp isNegative.
//        
//    bool isNegative(real n)
//        = n~realOp isNegative.
//
//    bool isNegative:n
//        <= isNegative(n value).
//                
////    isNonnegative value:n
////    [
////        system'NotSupportedException new; raise
////    ]
//    
//    bool isNonnegative(byte n)
//        = n~byteOp isNonnegative.
//        
//    bool isNonnegative(short n)
//        = n~shortOp isNonnegative.
//        
//    bool isNonnegative(int n)
//        = n~intOp isNonnegative.
//        
//    bool isNonnegative(long n)
//        = n~longOp isNonnegative.
//        
//    bool isNonnegative(real n)
//        = n~realOp isNonnegative.
//
//    bool isNonnegative:n
//        <= isNonnegative(n value).
                
    real power(real n, int m)
        = mixin realOp(n).power(m);
        
    real power(real n, real m)
        = mixin realOp(n).power(m);        
        
    power(byte n, int m)
        = mixin byteOp(n).power(m);       
        
    power(short n, int m)
        = mixin shortOp(n).power(m);        
        
    power(int n, int m)
        = mixin intOp(n).power(m);      
        
    power(long n, int m)
        = mixin longOp(n).power(m);
        
//    power(object n, object m)
//        <= power(n value, m).
        
    sqr(byte n)
        = n.sqr();
        
//    sqr value:n
//    [
//        system'NotSupportedException new; raise
//    ]

    sqr(short n)
        = n.sqr();
        
    sqr(int n)
        = n.sqr();
        
    sqr(long n)
        = n.sqr();
        
    sqr(real n)
        = n.sqr();
        
    sqr(n)
        <= sqr(n.Value);
}

// --- mathOp ---

public extension mathOp
{
//    absolute
//        = mathController absolute:self.

    mod(n)
        = mathController.mod(self, n);

//    bool isOdd
//        = mathController isOdd(self).
//
//    bool isEven
//        = mathController isEven(self).
//
//    bool isZero
//        = mathController isZero(self).
//
//    bool isPositive
//        = mathController isPositive(self).
//
//    bool isNegative
//        = mathController isNegative(self).
//
//    bool isNonnegative
//        = mathController isNonnegative(self).
//
//    real floor
//    [
//        real x := T<real>(self).
//        
//        if (x >= 0)
//        [
//            ^ x integer.
//        ];
//        [
//            real y := x integer.
//            
//            ^ (y == x)iif(y,y-1).
//        ].
//    ]
//
//    real ceil
//    [
//        real x := T<real>(self).
//        
//        x := x * (-1.0r).
//        
//        x := x~mathOp floor.
//        
//        x := x * (-1.0r).
//        
//        ^ x.
//    ]
//
//    real exp
//        = (T<real>(self))~realOp exp.
    
    real ln()
        = cast real(self).ln();
  
    real log2()
    {
        real x := self.ln();
        real y := mixin mathOp(2).ln();
        
        ^ x / y
    }
        
    real log10()
    {
        real x := self.ln();
        real y := mixin mathOp(10).ln();
        
        ^ x / y
    }
    
    sqrt()
        = cast real(self).sqrt();

    power(p)
        = mathController.power(self, p);

    sqr()
        = mathController.sqr(self);
        
//    rounded
//        = T<real>(self) rounded.
//
//    roundedInt
//        = intConvertor convert(T<real>(self) rounded).
//
//    integer
//        = (T<real>(self))~realOp integer.
//
//    integerInt
//        = intConvertor convert((T<real>(self))~realOp integer).
//
//    frac
//        = (T<real>(self))~realOp frac.
//
//    sin
//        = (T<real>(self))~realOp sin.
//
//    cos
//        = (T<real>(self))~realOp cos.
//
//    tan
//        = (T<real>(self))~realOp tan.
//
//    arctan
//        = (T<real>(self))~realOp arctan.
//            
//    arcsin
//        = (T<real>(self))~realOp arcsin.
//            
//    arccos
//        = (T<real>(self))~realOp arccos.
//            
//    radian
//        = (T<real>(self))~realOp radian.
//            
//    degree
//        = (T<real>(self))~realOp degree.
//            
//    reciprocal
//        = (T<real>(self))~realOp reciprocal.
}