#define system.
#define system'routines.
#define system'text.

////#define(const)convertor = extensions'convertor.

#class(extension) enumerableEx
{
    #method asLiteral
    [
        #var(type:textbuffer) theOutput := TextBuffer new.
        
        control run:self &forEach: anItem [ theOutput write:(convertor toLiteral:anItem) write:"," ].
        
        #var(type:int)aLength.
        theOutput readLength &vint:aLength.

        (0 < aLength)
            ? [ theOutput set &length:(-1 + aLength). ].
        
        ^ theOutput literal.
    ]
    
    #method asWide
    [
        #var(type:textbuffer) theOutput := TextBuffer new.
        
        control run:self &forEach: anItem [ theOutput write:(convertor toWide:anItem) write:"," ].
        
        #var(type:int)aLength.
        theOutput readLength &vint:aLength.

        (0 < aLength)
            ? [ theOutput set &length:(-1 + aLength). ].
        
        ^ theOutput wide.
    ]
}

#symbol convertor =
{
    #method(type:byte) toByte : anObject
        = anObject cast:%convert &to:byteConvertor byte.

    #method(type:short) toShort : anObject
        = anObject cast:%convert &to:shortConvertor short.

    #method(type:int) toInt : anObject
        = anObject cast:%convert &to:intConvertor int.

    #method(type:long) toLong : anObject
        = anObject cast:%convert &to:longConvertor long.
    
    #method(type:real) toReal : anObject
        = anObject cast:%convert &to:realConvertor real.

    #method(type:literal) toLiteral:anObject
        = anObject cast:%eval &to:
                   { 
                      eval &enumerable : aList = aList::enumerableEx asLiteral. 
                      
                      eval &array : anArray = anArray::enumerableEx asLiteral.
                      
                      ! : anObject = anObject.
                   } literal.

     #method(type:literal) toLiteral : anObject &base:aBase
        = anObject cast:%eval &to:
                   { 
                      eval &byte : anInt = literalConvertor convert &byte:anInt &index:(aBase int).
                      
                      eval &short : anInt = literalConvertor convert &short:anInt &index:(aBase int).
                      
                      eval &int : anInt = literalConvertor convert &int:anInt &index:(aBase int).
                      
                      eval &long : anInt = literalConvertor convert &long:anInt &index:(aBase int).
                      
                   } literal.

    #method(type:wide) toWide:anObject
        = anObject cast:%eval &to:
                   { 
                      eval &enumerable : aList = aList::enumerableEx asWide. 
                      
                      eval &array : anArray = anArray::enumerableEx asWide.
                      
                      ! : anObject = anObject.
                   } wide.

     #method(type:wide) toWide : anObject &base:aBase
        = anObject cast:%eval &to:
                   { 
                      eval : aBase &byte : anInt = wideConvertor convert &byte:anInt &index:(aBase int).
                      
                      eval : aBase &short : anInt = wideConvertor convert &short:anInt &index:(aBase int).
                      
                      eval : aBase &int : anInt = wideConvertor convert &int:anInt &index:(aBase int).
                      
                      eval : aBase &long : anInt = wideConvertor convert &long:anInt &index:(aBase int).
                      
                   } &with:aBase wide.
                                      
    #method(type:array) toArray : anEnumerable
        = anEnumerable::enumerableOp asArray.

    #method save : anObject &to:aTarget
        = aTarget write:(aTarget cast:%eval &to:{ ! : x : y = (Variant new:x)::$subject get. } &with:anObject).
}.
