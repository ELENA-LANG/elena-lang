#define system.

// === pattern enumerators ==

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
            theFilter eval:(theEnumerator get) 
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
