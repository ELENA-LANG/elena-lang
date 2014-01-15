#define system.

// --- EnumerableIndexer ---

#class EnumerableIndexer
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

    #method goTo &index:anIndex
    [
        theIndex > anIndex ?
        [
            theIndex << -1.
            theEnumerator := theList enumerator.
        ].
        
        control while:[ theIndex < anIndex ] &do:
        [
            theEnumerator next
                ! [ #throw OutOfRangeException new:"Index out of the range". ].
            
            theIndex += 1.
        ].        
    ]

    #method index = theIndex number.
    
    #method set &index:anIndex
    [
        self goTo &index:anIndex.        
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