#define system.
#define system'routines.
#define system'text.

#define(const)convertor = extensions'convertor.

#class(extension:enumerable) enumerableEx
{
    #method literal
    [
        #var(type:textbuffer) theOutput := TextBuffer new.
        
        control run:self &forEach: anItem [ theOutput write:(convertor toLiteral:anItem) write:"," ].
        
        #var(type:int)aLength := theOutput length.

        (0 < aLength)
            ? [ theOutput set &length:(-1 + aLength). ].
        
        ^ theOutput literal.
    ]
}

#symbol/*(type:convertor)*/ convertor =
{
    toInt : anObject
        = intConvertor convert::anObject.

    toLong : anObject
        = longConvertor convert::anObject. 
    
    toReal : anObject
        = realConvertor convert::anObject.

    toLiteral:anObject
        = anObject cast:
                   { 
                      is &numeric = false.
                   
                      eval &enumerable : aList = aList~enumerableEx literal. 
                      
                      eval &array : anArray = anArray~enumerableEx literal.
                      
                      ! : anObject = anObject literal.
                   }.

    toLiteral : anObject &base:aBase
        = anObject cast:
                   { 
                      eval &int : anInt = literalConvertor convert &int:anInt &index:(aBase int).
                   }.
                   
    toArray : anEnumerable
        = anEnumerable ~enumerable array.

    save : anObject &to:aTarget
        = aTarget write:(aTarget cast:{ ! : x = Variant new:anObject ~$subject get. }).
}.
