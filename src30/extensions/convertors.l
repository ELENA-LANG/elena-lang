#define system.
#define system'routines.
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
        #var anOutput := strOp convert &type'int:anInteger &type'index:aRadix.
        
        ^ anOutput.
    ]
    
    convert &type'long:aLong
        = aLong Literal.
    
    convert &type'long:aLong &type'base:aRadix
    [
        #var anOutput := strOp convert &type'long:aLong &type'index:aRadix.
        
        ^ anOutput.
    ]
    
    convert &type'real:aReal
        = aReal Literal.
    
    convert &type'real:aReal &type'length:aLength
    [
        #var anOutput := strOp convert &type'real:aReal &type'long:aLength.
        
        ^ anOutput.
    ]
    
    convert &type'real:aReal &type'length:aLength &type'length:aDecimalLength
    [
        #var aPrecision := extensions'math'power:10.0r:aDecimalLength.
        #var aRounded := (extensions'math'round:(aReal * aPrecision)) / aPrecision.
        
        ^ literalConvertor convert &type'real:aRounded &type'length:aLength.
    ]
    
    convert &type'widestr:aLiteral
        = aLiteral.

    convert &widechar:aChar
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

    convert &type'array:anArray &widechar:aSeparator
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
        = aTarget write:((Variant new:anObject)~(typeControl eval::aTarget) get).
}.

// --- value helpers ---

#symbol realValue = &&:x
[
    realConvertor eval::x
].
