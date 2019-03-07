import system'routines;
//import extensions.

// --- Strategy ---

interface BaseStrategy
{
    abstract at(Indexer[] indexers, int length) {}
    
    abstract BaseStrategy eval(Indexer[] indexers, int length) {}
    
    abstract get bool Result() {}
}

// --- Combination strategies ---

singleton combinationEnd : BaseStrategy
{
    at(Indexer[] indexers, int length) = nil;
    
    BaseStrategy eval(Indexer[] indexers, int length) = nil;
    
    bool Result = false;
}

singleton sequentialNext : BaseStrategy
{
    bool Result = true;
    
    BaseStrategy eval(Indexer[] indexers, int length)
    {
        if(nil == (indexers.seekEach:(indexer)
                {
                    auto it := cast Indexer(indexer); 
                    
                    ifnot(it.appendIndex(1).Available)
                        { it.Index := 0; ^ false };
                        
                    ^ true
                }))
            { ^ combinationEnd };
            
        ^ self          
    }
        
    at(Indexer[] indexers, int length)
    {
        ^ Array.allocate(length).populate:(int i => indexers.at(i).get())
    }
}

singleton sequentialStart : BaseStrategy
{
    bool Result = false;
    
    at(Indexer[] indexers, int length) = nil;
    
    BaseStrategy eval(Indexer[] indexers, int length)
    {
        indexers.forEach:(indexer){ indexer.Index := 0 };
        
        ^ sequentialNext
    }
}

// --- Parallel strategies

singleton parallelNext : BaseStrategy
{
    bool Result = true;
    
    BaseStrategy eval(Indexer[] indexers, int length)
    {
        if(nil != (indexers.seekEach:(indexer)
                {
                    auto it := cast Indexer(indexer); 
                    
                    ^ it.appendIndex(1).Available.Inverted
                }))
            { ^ combinationEnd };
            
        ^ self          
    }
        
    at(Indexer[] indexers, int length)
    {
        ^ Array.allocate(length).populate:(int i => indexers.at(i).get())
    }
}

singleton parallelStart : BaseStrategy
{
    bool Result = false;
    
    at(Indexer[] indexers, int length) = nil;
    
    BaseStrategy eval(Indexer[] indexers, int length)
    {
        indexers.forEach:(indexer){ indexer.Index := 0 };
        
        ^ parallelNext
    }
}

//// --- combination strategy ---
//
//class combinationNext = BaseStrategy::
//{
//    bool result = true.
//
//    getAt(Array Indexers, int aLength)
//    [
//        ^ Array new(aLength); populate(:i)<int>(T<Array>(Indexers) getAt(i); get)
//    ]
//        
//    T<BaseStrategy> eval(Array Indexers, int aLength)
//    [
//        T<Array> anArray := T<Array>(Indexers).
//        int      k := anArray length.
//        int      i := k - 1.
//        
//        T<Indexer> aCurrentIndexer := anArray getAt(i).
//        
//        if(aCurrentIndexer appendIndex:1; available)
//            [ ^ self ].
//             
//        while ((i > 0) && (T<int>(anArray getAt(i - 1); index) + k - i + 1 >= aLength))
//            [ i := i - 1 ].
//        
//        if(i == 0)
//            [ ^ combinationEnd ].
//        
//        anArray getAt(i - 1); appendIndex:1.
//        
//        //i to &int:(k - 1)
//        IntNumber new(i); runIntLoop(k - 1,
//            (:j)<int>[ anArray getAt(j); writeIndex(IntNumber new(anArray getAt(j - 1); index + 1)) ]).
//            
//        ^ self          
//    ]
//}.
//
//class combinationStart = BaseStrategy::
//{
//    bool result = false.
//
//    getAt(Array Indexers, int aLength) = nil.
//
//    T<BaseStrategy> eval(Array Indexers, int n)
//    [
//        auto anArray := T<Array>(Indexers).
//        int aLength := anArray length.
//        
//        0~intLoopOp to(aLength - 1) do(:i)<int>[ anArray getAt(i); writeIndex(i) ].
//        
//        ^ combinationNext
//    ]
//}.

// --- BaseRowEnumerator ---

public abstract class BaseRowEnumerator : Enumerator
{
    Indexer[]    theIndexers;
    int          theLength;  
    BaseStrategy theStrategy;

//    generic constructor new(V<object> Arguments)
//    [
//        theLength := Arguments length.
//        theIndexers := Array new(theLength).
//        
//        int i := 0.
//        while (i < theLength)
//        [
//            theIndexers setAt(i, Arguments[i] indexer).
//            
//            i := i + 1
//        ].
//        
//        self reset.
//    ]
    
    bool next()
    {
        theStrategy := theStrategy.eval(theIndexers, theLength);

        ^ theStrategy.Result
    }
    
    get()
        = theStrategy.at(theIndexers, theLength);
        
    enumerable() = theIndexers;
}

// --- SequentialEnumerator ---

public class SequentialEnumerator : BaseRowEnumerator
{
    constructor new(object[] arguments)
    {
        theLength := arguments.Length;
        theIndexers := new Indexer[](theLength);
        
        for (int i := 0, i < theLength, i += 1)
        {
            Indexer it := arguments[i].indexer();
            
            theIndexers[i] := /*arguments[i].indexer()*/it
        };
        
        self.reset()
    }
        
    constructor new(params object[] arguments)
        <= new(cast object[](arguments));

    reset()
    {
        theStrategy := sequentialStart;
    }
}

// --- ParallelEnumerator ---

public class ParallelEnumerator : BaseRowEnumerator
{
    constructor new(object[] arguments)
    {
        theLength := arguments.Length;
        theIndexers := new Indexer[](theLength);
        
        for (int i := 0, i < theLength, i += 1)
        {
            Indexer it := arguments[i].indexer();
            
            theIndexers[i] := /*arguments[i].indexer()*/it
        };
        
        self.reset()        
    }
        
    constructor new(Array a)
        <= new(a.Value);    
        
    reset()
    {
        theStrategy := parallelStart
    }
}

//// --- Combinator ---
//
//public class Combinator :: BaseRowEnumerator
//{
//    int theTotalLength.
//    
//    constructor new : Arguments
//    [
//        theLength := Arguments length.
//        theIndexers := Array new(theLength).
//        
//        int i := 0.
//        while (i < theLength)
//        [
//            theIndexers setAt(i, Arguments[i] indexer).
//            
//            i := i + 1
//        ].
//        
//        self reset
//    ]
//        
//    constructor new : aSubLength of:aSet
//        <= new : (Array new(T<int>(aSubLength)); populate(:n)<int>(aSet));
//    [
//        theTotalLength := aSet length.
//    ]
//    
//    bool next
//    [
//        theStrategy := theStrategy eval(theIndexers, theTotalLength).
//
//        ^ theStrategy result
//    ]
//    
//    reset
//    [
//        theStrategy := combinationStart
//    ]
//}
