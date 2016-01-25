// ==== ver 3.4.2 ===

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
    
    #method(suppress:w2) next
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

// --- Repeater ---

#class Repeater :: Enumerator
{
    #field(type:int)   theMin.
    #field(type:int)   theMax.
    #field(type:int)   theCurrent.
    #field(type:func1) theFunc.
    
    #constructor new &till:maxValue &each:func
    [
        theMax := maxValue int.
        theMin := 0.
        theCurrent := -1.
        theFunc := func func1.
    ]
    
    #constructor new &from:minValue &till:maxValue &each:func
    [
        theMin := minValue int.
        theMax := maxValue int.
        theCurrent := theMin - 1.
        theFunc := func func1.
    ]
    
    #method get = theFunc eval:theCurrent.
    
    #method reset
    [
        theCurrent := theMin - 1.
    ]
    
    #method next
    [
        theCurrent := theCurrent + 1.
        
        ^ (theCurrent < theMax).
    ]
}

// --- PairEnumerator ---

#class ZipEnumerator :: Enumerator
{
    #field(type:enumerator) theEnumerator1.
    #field(type:enumerator) theEnumerator2.
    
    #field(type:func2) theZipper.
    
    #constructor new:combinator &enumerator:enum1 &enumerator:enum2
    [
        theZipper := combinator func2.
        
        theEnumerator1 := enum1.
        theEnumerator2 := enum2.
    ]
    
    #method get = theZipper eval:(theEnumerator1 get):(theEnumerator2 get).
    
    #method next = theEnumerator1 next && theEnumerator2 next.
    
    #method reset
    [
        theEnumerator1 reset.
        theEnumerator2 reset.
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

// --- Accumulating ---

#class Accumulating :: BasePattern
{
    #field(type:func2) theOperation.
    
    #constructor new : aVariable &with:operation
    [
        theResult := aVariable.
        theOperation := operation func2.
    ]

    #method eval : aParam [ theOperation eval:theResult:aParam. ]
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
        
    #method accumulate : aVariable &with:operation
    [
        Accumulating new:aVariable &with:operation run &enumerator:(self enumerator).
        
        ^ aVariable value.
    ]
    
    #method accumulate &with:operation 
        = self::patternOp accumulate:(Variable new) &with:operation.
}
