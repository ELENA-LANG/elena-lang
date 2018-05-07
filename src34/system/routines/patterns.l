//import system'collections.
//
//symbol idleFunc1 = (:x) [].
//
//// === pattern enumerators ==
//
//// --- Toping ---
//
//class TopFilter :: Enumerator
//{
//    vint       theOffset.    
//    vint       theCounter.    
//    enumerator theEnumerator.
//    
//    constructor new int:aCounter enumerator:anEnumerator
//    [
//        theCounter := aCounter.
//        theOffset := 0.
//        theEnumerator := anEnumerator.
//    ]
//    
//    constructor new int:counter int:offset enumerator:enumerator
//    [
//        theCounter := counter int.
//        theOffset := offset int.
//        
//        theEnumerator := enumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if (theCounter != 0)
//        [
//            while (theEnumerator next)
//            [
//                if (theOffset == 0)
//                    [  
//                        theCounter reduce int:1. ^ true
//                    ];
//                    [ theOffset reduce int:1 ]
//            ].
//        ].
//        
//        ^ false
//    ]
//    
//    reset 
//    [
//        theEnumerator reset.
//    ]    
//}
//
//// --- SkipFilter ---
//
//class SkipFilter :: Enumerator
//{
//    vint       theCounter.    
//    bool       skipped.    
//    enumerator theEnumerator.
//    
//    constructor new int:aCounter enumerator:anEnumerator
//    [
//        theCounter := aCounter.
//        skipped := false.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        ifnot (skipped)
//        [
//            skipped := true.
//            
//            int i := theCounter.
//            while (i > 0)            
//            [
//                ifnot (theEnumerator next)
//                [
//                    ^ false
//                ].
//                
//                i -= 1.
//            ]
//        ].
//        
//        ^ theEnumerator next
//    ]
//    
//    reset 
//    [
//        theEnumerator reset.
//        skipped := false.
//    ]    
//}
//
//// --- Filtering ---
//
//class Filter :: Enumerator
//{
//    func1      theFilter.
//    enumerator theEnumerator.
//
//    constructor new func1:aSelector enumerator:anEnumerator
//    [
//        theFilter := aSelector.
//        theEnumerator := anEnumerator.
//    ]
//    
//    constructor new ext_mssg:anExtension enumerator:anEnumerator
//    [
//        theFilter := (:x)[ ^ x~anExtension eval ].
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        while (theEnumerator next)
//        [
//            if (theFilter(theEnumerator get))
//                [ ^ true ]
//        ].
//        
//        ^ false
//    ]
//    
//    reset 
//    [
//        theEnumerator reset.
//    ]    
//}
//
//// --- PatternFilter2 ---
//
//class PatternFilter2 :: Enumerator
//{
//    func2      theFilter.
//    enumerator theEnumerator.
//    object     thePrevious.
//    
//    constructor new func2:aFilter enumerator:anEnumerator
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if ($nil == thePrevious)
//        [
//            ifnot (theEnumerator next)
//                [ ^ false ].
//            
//            thePrevious := theEnumerator get.
//        ].
//        
//        while (theEnumerator next)
//        [
//            if (theFilter(thePrevious,theEnumerator get))
//                [ ^ true ].
//            
//            thePrevious := theEnumerator get
//        ].
//        
//        ^ false
//    ]
//}
//
//// --- PatternFilter3 ---
//
//class PatternFilter3 :: Enumerator
//{
//    func3      theFilter.
//    enumerator theEnumerator.
//    object     thePrevious.
//    object     theBeforePrev.
//    
//    constructor new func3:aFilter enumerator:anEnumerator
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if ($nil == thePrevious)
//        [
//            ifnot (theEnumerator next)
//                [ ^ false ].
//            
//            theBeforePrev := theEnumerator get.
//    
//            ifnot (theEnumerator next)
//                [ ^ false ].
//            
//            thePrevious := theEnumerator get
//        ].
//                
//        while (theEnumerator next)
//        [
//            if(theFilter(theBeforePrev,thePrevious,theEnumerator get))
//                [ ^ true ].
//            
//            theBeforePrev := thePrevious.
//            thePrevious := theEnumerator get
//        ].
//        
//        ^ false
//    ]
//}
//
//// --- PatternFilter4 ---
//
//class PatternFilter4 :: Enumerator
//{
//    func4      theFilter.
//    enumerator theEnumerator.
//    object     thePrevious.
//    object     theBeforePrev.
//    object     theBefore2Prev.
//    
//    constructor new func4:aFilter enumerator:anEnumerator
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if($nil == thePrevious)
//        [
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBefore2Prev := theEnumerator get.
//            
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBeforePrev := theEnumerator get.
//    
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            thePrevious := theEnumerator get.
//        ].
//                
//        while (theEnumerator next)
//        [
//            if (theFilter(theBefore2Prev,theBeforePrev,thePrevious,theEnumerator get))
//                [ ^ true ].
//            
//            theBefore2Prev := theBeforePrev.
//            theBeforePrev := thePrevious.
//            thePrevious := theEnumerator get
//        ].
//        
//        ^ false.
//    ]
//}
//
//// --- PatternFilter5 ---
//
//class PatternFilter5 :: Enumerator
//{
//    func5      theFilter.
//    enumerator theEnumerator.
//    object     thePrevious.
//    object     theBeforePrev.
//    object     theBefore2Prev.
//    object     theBefore3Prev.
//    
//    constructor new func5:aFilter enumerator:anEnumerator
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if ($nil == thePrevious)
//        [
//            ifnot (theEnumerator next)
//                [ ^ false ].
//            
//            theBefore3Prev := theEnumerator get.
//            
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBefore2Prev := theEnumerator get.
//            
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBeforePrev := theEnumerator get.
//    
//            ifnot (theEnumerator next)
//                [ ^ false ].
//            
//            thePrevious := theEnumerator get.
//        ].
//                
//        while (theEnumerator next)
//        [
//            if (theFilter(theBefore3Prev,theBefore2Prev,theBeforePrev,thePrevious,theEnumerator get))
//                [ ^ true ].
//            
//            theBefore3Prev := theBefore2Prev.
//            theBefore2Prev := theBeforePrev.
//            theBeforePrev := thePrevious.
//            thePrevious := theEnumerator get
//        ].
//        
//        ^ false
//    ]
//}
//
//// --- PatternFilter6 ---
//
//class PatternFilter6 :: Enumerator
//{
//    func6      theFilter.
//    enumerator theEnumerator.
//    object     thePrevious.
//    object     theBeforePrev.
//    object     theBefore2Prev.
//    object     theBefore3Prev.
//    object     theBefore4Prev.
//    
//    constructor new func6:aFilter enumerator:anEnumerator
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if ($nil == thePrevious)
//        [
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBefore4Prev := theEnumerator get.
//            
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBefore3Prev := theEnumerator get.
//            
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBefore2Prev := theEnumerator get.
//            
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            theBeforePrev := theEnumerator get.
//    
//            ifnot(theEnumerator next)
//                [ ^ false ].
//            
//            thePrevious := theEnumerator get
//        ].
//                                
//        while (theEnumerator next)
//        [
//            if (theFilter(theBefore4Prev,theBefore3Prev,theBefore2Prev,theBeforePrev,thePrevious,theEnumerator get))
//                [ ^ true ].
//            
//            theBefore4Prev := theBefore3Prev.
//            theBefore3Prev := theBefore2Prev.
//            theBefore2Prev := theBeforePrev.
//            theBeforePrev := thePrevious.
//            thePrevious := theEnumerator get
//        ].
//        
//        ^ false
//    ]
//}

// --- Selecting ---

class Selector :: Enumerator
{
    T<Func1>      theSelector.
    T<Enumerator> theEnumerator.

    constructor new(Func1 aSelector, Enumerator anEnumerator)
    [
        theSelector := aSelector.
        theEnumerator := anEnumerator.
    ]
    
//    constructor new ext_mssg:anExtension enumerator:anEnumerator
//    [
//        theSelector := (:x)[ ^ x~anExtension eval ].
//        theEnumerator := anEnumerator.
//    ]
    
    get = theSelector(theEnumerator get).
    
    bool next = theEnumerator next.
    
    reset 
    [
        theEnumerator reset.
    ]    
    
    enumerable => theEnumerator.
}

//// --- Repeater ---
//
//class Repeater :: Enumerator
//{
//    int   theMin.
//    int   theMax.
//    int   theCurrent.
//    func1 theFunc.
//
//    constructor new int:minValue int:maxValue func1:aRepetition
//    [
//        theFunc := aRepetition.
//        theMin := minValue.
//        theMax := maxValue.
//        theCurrent := theMin - 1.
//    ]
//    
//    constructor new int:minValue int:maxValue ext_mssg:anExtension
//    [
//        theFunc := (:x)[ ^ x~anExtension eval ].
//        theMin := minValue.
//        theMax := maxValue.
//        theCurrent := theMin - 1.
//    ]
//
//
//    get = theFunc(theCurrent).
//    
//    reset
//    [
//        theCurrent := theMin - 1.
//    ]
//    
//    bool next
//    [
//        theCurrent := theCurrent + 1.
//        
//        ^ (theCurrent < theMax).
//    ]
//}
//
//// --- ZipEnumerator ---
//
//class ZipEnumerator :: Enumerator
//{
//    enumerator theEnumerator1.
//    enumerator theEnumerator2.
//    
//    func2      theZipper.
//    
//    constructor new(BaseFunction2 fn, Enumerator enum1, Enumerator enum2)
//    [
//        theZipper := fn.
//        
//        theEnumerator1 := enum1.
//        theEnumerator2 := enum2.
//    ]
//    
//    constructor new(ExtensionMessage ext, Enumerator enum1, Enumerator enum2)
//        <= new func2:(:n:m)(n~ext eval:m) enumerator:enum1 enumerator:enum2.
//    
//    get = theZipper(theEnumerator1 get, theEnumerator2 get).
//    
//    bool next = theEnumerator1 next && theEnumerator2 next.
//    
//    reset
//    [
//        theEnumerator1 reset.
//        theEnumerator2 reset.
//    ]
//}

// --- RangeEnumerator ---

public class RangeEnumerator :: Enumerator
{
    object theVariable.
    object theStart.
    object theEnd.
    object theStep.
    
    bool   theForward.
    
    constructor for:aVariable from:aStart to:anEnd by:aStep
    [
        theVariable := aVariable.
        theStart := aStart.
        theEnd := anEnd.
        theStep := aStep.
        
        theForward := (aStart < anEnd).
        
        self reset.
    ]

    constructor from:aStart to:anEnd by:aStep
        <= for:(Variable new) from:aStart to:anEnd by:aStep.

    constructor from:aStart to:anEnd
        <= for:(Variable new) from:aStart to:anEnd by:1.
        
    reset 
    [
        theVariable value := theStart.

        if(theForward)
            [ theVariable reduce:theStep ];
            [ theVariable append:theStep ]
    ]    
    
    get = theVariable value.
    
    bool next
    [
        if(theForward)
            [ theVariable append:theStep. ^ (theVariable <= theEnd) ].
        
        theVariable reduce:theStep. 
        
        ^ (theVariable >= theEnd)
    ]
    
    enumerable = theVariable.
}

//// --- BaseIndexer ---
//
//class IntRangeEnumerator :: Enumerator
//{
//    int theIndex.
//    int theStart.
//    int theEnd.
//
//    stacksafe constructor new(IntNumber start, IntNumber end)
//    [
//        theStart := start.
//        theEnd := end.
//        
//        $self reset.
//    ]
//
//    get = theIndex.
//            
//    /// Returns false if the index out of range
//    bool next 
//    [
//        theIndex += 1.
//        
//        ^ theIndex < theEnd
//    ]
//    
//    /// Sets the i-index by specified value
//    reset
//    [ 
//        theIndex := theStart - 1.
//    ] 
//}
//
//// --- Range --
//
//class Range :: BaseEnumerable
//{
//    int theStart.
//    int theEnd.
//    
//    constructor new(IntNumber start, IntNumber count)
//    [
//        theStart := start.
//        theEnd := start + count.
//    ]
//    
//    constructor from(IntNumber start) till(IntNumber end)
//    [
//        theStart := start.
//        theEnd := end.
//    ]
//    
//    constructor from(IntNumber start) to(IntNumber end)
//    [
//        theStart := start.
//        theEnd := end + 1.
//    ]
//    
//    stacksafe readLengthTo vint:retVal
//    [
//        int l := theEnd - theStart.
//        
//        retVal int := l.
//    ]
//    
//    int rvprop length :: readLengthTo.
//    
//    enumerator = IntRangeEnumerator new(theStart, theEnd).
//}

// --- IntervalEnumerator ---

public class IntervalEnumerator :: Enumerator
{
    object theVariable.
    object theStart.
    object theEnd.
    object theStep.
    
    bool   theForward.
    
    constructor for:aVariable from:aStart till:anEnd by:aStep
    [
        theVariable := aVariable.
        theStart := aStart.
        theEnd := anEnd.
        theStep := aStep.
        
        theForward := (aStart < anEnd).
        
        self reset.
    ]

    constructor from:aStart till:anEnd by:aStep
        <= for:(Variable new) from:aStart till:anEnd by:aStep.

    constructor from:aStart till:anEnd
        <= for:(Variable new) from:aStart till:anEnd by:1.
        
    reset 
    [
        theVariable value := theStart.

        if(theForward)
            [ theVariable reduce:theStep ];
            [ theVariable append:theStep ]
    ]    
    
    get = theVariable value.
    
    bool next
    [
        if(theForward )
            [ theVariable append:theStep. ^ (theVariable < theEnd) ].
        
        theVariable reduce:theStep. 
        
        ^ (theVariable > theEnd)
    ]
    
    enumerable = theVariable.
}

//// --- GroupingEnumerator ---
//
//class GroupingEnumerator :: Enumerator
//{
//    dictionary theDictionary.
//    enumerator theEnumerator.
//    func1      theGroupFunc.    
//    enumerator theGroupEnumerator.
//    
//    constructor new func1:grouper enumerator:enumerator
//    [
//        theGroupFunc := grouper.
//        theDictionary := Dictionary new.
//        theEnumerator := enumerator.
//        
//        $self reset.
//    ]
//    
//    constructor new ext_mssg:fn enumerator:enumerator
//        <= new func1:(:x)(x~fn eval) enumerator:enumerator.
//    
//    reset
//    [
//        theEnumerator reset.
//        
//        while (theEnumerator next)
//        [
//            var item := theEnumerator get.
//            var key := theGroupFunc(item).
//            
//            var groupItem := theDictionary[key].
//            if ($nil == groupItem)
//            [
//                groupItem := ArrayList new.
//                
//                theDictionary[key] := groupItem.
//            ].
//            
//            groupItem append:item.
//        ].
//        
//        theGroupEnumerator := theDictionary enumerator.
//    ]
//    
//    get = theGroupEnumerator get.
//    
//    bool next
//        = theGroupEnumerator next.
//}
//
//// --- OrderingEnumerator ---
//
//class OrderingEnumerator :: Enumerator
//{
//    enumerator theEnumerator.
//    func2      theOrderFunc.    
//    enumerator theListEnumerator.
//    
//    constructor new func2:orderFunc enumerator:enumerator
//    [
//        theOrderFunc := orderFunc.
//        theEnumerator := enumerator.
//        
//        $self reset.
//    ]
//    
//    constructor new func1:orderVal enumerator:enumerator
//    [
//        theOrderFunc := (:p:n)(orderVal(p) < orderVal(n) ).
//        theEnumerator := enumerator.
//        
//        $self reset.
//    ]
//    
//    constructor new ext_mssg:fn enumerator:enumerator
//        <= new func2:(:x:y)(x~fn eval:y) enumerator:enumerator.
//    
//    reset
//    [
//        theEnumerator reset.
//        
//        array list := Summing new:(ArrayList new); run enumerator:theEnumerator.
//        list sort func2:theOrderFunc.
//        
//        theListEnumerator := list enumerator.
//    ]
//    
//    get = theListEnumerator get.
//    
//    bool next
//        = theListEnumerator next.
//}
//
//// === patterns ===
//
//// --- BasePattern ---
//
//class BasePattern :: BaseFunction1
//{
//    object theResult.
//    
//    run enumerator:anEnumerator
//    [
//        $super run enumerator:anEnumerator.
//        
//        ^ theResult
//    ]
//}
//
//// --- Summing ---
//
//class Summing :: BasePattern
//{
//    constructor new : aVariable
//    [
//        theResult := aVariable.
//    ]
//
//    closure : aParam [ theResult append:aParam. ]
//}
//
//// --- Accumulating ---
//
//class Accumulating :: BasePattern
//{
//    func2 theOperation.
//    
//    constructor new(object aVariable, func2 anOperation)
//    [
//        theResult := aVariable.
//        theOperation := anOperation.
//    ]
//
//    constructor new(object aVariable, ext_mssg anOperation)
//    [
//        theResult := aVariable.
//        theOperation := (:a:b)[ a~anOperation eval:b ].
//    ]
//
//    closure : aParam [ theResult set(theOperation(theResult,aParam)). ]
//}
//
//// --- patternOp ---
//
//extension patternOp
//{
//    summarize : aVariable
//    [
//        Summing new:aVariable; run enumerator:(self enumerator).
//        
//        ^ aVariable value.
//    ]
//    
//    summarize
//        = self~patternOp summarize:(Variable new).
//        
//    accumulate : aVariable with:operation
//    [
//        Accumulating new(aVariable, operation); run enumerator:self.
//        
//        ^ aVariable value.
//    ]
//    
//    accumulate with:operation 
//        = self~patternOp accumulate:(Variable new) with:operation.
//
//    top:aCounter
//        = TopFilter new int:aCounter enumerator:(self enumerator).
//
//    skipping:aCounter
//        = SkipFilter new int:aCounter enumerator:(self enumerator).
//
//    last:counter
//        = TopFilter new int:counter int(self length - counter) enumerator:(self enumerator).
//
//    filterBy func1:aSelector
//        = Filter new(aSelector, self enumerator).
//        
//    filterBy func2:aSelector
//        = PatternFilter2 new(aSelector, self enumerator).
//        
//    filterBy func3:aSelector
//        = PatternFilter3 new(aSelector, self enumerator).
//        
//    filterBy func4:aSelector
//        = PatternFilter4 new(aSelector, self enumerator).
//        
//    filterBy func5:aSelector
//        = PatternFilter5 new(aSelector, self enumerator).
//        
//    filterBy func6:aSelector
//        = PatternFilter6 new(aSelector, self enumerator).        
//        
//    zip:anEnumerable by:aZipper
//        = ZipEnumerator new(aZipper, self enumerator, anEnumerable enumerator).
//
//    zip:anEnumerable forEach:func2
//        = self~patternOp zip:anEnumerable by:func2; forEach:idleFunc1.
//
//    groupBy:groupFunc
//        = GroupingEnumerator new(groupFunc, self enumerator).                                       
//
//    orderBy:orderFunc
//        = OrderingEnumerator new(orderFunc, self enumerator).
//
//    filter pattern:patternFun
//        = Filter new(patternFun, self enumerator).
//        
//    till:max repeat:aRepeater
//        = Repeater new(self, max, aRepeater).
//        
//    to:max repeat:aRepeater
//        = Repeater new(self, max + 1, aRepeater).
//
//    repeatTill:n
//        = RangeEnumerator new from:self to(n - 1) by:1.              
//        
//    repeatTo:n
//        = RangeEnumerator new from:self to:n by:1.              
//}
