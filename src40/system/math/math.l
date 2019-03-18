// --- mathController ---

singleton mathController
{
////    absolute value:n
////    [
////        system'NotSupportedException new; raise
////    ]
    
    absolute(byte n)
        = n.Absolute;
        
    absolute(short n)
        = n.Absolute;
        
    absolute(int n)
        = n.Absolute;
        
    absolute(long n)
        = n.Absolute;
        
    absolute(real n)
        = n.Absolute;
        
    absolute(n)
        <= absolute(n.Value);
        
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
                
//    isOdd value:n
//    [
//        system'NotSupportedException new; raise
//    ]
    
    bool isOdd(byte n)
        = n.isOdd();
        
    bool isOdd(short n)
        = n.isOdd();
        
    bool isOdd(int n)
        = n.isOdd();
        
    bool isOdd(long n)
        = n.isOdd();

    bool isOdd(n)
        <= isOdd(n.Value);
                        
//    isEven value:n
//    [
//        system'NotSupportedException new; raise
//    ]
    
    bool isEven(byte n)
        = n.isEven();
        
    bool isEven(short n)
        = n.isEven();
        
    bool isEven(int n)
        = n.isEven();
        
    bool isEven(long n)
        = n.isEven();

    bool isEven(n)
        <= isEven(n.Value);
                
//    isZero value:n
//    [
//        system'NotSupportedException new; raise
//    ]
    
    bool isZero(byte n)
        = n.isZero();
        
    bool isZero(short n)
        = n.isZero();
        
    bool isZero(int n)
        = n.isZero();
        
    bool isZero(long n)
        = n.isZero();
        
    bool isZero(real n)
        = n.isZero();
        
    bool isZero(n)
        <= isZero(n.Value);
        
//    isPositive value:n
//    [
//        system'NotSupportedException new; raise
//    ]
    
    bool isPositive(byte n)
        = n.isPositive();
        
    bool isPositive(short n)
        = n.isPositive();
        
    bool isPositive(int n)
        = n.isPositive();
        
    bool isPositive(long n)
        = n.isPositive();
        
    bool isPositive(real n)
        = n.isPositive();
        
    bool isPositive(n)
        <= isPositive(n.Value);
        
//    isNegative value:n
//    [
//        system'NotSupportedException new; raise
//    ]
    
    bool isNegative(byte n)
        = n.isNegative();
        
    bool isNegative(short n)
        = n.isNegative();
        
    bool isNegative(int n)
        = n.isNegative();
        
    bool isNegative(long n)
        = n.isNegative();
        
    bool isNegative(real n)
        = n.isNegative();

    bool isNegative(n)
        <= isNegative(n.Value);
                
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
        
    power(object n, object m)
        <= power(n.Value, m);
        
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
    Absolute
        = mathController.absolute:self;

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

    sin()
        = cast real(self).sin();

    cos()
        = cast real(self).cos();

    tan()
        = cast real(self).tan();

    arctan()
        = cast real(self).arctan();
            
    arcsin()
        = cast real(self).arcsin();
                    
    arccos()
        = cast real(self).arccos();
            
    Radian
        = cast real(self).Radian;
            
    Degree
        = cast real(self).Degree;
            
//    reciprocal
//        = (T<real>(self))~realOp reciprocal.
}