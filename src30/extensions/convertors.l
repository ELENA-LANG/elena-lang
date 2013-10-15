#define system.

// --- convertor helpers ---

#symbol intConvertor =
{
    eval &type'widestr:aLiteral
        = aLiteral~ELiteralConvertor read &type'int.
}.

#symbol longConvertor =
{
    eval &type'widestr:aLiteral
        = aLiteral~ELiteralConvertor read &type'long.
}.

#symbol realConvertor =
{
    eval &type'widestr:aLiteral
        = aLiteral~ELiteralConvertor read &type'real.
}.

// --- convertControl ---

#symbol convertControl =
{
    toInt : anObject
        = intConvertor eval::anObject.
    
    toLong : anObject
        = longConvertor eval::anObject. 
    
    toReal : anObject
        = realConvertor eval::anObject.
        
    toLiteral : anObject &radix:anOrder
        = extensions'text'presenterFactory new:anObject &radix:anOrder Literal.
        
    save : anObject &to:aTarget
        = aTarget write:(Variant new:anObject).
}.