//class mathOpDispatcher =
//{
//    eval byte:aValue = byteOp.
//    
//    eval short:aValue = shortOp.
//    
//    eval int:aValue = intOp.
//    
//    eval long:aValue = longOp.
//    
//    eval real:aValue = realOp.
//}.

// --- mathController ---

$mathController =
{
    absolute value:n
    [
        system'NotSupportedException new; raise
    ]
    
    absolute byte:n
        = n~byteOp absolute.
        
    absolute short:n
        = n~shortOp absolute.
        
    absolute int:n
        = n~intOp absolute.
        
    absolute long:n
        = n~longOp absolute.
        
    absolute real:n 
        = n~realOp absolute.
        
    multi absolute : n
        <= absolute(n value).
        
    mod byte:n object:m
        = n~byteOp mod:m.
        
    mod short:n object:m
        = n~shortOp mod:m.
        
    mod int:n object:m
        = n~intOp mod:m.
        
    mod long:n object:m
        = n~longOp mod:m.
        
    mod real:n object:m
        = n~realOp mod:m.

    multi mod : n : m
        <= mod(n value, m).
                
    isOdd value:n
    [
        system'NotSupportedException new; raise
    ]
    
    bool isOdd byte:n
        = n~byteOp isOdd.
        
    bool isOdd short:n
        = n~shortOp isOdd.
        
    bool isOdd int:n
        = n~intOp isOdd.
        
    bool isOdd long:n
        = n~longOp isOdd.

    bool multi isOdd:n
        <= isOdd(n value).
                        
    isEven value:n
    [
        system'NotSupportedException new; raise
    ]
    
    bool isEven byte:n
        = n~byteOp isEven.
        
    bool isEven short:n
        = n~shortOp isEven.
        
    bool isEven int:n
        = n~intOp isEven.
        
    bool isEven long:n
        = n~longOp isEven.

    bool multi isEven:n
        <= isEven(n value).
                
    isZero value:n
    [
        system'NotSupportedException new; raise
    ]
    
    bool isZero byte:n
        = n~byteOp isZero.
        
    bool isZero short:n
        = n~shortOp isZero.
        
    bool isZero int:n
        = n~intOp isZero.
        
    bool isZero long:n
        = n~longOp isZero.
        
    bool isZero real:n 
        = n~realOp isZero.
        
    bool multi isZero:n
        <= isZero(n value).
        
    isPositive value:n
    [
        system'NotSupportedException new; raise
    ]
    
    bool isPositive byte:n
        = n~byteOp isPositive.
        
    bool isPositive short:n
        = n~shortOp isPositive.
        
    bool isPositive int:n
        = n~intOp isPositive.
        
    bool isPositive long:n
        = n~longOp isPositive.
        
    bool isPositive real:n 
        = n~realOp isPositive.
        
    bool multi isPositive:n
        <= isPositive(n value).
        
    isNegative value:n
    [
        system'NotSupportedException new; raise
    ]
    
    bool isNegative byte:n
        = n~byteOp isNegative.
        
    bool isNegative short:n
        = n~shortOp isNegative.
        
    bool isNegative int:n
        = n~intOp isNegative.
        
    bool isNegative long:n
        = n~longOp isNegative.
        
    bool isNegative real:n 
        = n~realOp isNegative.

    bool multi isNegative:n
        <= isNegative(n value).
                
    isNonnegative value:n
    [
        system'NotSupportedException new; raise
    ]
    
    bool isNonnegative byte:n
        = n~byteOp isNonnegative.
        
    bool isNonnegative short:n
        = n~shortOp isNonnegative.
        
    bool isNonnegative int:n
        = n~intOp isNonnegative.
        
    bool isNonnegative long:n
        = n~longOp isNonnegative.
        
    bool isNonnegative real:n 
        = n~realOp isNonnegative.

    bool multi isNonnegative:n
        <= isNonnegative(n value).
                
    real power real:n int:m
        = n~realOp power int:m.        
        
    real power real:n real:m
        = n~realOp power real:m.        
        
    power byte:n int:m
        = n~byteOp power int:m.        
        
    power short:n int:m
        = n~shortOp power int:m.        
        
    power int:n int:m
        = n~intOp power int:m.        
        
    power long:n int:m
        = n~longOp power int:m.        
        
    multi power:n:m
        <= power(n value, m).
        
    sqr byte:n
        = n~byteOp sqr.
        
    sqr value:n
    [
        system'NotSupportedException new; raise
    ]
    
    sqr short:n
        = n~shortOp sqr.
        
    sqr int:n
        = n~intOp sqr.
        
    sqr long:n
        = n~longOp sqr.
        
    sqr real:n 
        = n~realOp sqr.
        
    multi sqr:n
        <= sqr(n value).
}.

// --- mathOp ---

extension mathOp
{
    absolute
        = $mathController absolute:self.

    mod : anOperand
        = $mathController mod(self, anOperand).

    bool isOdd
        = $mathController isOdd(self).

    bool isEven
        = $mathController isEven(self).

    bool isZero
        = $mathController isZero(self).

    bool isPositive
        = $mathController Positive(self).

    bool isNegative
        = $mathController isNegative(self).

    bool isNonnegative
        = $mathController isNonnegative(self).

    exp
        = (self real)~realOp exp.
    
    real ln
        = self real; ln.
    
    sqrt
        = self real; sqrt.

    power : aPower
        = $mathController power(self, aPower).

    sqr
        = $mathController sqr(self).
        
    rounded
        = self real; rounded.

    roundedInt
        = intConvertor convert real:(self real; rounded).

    integer
        = (self real)~realOp integer.

    integerInt
        = intConvertor convert real:((self real)~realOp integer).

    frac
        = (self real)~realOp frac.

    sin
        = (self real)~realOp sin.

    cos
        = (self real)~realOp cos.

    tan
        = (self real)~realOp tan.

    arctan
        = (self real)~realOp arctan.
            
    arcsin
        = (self real)~realOp arcsin.
            
    arccos
        = (self real)~realOp arccos.
            
    radian
        = (self real)~realOp radian.
            
    degree
        = (self real)~realOp degree.
            
    reciprocal
        = (self real)~realOp reciprocal.
}