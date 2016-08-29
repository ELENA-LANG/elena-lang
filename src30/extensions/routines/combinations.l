#import system.
#import system'routines.
#import extensions.

#subject $strategy = BaseStrategy.

// --- Strategy ---

#class(limited) BaseStrategy
{
    #method getAt : Indexers &int:aLength = $nil.
    
    #method($strategy) eval : Indexers &int:aLength = $self.
    
    #method(bool) result = false.
}

// --- Combination strategies ---

#symbol($strategy) combinationEnd = BaseStrategy
{
    result = false.
}.

#symbol($strategy) sequentialNext = BaseStrategy
{
    result = true.
    
    #method(w2) eval : Indexers &int:aLength
    [
        ($nil == (Indexers seek &each: anIndexer 
                [
                    #var(iterator) it := anIndexer iterator. 
                    
                    (it append &index:1 available)
                        ! [ it writeIndex &int:0. ^ false. ].
                        
                    ^ true.
                ]))
            ? [ ^ combinationEnd. ].
            
        ^ $self.            
    ]
        
    getAt : Indexers &int:aLength
    [
        ^ Array new &int:aLength set &nfunc: (&int:i) [ Indexers array getAt &int:i get ].
    ]
}.

#symbol($strategy) sequentialStart = BaseStrategy
{
    eval : Indexers &int:aLength
    [
        Indexers run &each: anIndexer [ anIndexer writeIndex &int:0. ].
        
        ^ sequentialNext.
    ]
}.

// --- Parallel strategies

#symbol($strategy) parallelNext = BaseStrategy
{
    result = true.
    
    eval : Indexers &int:aLength
    [
        ($nil != Indexers seek &each: anIndexer [ anIndexer append &index:1 available not ])
            ? [ ^ combinationEnd. ].
            
        ^ $self.            
    ]
        
    getAt : Indexers &int:aLength
    [
        ^ Array new &int:aLength set &nfunc: (&int:i) [ Indexers array getAt &int:i get ].
    ]
}.

#symbol($strategy) parallelStart = BaseStrategy
{
    eval : Indexers &int:aLength
    [
        Indexers run &each: anIndexer [ anIndexer writeIndex &int:0. ].
        
        ^ parallelNext.
    ]
}.

// --- combination strategy ---

#symbol($strategy)  combinationNext = BaseStrategy
{
    result = true.

    getAt : Indexers &int:aLength
    [
        ^ Array new &int:aLength set &nfunc: (&int:i) [ Indexers array getAt &int:i get ].
    ]
        
    #method(w2) eval : Indexers &int:aLength
    [
        #var(array)anArray := Indexers array.
        #var(int)k := anArray length.
        #var(int)i := k - 1.
        
        #var(iterator) aCurrentIndexer := anArray getAt &int:i iterator.
        
        (aCurrentIndexer append &index:1 available)
            ? [ ^ $self. ].
             
        #loop ((i > 0) and:[ anArray getAt &int:(i - 1) iterator index int + k - i + 1 >= aLength ]) ?
            [ i := i - 1. ].
        
        (i == 0)
            ? [ ^ combinationEnd. ].
        
        anArray getAt &int:(i - 1) append &index:1.
        
        //i to &int:(k - 1)
        IntNumber new &int:i to &int:(k - 1)
            &nfunc: (&int:j) [ anArray getAt &int:j writeIndex &int:(IntNumber new &int:(anArray getAt &int:(j - 1) index int + 1)). ].
            
        ^ $self.            
    ]
}.

#symbol($strategy)  combinationStart = BaseStrategy
{
    eval : Indexers &int:n
    [
        #var(array)anArray := Indexers array.
        #var(int)aLength := anArray length.
        
        0::intLoopOp to &int:(aLength - 1) &nfunc: (&int:i) [ anArray getAt &int:i iterator writeIndex &int:i. ].
        
        ^ combinationNext.
    ]
}.

// --- BaseRowEnumerator ---

#class BaseRowEnumerator :: Enumerator
{
    #field(array) theIndexers.
    #field(int)theLength.
    
    #field($strategy) theStrategy.

    #constructor new &args:Arguments
    [
        theLength := Arguments length int.
        theIndexers := Array new &int:theLength.
        
        #var(int)i := 0.
        #loop (i < theLength) ?
        [
            theIndexers setAt &int:i &object:((Arguments @ i) iterator).
            
            i := i + 1.
        ].
        
        $self reset.
    ]
    
    #constructor new : Arguments
    [
        theLength := Arguments length int.
        theIndexers := Array new &int:theLength.
        
        #var(int)i := 0.
        #loop (i < theLength) ?
        [
            theIndexers setAt &int:i &object:((Arguments @ (IntNumber new &int:i)) iterator).
            
            i := i + 1.
        ].
        
        $self reset.
    ]
    
    #method next
    [
        theStrategy := theStrategy eval : theIndexers &int:theLength.

        ^ theStrategy result.
    ]
    
    #method get
        = theStrategy getAt : theIndexers &int:theLength.
}

// --- SequentialEnumerator ---

#class SequentialEnumerator :: BaseRowEnumerator
{
    #constructor new : Arguments
        <= new:Arguments.
    
    #constructor new &args:Arguments    
        <= new &args:Arguments.

    #method reset
    [
        theStrategy := sequentialStart.
    ]
}

// --- ParallelEnumerator ---

#class ParallelEnumerator :: BaseRowEnumerator
{
    #constructor new : Arguments
        <= new:Arguments.
    
    #method reset
    [
        theStrategy := parallelStart.
    ]
}

// --- Combinator ---

#class Combinator :: BaseRowEnumerator
{
    #field(int) theTotalLength.
    
    #constructor new : aSubLength &of:aSet
        <= (%new : (Array new &int:(aSubLength int) set &nfunc: (&int:n) [ ^ aSet. ]))
    [
        theTotalLength := aSet length int.
    ]
    
    #method next
    [
        theStrategy := theStrategy eval:theIndexers &int:theTotalLength.

        ^ theStrategy result.
    ]
    
    #method reset
    [
        theStrategy := combinationStart.
    ]
}
