#define system.

// --- EnumerationIterator ---

#class EnumerationIterator
{
    #field theList.
    #field theEnumerator.
    #field theIndex.

    #constructor new &enumerable:aList
    [
        theList := aList.
        theEnumerator := aList enumerator.
        
        theIndex:= Integer new &int:-1.
    ]

    #method index = theIndex int.
    
    #method set &index:anIndex
    [
        #var(type:int)aCurrent.
        theIndex read &out'int:aCurrent.
        
        anIndex less &int:aCurrent ?
        [
            aCurrent := -1.
            theEnumerator := theList enumerator.
        ].
        
        #loop (aCurrent less &int:anIndex)?
        [
            theEnumerator next
                ! [ #throw OutOfRangeException new:"Index out of the range". ].
            
            aCurrent := aCurrent + 1.
        ].
        
        theIndex write &int:aCurrent.
    ]
            
    #method get
    [
        ^ theEnumerator get.
    ]
    
    #method set : anObject
    [
        theEnumerator set:anObject        
    ]

    #method next
    [
        theIndex += 1.
        
        ^ theEnumerator next.
    ]
}

// --- TopEnumerator ---

#class TopEnumerator
{
    #field theEnumerator.
    #field theCounter.
    #field theMaxValue.
    
    #constructor new : anEnumerable &max:aMaxValue
    [
        theEnumerator := anEnumerable enumerator.
        theCounter := Integer new:aMaxValue.
        theMaxValue := aMaxValue.
    ]
    
    #method reset
    [
        $super reset.
        
        theCounter << theMaxValue.
    ]
    
    #method next
        = (theCounter > 0)
            ? [ theCounter -= 1. ^ theEnumerator next. ]
            ! [ false ].
    
    #method => theEnumerator.
}