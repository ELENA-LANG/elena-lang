#define system.
#define system'collections.
#define system'routines.
#define system'calendar.
#define system'text.

// --- convertors ---

#symbol literalExConvertor =
{
    convert &enumerable:aList
    [
        #var(type:textbuffer) theOutput := TextBuffer new.

        aList run:theOutput &eachPair:(:aParam:anItem) [ aParam write:(anItem::convertorOp toLiteral) write:"," ].
        
        #var(type:int)aLength.
        theOutput readLength &vint:aLength.

        (0 < aLength)
            ? [ theOutput setLength &int:(-1 + aLength). ].
        
        ^ theOutput literal.
    ]
}.

#symbol wideExConvertor =
{
    convert &enumerable:aList
    [
        #var(type:textbuffer) theOutput := TextBuffer new.

        aList run:theOutput &eachPair:(:aParam:anItem) [ aParam write:(anItem::convertorOp toWide) write:"," ].
        
        #var(type:int)aLength.
        theOutput readLength &vint:aLength.

        (0 < aLength)
            ? [ theOutput setLength &int:(-1 + aLength). ].
        
        ^ theOutput wide.
    ]
}.

#symbol arrayExConvertor =
{
    convert &enumerable:aList
    [
        #var(type:array_list)aRetVal := ArrayList new.
        
        aList run &each:anItem [ aRetVal append:anItem ].

        ^ aRetVal array.
    ]
}.

// --- convertorOp ---

#class(extension)convertorOp
{
    #method toArray
        = self cast:%eval &to:
            {
                literal : aLiteral = arrayExConvertor convert &enumerable:aLiteral.
                
                wide : aWide = arrayExConvertor convert &enumerable:aWide.
                
                array : anArray
                    = anArray.
                    
                enumerable : aList
                    = arrayExConvertor convert &enumerable:aList.
                    
                enumerator : anEnumerator
                    = arrayExConvertor convert &enumerable:anEnumerator.
            }.
    
    #method toLiteral
        = self cast:%eval &to:
            {
                literal : aLiteral = aLiteral.
                
                wide : aWide = aWide literal.
                
                array : anArray
                    = literalExConvertor convert &enumerable:anArray.
                
                enumerable : aList
                    = literalExConvertor convert &enumerable:aList.
                    
                int : aValue = aValue literal.
                    
                short : aValue = aValue literal.
                    
                byte : aValue = aValue literal.
                    
                long : aValue = aValue literal.
                    
                real : aValue = aValue literal.
                    
                datetime : aValue = aValue literal.
                    
                timestamp : aValue = aValue literal.
                
                ! : anObject = anObject literal.
            }.
    
    #method toWide
        = self cast:%eval &to:
            {
                literal : aLiteral = aLiteral wide.
                
                wide : aWide = aWide.
                
                array : anArray
                    = wideExConvertor convert &enumerable:anArray.
                
                enumerable : aList
                    = wideExConvertor convert &enumerable:aList.
                    
                int : aValue = aValue wide.
                    
                short : aValue = aValue wide.
                    
                byte : aValue = aValue wide.
                    
                long : aValue = aValue wide.
                    
                real : aValue = aValue wide.
                    
                datetime : aValue = aValue wide.
                    
                timestamp : aValue = aValue wide.
                
                ! : anObject = anObject wide.
            }.

     #method toLiteral &base:aBase
        = self cast:%eval &to:
                   { 
                      eval : aBase &byte : anInt = literalConvertor convert &byte:anInt &index:(aBase int).
                      
                      eval : aBase &short : anInt = literalConvertor convert &short:anInt &index:(aBase int).
                      
                      eval : aBase &int : anInt = literalConvertor convert &int:anInt &index:(aBase int).
                      
                      eval : aBase &long : anInt = literalConvertor convert &long:anInt &index:(aBase int).
                      
                   }&with:aBase.

     #method toWide &base:aBase
        = self cast:%eval &to:
                   { 
                      eval : aBase &byte : anInt = wideConvertor convert &byte:anInt &index:(aBase int).
                      
                      eval : aBase &short : anInt = wideConvertor convert &short:anInt &index:(aBase int).
                      
                      eval : aBase &int : anInt = wideConvertor convert &int:anInt &index:(aBase int).
                      
                      eval : aBase &long : anInt = wideConvertor convert &long:anInt &index:(aBase int).
                      
                   } &with:aBase.
            
    #method toByte
        = self cast:%convert &to:byteConvertor.

    #method toShort
        = self cast:%convert &to:shortConvertor.

    #method toInt
        = self cast:%convert &to:intConvertor.

    #method toLong
        = self cast:%convert &to:longConvertor.
    
    #method toReal
        = self cast:%convert &to:realConvertor.

    #method convertTo : aTarget
        = aTarget write:(aTarget cast:%eval &to:{ ! : x : y = (Variant new:x)::$subject get. } &with:self).
}

#class(extension:datetime) dateFormatterOp 
{
    #method toShortTimeLiteral
        = self __time_st toShortTime literal.
}
 