#define system.
#define system'routines.
#define system'text.

// --- literalConvertorEx ---

#class(role)literalConvertorEx : literalConvertor
{
//    #method convert &type'real:aReal &type'length:aLength
//    [
//        #var anOutput := strOp convert &type'real:aReal &type'long:aLength.
//        
//        ^ anOutput.
//    ]
//    
//    #method convert &type'real:aReal &type'length:aLength &type'length:aDecimalLength
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
        
        #var(type:int)aLength.
        theOutput read &out'length:aLength.
        
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

    #method convert : anObject
        = anObject literal.
}

// --- convertControl ---

#symbol baseDispatcher = 
{
    int : n
        = %(int&base).
        
    long : aLong
        = %(long&base).
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
        = literalConvertorEx convert::anObject.

    toLiteral : anObject &base:aBase
    [
        #var aValue := anObject value.
        
        ^ literalConvertorEx~(baseDispatcher eval::aValue) convert:aValue:aBase.
    ]

    save : anObject &to:aTarget
        = aTarget write:(Variant new:anObject).
}.
