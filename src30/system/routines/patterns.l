#import system.

// === pattern enumerators ==

// --- Toping ---

#class TopFilter :: Enumerator
{
    #field(type:vint)theCounter.
    #field(type:enumerator)theEnumerator.
    
    #constructor new : aCounter &enumerator:anEnumerator
    [
        theCounter := Integer new &int:(aCounter int).
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method next
    [
        ((theCounter > 0)and:[ theEnumerator next ])
            ? [ theCounter -= 1. ^ true. ].
        
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
        theFilter := aFilter func1.
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
        theSelector := aSelector func1.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theSelector eval:(theEnumerator get).
    
    #method next = theEnumerator next.
    
    #method reset 
    [
        theEnumerator reset.
    ]    
}

// --- RangeEnumerator ---

#class RangeEnumerator :: Enumerator
{
    #field theVariable.
    #field theStart.
    #field theEnd.
    #field theStep.
    
    #field(type:bool)theForward.
    
    #constructor new &for:aVariable &from:aStart &to:anEnd &by:aStep
    [
        theVariable := aVariable.
        theStart := aStart.
        theEnd := anEnd.
        theStep := aStep.
        
        theForward := (aStart < anEnd) bool.
        
        $self reset.
    ]

    #constructor new &from:aStart &to:anEnd &by:aStep
        <= new &for:(Variable new) &from:aStart &to:anEnd &by:aStep.

    #constructor new &from:aStart &to:anEnd
        <= new &for:(Variable new) &from:aStart &to:anEnd &by:1.
        
    #method reset 
    [
        theVariable << theStart.

        theForward 
            ? [ theVariable -= theStep. ]
            ! [ theVariable += theStep. ].
    ]    
    
    #method get = theVariable value.
    
    #method next
    [
        theForward 
            ? [ theVariable += theStep. ^ (theVariable <= theEnd) bool. ].
        
        theVariable -= theStep. 
        
        ^ (theVariable >= theEnd) bool.
    ]
}

// --- RangeEnumerator ---

#class IntervalEnumerator :: Enumerator
{
    #field theVariable.
    #field theStart.
    #field theEnd.
    #field theStep.
    
    #field(type:bool)theForward.
    
    #constructor new &for:aVariable &from:aStart &till:anEnd &by:aStep
    [
        theVariable := aVariable.
        theStart := aStart.
        theEnd := anEnd.
        theStep := aStep.
        
        theForward := (aStart < anEnd) bool.
        
        $self reset.
    ]

    #constructor new &from:aStart &till:anEnd &by:aStep
        <= new &for:(Variable new) &from:aStart &till:anEnd &by:aStep.

    #constructor new &from:aStart &till:anEnd
        <= new &for:(Variable new) &from:aStart &till:anEnd &by:1.
        
    #method reset 
    [
        theVariable << theStart.

        theForward 
            ? [ theVariable -= theStep. ]
            ! [ theVariable += theStep. ].
    ]    
    
    #method get = theVariable value.
    
    #method next
    [
        theForward 
            ? [ theVariable += theStep. ^ (theVariable < theEnd) bool. ].
        
        theVariable -= theStep. 
        
        ^ (theVariable > theEnd) bool.
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

// --- patternOp ---

#class(extension)patternOp
{
    #method summarize : aVariable
    [
        Summing new:aVariable run &enumerator:(self enumerator).
        
        ^ aVariable value.
    ]
    
    #method summarize
        = self::patternOp summarize:(Variable new).
}
