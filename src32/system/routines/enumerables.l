import system'collections.

// --- literalSplitOp ---

literal extension literalListOp
{
    array split char:aChar
    [
        array_list aList := ArrayList new.
        int        aStart := 0.
        int        anIndex := 0.
        int        aCharLen := aChar length.
        
        while (anIndex >= 0)
        [
            self indexOf int:aStart char:aChar vint:anIndex.
            if (-1 != anIndex)
            [
                aList append:(self Substring int:aStart int:(anIndex - aStart)).
                aStart := anIndex + aCharLen.
            ];
            [
                aList append:(self~literalOp Substring int:aStart)
            ]
        ].
        
        ^ aList array
    ]
    
    split literal:aValue
    [
        array_list aList := ArrayList new.
        int        aStart := 0.
        int        anIndex := 0.
        int        aValueLen := aValue length.
        int        aLiteralLen := self length.
        
        while (anIndex >= 0)
        [
            self indexOf int:aStart literal:aValue vint:anIndex.
            if (-1 != anIndex)
            [
                aList append:(self Substring int:aStart int:(anIndex - aStart)).
                aStart := anIndex + aValueLen
            ];
            [
                aList append:(self Substring int:aStart int:(aLiteralLen - aStart))
            ]
        ].
        
        ^ aList array
    ]
}

// --- wideListOp ---

wide extension wideListOp
{
    array split char:aChar
    [
        array_list aList := ArrayList new.
        int        aStart := 0.
        int        anIndex := 0.
        int        aCharLen := aChar length.

        int        aLiteralLen := self length.
                
        while (anIndex >= 0)
        [
            self indexOf int:aStart char:aChar vint:anIndex.
            if (-1 != anIndex)
            [
                aList append:(self Substring int:aStart int:(anIndex - aStart)).
                aStart := anIndex + aCharLen
            ];
            [
                aList append:(self Substring int:aStart int:(aLiteralLen - aStart))
            ]
        ].
        
        ^ aList array
    ]
    
    split wide:aValue
    [
        array_list aList := ArrayList new.
        int        aStart := 0.
        int        anIndex := 0.
        int        aLiteralLen := self length.
        int        aValueLen := aValue length.
        
        while (anIndex >= 0)
        [
            self indexOf int:aStart wide:aValue vint:anIndex.
            if (-1 != anIndex)
            [
                aList append:(self Substring int:aStart int:(anIndex - aStart)).
                aStart := anIndex + aValueLen
            ];
            [
                aList append:(self Substring int:aStart int:(aLiteralLen - aStart))
            ]
        ].
        
        ^ aList array.
    ]
}

// --- enumerableOp ---

extension enumerableOp
{
    run each:aFunction
      = aFunction run enumerator:(self enumerator).
      
    run eachPair:aFunction
      = aFunction run:self enumerator:(self enumerator).

    run:aParam eachPair:aFunction
      = aFunction run:aParam enumerator:(self enumerator).

    seek each:aFunction
      = aFunction retrieve enumerator:(self enumerator).

    seek:param eachPair:function
      = function retrieve:param enumerator:(self enumerator).

    safeSeek : aValue
        = (:x)[ ^aValue safeEqual:x ] retrieve enumerator:(self enumerator).

    seek value:aValue
        = (:x)[ ^aValue equal:x ] retrieve enumerator:(self enumerator).

    bool ifExists : aValue
        = $nil != self~enumerableOp safeSeek:aValue.

    equal enumerable:list
    [
        ifnot (self length == list length)
            [ ^ false ].
            
        enumerator it1 := self enumerator.
        enumerator it2 := list enumerator.
        
        while (it1 next)
        [
            it2 next.
            
            ifnot (it1 get safeEqual:(it2 get))
                [ ^ false ]
        ].
        
        ^ true
    ]
             
    sequenceEqual:list
        = self~enumerableOp equal enumerable:list.
                                                  
    firstMember
    [
        enumerator anEnumerator := self enumerator.
        
        if (anEnumerator next) 
            [ ^ anEnumerator get ];
            [ ^ $nil ]
    ]

    lastMember
    [
        enumerator anEnumerator := self enumerator.
        object     aLastItem := $nil.
        
        while (anEnumerator next)
            [ aLastItem := anEnumerator get ].
            
        ^ aLastItem
    ]
    
    int count
    [
        enumerator en := self enumerator.
        int        counter := 0.
        while (en next)
        [
            counter := counter + 1
        ].
        
        ^ counter
    ]
}

// --- indexableOp ---

extension indexableOp
{
    exchange : anIndex1 : anIndex2
        = self~indexableOp exchange int:anIndex1 int:anIndex2.
        
    stacksafe exchange int:anIndex1 int:anIndex2
    [
        indexer anIndexer := self indexer.
        
        anIndexer writeIndex int:anIndex1.
        
        var anObject1 := anIndexer get.
        
        anIndexer writeIndex int:anIndex2.
        var anObject2 := anIndexer get.
        
        anIndexer set:anObject1.
        
        anIndexer writeIndex int:anIndex1.
        anIndexer set:anObject2
    ]
    
    run every:nfunc
    [
        indexer it := self indexer.
        
        while (it available)
        [
            nfunc eval int:(it index).
            
            it append index:1.
        ].
    ]
    
    indexOfElement : anObject
    [
        indexer anIndexer := self indexer.
        
        while (anIndexer available)
        [
            if (anObject == anIndexer get)
                [ ^ anIndexer index ].
            
            anIndexer append index:1.
        ].
        
        ^ -1
    ]
    
//    #method deepClone
//    [
//        #var aCopy := self clone.
//        
//        #var(indexer)aIndexer := aCopy indexer.
//        
//        #loop (aIndexer available)?
//        [
//            aIndexer set:(aIndexer get clone).
//            
//            aIndexer append &index:1.
//        ].
//        
//        ^ aCopy.
//    ]
    
    set every:anIndexFunc
        = self~indexableOp set nfunc:(anIndexFunc nfunc).
        
    set nfunc:anIndexFunc
    [
        indexer aIndexer := self indexer.

        while (aIndexer available)
        [
            int anIndex := 0.
            aIndexer readIndexTo vint:anIndex.
            
            aIndexer set:(anIndexFunc eval int:anIndex).
            
            aIndexer append index:1
        ].
        
        ^ self
    ]    

    sort : aCompareFunc
        = self~indexableOp sort func2:(aCompareFunc func2).
    
    sort func2:aCompareFunc
    [
        indexer aFormer := self indexer.
        indexer aLater := self indexer.

        while (aFormer available)
        [
            int i := aFormer index.
            
            aLater writeIndex int:(i + 1).
            
            while (aLater available)
            [
                ifnot (aCompareFunc eval:(aFormer get):(aLater get))
                    [
                        var aTmp := aFormer get.
                        aFormer set:(aLater get).
                        aLater set:aTmp.
                    ].
                    
                aLater append index:1
            ].
            aFormer append index:1
        ].
        
        ^ self
    ]
    
    ascendant
        = (self clone)~indexableOp sort:ifOrdered.

    Subarray : aLength at:anIndex
        = self~indexableOp Subarray int:anIndex int:aLength.
                
    /// obsolete
    Subarray index:anIndex length:aLength
        = self~indexableOp Subarray int:anIndex int:aLength.
                
    Subarray int:anIndex int:aLength
    [
        indexer it := self indexer.
        
        ^ Array new int:aLength; set nfunc: (&int:i)[ ^it writeIndex int:(anIndex + i); get ]
    ]
    
    equal indexable:anArray
    [
        if (self length != anArray length)
            [ ^ false ].
            
        indexer a := self indexer.
        indexer b := anArray indexer.
        
        while (a available)
        [
            ifnot (a get == b get)
                [ ^ false ].
                
            a append index:1.
            b append index:1
        ].
          
        ^ true                
    ]
            
    reverse
        = self~indexableOp reverse:(self length) at:0.
            
    reverse:counter at:index
    [
        indexer it := self indexer.
        
        int      i := index int.
        int      j := counter int.
        j := j + i.
        j := j - 1.
        
        while (i < j)
        [
            it writeIndex int:i.
            var temp1 := it get.
            it writeIndex int:j.
            var temp2 := it get.
            it set:temp1.
            it writeIndex int:i.
            it set:temp2.
            
            i := i + 1.
            j := j - 1
        ].
        
        ^ self
    ]    
}

// --- stringListOp ---

class stringListOpDispatcher =
{
    eval literal:aLiteral = literalListOp.
    
    eval wide:aLiteral = wideListOp.
}.

extension stringListOp
{
    array split char:aChar
    [
        var dispatcher := self cast:%eval to:stringListOpDispatcher.
        
        ^ self~dispatcher split char:aChar.
    ]
    
    split literal:aValue
    [
        var dispatcher := self cast:%eval to:stringListOpDispatcher.
        
        ^ self~dispatcher split literal:aValue.
    ]
    
    split by:aSplitter
    [
        literal aLiteral := self literal.
        
        ^ aSplitter cast:%eval to:
            {
                eval char:aChar = aLiteral~stringListOp split char:aChar.
                
                eval literal:aSplitter = aLiteral~stringListOp split literal:aSplitter.
          }.
    ]
}
