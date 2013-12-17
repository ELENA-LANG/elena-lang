#define system.

// --- IndexedListEnumerator ---

#class IndexedListEnumerator
{
    #field theList.
    #field theEnumerator.
    #field theIndex.
    
    #constructor new : aList
    [
        theEnumerator := aList Enumerator.
        theIndex:= Integer new &type'int:-1.
    ]
    
    #method reset
    [
        theIndex << -1.
        
        theEnumerator reset.
    ]
    
    #method next
    [
        theIndex += 1.
        
        ^ theEnumerator next.
    ] 

    #method get => theEnumerator.
            
    #method set : aValue
    [
        theEnumerator set:aValue.
    ]
            
    #method set &type'index:anIndex
    [
        theIndex > anIndex ?
        [
            $self reset.
        ].
        
        control while:[ theIndex < anIndex ] &do:
        [
            theEnumerator next
                ! [ #throw OutOfRangeException new:"Index out of the range". ].
            
            theIndex += 1.
        ].
    ]
    
    #method type'index
        = theIndex Number.
        
    #method Index => %type'index.
}

// --- TopEnumerator ---

#class TopEnumerator
{
    #field theEnumerator.
    #field theCounter.
    #field theMaxValue.
    
    #constructor new : anEnumerable &max:aMaxValue
    [
        theEnumerator := anEnumerable Enumerator.
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