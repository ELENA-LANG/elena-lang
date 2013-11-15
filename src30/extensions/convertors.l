#define system.
#define system'text.

// --- convertor helpers ---

#symbol intConvertor =
{
    convert &type'widestr:aLiteral
        = aLiteral~ELiteralConvertor read &type'int.
}.

#symbol longConvertor =
{
    convert &type'widestr:aLiteral
        = aLiteral~ELiteralConvertor read &type'long.
}.

#symbol realConvertor =
{
    eval &type'int:aNumber
        = RealNumber new &type'int:aNumber.
        
    eval &type'long:aNumber
        = RealNumber new &type'long:aNumber.
        
    eval &type'real : aNumber
        = aNumber.
    
    convert &type'widestr:aLiteral
        = aLiteral~ELiteralConvertor read &type'real.
}.

// --- literalConvertor ---

#symbol literalConvertor =
{
    convert &type'int:anInteger
        = anInteger Literal.
    
    convert &type'int:anInteger &type'base:aRadix
    [
        #var anOutput := LiteralValue new &type'length:20.
        
        system'external'CoreAPI convert &out'type'widestr:anOutput &type'int:anInteger &type'int:aRadix.
        
        ^ anOutput.
    ]
    
    convert &type'long:aLong
        = aLong Literal.
    
    convert &type'long:aLong &type'base:aRadix
    [
        #var anOutput := LiteralValue new &type'length:50.
        
        system'external'CoreAPI convert &out'type'widestr:anOutput &type'long:aLong &type'int:aRadix.
        
        ^ anOutput.
    ]
    
    convert &type'real:aReal
        = aReal Literal.
    
    convert &type'real:aReal &type'length:aLength
        = system'external'CoreAPI convert &out'type'widestr:(system'LiteralValue new &type'length:30) &type'real:aReal &type'int:aLength.
    
    convert &type'real:aReal &type'length:aLength &type'length:aDecimalLength
    [
        #var aPrecision := extensions'math'power:10.0r:aDecimalLength.
        #var aRounded := (extensions'math'round:(aReal * aPrecision)) / aPrecision.
        
        ^ literalConvertor convert &type'real:aRounded &type'length:aLength.
    ]
    
    convert &type'widestr:aLiteral
        = aLiteral.

    convert &type'widechar:aChar
        = aChar Literal.

    convert &type'enumerable:aList &type'widestr:aSeparator
    [
        #var theOutput := TextBuffer new.
        
        aList~EList run: &&:anItem [ theOutput append:(literalConvertor convert::anItem) append:aSeparator ].
        
        #var aLength := theOutput type'length.
        (0 < aLength)
            ? [ theOutput set &type'length:(-1 + aLength). ].
        
        ^ theOutput Literal.
    ]

    convert &type'enumerable:aList
        <= convert &type'enumerable:aList &type'widestr:",".

    convert &type'array : anArray
        <= convert &type'enumerable:anArray &type'widestr:",".

    convert &type'array:anArray &type'widechar:aSeparator
        <= convert &type'enumerable:anArray &type'widestr:aSeparator.

    convert &type'reference : anObject
        = anObject Literal.
}.

// --- convertControl ---

#symbol radixDispatcher = 
{
    eval &type'int:anInteger
        = %(&type'int&type'base).
        
    eval &type'long:aLong
        = %(&type'long&type'base).
}.

#symbol convertControl =
{
    toInt : anObject
        = intConvertor convert::anObject.
    
    toLong : anObject
        = longConvertor convert::anObject. 
    
    toReal : anObject
        = realConvertor convert::anObject.
        
    toLiteral:anObject
        = literalConvertor convert:anObject.
        
    toLiteral : anObject &radix:anOrder
        = literalConvertor~(radixDispatcher eval::anObject) convert:anObject:(anOrder type'int).

    toLiteral:anObject &length:aLength &decimals:aDecimalLength
        = literalConvertor convert &type'real::anObject &type'length::aLength &type'length::aDecimalLength.
        
    save : anObject &to:aTarget
        = aTarget write:(Variant new:anObject).
}.

// --- value helpers ---

#symbol realValue = &&:x
[
    realConvertor eval::x
].
