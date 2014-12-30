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

#symbol convertor =
{
    #method(type:int) toInt : anObject
        = anObject cast:%convert &to:intConvertor int.

    #method(type:long) toLong : anObject
        = anObject cast:%convert &to:longConvertor long.
    
    #method(type:real) toReal : anObject
        = anObject cast:%convert &to:realConvertor real.

    #method(type:literal) toLiteral:anObject
        = anObject cast:%eval &to:
                   { 
                      eval &enumerable : aList = aList::enumerableEx literal. 
                      
                      eval &array : anArray = anArray::enumerableEx literal.
                      
                      ! : anObject = anObject.
                   } literal.

     #method(type:literal) toLiteral : anObject &base:aBase
        = anObject cast:%eval &to:
                   { 
                      eval &int : anInt = literalConvertor convert &int:anInt &index:(aBase int).
                   } literal.
                   
    #method(type:array) toArray : anEnumerable
        = anEnumerable::enumerableOp array.

    #method save : anObject &to:aTarget
        = aTarget write:(aTarget cast:%eval &to:{ ! : x = (Variant new:anObject)::$subject get. }).
}.
