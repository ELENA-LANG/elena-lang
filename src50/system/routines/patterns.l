//import system'collections;
//
//idleFunc1 = (x){};
//
//// === pattern enumerators ==
//
//// --- Toping ---
//
//public class TopFilter : Enumerator
//{
//    ref<int>   theOffset;
//    ref<int>   theCounter;
//    Enumerator theEnumerator;
//    
//    constructor new(int counter, Enumerator enumerator)
//    {
//        theCounter := counter;
//        theOffset := 0;
//        theEnumerator := enumerator;
//    }
//    
//    constructor new(int counter, int offset, Enumerator enumerator)
//    {
//        theCounter := counter;
//        theOffset := offset;
//        
//        theEnumerator := enumerator;
//    }
//    
//    get() = theEnumerator.get();
//    
//    bool next()
//    {
//        if (theCounter != 0)
//        {
//            while (theEnumerator.next())
//            {
//                if (theOffset == 0)
//                {
//                    theCounter.reduce(1); ^ true
//                }
//                else
//                {
//                    theOffset.reduce(1)
//                }
//            }
//        };
//        
//        ^ false
//    }
//    
//    reset() => theEnumerator;
//    
//    enumerable() => theEnumerator;
//}
//
//// --- SkipFilter ---
//
//public class SkipFilter : Enumerator
//{
//    int        theCounter;    
//    bool       skipped;    
//    Enumerator theEnumerator;
//    
//    constructor new(int counter, Enumerator en)
//    {
//        theCounter := counter;
//        skipped := false;
//        theEnumerator := en
//    }
//    
//    get() = theEnumerator.get();
//    
//    bool next()
//    {
//        ifnot (skipped)
//        {
//            skipped := true;
//            
//            int i := theCounter;
//            while (i > 0)            
//            {
//                ifnot (theEnumerator.next())
//                {
//                    ^ false
//                };
//                
//                i -= 1
//            }
//        };
//        
//        ^ theEnumerator.next()
//    }
//    
//    reset()
//    {
//        theEnumerator.reset();
//        skipped := false
//    }
//    
//    enumerable() => theEnumerator;
//}
//
//// --- Filtering ---
//
//public class Filter : Enumerator
//{
//    Func1      theFilter;
//    Enumerator theEnumerator;
//
//    constructor new(Func1 selector, Enumerator enumerator)
//    {
//        theFilter := selector;
//        theEnumerator := enumerator
//    }
//    
//    constructor new(ExtensionMessage extension, Enumerator enumerator)
//    {
//        theFilter := (x => extension(x));
//        theEnumerator := enumerator
//    }
//    
//    get() = theEnumerator.get();
//    
//    bool next()
//    {
//        while (theEnumerator.next())
//        {
//            if (theFilter(theEnumerator.get()))
//                { ^ true }
//        };
//        
//        ^ false
//    }
//    
//    reset () => theEnumerator;
//    
//    enumerable() => theEnumerator;
//}
//
//// --- PatternFilter2 ---
//
//public class PatternFilter2 : Enumerator
//{
//    Func2      filter;
//    Enumerator enumerator;
//    object     previous;
//    
//    constructor new(Func2 filter, Enumerator enumerator)
//    {
//        this filter := filter;
//        this enumerator := enumerator;
//    }
//    
//    get() = enumerator.get();
//    
//    bool next()
//    {
//        if (nil == previous)
//        {
//            ifnot (enumerator.next())
//                { ^ false };
//            
//            previous := enumerator.get();
//        };
//        
//        while (enumerator.next())
//        {
//            if (filter(previous,enumerator.get()))
//                { ^ true };
//            
//            previous := enumerator.get()
//        };
//        
//        ^ false
//    }
//    
//    reset()
//    {
//        enumerator.reset();
//        previous := nil;
//    }
//    
//    enumerable() => enumerator;
//}
//
//// --- PatternFilter3 ---
//
//public class PatternFilter3 : Enumerator
//{
//    Func3      filter;
//    Enumerator enumerator;
//    object     previous;
//    object     beforePrev;
//    
//    constructor new(Func3 filter, Enumerator enumerator)
//    {
//        this filter := filter;
//        this enumerator := enumerator;
//        previous := nil;
//    }
//    
//    get() = enumerator.get();
//    
//    reset()
//    {
//        enumerator.reset();
//        previous := nil
//    }
//    
//    bool next()
//    {
//        if (nil == previous)
//        {
//            ifnot (enumerator.next())
//                { ^ false };
//            
//            beforePrev := enumerator.get();
//    
//            ifnot (enumerator.next())
//                { ^ false };
//            
//            previous := enumerator.get()
//        };
//                
//        while (enumerator.next())
//        {
//            if(filter(beforePrev,previous,enumerator.get()))
//                { ^ true };
//            
//            beforePrev := previous;
//            previous := enumerator.get()
//        };
//        
//        ^ false
//    }
//    
//    enumerable() => enumerator;
//}
//
//// --- PatternFilter4 ---
//
//public class PatternFilter4 : Enumerator
//{
//    Func4       filter;
//    Enumerator  enumerator;
//    object      previous;
//    object      beforePrev;
//    object      before2Prev;
//    
//    constructor new(Func4 filter, Enumerator enumerator)
//    {
//        this filter := filter;
//        this enumerator := enumerator;
//        previous := nil;
//    }
//    
//    get() = enumerator.get();
//    
//    reset()
//    {
//        enumerator.reset();
//        previous := nil
//    }
//    
//    bool next()
//    {
//        if(nil == previous)
//        {
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            before2Prev := enumerator.get();
//            
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            beforePrev := enumerator.get();
//    
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            previous := enumerator.get()
//        };
//                
//        while (enumerator.next())
//        {
//            if (filter(before2Prev,beforePrev,previous,enumerator.get()))
//                { ^ true };
//            
//            before2Prev := beforePrev;
//            beforePrev := previous;
//            previous := enumerator.get()
//        };
//        
//        ^ false
//    }
//    
//    enumerable() => enumerator;
//}
//
//// --- PatternFilter5 ---
//
//public class PatternFilter5 : Enumerator
//{
//    Func5      filter;
//    Enumerator enumerator;
//    object     previous;
//    object     beforePrev;
//    object     before2Prev;
//    object     before3Prev;
//    
//    constructor new(Func5 filter, Enumerator enumerator)
//    {
//        this filter := filter;
//        this enumerator := enumerator;
//        previous := nil;
//    }
//    
//    get() = enumerator.get();
//    
//    reset()
//    {
//        enumerator.reset();
//        previous := nil
//    }
//    
//    bool next()
//    {
//        if (nil == previous)
//        {
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            before3Prev := enumerator.get();
//            
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            before2Prev := enumerator.get();
//            
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            beforePrev := enumerator.get();
//    
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            previous := enumerator.get();
//        };
//                
//        while (enumerator.next())
//        {
//            if (filter(before3Prev,before2Prev,beforePrev,previous,enumerator.get()))
//                { ^ true };
//            
//            before3Prev := before2Prev;
//            before2Prev := beforePrev;
//            beforePrev := previous;
//            previous := enumerator.get()
//        };
//        
//        ^ false
//    }
//    
//    enumerable() => enumerator;
//}
//
//// --- PatternFilter6 ---
//
//public class PatternFilter6 : Enumerator
//{
//    Func6      filter;
//    Enumerator enumerator;
//    object     previous;
//    object     beforePrev;
//    object     before2Prev;
//    object     before3Prev;
//    object     before4Prev;
//    
//    constructor new(Func6 filter, Enumerator enumerator)
//    {
//        this filter := filter;
//        this enumerator := enumerator;
//        previous := nil;
//    }
//    
//    get() = enumerator.get();
//    
//    reset()
//    {
//        enumerator.reset();
//        previous := nil
//    }
//    
//    bool next()
//    {
//        if (nil == previous)
//        {
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            before4Prev := enumerator.get();
//            
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            before3Prev := enumerator.get();
//            
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            before2Prev := enumerator.get();
//            
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            beforePrev := enumerator.get();
//    
//            ifnot(enumerator.next())
//                { ^ false };
//            
//            previous := enumerator.get();
//        };
//                                
//        while (enumerator.next())
//        {
//            if (filter(before4Prev,before3Prev,before2Prev,beforePrev,previous,enumerator.get()))
//                { ^ true };
//            
//            before4Prev := before3Prev;
//            before3Prev := before2Prev;
//            before2Prev := beforePrev;
//            beforePrev := previous;
//            previous := enumerator.get()
//        };
//        
//        ^ false
//    }
//    
//    enumerable() => enumerator;
//}
//
// --- Selecting ---

public class Selector : Enumerator
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
        theSelector := (x => extension(x));
        theEnumerator := enumerator
    }
    
    get Value() = theSelector(*theEnumerator);
    
    bool next() = theEnumerator.next();
    
    reset()
    {
        theEnumerator.reset()
    }    
    
    enumerable() => theEnumerator;
}

//// --- Repeater ---
//
//public class Repeater : Enumerator
//{
//    int      min;
//    int      max;
//    int      current;
//    Func1    func;
//
//    constructor new(int minValue, int maxValue, Func1 repetition)
//    {
//        func := repetition;
//        min := minValue;
//        max := maxValue;
//        current := min - 1
//    }
//    
//    constructor new(int minValue, int maxValue, ExtensionMessage extension)
//    {
//        func := (x => extension(x));
//        min := minValue;
//        max := maxValue;
//        current := min - 1;
//    }
//
//    get() = func(current);
//    
//    reset()
//    {
//        current := min - 1
//    }
//    
//    bool next()
//    {
//        current := current + 1;
//        
//        ^ (current < max)
//    }
//    
//    enumerable() = Range.for(min, max);
//}
//
//// --- ZipEnumerator ---
//
//public class ZipEnumerator : Enumerator
//{
//    Enumerator theEnumerator1;
//    Enumerator theEnumerator2;
//    
//    Func2      theZipper;
//    
//    constructor new(Func2 fn, Enumerator enum1, Enumerator enum2)
//    {
//        theZipper := fn;
//        
//        theEnumerator1 := enum1;
//        theEnumerator2 := enum2;
//    }
//    
//    constructor new(ExtensionMessage ext, Enumerator enum1, Enumerator enum2)
//        <= new((n,m => ext(n,m)), enum1, enum2);
//    
//    get() = theZipper(theEnumerator1.get(), theEnumerator2.get());
//    
//    bool next() = theEnumerator1.next() && theEnumerator2.next();
//    
//    reset()
//    {
//        theEnumerator1.reset();
//        theEnumerator2.reset();
//    }
//    
//    enumerable() => theEnumerator1;
//}
//
//// --- RangeEnumerator ---
//
//public class RangeEnumerator : Enumerator
//{
//    object theVariable;
//    object theStart;
//    object theEnd;
//    object theStep;
//    
//    bool   theForward;
//    
//    constructor new(var,start,end,step)
//    {
//        theVariable := var;
//        theStart := start;
//        theEnd := end;
//        theStep := step;
//        
//        theForward := (start < end);
//        
//        self.reset()
//    }
//
//    constructor new(start, end, step)
//        <= new(new Variable(), start, end, step);
//
//    constructor new(start, end)
//        <= new(new Variable(), start, end, 1);
//        
//    reset() 
//    {
//        theVariable.Value := theStart;
//
//        if(theForward)
//        {
//            theVariable.reduce:theStep 
//        }
//        else
//        {
//            theVariable.append:theStep
//        }
//    }    
//    
//    get() = theVariable.Value;
//    
//    bool next()
//    {
//        if(theForward)
//            { theVariable.append:theStep; ^ (theVariable <= theEnd) };
//        
//        theVariable.reduce:theStep; 
//        
//        ^ (theVariable >= theEnd)
//    }
//    
//    enumerable() = theVariable;
//}
//
//// --- IntervalEnumerator ---
//
//public class IntervalEnumerator : Enumerator
//{
//    object variable;
//    object start;
//    object end;
//    object step;
//    
//    bool   forward;
//    
//    constructor new(variable, start, end, step)
//    {
//        this variable := variable;
//        this start := start;
//        this end := end;
//        this step := step;
//        
//        forward := (start < end);
//        
//        self.reset()
//    }
//
//    constructor new(start, end, step)
//        <= new(new Variable(), start, end, step);
//
//    constructor new(start, end)
//        <= new(new Variable(), start, end, 1);
//        
//    reset() 
//    {
//        variable.Value := start;
//
//        if(forward) 
//        {
//            variable.reduce(step)
//        }
//        else variable.append(step)
//    }    
//    
//    get() = variable.Value;
//    
//    bool next()
//    {
//        if(forward )
//            { variable.append(step); ^ (variable < end) };
//        
//        variable.reduce(step); 
//        
//        ^ (variable > end)
//    }
//    
//    enumerable() = variable;
//}
//
//// --- GroupingEnumerator ---
//
//public class GroupingEnumerator : Enumerator
//{
//    Dictionary theDictionary;
//    Enumerator theEnumerator;
//    Func1      theGroupFunc;   
//    Enumerator theGroupEnumerator;
//    
//    constructor new(Func1 grouper, Enumerator enumerator)
//    {
//        theGroupFunc := grouper;
//        theDictionary := Dictionary.new();
//        theEnumerator := enumerator;
//        
//        self.reset()
//    }
//    
//    constructor new(ExtensionMessage fn, Enumerator enumerator)
//        <= new((x => fn(x)), enumerator);
//    
//    reset()
//    {
//        theEnumerator.reset();
//        
//        while (theEnumerator.next())
//        {
//            var item := theEnumerator.get();
//            var key := theGroupFunc(item);
//            
//            var groupItem := theDictionary[key];
//            if (nil == groupItem)
//            {
//                groupItem := new ArrayList();
//                
//                theDictionary[key] := groupItem
//            };
//            
//            groupItem.append:item
//        };
//        
//        theGroupEnumerator := theDictionary.enumerator();
//    }
//    
//    get() = theGroupEnumerator.get();
//    
//    bool next()
//        = theGroupEnumerator.next();
//        
//    enumerable()
//        => theEnumerator;
//}
//
//// --- OrderingEnumerator ---
//
//public class OrderingEnumerator : Enumerator
//{
//    Enumerator theEnumerator;
//    Func2      theOrderFunc;   
//    Enumerator theListEnumerator;
//    
//    constructor new(Func2 orderFunc, Enumerator enumerator)
//    {
//        theOrderFunc := orderFunc;
//        theEnumerator := enumerator;
//        
//        self.reset();
//    }
//    
//    constructor new(Func1 orderVal, Enumerator enumerator)
//    {
//        theOrderFunc := (p,n => orderVal(p) < orderVal(n) );
//        theEnumerator := enumerator;
//        
//        self.reset()
//    }
//    
//    constructor new(ExtensionMessage fn, Enumerator enumerator)
//        <= new((x,y => fn(x, y)), enumerator);
//    
//    reset()
//    {
//        theEnumerator.reset();
//        
//        Array list := new Summing(new ArrayList()).run(theEnumerator);
//        list.sort(theOrderFunc);
//        
//        theListEnumerator := list.enumerator();
//    }
//    
//    get() = theListEnumerator.get();
//    
//    bool next()
//        = theListEnumerator.next();
//        
//    enumerable()
//        => theEnumerator;
//}
//
//// === patterns ===
//
//// --- BasePattern ---
//
//public abstract class BasePattern : Func1
//{
//    object theResult;
//    
//    run(Enumerator en)
//    {
//        super.run(en);
//        
//        ^ theResult
//    }
//}
//
//// --- Summing ---
//
//public class Summing : BasePattern
//{
//    constructor(variable)
//    {
//        theResult := variable
//    }
//
//    function(param) { theResult.append(param) }
//}
//
//// --- Accumulating ---
//
//public class Accumulating : BasePattern
//{
//    Func2 theOperation;
//    
//    constructor new(object variable, Func2 operation)
//    {
//        theResult := variable;
//        theOperation := operation
//    }
//
//    constructor new(object variable, ExtensionMessage operation)
//    {
//        theResult := variable;
//        theOperation := (a,b){ operation(a, b) }
//    }
//
//    function(param) { theResult.Value := theOperation(theResult.Value, param) }
//}
//
//// --- JoinEnumerator ---
//
//public class JoinEnumerator : Enumerator
//{
//    Enumerator left;
//    Enumerator right;
//
//    Func2      filter;
//    Func2      selector;
//
//    constructor new(Func2 filter_f, Func2 select_f, Enumerator left_e, Enumerator right_e)
//    {
//        left := left_e;
//        right := right_e;
//        
//        filter := filter_f;
//        selector := select_f;
//    }
//    
//    reset()
//    {
//        left.reset();
//        right.reset()
//    }
//    
//    bool next()
//    {
//        while(left.next())
//        {
//            right.reset();
//            
//            while (right.next())
//            {
//                if(filter(left.get(), right.get())) 
//                {
//                    ^ true
//                }
//            }
//        };
//        
//        ^ false
//    }
//    
//    get() = selector(left.get(), right.get());
//    
//    enumerable() => left;
//}
//
//// --- patternOp ---
//
//public extension patternOp
//{
//    skipping(int counter)
//        = SkipFilter.new(counter, cast Enumerator(self.enumerator()));
//
//    last(int counter)
//        = TopFilter.new(counter, self.Length - counter, cast Enumerator(self.enumerator()));
//}
