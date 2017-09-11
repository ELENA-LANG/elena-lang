import system'collections.

// --- literalSplitOp ---

literal extension literalListOp
{
    array split
        = self~literalListOp split char:$32.
    
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
    array split
        <= split char:$32.
    
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
    forEach(BaseFunction1 aFunction)
        = aFunction run enumerator:(self enumerator).

    forEach(ExtensionMessage anExtMessage)
        = self~enumerableOp forEach(:n)(n~anExtMessage eval).
        
    multi forEach:fn
        = self~enumerableOp forEach(:n)[ fn eval:n ].
    
    forEachPair(object object, ext_mssg anExtMessage)
        = self~enumerableOp forEachPair(object, (:o:n)(o~anExtMessage eval:n)).
    
    forEachPair(object anObject, func2 aFunction)
      = aFunction run object:anObject enumerator:(self enumerator).
    
    multi forEachPair:object:fn
        = self~enumerableOp forEachPair(object, (:o:n)[ fn eval(o,n) ]).

    runFor(object aParam) eachPair(BaseFunction2 aFunction)
      = aFunction run object:aParam enumerator:(self enumerator).

    runFor(object aParam) eachPair(ExtensionMessage anExtMessage)
        = self~enumerableOp runFor:aParam eachPair(:n:m)(n~anExtMessage eval:m).

    multi runFor:aParam eachPair:aFunction
        = self~enumerableOp runFor:aParam eachPair(:n:m)(aFunction eval(n,m)).

    seekEach(BaseFunction1 aFunction)
        = aFunction retrieve enumerator:(self enumerator).

    seekEach(ExtensionMessage anExtMessage)
        = self~enumerableOp seekEach(:n)(n~anExtMessage eval).
        
    seekEach value:v
        = (:x)(v equal:x) retrieve enumerator:(self enumerator).        
        
    multi seekEach:fn
        = self~enumerableOp seekEach(:n)[ fn eval:n ].

    seekFor(Object param) eachPair(BaseFunction2 aFunction)
        = aFunction retrieve object:param enumerator:self.

    seekFor(Object param) eachPair(ExtensionMessage ext)
        = self~enumerableOp seekFor:param eachPair(:n:m)( n~ext eval(m) ).

    multi seekFor:param eachPair:fn
        = self~enumerableOp seekFor:param eachPair(:n:m)( fn eval(n,m) ).

//    safeSeek : aValue
//        = (:x)[ ^aValue safeEqual:x ] retrieve enumerator:(self enumerator).

    seekFor:aValue
        = (:x)[ ^aValue equal:x ] retrieve enumerator(self enumerator).

    bool ifExists : aValue
        = $nil != self~enumerableOp seekFor:aValue.

    equal enumerable:list
    [
        ifnot (self length == list length)
            [ ^ false ].
            
        enumerator it1 := self enumerator.
        enumerator it2 := list enumerator.
        
        while (it1 next)
        [
            it2 next.
            
            ifnot (it1 get; equal(it2 get))
                [ ^ false ]
        ].
        
        ^ true
    ]
             
//    sequenceEqual:list
//        = self~enumerableOp equal enumerable:list.
                                                  
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
    
    int countMembers
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
    
    forEvery nfunc:fn
    [
        indexer it := self indexer.
        
        while (it available)
        [
            fn eval int:(it index).
            
            it append index:1.
        ].
    ]
    
    forEvery(BaseFunction1 aFunction)
        = self~indexableOp forEvery(&int:n)(aFunction eval:n).
    
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
        
    populate(IndexFunction1 anIndexFunc)
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

    populate(ExtensionMessage anExtMessage)
        = self~indexableOp populate(&int:n)[ ^n~anExtMessage get ].

    populate(BaseFunction1 aFunction)
        = self~indexableOp populate(&int:n)[ ^aFunction eval:n ].

    multi populate : anAction
        = self~indexableOp populate(&int:n)[ ^anAction eval:n ].

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
                ifnot (aCompareFunc eval(aFormer get, aLater get))
                    [
                        var aTmp := aFormer get.
                        aFormer set(aLater get).
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
                
    Subarray int:anIndex int:aLength
    [
        indexer it := self indexer.
        
        ^ Array new int:aLength; populate(&int:i)(it writeIndex int(anIndex + i); get)
    ]
    
    sequenceEqual:anArray
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
            
    sequenceReverse
        = self~indexableOp sequenceReverse(self length) at:0.
            
    sequenceReverse:counter at:index
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

singleton $stringController
{
    split literal:l char:ch
        = l~literalListOp split char:ch.
        
    split literal:l literal:sp_l
        = l~literalListOp split literal:sp_l.
        
    split wide:l char:ch
        = l~wideListOp split char:ch.
        
    split wide:l wide:sp_l
        = l~wideListOp split wide:sp_l.
        
}

extension stringListOp
{
    split by:aSplitter
        = $stringController split(self, aSplitter).
}
