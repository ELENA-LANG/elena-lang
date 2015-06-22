#define system.

#subject degree.
#subject radian.
//#subject reciprocal,ln,exp,sin,cos

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
    
    #method(type:byte)power &byte:y
    [
        #var(type:byte)x := self byte.
        
        #var(type:byte)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ByteNumber new &byte:x.
    ]
    
    #method(type:byte) sqr
    [
        #var(type:byte)n := self byte.        
        #var(type:byte)aRetVal.
        
        aRetVal := n * n.
        
        ^ ByteNumber new &byte:aRetVal.
    ]

    #method power : aPower
        = self::byteOp power &byte:(aPower byte).
                
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
    
    #method(type:short)power &short:y
    [
        #var(type:short)x := self short.
        
        #var(type:short)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ ShortNumber new &short:x.
    ]
    
    #method(type:short) sqr
    [
        #var(type:short)n := self short.        
        #var(type:short)aRetVal.
        
        aRetVal := n * n.
        
        ^ ShortNumber new &short:aRetVal.
    ]

    #method power : aPower
        = self::shortOp power &short:(aPower short).
                
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
    
    #method(type:int)power &int:y
    [
        #var(type:int)x := self int.
        
        #var(type:int)i := 0.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ IntNumber new &int:x.
    ]
    
    #method(type:int) sqr
    [
        #var(type:int)n := self int.        
        #var(type:int)aRetVal.
        
        aRetVal := n * n.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method power : aPower
        = self::intOp power &int:(aPower int).
                
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
    
    #method(type:long)power &long:y
    [
        #var(type:long)x := self long.
        
        #var(type:long)i := 0l.
        #loop (i < y)?
        [
            x := x * x.
        ].
        
        ^ LongNumber new &long:x.
    ]
    
    #method(type:long) sqr
    [
        #var(type:long)n := self long.        
        #var(type:long)aRetVal.
        
        aRetVal := n * n.
        
        ^ LongNumber new &long:aRetVal.
    ]

    #method power : aPower
        = self::longOp power &long:(aPower long).
                
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
        
    sin &real:aReal &vreal:aRetVal
        => system'internal'r_sin.
        
    cos &real:aReal &vreal:aRetVal
        => system'internal'r_cos.
        
    arctan &real:aReal &vreal:aRetVal
        => system'internal'r_arctan.
                
    sqrt &real:aReal &vreal:aRetVal
        => system'internal'r_sqrt.
        
    pi &vreal:aRetVal
        => system'internal'r_pi.
        
    #method(type:real) pi
    [
        #var(type:real)aRetVal.
        $self pi &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]        
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
        #var(type:real)aRetVal.
        
        realOpController exp &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(type:real) ln
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController ln &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method(type:real) sqrt
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController sqrt &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real)power &real:y
    [
        #var(type:real)x := self real.
        
        ^ (x ln * y) exp.
    ]

    #method power : aPower
        = self::realOp power &real:(aPower real).

    #method(type:real) sqr
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        aRetVal := n * n.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) rounded
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController round &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) integer
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController truncate &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real) frac
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController abs &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:(n - aRetVal).
    ]

    #method(type:real)sin
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController sin &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real)cos
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController cos &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method(type:real)tan
    [
        #var(type:real)n := self real.        
        #var(type:real)aSinVal.
        #var(type:real)aCosVal.
        
        realOpController sin &real:n &vreal:aSinVal.
        realOpController cos &real:n &vreal:aCosVal.
        
        ^ aSinVal / aCosVal.
    ]

    #method(type:real)arctan
    [
        #var(type:real)n := self real.        
        #var(type:real)aRetVal.
        
        realOpController arctan &real:n &vreal:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
            
    #method(type:real)arcsin
    [
        #var(type:real)n := self real.        
        #var(type:real)arg.
        
        n := 1.0r - n*n.
        
        realOpController sqrt &real:n &vreal:arg.
        
        ^ arg arctan.
    ]
            
    #method(type:real)arccos
    [
        #var(type:real)n := self real.        
        #var(type:real)arg.
        
        n := 1.0r - n*n.
        
        realOpController sqrt &real:n &vreal:arg.
        
        arg := arg / n.
        
        ^ arg arctan.
    ]
            
    #method(type:real) radian
    [
        #var(type:real)aValue := self real.
        
        aValue := aValue * pi_value.
        
        ^ RealNumber new &real:(aValue / 180.0r).
    ]
            
    #method(type:real) degree
    [
        #var(type:real)aValue := self real.
        
        aValue := aValue * 180.0r.
        
        ^ RealNumber new &real:(aValue / pi_value).
    ]
            
    #method(type:real) reciprocal
    [
        #var(type:real)aValue := self real.
        
        ^ RealNumber new &real:(1.0r / aValue).
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

#symbol(type:real)pi_value = realOpController pi.

// --- mathOp ---

#symbol mathOpDispatcher =
{
    #method eval &byte:aValue = byteOp.
    
    #method eval &short:aValue = shortOp.
    
    #method eval &int:aValue = intOp.
    
    #method eval &long:aValue = longOp.
    
    #method eval &real:aValue = realOp.
}.

#class(extension)mathOp
{
    #method absolute
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher absolute.
    ]
    
    #method mod : anOperand
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher mod:anOperand.
    ]
    
    #method is &odd
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher is &odd.
    ]
        
    #method is &even
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher is &even.
    ]
        
    #method is &zero
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher is &zero.
    ]
        
    #method is &positive
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher is &positive.
    ]
        
    #method is &negative
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher is &negative.
    ]
        
    #method is &nonnegative
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher is &nonnegative.
    ]
    
    #method exp
        = (self real)::realOp exp.
    
    #method(type:real) ln
        = (self real)::realOp ln.
    
    #method sqrt
        = (self real)::realOp sqrt.

    #method power &real:aPower
        = (self real)::realOp power &real:aPower.

    #method power : aPower
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher power : aPower.
    ]

    #method sqr
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher sqr.
    ]

    #method rounded
        = (self real)::realOp rounded.

    #method integer
        = (self real)::realOp integer.

    #method frac
        = (self real)::realOp frac.

    #method sin
        = (self real)::realOp sin.

    #method cos
        = (self real)::realOp cos.

    #method tan
        = (self real)::realOp tan.

    #method arctan
        = (self real)::realOp arctan.
            
    #method arcsin
        = (self real)::realOp arcsin.
            
    #method arccos
        = (self real)::realOp arccos.
            
    #method radian
        = (self real)::realOp radian.
            
    #method degree
        = (self real)::realOp degree.
            
    #method reciprocal
        = (self real)::realOp reciprocal.
}