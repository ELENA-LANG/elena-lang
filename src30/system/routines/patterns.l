#import system.
#import system'collections.

#symbol idleFunc1 = x [].

// === pattern enumerators ==

// --- Toping ---

#class TopFilter :: Enumerator
{
    #field(vint)theOffset.    
    #field(vint)theCounter.    
    #field(enumerator)theEnumerator.
    
    #constructor(suppress:w3) new : aCounter &enumerator:anEnumerator
    [
        theCounter := aCounter int.
        theOffset := 0.
        theEnumerator := anEnumerator.
    ]
    
    #constructor(suppress:w3) new : counter &skipping:offset &enumerator:enumerator
    [
        theCounter := counter int.
        theOffset := offset int.
        
        theEnumerator := enumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method next
    [
        (theCounter != 0)?
        [
            #loop (theEnumerator next)?
            [
                (theOffset == 0)
                    ? [  
                        theCounter -= 1. ^ true.
                    ]
                    ! [ theOffset -= 1. ]
            ].
        ].
        
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
    #field(func1)theFilter.
    #field(enumerator)theEnumerator.
    
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

// --- PatternFilter2 ---

#class PatternFilter2 :: Enumerator
{
    #field(func2)theFilter.
    #field(enumerator)theEnumerator.
    #field thePrevious.
    
    #constructor new &func2:aFilter &enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method(suppress:w2) next
    [
        ($nil == thePrevious) ?
        [
            (theEnumerator next)
                ! [ ^ false. ].
            
            thePrevious := theEnumerator get.
        ].
        
        #loop (theEnumerator next)?
        [
            (theFilter eval:thePrevious:(theEnumerator get))
                ? [ ^ true. ].
            
            thePrevious := theEnumerator get.
        ].
        
        ^ false.
    ]
}

// --- PatternFilter3 ---

#class PatternFilter3 :: Enumerator
{
    #field(func3)theFilter.
    #field(enumerator)theEnumerator.
    #field thePrevious.
    #field theBeforePrev.
    
    #constructor new &func3:aFilter &enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method(suppress:w2) next
    [
        ($nil == thePrevious) ?
        [
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBeforePrev := theEnumerator get.
    
            (theEnumerator next)
                ! [ ^ false. ].
            
            thePrevious := theEnumerator get.
        ].
                
        #loop (theEnumerator next)?
        [
            (theFilter eval:theBeforePrev:thePrevious:(theEnumerator get))
                ? [ ^ true. ].
            
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get.
        ].
        
        ^ false.
    ]
}

// --- PatternFilter4 ---

#class PatternFilter4 :: Enumerator
{
    #field(func4)theFilter.
    #field(enumerator)theEnumerator.
    #field thePrevious.
    #field theBeforePrev.
    #field theBefore2Prev.
    
    #constructor new &func4:aFilter &enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method(suppress:w2) next
    [
        ($nil == thePrevious) ?
        [
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBefore2Prev := theEnumerator get.
            
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBeforePrev := theEnumerator get.
    
            (theEnumerator next)
                ! [ ^ false. ].
            
            thePrevious := theEnumerator get.
        ].
                
        #loop (theEnumerator next)?
        [
            (theFilter eval:theBefore2Prev:theBeforePrev:thePrevious:(theEnumerator get))
                ? [ ^ true. ].
            
            theBefore2Prev := theBeforePrev.
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get.
        ].
        
        ^ false.
    ]
}

// --- PatternFilter5 ---

#class PatternFilter5 :: Enumerator
{
    #field(func5)theFilter.
    #field(enumerator)theEnumerator.
    #field thePrevious.
    #field theBeforePrev.
    #field theBefore2Prev.
    #field theBefore3Prev.
    
    #constructor new &func5:aFilter &enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method(suppress:w2) next
    [
        ($nil == thePrevious) ?
        [
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBefore3Prev := theEnumerator get.
            
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBefore2Prev := theEnumerator get.
            
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBeforePrev := theEnumerator get.
    
            (theEnumerator next)
                ! [ ^ false. ].
            
            thePrevious := theEnumerator get.
        ].
                
        #loop (theEnumerator next)?
        [
            (theFilter eval:theBefore3Prev:theBefore2Prev:theBeforePrev:thePrevious:(theEnumerator get))
                ? [ ^ true. ].
            
            theBefore3Prev := theBefore2Prev.
            theBefore2Prev := theBeforePrev.
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get.
        ].
        
        ^ false.
    ]
}

// --- PatternFilter6 ---

#class PatternFilter6 :: Enumerator
{
    #field(func6)theFilter.
    #field(enumerator)theEnumerator.
    #field thePrevious.
    #field theBeforePrev.
    #field theBefore2Prev.
    #field theBefore3Prev.
    #field theBefore4Prev.
    
    #constructor new &func6:aFilter &enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    #method get = theEnumerator get.
    
    #method(suppress:w2) next
    [
        ($nil == thePrevious) ?
        [
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBefore4Prev := theEnumerator get.
            
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBefore3Prev := theEnumerator get.
            
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBefore2Prev := theEnumerator get.
            
            (theEnumerator next)
                ! [ ^ false. ].
            
            theBeforePrev := theEnumerator get.
    
            (theEnumerator next)
                ! [ ^ false. ].
            
            thePrevious := theEnumerator get.
        ].
                
                
        #loop (theEnumerator next)?
        [
            (theFilter eval:theBefore4Prev:theBefore3Prev:theBefore2Prev:theBeforePrev:thePrevious:(theEnumerator get))
                ? [ ^ true. ].
            
            theBefore4Prev := theBefore3Prev.
            theBefore3Prev := theBefore2Prev.
            theBefore2Prev := theBeforePrev.
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get.
        ].
        
        ^ false.
    ]
}

// --- Selecting ---

#class Selector :: Enumerator
{
    #field(func1)theSelector.
    #field(enumerator)theEnumerator.
    
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
    #field(int)   theMin.
    #field(int)   theMax.
    #field(int)   theCurrent.
    #field(func1) theFunc.
    
    #constructor new &till:maxValue &each:func
    [
        theMax := maxValue int.
        theMin := 0.
        theCurrent := -1.
        theFunc := func func1.
    ]
    
    #constructor(suppress:w3) new &from:minValue &till:maxValue &each:func
    [
        theMin := minValue int.
        theMax := maxValue int.
        theCurrent := theMin - 1.
        theFunc := func func1.
    ]
    
    #method get = theFunc eval:theCurrent.
    
    #method(suppress:w3) reset
    [
        theCurrent := theMin - 1.
    ]
    
    #method(suppress:w3) next
    [
        theCurrent := theCurrent + 1.
        
        ^ (theCurrent < theMax).
    ]
}

// --- ZipEnumerator ---

#class ZipEnumerator :: Enumerator
{
    #field(enumerator) theEnumerator1.
    #field(enumerator) theEnumerator2.
    
    #field(func2) theZipper.
    
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
    
    #field(bool)theForward.
    
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
        theVariable set:theStart.

        theForward 
            ? [ theVariable -= theStep. ]
            ! [ theVariable += theStep. ].
    ]    
    
    #method get = theVariable get.
    
    #method next
    [
        theForward 
            ? [ theVariable += theStep. ^ (theVariable <= theEnd) bool. ].
        
        theVariable -= theStep. 
        
        ^ (theVariable >= theEnd) bool.
    ]
}

// --- IntervalEnumerator ---

#class IntervalEnumerator :: Enumerator
{
    #field theVariable.
    #field theStart.
    #field theEnd.
    #field theStep.
    
    #field(bool)theForward.
    
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
        theVariable set:theStart.

        theForward 
            ? [ theVariable -= theStep. ]
            ! [ theVariable += theStep. ].
    ]    
    
    #method get = theVariable get.
    
    #method next
    [
        theForward 
            ? [ theVariable += theStep. ^ (theVariable < theEnd) bool. ].
        
        theVariable -= theStep. 
        
        ^ (theVariable > theEnd) bool.
    ]
}

// --- GroupingEnumerator ---

#class GroupingEnumerator :: Enumerator
{
    #field(dictionary) theDictionary.
    #field(enumerator) theEnumerator.
    #field(func1)      theGroupFunc.    
    #field(enumerator) theGroupEnumerator.
    
    #constructor new &func1:grouper &enumerator:enumerator
    [
        theGroupFunc := grouper.
        theDictionary := Dictionary new.
        theEnumerator := enumerator.
        
        $self reset.
    ]
    
    #method reset
    [
        theEnumerator reset.
        
        #loop (theEnumerator next)?
        [
            #var item := theEnumerator get.
            #var key := theGroupFunc eval:item.
            
            #var groupItem := theDictionary@key.
            ($nil == groupItem)?
            [
                groupItem := ArrayList new.
                
                theDictionary@key := groupItem.
            ].
            
            groupItem += item.
        ].
        
        theGroupEnumerator := theDictionary enumerator.
    ]
    
    #method get = theGroupEnumerator get.
    
    #method next
        = theGroupEnumerator next.
}

// --- OrderingEnumerator ---

#class OrderingEnumerator :: Enumerator
{
    #field(enumerator) theEnumerator.
    #field(func2)      theOrderFunc.    
    #field(enumerator) theListEnumerator.
    
    #constructor new &func2:orderFunc &enumerator:enumerator
    [
        theOrderFunc := orderFunc.
        theEnumerator := enumerator.
        
        $self reset.
    ]
    
    #method reset
    [
        theEnumerator reset.
        
        #var(array) list := Summing new:(ArrayList new) run &enumerator:theEnumerator array.
        list sort &func2:theOrderFunc.
        
        theListEnumerator := list enumerator.
    ]
    
    #method get = theListEnumerator get.
    
    #method next
        = theListEnumerator next.
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
    #field(func2) theOperation.
    
    #constructor new : aVariable &with:operation
    [
        theResult := aVariable.
        theOperation := operation func2.
    ]

    #method eval : aParam [ theResult set:(theOperation eval:theResult:aParam). ]
}

// --- patternOp ---

#class(extension)patternOp
{
    #method summarize : aVariable
    [
        Summing new:aVariable run &enumerator:(self enumerator).
        
        ^ aVariable get.
    ]
    
    #method summarize
        = self::patternOp summarize:(Variable new).
        
    #method accumulate : aVariable &with:operation
    [
        Accumulating new:aVariable &with:operation run &enumerator:(self enumerator).
        
        ^ aVariable get.
    ]
    
    #method accumulate &with:operation 
        = self::patternOp accumulate:(Variable new) &with:operation.

    #method top:aCounter
        = TopFilter new:aCounter &enumerator:(self enumerator).

    #method last:counter
        = TopFilter new:counter &skipping:(self length - counter) &enumerator:(self enumerator).

    #method filter &each:aFilter
        = Filter new &each:aFilter &enumerator:(self enumerator).

    #method select &each:aSelector
        = Selector new &each:aSelector &enumerator:(self enumerator).

    #method zip:anEnumerable &into:aZipper
        = ZipEnumerator new:aZipper &enumerator:(self enumerator) &enumerator:(anEnumerable enumerator).

    #method zip:anEnumerable &eachPair:func2
        = ZipEnumerator new:(:x:y)[ func2 eval:x:y ] &enumerator:(self enumerator) &enumerator:(anEnumerable enumerator) run &each:idleFunc1.

    #method group &by:groupFunc
        = GroupingEnumerator new &func1:(groupFunc func1) &enumerator:(self enumerator).                                       

    #method order &by:orderFunc
        = OrderingEnumerator new &func2:(orderFunc func2) &enumerator:(self enumerator).                                       

    #method filter &pattern:patternFun
        = patternFun cast:%eval &to:
        {
            eval:enumerable &func1:func1
                = Filter new &each:func1 &enumerator:(enumerable enumerator).
            
            eval:enumerable &func2:func2
                = PatternFilter2 new &func2:func2 &enumerator:(enumerable enumerator).
                
            eval:enumerable &func3:func3
                = PatternFilter3 new &func3:func3 &enumerator:(enumerable enumerator).

            eval:enumerable &func4:func4
                = PatternFilter4 new &func4:func4 &enumerator:(enumerable enumerator).

            eval:enumerable &func5:func5
                = PatternFilter5 new &func5:func5 &enumerator:(enumerable enumerator).

            eval:enumerable &func6:func6
                = PatternFilter6 new &func6:func6 &enumerator:(enumerable enumerator).

        } &with:self.
            
    /// obosolete
    #method repeat &each:aRepeater    
        = Repeater new &till:self &each:aRepeater.
        
    #method repeat &till:max &each:aRepeater
        = Repeater new &from:self &till:max &each:aRepeater.
        
    #method counting
        = RangeEnumerator new &from:1 &to:(self int) &by:1.
}
