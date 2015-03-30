#define system.

// --- Operations ---

// --- byteOp ---

#class(extension:byte) byteOp
{
    #method absolute
    [
        #var(type:byte)n := self byte.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ByteNumber new &byte:n.
    ]
    
    #method mod : anOperand
    [
        #var retVal := self byte / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:byteConvertor.
    ]    
                
    #method is &odd
        = self::byteOp mod:2 != 0.
        
    #method is &even
        = self::byteOp mod:2 == 0.
        
    #method is &zero
        = self byte equal &int:0.
        
    #method is &positive
        = self byte > 0.
        
    #method is &negative
        = self byte < 0.
        
    #method is &nonnegative
        = self byte <= 0.
}

// --- intOp ---

#class(extension:short) shortOp
{
    #method absolute
    [
        #var(type:short)n := self short.
        (n < 0)
            ? [ n := n * -1. ].
                
        ^ ShortNumber new &short:n.
    ]
    
    #method mod : anOperand
    [
        #var retVal := self short / anOperand * anOperand.
        
        ^ retVal cast:%convertor &to:shortConvertor.
    ]
                
    #method is &odd
        = self::shortOp mod:2 != 0.
        
    #method is &even
        = self::shortOp mod:2 == 0.
        
    #method is &zero
        = self short equal &int:0.
        
    #method is &positive
        = self short > 0.
        
    #method is &negative
        = self short < 0.
        
    #method is &nonnegative
        = self short <= 0.
    
    #method high
    [
        #var(type:short)aWord := self short.
        aWord := aWord && 0FF00h.
        aWord := aWord shift &index:8.
        
        ^ byteConvertor convert &short:aWord.
    ]
        
    #method low
    [
        #var(type:short)aWord := self short.
        aWord := aWord && 0FFh.
        
        ^ byteConvertor convert &short:aWord.
    ]
}

// --- intOp ---

#class(extension:int) intOp
{
    #method(type:int) absolute
    [
        #var(type:int)n := self int.
        
        (n < 0)
            ? [ n := n * -1. ].
        
        ^ IntNumber new &int:n.
    ]
    
    #method(type:int) mod : anOperand
    [
        #var(type:int)x := self int.
        #var(type:int)y := anOperand int.
        
        #var(type:int)z := x / y.
        z := z * y.
        
        ^ IntNumber new &int:(x - z).
    ]
                
    #method is &odd
        = self::intOp mod:2 != 0.
        
    #method is &even
        = self::intOp mod:2 == 0.
        
    #method is &zero
        = self int equal &int:0.
        
    #method is &positive
        = self int > 0.
        
    #method is &negative
        = self int < 0.
        
    #method is &nonnegative
        = self int <= 0.

    #method high
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FFFF0000h.
        aWord := aWord shift &index:16.
        
        ^ shortConvertor convert &int:aWord.
    ]

    #method low
    [
        #var(type:int)aWord := self int.
        aWord := aWord && 0FFFFh.
        
        ^ shortConvertor convert &int:aWord.
    ]
}

// --- longOp ---

#class(extension:long) longOp
{
    #method absolute
    [
        #var(type:long)n := self long.

        (n < 0l)
            ? [ n := n * -1l. ].
                
        ^ LongNumber new &long:n.
    ]
    
    #method mod : anOperand
    [
        #var(type:long)x := self long.
        #var(type:long)y := anOperand long.
        
        #var(type:long)z := x / y.
        z := z * y.
        
        ^ LongNumber new &long:(x - z).
    ]
                
    #method is &odd
        = self::longOp mod:2l != 0.
        
    #method is &even
        = self::longOp mod:2l == 0.
        
    #method is &zero
        = self long equal &long:0l.
        
    #method is &positive
        = self long > 0l.
        
    #method is &negative
        = self long < 0l.
        
    #method is &nonnegative
        = self long <= 0l.

    #method high
    [
        #var(type:long)aWord := self long.
        aWord := aWord && -4294967296l.
        aWord := aWord shift &index:32.
        
        ^ intConvertor convert &long:aWord.
    ]

    #method low
    [
        #var(type:long)aWord := self long.
        aWord := aWord && 4294967295l.
        
        ^ intConvertor convert &long:aWord.
    ]
}

// --- realOp ---

#symbol realOpController =
{
    exp &real:aReal &vreal:aRetVal
        => system'internal'r_exp.
        
    abs &real:aReal &vreal:aRetVal
        => system'internal'r_abs.
        
    ln &real:aReal &vreal:aRetVal
        => system'internal'r_ln.
        
    round &real:aReal &vreal:aRetVal
        => system'internal'r_round.
        
    truncate &real:aReal &vreal:aRetVal
        => system'internal'r_truncate.
}.

#class(extension:real) realOp
{        
    #method(type:real) absolute
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
        
    #method(type:real) exp
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController exp &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(type:real) ln
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController ln &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) rounded
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController round &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) integer
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController truncate &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) frac
    [
        #var(type:real)n := self real.        
        #var(type:vreal)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:(n - aRetVal).
    ]
    
    #method is &zero
        = self == 0.
        
    #method is &positive
        = self > 0.
        
    #method is &negative
        = self < 0.
        
    #method is &nonnegative
        = self <= 0.
}
