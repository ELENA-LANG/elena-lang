#define system.

// --- Operations ---

// --- intOp ---

#class(extension:int) intOp
{
    #method absolute
    [
        #var(type:int)n := self int.
        
        ^ (n < 0) ? [ -1 * n ] ! [ n ].
    ]
    
    #method mod : anOperand
        = self - (self / anOperand * anOperand).
                
    #method is &odd
        = self::intOp mod:2 != 0.
        
    #method is &even
        = self::intOp mod:2 == 0.
        
    #method is &zero
        = self equal &int:0.

    #method hiWord
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FFFF0000h.
        aWord := aWord shift &index:16.
        
        ^ IntNumber new &int:aWord.
    ]

    #method loWord
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FFFFh.
        
        ^ IntNumber new &int:aWord.
    ]
    
    #method hiByte
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FF00h.
        aWord := aWord shift &index:8.
        
        ^ IntNumber new &int:aWord.
    ]
        
    #method loByte
        = self int and &int:0FFh.
}

// --- realOp ---

#symbol realOpController =
{
    modulus &real:aReal &out'real:aRetVal
        => system'internal'r_abs.
        
    exp &real:aReal &out'real:aRetVal
        => system'internal'r_exp.
        
    abs &real:aReal &out'real:aRetVal
        => system'internal'r_abs.
        
    ln &real:aReal &out'real:aRetVal
        => system'internal'r_ln.
        
    round &real:aReal &out'real:aRetVal
        => system'internal'r_round.
        
    truncate &real:aReal &out'real:aRetVal
        => system'internal'r_truncate.
}.

#class(extension:real) realOp
{        
    #method modulus
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController modulus &real:n.
        
        ^ RealNumber new &real:aRetVal.
    ]        
        
    #method(type:real) absolute
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController abs &real:n.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    #method(type:real) exp
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController exp &real:n.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(type:real) ln
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController ln &real:n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) rounded
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController round &real:n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) integer
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController truncate &real:n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) frac
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal := realOpController modulus &real:n.
        
        ^ RealNumber new &real:(n subtract &real:aRetVal).
    ]
    
    #method isZero
        = self == 0.
        
    #method isPositive
        = self > 0.
        
    #method isNegative
        = self < 0.
        
    #method isNonnegative
        = self <= 0.
}
