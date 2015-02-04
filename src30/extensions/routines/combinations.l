#define system.
#define system'routines.
#define extensions.

#type(class:BaseStrategy)$strategy.

// --- Strategy ---

#class(limited) BaseStrategy
{
    #method $strategy = $self.
    
    #method getAt &indexers:Indexers &length:aLength = nil.
    
    #method(type:$strategy) eval &indexers:Indexers &length:aLength = $self.
    
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
    
    eval &indexers:Indexers &length:aLength
    [
        (nil equal:(control run:Indexers &seeking: anIndexer 
                [
                    (anIndexer append &index:1 available)
                        ! [ anIndexer write &index:0. ^ false. ].
                        
                    ^ true.
                ]))
            ? [ ^ combinationEnd. ].
            
        ^ $self.            
    ]
        
    getAt &indexers:Indexers &length:aLength
    [
        ^ Array new &length:aLength set &every: (&index:i) [ Indexers getAt &index:i get ].
    ]
}.

#symbol(type:$strategy) sequentialStart = BaseStrategy
{
    eval &indexers:Indexers &length:aLength
    [
        control run:Indexers &forEach: anIndexer [ anIndexer write &index:0. ].
        
        ^ sequentialNext.
    ]
}.

// --- Parallel strategies

#symbol(type:$strategy) parallelNext = BaseStrategy
{
    result = true.
    
    eval &indexers:Indexers &length:aLength
    [
        (nil != control run:Indexers &seeking: anIndexer [ anIndexer append &index:1 available not ])
            ? [ ^ combinationEnd. ].
            
        ^ $self.            
    ]
        
    getAt &indexers:Indexers &length:aLength
    [
        ^ Array new &length:aLength set &every: (&index:i) [ Indexers getAt &index:i get ].
    ]
}.

#symbol(type:$strategy) parallelStart = BaseStrategy
{
    eval &indexers:Indexers &length:aLength
    [
        control run:Indexers &forEach: anIndexer [ anIndexer write &index:0. ].
        
        ^ parallelNext.
    ]
}.

// --- combination strategy ---

#symbol(type:$strategy)  combinationNext = BaseStrategy
{
    result = true.

    getAt &indexers:Indexers &length:aLength
    [
        ^ Array new &length:aLength set &every: (&index:i) [ Indexers getAt &index:i get ].
    ]
        
    eval &indexers:Indexers &length:aLength
    [
        #var(type:array)anArray := Indexers array.
        #var(type:length)k := anArray length.
        #var(type:length)i := k - 1.
        
        #var aCurrentIndexer := anArray getAt &index:i.
        
        (aCurrentIndexer append &index:1 available)
            ? [ ^ $self. ].
             
        #loop ((i > 0) and:[ anArray getAt &index:(i - 1) index + k - i + 1 >= aLength ]) ?
            [ i := i - 1. ].
        
        (i == 0)
            ? [ ^ combinationEnd. ].
        
        anArray getAt &index:(i - 1) append &index:1.
        
        control run &int:i &int:(k - 1) 
            &forEvery: (&index:j) [ anArray getAt &index:j write &index:(anArray getAt &index:(j - 1) index + 1). ].
            
        ^ $self.            
    ]
}.

#symbol(type:$strategy)  combinationStart = BaseStrategy
{
    eval &indexers:Indexers &length:n
    [
        #var(type:array)anArray := Indexers array.
        #var(type:length)aLength := anArray length.
        
        control run &int:0 &int:(aLength - 1) &forEvery: (&index:i) [ anArray getAt &index:i write &index:i. ].
        
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
        theLength := Arguments length.
        theIndexers := Array new &length:theLength.
        
        #var(type:int)i := 0.
        #loop (i < theLength) ?
        [
            theIndexers setAt &index:i &object:((Arguments @ i) iterator).
            
            i := i + 1.
        ].
        
        $self reset.
    ]
    
    #constructor new : Arguments
    [
        theLength := Arguments length.
        theIndexers := Array new &length:theLength.
        
        #var(type:int)i := 0.
        #loop (i < theLength) ?
        [
            theIndexers setAt &index:i &object:((Arguments @ i) iterator).
            
            i := i + 1.
        ].
        
        $self reset.
    ]
    
    #method next
    [
        theStrategy := theStrategy eval &indexers:theIndexers &length:theLength.

        ^ theStrategy result.
    ]
    
    #method get
        = theStrategy getAt &indexers:theIndexers &length:theLength.
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
    
    #constructor new &k:aSubLength &of:aSet
        <= (%new : (Array new &length:(aSubLength int) set &every: (&index:n) [ ^ aSet. ]))
    [
        theTotalLength := aSet length.
    ]
    
    #method next
    [
        theStrategy := theStrategy eval &indexers:theIndexers &length:theTotalLength.

        ^ theStrategy result.
    ]
    
    #method reset
    [
        theStrategy := combinationStart.
    ]
}
