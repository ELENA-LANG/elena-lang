#define system.

// === pattern enumerators ==

// --- Toping ---

#class TopFilter :: Enumerator
{
    #field(type:intvar)theCounter.
    #field(type:enumerator)theEnumerator.
    
    #constructor new &length:aCounter &enumerator:anEnumerator
    [
        theCounter := Integer new &int:aCounter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method next
    [
        ((theCounter > 0)and:[ theEnumerator next ])
            ? [ theCounter reduce &int:1. ^ true. ].
        
        ^ false.
    ]
    
    #method reset 
    [
        theEnumerator reset.
    ]    
}

// --- Filtering ---

#class Filter :: Enumerator
{
    #field(type:func1)theFilter.
    #field(type:enumerator)theEnumerator.
    
    #constructor new &each:aFilter &enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method next
    [
        #loop (theEnumerator next)?
        [
            (theFilter eval:(theEnumerator get))
                ? [ ^ true. ].
        ].
        
        ^ false.
    ]
    
    #method reset 
    [
        theEnumerator reset.
    ]    
}

// --- Selecting ---

#class Selector :: Enumerator
{
    #field(type:func1)theSelector.
    #field(type:enumerator)theEnumerator.
    
    #constructor new &each:aSelector &enumerator:anEnumerator
    [
        theSelector := aSelector.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theSelector eval:(theEnumerator get).
    
    #method next = theEnumerator next.
    
    #method reset 
    [
        theEnumerator reset.
    ]    
}

// === patterns ===

// --- BasePattern ---

#class BasePattern :: BaseFunction1
{
    #field theResult.
    
    #method run &enumerator:anEnumerator
    [
        $super run &enumerator:anEnumerator.
        
        ^ theResult.
    ]
}

// --- Summing ---

#class Summing :: BasePattern
{
    #constructor new : aVariable
    [
        theResult := aVariable.
    ]

    #method eval : aParam [ theResult append:aParam. ]
}
