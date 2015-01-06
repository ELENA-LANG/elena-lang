#define system.
#define system'routines.
#define extensions.

// --- Combinator ---

#type(class:CombinatorStrategy)$strategy.

#class(limited) CombinatorStrategy
{
    #method $strategy = $self.
    
    #method getAt &indexers:Indexers &length:aLength = nil.
    
    #method(type:bool) eval &indexers:Indexers = false.
}

#symbol(type:$strategy) combinationStart = CombinatorStrategy
{
    eval &indexers:Indexers
    [
        control run:Indexers &forEach: anIndexer [ anIndexer write &index:0. ].
        
        ^ true.
    ]
}.

#symbol(type:$strategy) combinationNext = CombinatorStrategy
{
    eval &indexers:Indexers
        = nil != control run:Indexers &seeking: anIndexer 
        [
            anIndexer append &index:1 available
                ! [ anIndexer write &index:0. ^ false. ].
                
            ^ true.
        ].
        
    getAt &indexers:Indexers &length:aLength
    [
        ^ Array new &length:aLength set &every: (&index:i) [ Indexers getAt &index:i get ].
    ]
}.

#symbol(type:$strategy) combinationEnd = CombinatorStrategy
{
    eval &indexers:Indexers = false.
}.

#class Combinator :: Enumerator
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
        
        theStrategy := combinationStart.
    ]
    
    #method reset
    [
        theStrategy := combinationStart.
    ]
    
    #method next
    [
        #var(type:bool) aRetVal := theStrategy eval &indexers:theIndexers.
        
        aRetVal ? [ theStrategy := combinationNext. ]
                ! [ theStrategy := combinationEnd. ].

        ^ aRetVal.
    ]
    
    #method get
        = theStrategy getAt &indexers:theIndexers &length:theLength.
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
//
//// --- ParallelEnumeration ---
//
//#class ParallelEnumerator : CombinatorWithRepetition
//{
//    #method findNext
//        = nil == control foreach:theIndexers &until: anIndexer [ anIndexer next not ].
//}
