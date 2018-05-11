import system'routines.
import extensions.

// --- Strategy ---

interface class BaseStrategy
{
    abstract getAt(Array Indexers, int aLength) []
    
    abstract T<BaseStrategy> eval(Array Indexers, int aLength) []
    
    abstract bool result []
}

// --- Combination strategies ---

class combinationEnd = BaseStrategy::
{
    getAt(Array Indexers, int aLength) = nil.
    
    T<BaseStrategy> eval(Array Indexers, int aLength) = nil.
    
    bool result = false.
}.

class sequentialNext = BaseStrategy::
{
    bool result = true.
    
    T<BaseStrategy> eval(Array Indexers, int aLength)
    [
        if(nil == (Indexers seekEach(:anIndexer)
                [
                    T<Indexer> it := anIndexer indexer. 
                    
                    ifnot(it appendIndex:1; available)
                        [ it writeIndex(0). ^ false ].
                        
                    ^ true
                ]))
            [ ^ combinationEnd ].
            
        ^ self          
    ]
        
    getAt(Array Indexers, int aLength)
    [
        ^ Array new(aLength); populate(:i)<int,int>(Indexers getAt(i); get)
    ]
}.

class sequentialStart = BaseStrategy::
{
    bool result = false.
    
    getAt(Array Indexers, int aLength) = nil.
    
    T<BaseStrategy> eval(Array Indexers, int aLength)
    [
        Indexers forEach(:anIndexer) [ anIndexer writeIndex(0) ].
        
        ^ sequentialNext
    ]
}.

// --- Parallel strategies

class parallelNext = BaseStrategy::
{
    bool result = true.
    
    T<BaseStrategy> eval(Array Indexers, int aLength)
    [
        if(nil != Indexers seekEach(:anIndexer)(anIndexer appendIndex:1; available; inverted ))
            [ ^ combinationEnd ].
            
        ^ self       
    ]
        
    getAt(Array Indexers, int aLength)
    [
        ^ Array new(aLength); populate(:i)<int,int>(T<Array>(Indexers) getAt(i); get ).
    ]
}.

class parallelStart = BaseStrategy::
{
    bool result = false.
    
    getAt(Array Indexers, int aLength) = nil.
    
    T<BaseStrategy> eval(Array Indexers, int aLength)
    [
        Indexers forEach(:anIndexer)[ anIndexer writeIndex(0) ].
        
        ^ parallelNext
    ]
}.

// --- combination strategy ---

class combinationNext = BaseStrategy::
{
    bool result = true.

    getAt(Array Indexers, int aLength)
    [
        ^ Array new(aLength); populate(:i)<int,int>(T<Array>(Indexers) getAt(i); get)
    ]
        
    T<BaseStrategy> eval(Array Indexers, int aLength)
    [
        T<Array> anArray := T<Array>(Indexers).
        int      k := anArray length.
        int      i := k - 1.
        
        T<Indexer> aCurrentIndexer := anArray getAt(i); indexer.
        
        if(aCurrentIndexer appendIndex:1; available)
            [ ^ self ].
             
        while ((i > 0) && (T<int>(anArray getAt(i - 1); indexer; index) + k - i + 1 >= aLength))
            [ i := i - 1 ].
        
        if(i == 0)
            [ ^ combinationEnd ].
        
        anArray getAt(i - 1); appendIndex:1.
        
        //i to &int:(k - 1)
        IntNumber new(i); runIntLoop(k - 1,
            (:j)<int>[ anArray getAt(j); writeIndex(IntNumber new(anArray getAt(j - 1); index + 1)) ]).
            
        ^ self          
    ]
}.

class combinationStart = BaseStrategy::
{
    bool result = false.

    getAt(Array Indexers, int aLength) = nil.

    T<BaseStrategy> eval(Array Indexers, int n)
    [
        auto anArray := T<Array>(Indexers).
        int aLength := anArray length.
        
        0~intLoopOp to(aLength - 1) do(:i)<int>[ anArray getAt(i); indexer; writeIndex(i) ].
        
        ^ combinationNext
    ]
}.

// --- BaseRowEnumerator ---

public abstract class BaseRowEnumerator :: Enumerator
{
    T<Array>        theIndexers.
    int             theLength.    
    T<BaseStrategy> theStrategy.

    constructor new(V<object> Arguments)
    [
        theLength := Arguments length.
        theIndexers := Array new(theLength).
        
        int i := 0.
        while (i < theLength)
        [
            theIndexers setAt(i, Arguments[i] indexer).
            
            i := i + 1
        ].
        
        self reset.
    ]
    
    constructor new : Arguments
    [
        theLength := Arguments length.
        theIndexers := Array new(theLength).
        
        int i := 0.
        while (i < theLength)
        [
            theIndexers setAt(i, Arguments[i] indexer).
            
            i := i + 1
        ].
        
        self reset
    ]
    
    bool next
    [
        theStrategy := theStrategy eval(theIndexers, theLength).

        ^ theStrategy result
    ]
    
    get
        = theStrategy getAt(theIndexers, theLength).
        
    enumerable = theIndexers.
}

// --- SequentialEnumerator ---

public class SequentialEnumerator :: BaseRowEnumerator
{
    constructor new : Arguments
        <= new:Arguments.
    
    constructor new(V<object> Arguments)
        <= new(Arguments).

    reset
    [
        theStrategy := sequentialStart.
    ]
}

// --- ParallelEnumerator ---

public class ParallelEnumerator :: BaseRowEnumerator
{
    constructor new : Arguments
        <= new:Arguments.
    
    reset
    [
        theStrategy := parallelStart.
    ]
}

// --- Combinator ---

public class Combinator :: BaseRowEnumerator
{
    int theTotalLength.
    
    constructor new : aSubLength of:aSet
        <= new : (Array new(aSubLength int); populate(:n)<int,int>(aSet));
    [
        theTotalLength := aSet length.
    ]
    
    bool next
    [
        theStrategy := theStrategy eval(theIndexers, theTotalLength).

        ^ theStrategy result
    ]
    
    reset
    [
        theStrategy := combinationStart
    ]
}
