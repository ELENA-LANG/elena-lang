import system'collections.

symbol idleFunc1 = (:x) [].

// === pattern enumerators ==

// --- Toping ---

class TopFilter :: Enumerator
{
    vint       theOffset.    
    vint       theCounter.    
    enumerator theEnumerator.
    
    constructor new : aCounter enumerator:anEnumerator
    [
        theCounter := aCounter int.
        theOffset := 0.
        theEnumerator := anEnumerator.
    ]
    
    constructor new : counter skipping:offset enumerator:enumerator
    [
        theCounter := counter int.
        theOffset := offset int.
        
        theEnumerator := enumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        if (theCounter != 0)
        [
            while (theEnumerator next)
            [
                if (theOffset == 0)
                    [  
                        theCounter reduce int:1. ^ true
                    ];
                    [ theOffset reduce int:1 ]
            ].
        ].
        
        ^ false
    ]
    
    reset 
    [
        theEnumerator reset.
    ]    
}

// --- Filtering ---

class Filter :: Enumerator
{
    func1      theFilter.
    enumerator theEnumerator.
    
    constructor new each:aFilter enumerator:anEnumerator
    [
        theFilter := aFilter func1.
        theEnumerator := anEnumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        while (theEnumerator next)
        [
            if (theFilter eval:(theEnumerator get))
                [ ^ true ]
        ].
        
        ^ false
    ]
    
    reset 
    [
        theEnumerator reset.
    ]    
}

// --- PatternFilter2 ---

class PatternFilter2 :: Enumerator
{
    func2      theFilter.
    enumerator theEnumerator.
    object     thePrevious.
    
    constructor new func2:aFilter enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        if ($nil == thePrevious)
        [
            ifnot (theEnumerator next)
                [ ^ false ].
            
            thePrevious := theEnumerator get.
        ].
        
        while (theEnumerator next)
        [
            if (theFilter eval:thePrevious:(theEnumerator get))
                [ ^ true ].
            
            thePrevious := theEnumerator get
        ].
        
        ^ false
    ]
}

// --- PatternFilter3 ---

class PatternFilter3 :: Enumerator
{
    func3      theFilter.
    enumerator theEnumerator.
    object     thePrevious.
    object     theBeforePrev.
    
    constructor new func3:aFilter enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        if ($nil == thePrevious)
        [
            ifnot (theEnumerator next)
                [ ^ false ].
            
            theBeforePrev := theEnumerator get.
    
            ifnot (theEnumerator next)
                [ ^ false ].
            
            thePrevious := theEnumerator get
        ].
                
        while (theEnumerator next)
        [
            if(theFilter eval:theBeforePrev:thePrevious:(theEnumerator get))
                [ ^ true ].
            
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get
        ].
        
        ^ false
    ]
}

// --- PatternFilter4 ---

class PatternFilter4 :: Enumerator
{
    func4      theFilter.
    enumerator theEnumerator.
    object     thePrevious.
    object     theBeforePrev.
    object     theBefore2Prev.
    
    constructor new func4:aFilter enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        if($nil == thePrevious)
        [
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBefore2Prev := theEnumerator get.
            
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBeforePrev := theEnumerator get.
    
            ifnot(theEnumerator next)
                [ ^ false ].
            
            thePrevious := theEnumerator get.
        ].
                
        while (theEnumerator next)
        [
            if (theFilter eval:theBefore2Prev:theBeforePrev:thePrevious:(theEnumerator get))
                [ ^ true ].
            
            theBefore2Prev := theBeforePrev.
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get
        ].
        
        ^ false.
    ]
}

// --- PatternFilter5 ---

class PatternFilter5 :: Enumerator
{
    func5      theFilter.
    enumerator theEnumerator.
    object     thePrevious.
    object     theBeforePrev.
    object     theBefore2Prev.
    object     theBefore3Prev.
    
    constructor new func5:aFilter enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        if ($nil == thePrevious)
        [
            ifnot (theEnumerator next)
                [ ^ false ].
            
            theBefore3Prev := theEnumerator get.
            
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBefore2Prev := theEnumerator get.
            
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBeforePrev := theEnumerator get.
    
            ifnot (theEnumerator next)
                [ ^ false ].
            
            thePrevious := theEnumerator get.
        ].
                
        while (theEnumerator next)
        [
            if (theFilter eval:theBefore3Prev:theBefore2Prev:theBeforePrev:thePrevious:(theEnumerator get))
                [ ^ true ].
            
            theBefore3Prev := theBefore2Prev.
            theBefore2Prev := theBeforePrev.
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get
        ].
        
        ^ false
    ]
}

// --- PatternFilter6 ---

class PatternFilter6 :: Enumerator
{
    func6      theFilter.
    enumerator theEnumerator.
    object     thePrevious.
    object     theBeforePrev.
    object     theBefore2Prev.
    object     theBefore3Prev.
    object     theBefore4Prev.
    
    constructor new func6:aFilter enumerator:anEnumerator
    [
        theFilter := aFilter.
        theEnumerator := anEnumerator.
    ]
    
    get = theEnumerator get.
    
    next
    [
        if ($nil == thePrevious)
        [
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBefore4Prev := theEnumerator get.
            
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBefore3Prev := theEnumerator get.
            
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBefore2Prev := theEnumerator get.
            
            ifnot(theEnumerator next)
                [ ^ false ].
            
            theBeforePrev := theEnumerator get.
    
            ifnot(theEnumerator next)
                [ ^ false ].
            
            thePrevious := theEnumerator get
        ].
                                
        while (theEnumerator next)
        [
            if (theFilter eval:theBefore4Prev:theBefore3Prev:theBefore2Prev:theBeforePrev:thePrevious:(theEnumerator get))
                [ ^ true ].
            
            theBefore4Prev := theBefore3Prev.
            theBefore3Prev := theBefore2Prev.
            theBefore2Prev := theBeforePrev.
            theBeforePrev := thePrevious.
            thePrevious := theEnumerator get
        ].
        
        ^ false
    ]
}

// --- Selecting ---

class Selector :: Enumerator
{
    func1      theSelector.
    enumerator theEnumerator.
    
    constructor new each:aSelector enumerator:anEnumerator
    [
        theSelector := aSelector func1.
        theEnumerator := anEnumerator.
    ]
    
    get = theSelector eval:(theEnumerator get).
    
    next = theEnumerator next.
    
    reset 
    [
        theEnumerator reset.
    ]    
}

// --- Repeater ---

class Repeater :: Enumerator
{
    int   theMin.
    int   theMax.
    int   theCurrent.
    func1 theFunc.
    
    constructor new till:maxValue each:func
    [
        theMax := maxValue int.
        theMin := 0.
        theCurrent := -1.
        theFunc := func func1.
    ]
    
    constructor new from:minValue till:maxValue each:func
    [
        theMin := minValue int.
        theMax := maxValue int.
        theCurrent := theMin - 1.
        theFunc := func func1.
    ]
    
    get = theFunc eval:theCurrent.
    
    reset
    [
        theCurrent := theMin - 1.
    ]
    
    next
    [
        theCurrent := theCurrent + 1.
        
        ^ (theCurrent < theMax).
    ]
}

// --- ZipEnumerator ---

class ZipEnumerator :: Enumerator
{
    enumerator theEnumerator1.
    enumerator theEnumerator2.
    
    func2      theZipper.
    
    constructor new:combinator enumerator:enum1 enumerator:enum2
    [
        theZipper := combinator func2.
        
        theEnumerator1 := enum1.
        theEnumerator2 := enum2.
    ]
    
    get = theZipper eval:(theEnumerator1 get):(theEnumerator2 get).
    
    next = theEnumerator1 next && theEnumerator2 next.
    
    reset
    [
        theEnumerator1 reset.
        theEnumerator2 reset.
    ]
}

// --- RangeEnumerator ---

class RangeEnumerator :: Enumerator
{
    object theVariable.
    object theStart.
    object theEnd.
    object theStep.
    
    bool   theForward.
    
    constructor new for:aVariable from:aStart to:anEnd by:aStep
    [
        theVariable := aVariable.
        theStart := aStart.
        theEnd := anEnd.
        theStep := aStep.
        
        theForward := (aStart < anEnd).
        
        $self reset.
    ]

    constructor new from:aStart to:anEnd by:aStep
        <= new for:(Variable new) from:aStart to:anEnd by:aStep.

    constructor new from:aStart to:anEnd
        <= new for:(Variable new) from:aStart to:anEnd by:1.
        
    reset 
    [
        theVariable set:theStart.

        if(theForward)
            [ theVariable reduce:theStep ];
            [ theVariable append:theStep ]
    ]    
    
    get = theVariable get.
    
    next
    [
        if(theForward)
            [ theVariable append:theStep. ^ (theVariable <= theEnd) ].
        
        theVariable reduce:theStep. 
        
        ^ (theVariable >= theEnd)
    ]
}

// --- IntervalEnumerator ---

class IntervalEnumerator :: Enumerator
{
    object theVariable.
    object theStart.
    object theEnd.
    object theStep.
    
    bool   theForward.
    
    constructor new for:aVariable from:aStart till:anEnd by:aStep
    [
        theVariable := aVariable.
        theStart := aStart.
        theEnd := anEnd.
        theStep := aStep.
        
        theForward := (aStart < anEnd).
        
        $self reset.
    ]

    constructor new from:aStart till:anEnd by:aStep
        <= new for:(Variable new) from:aStart till:anEnd by:aStep.

    constructor new from:aStart till:anEnd
        <= new for:(Variable new) from:aStart till:anEnd by:1.
        
    reset 
    [
        theVariable set:theStart.

        if(theForward)
            [ theVariable reduce:theStep ];
            [ theVariable append:theStep ]
    ]    
    
    get = theVariable get.
    
    next
    [
        if(theForward )
            [ theVariable append:theStep. ^ (theVariable < theEnd) ].
        
        theVariable reduce:theStep. 
        
        ^ (theVariable > theEnd)
    ]
}

// --- GroupingEnumerator ---

class GroupingEnumerator :: Enumerator
{
    dictionary theDictionary.
    enumerator theEnumerator.
    func1      theGroupFunc.    
    enumerator theGroupEnumerator.
    
    constructor new func1:grouper enumerator:enumerator
    [
        theGroupFunc := grouper.
        theDictionary := Dictionary new.
        theEnumerator := enumerator.
        
        $self reset.
    ]
    
    reset
    [
        theEnumerator reset.
        
        while (theEnumerator next)
        [
            var item := theEnumerator get.
            var key := theGroupFunc eval:item.
            
            var groupItem := theDictionary@key.
            if ($nil == groupItem)
            [
                groupItem := ArrayList new.
                
                theDictionary@key := groupItem.
            ].
            
            groupItem append:item.
        ].
        
        theGroupEnumerator := theDictionary enumerator.
    ]
    
    get = theGroupEnumerator get.
    
    next
        = theGroupEnumerator next.
}

// --- OrderingEnumerator ---

class OrderingEnumerator :: Enumerator
{
    enumerator theEnumerator.
    func2      theOrderFunc.    
    enumerator theListEnumerator.
    
    constructor new func2:orderFunc enumerator:enumerator
    [
        theOrderFunc := orderFunc.
        theEnumerator := enumerator.
        
        $self reset.
    ]
    
    reset
    [
        theEnumerator reset.
        
        array list := Summing new:(ArrayList new); run enumerator:theEnumerator.
        list sort func2:theOrderFunc.
        
        theListEnumerator := list enumerator.
    ]
    
    get = theListEnumerator get.
    
    next
        = theListEnumerator next.
}

// === patterns ===

// --- BasePattern ---

class BasePattern :: BaseFunction1
{
    object theResult.
    
    run enumerator:anEnumerator
    [
        $super run enumerator:anEnumerator.
        
        ^ theResult
    ]
}

// --- Summing ---

class Summing :: BasePattern
{
    constructor new : aVariable
    [
        theResult := aVariable.
    ]

    eval : aParam [ theResult append:aParam. ]
}

// --- Accumulating ---

class Accumulating :: BasePattern
{
    func2 theOperation.
    
    constructor new : aVariable with:operation
    [
        theResult := aVariable.
        theOperation := operation func2.
    ]

    eval : aParam [ theResult set:(theOperation eval:theResult:aParam). ]
}

// --- patternOp ---

extension patternOp
{
    summarize : aVariable
    [
        Summing new:aVariable; run enumerator:(self enumerator).
        
        ^ aVariable get.
    ]
    
    summarize
        = self~patternOp summarize:(Variable new).
        
    accumulate : aVariable with:operation
    [
        Accumulating new:aVariable with:operation; run enumerator:(self enumerator).
        
        ^ aVariable get.
    ]
    
    accumulate with:operation 
        = self~patternOp accumulate:(Variable new) with:operation.

    top:aCounter
        = TopFilter new:aCounter enumerator:(self enumerator).

    last:counter
        = TopFilter new:counter skipping:(self length - counter) enumerator:(self enumerator).

    filter each:aFilter
        = Filter new each:aFilter enumerator:(self enumerator).

    select each:aSelector
        = Selector new each:aSelector enumerator:(self enumerator).

    zip:anEnumerable into:aZipper
        = ZipEnumerator new:aZipper enumerator:(self enumerator) enumerator:(anEnumerable enumerator).

    zip:anEnumerable eachPair:func2
        = ZipEnumerator new:(:x:y)[ func2 eval:x:y ] enumerator:(self enumerator) enumerator:(anEnumerable enumerator); run each:idleFunc1.

    group by:groupFunc
        = GroupingEnumerator new func1:(groupFunc func1) enumerator:(self enumerator).                                       

    order by:orderFunc
        = OrderingEnumerator new func2:(orderFunc func2) enumerator:(self enumerator).                                       

    filter pattern:patternFun
        = patternFun cast:%eval to:
        {
            eval:enumerable func1:func1
                = Filter new each:func1 enumerator:(enumerable enumerator).
            
            eval:enumerable func2:func2
                = PatternFilter2 new func2:func2 enumerator:(enumerable enumerator).
                
            eval:enumerable func3:func3
                = PatternFilter3 new func3:func3 enumerator:(enumerable enumerator).

            eval:enumerable func4:func4
                = PatternFilter4 new func4:func4 enumerator:(enumerable enumerator).

            eval:enumerable func5:func5
                = PatternFilter5 new func5:func5 enumerator:(enumerable enumerator).

            eval:enumerable func6:func6
                = PatternFilter6 new func6:func6 enumerator:(enumerable enumerator).

        } with:self.
            
    /// obosolete
    repeat each:aRepeater    
        = Repeater new till:self each:aRepeater.
        
    repeat till:max each:aRepeater
        = Repeater new from:self till:max each:aRepeater.
        
    counting
        = RangeEnumerator new from:1 to:(self int) by:1.
}
