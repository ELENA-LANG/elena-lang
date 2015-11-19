// ==== ver 3.4.1 ===

#import system.

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

    #method power &int:aPower
    [
        #var dispatcher := self cast:%eval &to:mathOpDispatcher.
        
        ^ self::dispatcher power &int:aPower.
    ]

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

    #method roundedInt
        = intConvertor convert &real:(self::realOp rounded).

    #method integer
        = (self real)::realOp integer.

    #method integerInt
        = intConvertor convert &real:((self real)::realOp integer).

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