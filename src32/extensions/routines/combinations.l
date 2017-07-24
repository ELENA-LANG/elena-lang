import system'routines.
import extensions.

type $strategy :: BaseStrategy.

// --- Strategy ---

limited class BaseStrategy
{
    getAt : Indexers int:aLength = $nil.
    
    $strategy eval : Indexers int:aLength = $self.
    
    bool result = false.
}

// --- Combination strategies ---

class combinationEnd = BaseStrategy::
{
    result = false.
}.

class sequentialNext = BaseStrategy::
{
    result = true.
    
    eval : Indexers int:aLength
    [
        if($nil == (Indexers seek each: (:anIndexer)
                [
                    indexer it := anIndexer indexer. 
                    
                    ifnot(it append index:1; available)
                        [ it writeIndex int:0. ^ false ].
                        
                    ^ true
                ]))
            [ ^ combinationEnd ].
            
        ^ $self          
    ]
        
    getAt : Indexers int:aLength
    [
        ^ Array new int:aLength; set nfunc: (&int:i) [ ^Indexers array; getAt int:i; get ]
    ]
}.

class sequentialStart = BaseStrategy::
{
    eval : Indexers int:aLength
    [
        Indexers run each: (:anIndexer) [ anIndexer writeIndex int:0 ].
        
        ^ sequentialNext
    ]
}.

// --- Parallel strategies

class parallelNext = BaseStrategy::
{
    result = true.
    
    eval : Indexers int:aLength
    [
        if($nil != Indexers seek each: (:anIndexer)[ ^anIndexer append index:1; available; not ])
            [ ^ combinationEnd ].
            
        ^ $self       
    ]
        
    getAt : Indexers int:aLength
    [
        ^ Array new int:aLength; set nfunc: (&int:i) [ ^Indexers array; getAt int:i; get ].
    ]
}.

class parallelStart = BaseStrategy::
{
    eval : Indexers int:aLength
    [
        Indexers run each: (:anIndexer)[ anIndexer writeIndex int:0 ].
        
        ^ parallelNext
    ]
}.

// --- combination strategy ---

class combinationNext = BaseStrategy::
{
    result = true.

    getAt : Indexers int:aLength
    [
        ^ Array new int:aLength; set nfunc: (&int:i) [ ^Indexers array; getAt int:i; get ]
    ]
        
    eval : Indexers int:aLength
    [
        array anArray := Indexers array.
        int   k := anArray length.
        int   i := k - 1.
        
        indexer aCurrentIndexer := anArray getAt int:i; indexer.
        
        if(aCurrentIndexer append index:1; available)
            [ ^ $self ].
             
        while ((i > 0) and:$(anArray getAt int:(i - 1); indexer; index; int + k - i + 1 >= aLength))
            [ i := i - 1 ].
        
        if(i == 0)
            [ ^ combinationEnd ].
        
        anArray getAt int:(i - 1); append index:1.
        
        //i to &int:(k - 1)
        IntNumber new int:i; to int:(k - 1)
            nfunc: (&int:j) [ anArray getAt int:j; writeIndex int:(IntNumber new int:(anArray getAt int:(j - 1); index; int + 1)) ].
            
        ^ $self          
    ]
}.

class combinationStart = BaseStrategy::
{
    eval : Indexers int:n
    [
        array anArray := Indexers array.
        int aLength := anArray length.
        
        0~intLoopOp to int:(aLength - 1) nfunc: (&int:i) [ anArray getAt int:i; indexer; writeIndex int:i ].
        
        ^ combinationNext
    ]
}.

// --- BaseRowEnumerator ---

class BaseRowEnumerator :: Enumerator
{
    array theIndexers.
    int   theLength.
    
    $strategy theStrategy.

    constructor new args:Arguments
    [
        theLength := Arguments length.
        theIndexers := Array new int:theLength.
        
        int i := 0.
        while (i < theLength)
        [
            theIndexers setAt int:i object:((Arguments @ i) indexer).
            
            i := i + 1
        ].
        
        $self reset.
    ]
    
    constructor new : Arguments
    [
        theLength := Arguments length.
        theIndexers := Array new int:theLength.
        
        int i := 0.
        while (i < theLength)
        [
            theIndexers setAt int:i object:((Arguments @ (IntNumber new int:i)) indexer).
            
            i := i + 1
        ].
        
        $self reset
    ]
    
    next
    [
        theStrategy := theStrategy eval : theIndexers int:theLength.

        ^ theStrategy result
    ]
    
    get
        = theStrategy getAt : theIndexers int:theLength.
}

// --- SequentialEnumerator ---

class SequentialEnumerator :: BaseRowEnumerator
{
    constructor new : Arguments
        <= new:Arguments.
    
    constructor new args:Arguments    
        <= new args:Arguments.

    reset
    [
        theStrategy := sequentialStart.
    ]
}

// --- ParallelEnumerator ---

class ParallelEnumerator :: BaseRowEnumerator
{
    constructor new : Arguments
        <= new:Arguments.
    
    reset
    [
        theStrategy := parallelStart.
    ]
}

// --- Combinator ---

class Combinator :: BaseRowEnumerator
{
    int theTotalLength.
    
    constructor new : aSubLength of:aSet
        <= new : (Array new int:(aSubLength int); set nfunc: (&int:n) [ ^ aSet ]);
    [
        theTotalLength := aSet length.
    ]
    
    next
    [
        theStrategy := theStrategy eval:theIndexers int:theTotalLength.

        ^ theStrategy result
    ]
    
    reset
    [
        theStrategy := combinationStart
    ]
}
