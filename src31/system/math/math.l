// --- mathOp ---

class mathOpDispatcher =
{
    eval byte:aValue = byteOp.
    
    eval short:aValue = shortOp.
    
    eval int:aValue = intOp.
    
    eval long:aValue = longOp.
    
    eval real:aValue = realOp.
}.

extension mathOp
{
    absolute
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher absolute.
    ]
    
    mod : anOperand
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher mod:anOperand.
    ]
    
    is odd
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher is odd.
    ]
        
    is even
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher is even.
    ]
        
    is zero
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher is zero.
    ]
        
    is positive
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher is positive.
    ]
        
    is negative
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher is negative.
    ]
        
    is nonnegative
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher is nonnegative.
    ]
    
    exp
        = (self real)~realOp exp.
    
    real ln
        = self real; ln.
    
    sqrt
        = self real; sqrt.

    power int:aPower
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher power int:aPower.
    ]

    power : aPower
        = aPower cast:%eval to:
        {
            eval:target int:n
            [
                var dispatcher := self cast:%eval to:mathOpDispatcher.
                
                ^ self~dispatcher power : aPower.                
            ]
                
            eval:target real:n
                = target real; power real:n.
        }
        with:self.

    sqr
    [
        var dispatcher := self cast:%eval to:mathOpDispatcher.
        
        ^ self~dispatcher sqr.
    ]

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