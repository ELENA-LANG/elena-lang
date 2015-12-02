// ==== ver 3.4.1 ===

#import system.
#import system'routines.
#import extensions.

#subject(class:BaseStrategy)$strategy.

// --- Strategy ---

#class(limited) BaseStrategy
{
    #method $strategy = $self.
    
    #method getAt : Indexers &int:aLength = $nil.
    
    #method(type:$strategy) eval : Indexers &int:aLength = $self.
    
    #method(type:bool) result = false.
}

// --- Combination strategies ---

#symbol(type:$strategy) combinationEnd = BaseStrategy
{
    result = false.
}.

#symbol(type:$strategy) sequentialNext = BaseStrategy
{
    result = true.
    
    #method(suppress:w2) eval : Indexers &int:aLength
    [
        ($nil == (Indexers seek &each: anIndexer 
                [
                    (anIndexer append &index:1 available)
                        ! [ anIndexer writeIndex &int:0. ^ false. ].
                        
                    ^ true.
                ]))
            ? [ ^ combinationEnd. ].
            
        ^ $self.            
    ]
        
    getAt : Indexers &int:aLength
    [
        ^ Array new &int:aLength set &nfunc: (&int:i) [ Indexers getAt &int:i get ].
    ]
}.

#symbol(type:$strategy) sequentialStart = BaseStrategy
{
    eval : Indexers &int:aLength
    [
        Indexers run &each: anIndexer [ anIndexer writeIndex &int:0. ].
        
        ^ sequentialNext.
    ]
}.

// --- Parallel strategies

#symbol(type:$strategy) parallelNext = BaseStrategy
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
        ^ Array new &int:aLength set &nfunc: (&int:i) [ Indexers getAt &int:i get ].
    ]
}.

#symbol(type:$strategy) parallelStart = BaseStrategy
{
    eval : Indexers &int:aLength
    [
        Indexers run &each: anIndexer [ anIndexer writeIndex &int:0. ].
        
        ^ parallelNext.
    ]
}.

// --- combination strategy ---

#symbol(type:$strategy)  combinationNext = BaseStrategy
{
    result = true.

    getAt : Indexers &int:aLength
    [
        ^ Array new &int:aLength set &nfunc: (&int:i) [ Indexers getAt &int:i get ].
    ]
        
    eval : Indexers &int:aLength
    [
        #var(type:array)anArray := Indexers array.
        #var(type:int)k.
        anArray readLength &vint:k.
        #var(type:int)i := k - 1.
        
        #var aCurrentIndexer := anArray getAt &int:i.
        
        (aCurrentIndexer append &index:1 available)
            ? [ ^ $self. ].
             
        #loop ((i > 0) and:[ anArray getAt &int:(i - 1) index int + k - i + 1 >= aLength ]) ?
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

#symbol(type:$strategy)  combinationStart = BaseStrategy
{
    eval : Indexers &int:n
    [
        #var(type:array)anArray := Indexers array.
        #var(type:int)aLength := anArray length.
        
        0::intLoopOp to &int:(aLength - 1) &nfunc: (&int:i) [ anArray getAt &int:i writeIndex &int:i. ].
        
        ^ combinationNext.
    ]
}.

// --- BaseRowEnumerator ---

#class BaseRowEnumerator :: Enumerator
{
    #field(type:array) theIndexers.
    #field(type:int)theLength.
    
    #field(type:$strategy) theStrategy.

    #constructor new &args:Arguments
    [
        theLength := Arguments length int.
        theIndexers := Array new &int:theLength.
        
        #var(type:int)i := 0.
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
        
        #var(type:int)i := 0.
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
    #method reset
    [
        theStrategy := sequentialStart.
    ]
}

// --- ParallelEnumerator ---

#class ParallelEnumerator :: BaseRowEnumerator
{
    #method reset
    [
        theStrategy := parallelStart.
    ]
}

// --- Combinator ---

#class Combinator :: BaseRowEnumerator
{
    #field(type:int) theTotalLength.
    
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
