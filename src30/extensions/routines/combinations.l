#define system.
#define system'routines.
#define extensions.

#type(class:BaseStrategy)$strategy.

// --- Strategy ---

#class(limited) BaseStrategy
{
    #method $strategy = $self.
    
    #method getAt &indexers:Indexers &length:aLength = nil.
    
    #method(type:$strategy) eval &indexers:Indexers = $self.
    
    #method(type:bool) result = false.
}

// --- Combination strategies ---

#symbol(type:$strategy) combinationEnd = BaseStrategy
{
    result = false.
}.

#symbol(type:$strategy) combinationNext = BaseStrategy
{
    result = true.
    
    eval &indexers:Indexers
    [
        (nil equal:(control run:Indexers &seeking: anIndexer 
                [
                    anIndexer append &index:1 available
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

#symbol(type:$strategy) combinationStart = BaseStrategy
{
    eval &indexers:Indexers
    [
        control run:Indexers &forEach: anIndexer [ anIndexer write &index:0. ].
        
        ^ combinationNext.
    ]
}.

// --- Parallel strategies

#symbol(type:$strategy) parallelNext = BaseStrategy
{
    result = true.
    
    eval &indexers:Indexers
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
    eval &indexers:Indexers
    [
        control run:Indexers &forEach: anIndexer [ anIndexer write &index:0. ].
        
        ^ parallelNext.
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
        theStrategy := theStrategy eval &indexers:theIndexers.

        ^ theStrategy result.
    ]
    
    #method get
        = theStrategy getAt &indexers:theIndexers &length:theLength.
}

// --- Combinator ---

#class Combinator :: BaseRowEnumerator
{
    #method reset
    [
        theStrategy := combinationStart.
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

//// --- Combinator ---
//
//#class Combinator : CombinatorWithRepetition
//{
//    #method reset
//    [
//        $super reset.
//        
//        $self reset:1.
//    ]
//
//    #method reset : anIndex
//    [
//        #var aPrevious := Integer new:(theIndexers@anIndex index).
//        
//        #var retVal := control forrange &int::anIndex &int:(theCount-1) &do: (&int:i)
//        [
//            theIndexers@i set &index:(aPrevious append &int:1 int) available
//                ! [ #break self. ].
//        ].
//        
//        ^ nil == retVal.
//    ]
//    
//    #method findNext
//    [
//        #var aCurrentIndexer := theIndexers @ (theCount - 1).
//        
//        ^ aCurrentIndexer next 
//            ? [ true ]
//            ! [
//                #var aPrevious := Integer new:theCount.
//                
//                control do:[ aPrevious -= 1. ] &until:[ (aPrevious < 0)or:[ self reset:aPrevious ] ].
//                
//                ^ (0 <= aPrevious).
//            ].
//    ]
//}
