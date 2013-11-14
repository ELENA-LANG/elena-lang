#define system.

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
    
    #method next
    [
        #var aRetVal := (nil == theCurrent)
            ? [ $self reset. ^ true. ]
            ! [  
                nil != control foreach:theIndexers &until: &&:anIndexer 
                [
                    anIndexer next
                        ? [ true ]
                        ! [ anIndexer set &type'index:0. ^ false. ]
                ]
            ].
            
        theCurrent := Array new &type'length:theCount &function: &&:i [ (theIndexers@i) get ].
        
        ^ aRetVal.
    ]
    
    #method get
        = theCurrent.
}