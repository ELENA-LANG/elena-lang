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
    toInt : anObject
        = anObject cast:%convert &to:intConvertor.

    toLong : anObject
        = anObject cast:%convert &to:longConvertor.
    
    toReal : anObject
        = anObject cast:%convert &to:realConvertor.

    toLiteral:anObject
        = anObject cast:%eval &to:
                   { 
                      eval &enumerable : aList = aList::enumerableEx literal. 
                      
                      eval &array : anArray = anArray::enumerableEx literal.
                      
                      ! : anObject = anObject literal.
                   }.

    toLiteral : anObject &base:aBase
        = anObject cast:%eval &to:
                   { 
                      eval &int : anInt = literalConvertor convert &int:anInt &index:(aBase int).
                   }.
                   
    toArray : anEnumerable
        = anEnumerable::enumerableOp array.

    save : anObject &to:aTarget
        = aTarget write:(aTarget cast:%eval &to:{ ! : x = (Variant new:anObject)::$subject get. }).
}.
