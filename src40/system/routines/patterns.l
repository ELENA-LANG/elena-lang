import system'collections;

idleFunc1 = (x){};

// === pattern enumerators ==

// --- Toping ---

public class TopFilter : Enumerator
{
    ref<int>   theOffset;
    ref<int>   theCounter;
    Enumerator theEnumerator;
    
    constructor(int counter, Enumerator enumerator)
    {
        theCounter := counter;
        theOffset := 0;
        theEnumerator := enumerator;
    }
    
    constructor new(int counter, int offset, Enumerator enumerator)
    {
        theCounter := counter;
        theOffset := offset;
        
        theEnumerator := enumerator;
    }
    
    get() = theEnumerator.get();
    
    bool next()
    {
        if (theCounter != 0)
        {
            while (theEnumerator.next())
            {
                if (theOffset == 0)
                {
                    theCounter.reduce(1); ^ true
                }
                else
                {
                    theOffset.reduce(1)
                }
            }
        };
        
        ^ false
    }
    
    reset() => theEnumerator;
    
    enumerable() => theEnumerator;
}

//// --- SkipFilter ---
//
//public class SkipFilter :: Enumerator
//{
//    ref<int>      theCounter.    
//    bool          skipped.    
//    T<Enumerator> theEnumerator.
//    
//    constructor new(int aCounter, Enumerator en)
//    [
//        theCounter := aCounter.
//        skipped := false.
//        theEnumerator := en.
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
//    
//    enumerable => theEnumerator.
//}

// --- Filtering ---

public class Filter : Enumerator
{
    Func1      theFilter;
    Enumerator theEnumerator;

    constructor(Func1 selector, Enumerator enumerator)
    {
        theFilter := selector;
        theEnumerator := enumerator
    }
    
//    constructor new(ExtensionMessage anExtension, Enumerator anEnumerator)
//    [
//        theFilter := (:x)[ ^ x~anExtension eval ].
//        theEnumerator := anEnumerator.
//    ]
    
    get() = theEnumerator.get();
    
    bool next()
    {
        while (theEnumerator.next())
        {
            if (theFilter(theEnumerator.get()))
                { ^ true }
        };
        
        ^ false
    }
    
    reset () => theEnumerator;
    
    enumerable() => theEnumerator;
}

//// --- PatternFilter2 ---
//
//public class PatternFilter2 :: Enumerator
//{
//    T<Func2>      theFilter.
//    T<Enumerator> theEnumerator.
//    object        thePrevious.
//    
//    constructor new(Func2 aFilter, Enumerator anEnumerator)
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    bool next
//    [
//        if (nil == thePrevious)
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
//    
//    reset
//    [
//        theEnumerator reset.
//        thePrevious := nil.
//    ]
//    
//    enumerable => theEnumerator.
//}
//
//// --- PatternFilter3 ---
//
//public class PatternFilter3 :: Enumerator
//{
//    T<Func3>      theFilter.
//    T<Enumerator> theEnumerator.
//    object        thePrevious.
//    object        theBeforePrev.
//    
//    constructor new(Func3 aFilter, Enumerator anEnumerator)
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    reset
//    [
//        theEnumerator reset.
//        thePrevious := nil.
//    ]
//    
//    bool next
//    [
//        if (nil == thePrevious)
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
//    
//    enumerable => theEnumerator.
//}
//
//// --- PatternFilter4 ---
//
//public class PatternFilter4 :: Enumerator
//{
//    T<Func4>      theFilter.
//    T<Enumerator> theEnumerator.
//    object        thePrevious.
//    object        theBeforePrev.
//    object        theBefore2Prev.
//    
//    constructor new(Func4 aFilter, Enumerator anEnumerator)
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    reset
//    [
//        theEnumerator reset.
//        thePrevious := nil.
//    ]
//    
//    bool next
//    [
//        if(nil == thePrevious)
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
//    
//    enumerable => theEnumerator.
//}
//
//// --- PatternFilter5 ---
//
//public class PatternFilter5 :: Enumerator
//{
//    T<Func5>      theFilter.
//    T<Enumerator> theEnumerator.
//    object        thePrevious.
//    object        theBeforePrev.
//    object        theBefore2Prev.
//    object        theBefore3Prev.
//    
//    constructor new(Func5 aFilter, Enumerator anEnumerator)
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    reset
//    [
//        theEnumerator reset.
//        thePrevious := nil.
//    ]
//    
//    bool next
//    [
//        if (nil == thePrevious)
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
//    
//    enumerable => theEnumerator.
//}
//
//// --- PatternFilter6 ---
//
//public class PatternFilter6 :: Enumerator
//{
//    T<Func6>      theFilter.
//    T<Enumerator> theEnumerator.
//    object        thePrevious.
//    object        theBeforePrev.
//    object        theBefore2Prev.
//    object        theBefore3Prev.
//    object        theBefore4Prev.
//    
//    constructor new(Func6 aFilter, Enumerator anEnumerator)
//    [
//        theFilter := aFilter.
//        theEnumerator := anEnumerator.
//    ]
//    
//    get = theEnumerator get.
//    
//    reset
//    [
//        theEnumerator reset.
//        thePrevious := nil.
//    ]
//    
//    bool next
//    [
//        if (nil == thePrevious)
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
//    
//    enumerable => theEnumerator.
//}

// --- Selecting ---

class Selector : Enumerator
{
    Func1      theSelector;
    Enumerator theEnumerator;

    constructor new(Func1 selector, Enumerator enumerator)
    {
        theSelector := selector;
        theEnumerator := enumerator
    }
    
    constructor new(ExtensionMessage extension, Enumerator enumerator)
    {
        theSelector := (x){ ^ mixin extension(x).eval() };
        theEnumerator := enumerator
    }
    
    get() = theSelector(theEnumerator.get());
    
    bool next() = theEnumerator.next();
    
    reset()
    {
        theEnumerator.reset()
    }    
    
    enumerable() => theEnumerator;
}

//// --- Repeater ---
//
//public class Repeater :: Enumerator
//{
//    int      theMin.
//    int      theMax.
//    int      theCurrent.
//    T<Func1> theFunc.
//
//    constructor new(int minValue, int maxValue, Func1 aRepetition)
//    [
//        theFunc := aRepetition.
//        theMin := minValue.
//        theMax := maxValue.
//        theCurrent := theMin - 1.
//    ]
//    
//    constructor new(int minValue, int maxValue, ExtensionMessage anExtension)
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
//    
//    enumerable = Range from:theMin to:theMax.
//}

// --- ZipEnumerator ---

public class ZipEnumerator : Enumerator
{
    Enumerator theEnumerator1;
    Enumerator theEnumerator2;
    
    Func2      theZipper;
    
    constructor new(Func2 fn, Enumerator enum1, Enumerator enum2)
    {
        theZipper := fn;
        
        theEnumerator1 := enum1;
        theEnumerator2 := enum2;
    }
    
//    constructor new(ExtensionMessage ext, Enumerator enum1, Enumerator enum2)
//        <= new((:n:m)(n~ext eval:m), enum1, enum2).
    
    get() = theZipper(theEnumerator1.get(), theEnumerator2.get());
    
    bool next() = theEnumerator1.next() && theEnumerator2.next();
    
    reset()
    {
        theEnumerator1.reset();
        theEnumerator2.reset();
    }
    
    enumerable() => theEnumerator1;
}

// --- RangeEnumerator ---

public class RangeEnumerator : Enumerator
{
    object theVariable;
    object theStart;
    object theEnd;
    object theStep;
    
    bool   theForward;
    
    constructor(var,start,end,step)
    {
        theVariable := var;
        theStart := start;
        theEnd := end;
        theStep := step;
        
        theForward := (start < end);
        
        self.reset()
    }

    constructor(start, end, step)
        <= (new Variable(), start, end, step);

//    constructor from:aStart to:anEnd
//        <= for:(Variable new) from:aStart to:anEnd by:1.
        
    reset() 
    {
        theVariable.Value := theStart;

        if(theForward)
        {
            theVariable.reduce:theStep 
        }
        else
        {
            theVariable.append:theStep
        }
    }    
    
    get() = theVariable.Value;
    
    bool next()
    {
        if(theForward)
            { theVariable.append:theStep; ^ (theVariable <= theEnd) };
        
        theVariable.reduce:theStep; 
        
        ^ (theVariable >= theEnd)
    }
    
    enumerable() = theVariable;
}

//// --- IntervalEnumerator ---
//
//public class IntervalEnumerator :: Enumerator
//{
//    object theVariable.
//    object theStart.
//    object theEnd.
//    object theStep.
//    
//    bool   theForward.
//    
//    constructor for:aVariable from:aStart till:anEnd by:aStep
//    [
//        theVariable := aVariable.
//        theStart := aStart.
//        theEnd := anEnd.
//        theStep := aStep.
//        
//        theForward := (aStart < anEnd).
//        
//        self reset.
//    ]
//
//    constructor from:aStart till:anEnd by:aStep
//        <= for:(Variable new) from:aStart till:anEnd by:aStep.
//
//    constructor from:aStart till:anEnd
//        <= for:(Variable new) from:aStart till:anEnd by:1.
//        
//    reset 
//    [
//        theVariable value := theStart.
//
//        if(theForward)
//            [ theVariable reduce:theStep ];
//            [ theVariable append:theStep ]
//    ]    
//    
//    get = theVariable value.
//    
//    bool next
//    [
//        if(theForward )
//            [ theVariable append:theStep. ^ (theVariable < theEnd) ].
//        
//        theVariable reduce:theStep. 
//        
//        ^ (theVariable > theEnd)
//    ]
//    
//    enumerable = theVariable.
//}

// --- GroupingEnumerator ---

public class GroupingEnumerator : Enumerator
{
    Dictionary theDictionary;
    Enumerator theEnumerator;
    Func1      theGroupFunc;   
    Enumerator theGroupEnumerator;
    
    constructor new(Func1 grouper, Enumerator enumerator)
    {
        theGroupFunc := grouper;
        theDictionary := new Dictionary();
        theEnumerator := enumerator;
        
        self.reset()
    }
    
//    constructor new(ExtensionMessage fn, Enumerator enumerator)
//        <= new((:x)(x~fn eval), enumerator).
    
    reset()
    {
        theEnumerator.reset();
        
        while (theEnumerator.next())
        {
            var item := theEnumerator.get();
            var key := theGroupFunc(item);
            
            var groupItem := theDictionary[key];
            if (nil == groupItem)
            {
                groupItem := new ArrayList();
                
                theDictionary[key] := groupItem
            };
            
            groupItem.append:item
        };
        
        theGroupEnumerator := theDictionary.enumerator();
    }
    
    get() = theGroupEnumerator.get();
    
    bool next()
        = theGroupEnumerator.next();
        
    enumerable()
        => theEnumerator;
}

// --- OrderingEnumerator ---

public class OrderingEnumerator : Enumerator
{
    Enumerator theEnumerator;
    Func2      theOrderFunc;   
    Enumerator theListEnumerator;
    
    constructor new(Func2 orderFunc, Enumerator enumerator)
    {
        theOrderFunc := orderFunc;
        theEnumerator := enumerator;
        
        self.reset();
    }
    
//    constructor new(Func1 orderVal, Enumerator enumerator)
//    [
//        theOrderFunc := (:p:n)(orderVal(p) < orderVal(n) ).
//        theEnumerator := enumerator.
//        
//        self reset.
//    ]
//    
//    constructor new(ExtensionMessage fn, Enumerator enumerator)
//        <= new((:x:y)(x~fn eval:y), enumerator).
    
    reset()
    {
        theEnumerator.reset();
        
        Array list := new Summing(new ArrayList()).run(theEnumerator);
        list.sort(theOrderFunc);
        
        theListEnumerator := list.enumerator();
    }
    
    get() = theListEnumerator.get();
    
    bool next()
        = theListEnumerator.next();
        
    enumerable()
        => theEnumerator;
}

// === patterns ===

// --- BasePattern ---

public abstract class BasePattern : Func1
{
    object theResult;
    
    run(Enumerator en)
    {
        super.run(en);
        
        ^ theResult
    }
}

// --- Summing ---

public class Summing : BasePattern
{
    constructor(variable)
    {
        theResult := variable
    }

    closure(param) { theResult.append(param) }
}

// --- Accumulating ---

public class Accumulating : BasePattern
{
    Func2 theOperation;
    
    constructor new(object variable, Func2 operation)
    {
        theResult := variable;
        theOperation := operation
    }

    constructor new(object variable, ExtensionMessage operation)
    {
        theResult := variable;
        theOperation := (a,b){ mixin operation(a).eval(b) }
    }

    closure(param) { theResult.Value := theOperation(theResult.Value, param) }
}

//// --- JoinEnumerator ---
//
//public class JoinEnumerator :: Enumerator
//{
//    T<Enumerator> _left.
//    T<Enumerator> _right.
//
//    T<Func2> _filter.
//    T<Func2> _selector.
//
//    constructor new(Func2 filter_f, Func2 select_f, Enumerator left_e, Enumerator right_e)
//    [
//        _left := left_e.
//        _right := right_e.
//        
//        _filter := filter_f.
//        _selector := select_f.
//    ]
//    
//    reset
//    [
//        _left reset.
//        _right reset.
//    ]
//    
//    bool next
//    [
//        while(_left next)
//        [
//            _right reset.
//            
//            while (_right next)
//            [
//                if(_filter(_left get, _right get)) 
//                [
//                    ^ true.
//                ]
//            ]
//        ].
//        
//        ^ false.
//    ]
//    
//    get = _selector(_left get, _right get).
//    
//    enumerable => _left.
//}
//
//// --- patternOp ---
//
//public extension patternOp
//{
//    skipping:counter
//        = SkipFilter new(counter, T<Enumerator>(self enumerator)).
//
//    last:counter
//        = TopFilter new(counter, self length - counter, T<Enumerator>(self enumerator)).
//
//    orderBy:orderFunc
//        = OrderingEnumerator new(orderFunc, T<Enumerator>(self enumerator)).
//
////    filter pattern:patternFun
////        = Filter new(patternFun, T<Enumerator>(self enumerator)).
//}
