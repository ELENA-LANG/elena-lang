#define system.
#define system'routines.
#define system'text.

// --- literalConvertorEx ---

#static literalConvertorEx
    = GroupExtension(extensions'literalConvertorExtension, literalConvertor).

// --- literalConvertorExtension ---

#class(role)literalConvertorExtension
{
//    convert &type'real:aReal &type'length:aLength
//    [
//        #var anOutput := strOp convert &type'real:aReal &type'long:aLength.
//        
//        ^ anOutput.
//    ]
//    
//    convert &type'real:aReal &type'length:aLength &type'length:aDecimalLength
//    [
//        #var aPrecision := extensions'math'power:10.0r:aDecimalLength.
//        #var aRounded := (extensions'math'round:(aReal * aPrecision)) / aPrecision.
//        
//        ^ literalConvertor convert &type'real:aRounded &type'length:aLength.
//    ]

    #method convert &enumerable:aList &char:aSeparator
    [
        #var theOutput := TextBuffer new.
        
        aList~enumerable run: anItem [ theOutput write:(literalConvertorEx convert::anItem) write:aSeparator ].
        
        #var aLength := theOutput length.
        (0 < aLength)
            ? [ theOutput set &length:(-1 + aLength). ].
        
        ^ theOutput literal.
    ]

    #method convert &enumerable:aList
        = $self convert &enumerable:aList &char:",".

    #method convert &array : anArray
        = $self convert &enumerable:anArray &char:",".

    #method convert &array:anArray &char:aSeparator
        = $self convert &enumerable:anArray &char:aSeparator.

    #method convert &object : anObject
        = anObject literal.
}

// --- convertControl ---

#symbol radixDispatcher = 
{
    eval &int:anInteger
        = %(int&base).
        
    eval &type'long:aLong
        = %(long&base).
}.

#symbol typecaster =
{
    int:n = self int.
    
    long:n = self long.
    
    real:n = self real.
    
    literal:n = self literal.
}.

#symbol convertor =
{
    toInt : anObject
        = intConvertor convert::anObject.

    toLong : anObject
        = longConvertor convert::anObject. 
    
    toReal : anObject
        = realConvertor convert::anObject.
                
    toLiteral:anObject
        = anObject literal | ifFailed:[ literalConvertorEx convert::anObject ].
//        = anObject literal || ~misc back:(literalConvertorEx convert::anObject).
        
    toLiteral : anObject &base:anOrder
        = literalConvertor~(radixDispatcher eval::anObject) convert:anObject:(anOrder int).

//    toLiteral:anObject &length:aLength &decimals:aDecimalLength
//        = literalConvertor convert &type'real::anObject &type'length::aLength &type'length::aDecimalLength.

    save : anObject &to:aTarget
        = aTarget write:(anObject~typecaster eval::aTarget).
        
    saveVariant : anObject &to:aTarget
        => &save:(Variant new:anObject) &to:aTarget.
}.



// --- value helpers ---

#symbol realValue = (:x)
[
    realConvertor convert::x
].

#symbol intValue = (:x)
[
    intConvertor convert::x
].
