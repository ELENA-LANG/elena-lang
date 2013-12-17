#define system.
#define system'routines.
#define extensions.

// --- CombinatorWithRepetition ---

#class CombinatorWithRepetition
{
    #field theIndexers.
    #field theCurrent.
    #field theCount.
    
    #constructor new : aCollection
    [
        theCount := aCollection type'length.
        theIndexers := Array new &type'length:theCount &function: &&:i [ aCollection~EList getAt:i Enumerator ].
        theCurrent := nil.
    ]
    
    #method reset
    [
        control foreach:theIndexers &do: &&:anIndexer [ anIndexer set &type'index:0. ]. 
        theCurrent := nil.
    ]
    
    #method refresh
    [
        theCurrent := Array new &type'length:theCount &function: &&:i [ (theIndexers@i) get ].
    ]

    #method findNext
        = nil != control foreach:theIndexers &until: &&:anIndexer 
        [
            anIndexer next
                ? [ true ]
                ! [ anIndexer set &type'index:0. ^ false. ]
        ].
            
    #method next
    [
        #var aRetVal := (nil == theCurrent)
            ? [ $self reset. ^ true. ]
            ! [ $self findNext ].
            
        aRetVal 
            ? [ $self refresh. ]
            ! [ theCurrent := nil. ].
        
        ^ aRetVal.
    ]
    
    #method get
        = theCurrent.
}

// --- Combinator ---

#class Combinator : CombinatorWithRepetition
{
    #method reset
    [
        $super reset.
        
        $self reset:1.
    ]

    #method reset : anIndex
    [
        #var aPrevious := Integer new:(theIndexers@anIndex Index).
        
        #var retVal := control from:anIndex &till:theCount &do: &&:i
        [
            theIndexers@i set &type'index:(aPrevious += 1) EOF
                ? [ #break self. ].            
        ].
        
        ^ nil == retVal.
    ]
    
    #method findNext
    [
        #var aCurrentIndexer := theIndexers @ (theCount - 1).
        
        ^ aCurrentIndexer next 
            ? [ true ]
            ! [
                #var aPrevious := Integer new:theCount.
                
                control do:[ aPrevious -= 1. ] &until:[ (aPrevious < 0)or:[ self reset:aPrevious ] ].
                
                ^ (0 <= aPrevious).
            ].
    ]
}
