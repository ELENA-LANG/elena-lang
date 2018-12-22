//import system'collections.
//
//// === actions ===
//
//public symbol ifOrdered
//    = (:aFormer:aLater) [ ^aFormer < aLater ].
//
//public symbol ifDecrescent
//    = (:aFormer:aLater) [ ^aFormer > aLater ].
//
//// --- literalSplitOp ---
//
//public extension<literal> literalListOp
//{
//    T<Array> split
//        = self~literalListOp split($32).
//    
//    T<Array> split(char aChar)
//    [
//        T<ArrayList> aList := ArrayList new.
//        int          aStart := 0.
//        int          anIndex := 0.
//        int          aCharLen := aChar length.
//        
//        while (anIndex >= 0)
//        [
//            self indexOf(aStart, aChar, &anIndex).
//            if (-1 != anIndex)
//            [
//                aList append(self Substring(aStart,anIndex - aStart)).
//                aStart := anIndex + aCharLen.
//            ];
//            [
//                aList append(self Substring(aStart))
//            ]
//        ].
//        
//        ^ aList
//    ]
//    
//    T<Array> split(literal aValue)
//    [
//        T<ArrayList> aList := ArrayList new.
//        int          aStart := 0.
//        int          anIndex := 0.
//        int          aValueLen := aValue length.
//        int          aLiteralLen := self length.
//        
//        while (anIndex >= 0)
//        [
//            self indexOf(aStart, aValue, &anIndex).
//            if (-1 != anIndex)
//            [
//                aList append(self Substring(aStart, anIndex - aStart)).
//                aStart := anIndex + aValueLen
//            ];
//            [
//                aList append(self Substring(aStart, aLiteralLen - aStart))
//            ]
//        ].
//        
//        ^ aList
//    ]
//}
//
//// --- wideListOp ---
//
//public extension<wide> wideListOp
//{
//    T<Array> split
//        <= split($32).
//    
//    T<Array> split(char aChar)
//    [
//        T<ArrayList> aList := ArrayList new.
//        int          aStart := 0.
//        int          anIndex := 0.
//        int          aCharLen := aChar length.
//
//        int          aLiteralLen := self length.
//                
//        while (anIndex >= 0)
//        [
//            self indexOf(aStart, aChar, &anIndex).
//            if (-1 != anIndex)
//            [
//                aList append(self Substring(aStart, anIndex - aStart)).
//                aStart := anIndex + aCharLen
//            ];
//            [
//                aList append(self Substring(aStart, aLiteralLen - aStart))
//            ]
//        ].
//        
//        ^ aList
//    ]
//    
//    T<Array> split(wide aValue)
//    [
//        T<ArrayList> aList := ArrayList new.
//        int          aStart := 0.
//        int          anIndex := 0.
//        int          aLiteralLen := self length.
//        int          aValueLen := aValue length.
//        
//        while (anIndex >= 0)
//        [
//            self indexOf(aStart, aValue, &anIndex).
//            if (-1 != anIndex)
//            [
//                aList append(self Substring(aStart, anIndex - aStart)).
//                aStart := anIndex + aValueLen
//            ];
//            [
//                aList append(self Substring(aStart, aLiteralLen - aStart))
//            ]
//        ].
//        
//        ^ aList.
//    ]
//}

// --- enumeratorOp ---

//public extension enumeratorOp : Enumerator
//{
////    forEach(Func1 aFunction)
////        = aFunction run(self).
////
////    selectBy:aSelector
////        = Selector new(aSelector, self).
//}

// --- enumerableOp ---

public extension enumerableOp
{
//    forEach(Func1 aFunction)
//        = aFunction run(T<Enumerator>(self enumerator)).
//
//    forEach(ExtensionMessage anExtMessage)
//        = self~enumerableOp forEach(:n)(n~anExtMessage eval).
//        
//    forEach:fn
//        = self~enumerableOp forEach(:n)[ fn(n) ].
//
//    selectBy:aSelector
//        = Selector new(aSelector, T<Enumerator>(self enumerator)).
//
////    forEachPair(object object, ext_mssg anExtMessage)
////        = self~enumerableOp forEachPair(object, (:o:n)(o~anExtMessage eval:n)).
//    
//    forEachPair(object anObject, Func2 aFunction)
//      = aFunction run(anObject, T<Enumerator>(self enumerator)).
//    
////    multi forEachPair:object:fn
////        = self~enumerableOp forEachPair(object, (:o:n)[ fn(o,n) ]).
//
//    runFor(object param) eachPair(Func2 f)
//        = f run(param, T<Enumerator>(self enumerator)).
//
////    runFor(object aParam) eachPair(ExtensionMessage anExtMessage)
////        = self~enumerableOp runFor:aParam eachPair(:n:m)(n~anExtMessage eval:m).
////
////    multi runFor:aParam eachPair:aFunction
////        = self~enumerableOp runFor:aParam eachPair(:n:m)(aFunction(n,m)).
//
//    seekEach(Func1 f)
//        = f retrieve(T<Enumerator>(self enumerator)).
//
////    seekEach(ExtensionMessage anExtMessage)
////        = self~enumerableOp seekEach(:n)(n~anExtMessage eval).
////        
////    seekEach value:v
////        = (:x)(v equal:x) retrieve enumerator:(self enumerator).        
////        
////    multi seekEach:fn
////        = self~enumerableOp seekEach(:n)[ fn(n) ].
//
//    seekFor(object param) eachPair(Func2 f)
//        = f retrieve(param, T<Enumerator>(self enumerator)).
//
//    seekFor(Object param) eachPair(ExtensionMessage ext)
//        = self~enumerableOp seekFor:param eachPair(:n:m)( n~ext eval(m) ).
//
////    multi seekFor:param eachPair:fn
////        = self~enumerableOp seekFor:param eachPair(:n:m)( fn(n,m) ).
////
//////    safeSeek : aValue
//////        = (:x)[ ^aValue safeEqual:x ] retrieve enumerator:(self enumerator).
//
//    seekFor:aValue
//        = (:x)[ ^aValue equal:x ] retrieve(T<Enumerator>(self enumerator)).
//
//    bool ifExists : aValue
//        = nil != self~enumerableOp seekFor:aValue.
//
////    equal enumerable:list
////    [
////        ifnot (self length == list length)
////            [ ^ false ].
////            
////        enumerator it1 := self enumerator.
////        enumerator it2 := list enumerator.
////        
////        while (it1 next)
////        [
////            it2 next.
////            
////            ifnot (it1 get; equal(it2 get))
////                [ ^ false ]
////        ].
////        
////        ^ true
////    ]
//             
////    sequenceEqual:list
////        = self~enumerableOp equal enumerable:list.
//                                                  
//    firstMember
//    [
//        auto anEnumerator := T<Enumerator>(self enumerator).
//        
//        if (anEnumerator next) 
//            [ ^ anEnumerator get ];
//            [ ^ nil ]
//    ]
//
//    maximalMember
//    [
//        auto en := T<Enumerator>(self enumerator).
//        
//        object maximal := nil.
//        while (en next)
//        [
//            var item := en get.
//            if (nil == maximal)
//            [
//                maximal := item
//            ]; if (maximal < item)
//            [
//                maximal := item
//            ]
//        ].
//        
//        ^ maximal   
//    ]
//    
//    minimalMember
//    [
//        auto en := T<Enumerator>(self enumerator).
//        
//        object maximal := nil.
//        while (en next)
//        [
//            var item := en get.
//            if (nil == maximal)
//            [
//                maximal := item
//            ]; if (maximal > item)
//            [
//                maximal := item
//            ]
//        ].
//        
//        ^ maximal   
//    ]
//    
//    lastMember
//    [
//        auto   anEnumerator := T<Enumerator>(self enumerator).
//        object aLastItem := nil.
//        
//        while (anEnumerator next)
//            [ aLastItem := anEnumerator get ].
//            
//        ^ aLastItem
//    ]
//    
//    int countMembers
//    [
//        auto   en := T<Enumerator>(self enumerator).
//        int    counter := 0.
//        while (en next)
//        [
//            counter := counter + 1
//        ].
//        
//        ^ counter
//    ]
//    
//    allMatchedBy(Func1 f)
//    [
//        auto   en := T<Enumerator>(self enumerator).
//        while (en next)
//        [
//            ifnot (f(en get))
//            [
//                ^ false
//            ]
//        ].
//        
//        ^ true.
//    ]
//    
//    allMatchedBy(ExtensionMessage extMessage)
//        = self~enumerableOp allMatchedBy(:n)(n~extMessage eval).
//
//    summarize : aVariable
//    [
//        Summing new:aVariable; run(T<Enumerator>(self enumerator)).
//        
//        ^ aVariable value.
//    ]
//    
//    summarize
//        = self~enumerableOp summarize:(Variable new).
//        
//    filterBy(Func1 aSelector)
//        = Filter new(aSelector, T<Enumerator>(self enumerator)).
//        
//    filterBy(Func2 aSelector)
//        = PatternFilter2 new(aSelector, T<Enumerator>(self enumerator)).
//        
//    filterBy(Func3 aSelector)
//        = PatternFilter3 new(aSelector, T<Enumerator>(self enumerator)).
//        
//    filterBy(Func4 aSelector)
//        = PatternFilter4 new(aSelector, T<Enumerator>(self enumerator)).
//        
//    filterBy(Func5 aSelector)
//        = PatternFilter5 new(aSelector, T<Enumerator>(self enumerator)).
//        
//    filterBy(Func6 aSelector)
//        = PatternFilter6 new(aSelector, T<Enumerator>(self enumerator)).
//        
//    repeatTill:n
//        = RangeEnumerator from:self to(n - 1) by:1.
//
//    repeatTo:n
//        = RangeEnumerator from:self to:n by:1.
//
//    top:aCounter
//        = TopFilter new(aCounter, T<Enumerator>(self enumerator)).
//        
//    till:max repeat:aRepeater
//        = Repeater new(self, max, aRepeater).
//        
//    to:max repeat:aRepeater
//        = Repeater new(self, max + 1, aRepeater).
//        
//    accumulate : aVariable with:operation
//    [
//        Accumulating new(aVariable, operation); run(T<Enumerator>(self enumerator)).
//        
//        ^ aVariable value.
//    ]
//    
//    accumulateWith:operation 
//        = self~enumerableOp accumulate(Variable new) with:operation.        
//        
//    zip:anEnumerable by:aZipper
//        = ZipEnumerator new(aZipper, T<Enumerator>(self enumerator), T<Enumerator>(anEnumerable enumerator)).
//
//    zip:anEnumerable forEach:func2
//        = self~enumerableOp zip:anEnumerable by:func2; forEach:idleFunc1.
//        
//    orderBy:orderFunc
//        = OrderingEnumerator new(orderFunc, T<Enumerator>(self enumerator)).
//        
//    groupBy:groupFunc
//        = GroupingEnumerator new(groupFunc, T<Enumerator>(self enumerator)).                                       
//        
//    join(object list) on(Func2 filter_f) selectBy(Func2 select_f)
//        = JoinEnumerator new(filter_f, select_f, T<Enumerator>(self enumerator), T<Enumerator>(list enumerator)).        
}

// --- indexableOp ---

public extension indexableOp
{
////    exchange : anIndex1 : anIndex2
////        = self~indexableOp exchange int:anIndex1 int:anIndex2.
//        
//    exchange(int anIndex1, int anIndex2)
//    [
//        T<Indexer> anIndexer := self indexer.
//        
//        anIndexer writeIndex(anIndex1).
//        
//        var anObject1 := anIndexer get.
//        
//        anIndexer writeIndex(anIndex2).
//        var anObject2 := anIndexer get.
//        
//        anIndexer set:anObject1.
//        
//        anIndexer writeIndex(anIndex1).
//        anIndexer set:anObject2
//    ]
//    
//    forEvery(Func<int,object> fn)
//    [
//        T<Indexer> it := self indexer.
//        
//        while (it available)
//        [
//            fn(it index).
//            
//            it appendIndex:1.
//        ].
//    ]
//    
//    forEvery(Func1 fn)
//        = self~indexableOp forEvery(:n)<int>(fn(n)).
//    
//    indexOfElement : anObject
//    [
//        T<Indexer> anIndexer := self indexer.
//        
//        while (anIndexer available)
//        [
//            if (anObject == anIndexer get)
//                [ ^ anIndexer index ].
//            
//            anIndexer appendIndex:1.
//        ].
//        
//        ^ -1
//    ]
//    
////    #method deepClone
////    [
////        #var aCopy := self clone.
////        
////        #var(indexer)aIndexer := aCopy indexer.
////        
////        #loop (aIndexer available)?
////        [
////            aIndexer set:(aIndexer get clone).
////            
////            aIndexer append &index:1.
////        ].
////        
////        ^ aCopy.
////    ]
//        
//    populate(Func<int,object> f)
//    [
//        T<Indexer> it := self indexer.
//
//        while (it available)
//        [
//            int i := 0.
//            it readIndexTo(&i).
//            
//            it set(f(i)).
//            
//            it appendIndex:1
//        ].
//        
//        ^ self
//    ]    
//
//    populate(ExtensionMessage ext_mssg)
//        = self~indexableOp populate(:n)<int>[ ^n~ext_mssg get ].
//
    populate(Func1 f)
    {
        Indexer it := self.Indexer;

        while (it.Available)
        {
            int i := 0;
            it.readIndexTo(ref i);
            
            it.set(f(i));
            
            it.appendIndex:1
        };
    }    

////    populate : anAction
////        = self~indexableOp populate(:n)<int>[ ^anAction(n) ].
//
//    bool isSorted(Func2 aCompareFunc)
//    [
//        auto it := T<Indexer>(self indexer).
//        if (it available)
//        [
//            var previous := it get.
//            it appendIndex:1.
//            while (it available)
//            [
//                ifnot (aCompareFunc(previous, it get))
//                [
//                    ^ false
//                ].
//                
//                previous := it get.
//                it appendIndex:1
//            ]
//        ].
//        
//        ^ true.
//    ]
//
//    sort(Func2 aCompareFunc)
//    [
//        T<Indexer> aFormer := self indexer.
//        T<Indexer> aLater := self indexer.
//
//        while (aFormer available)
//        [
//            int i := aFormer index.
//            
//            aLater writeIndex(i + 1).
//            
//            while (aLater available)
//            [
//                ifnot (aCompareFunc(aFormer get, aLater get))
//                    [
//                        var aTmp := aFormer get.
//                        aFormer set(aLater get).
//                        aLater set:aTmp.
//                    ].
//                    
//                aLater appendIndex:1
//            ].
//            aFormer appendIndex:1
//        ].
//        
//        ^ self
//    ]
//    
//    ascendant
//        = (self clone)~indexableOp sort:ifOrdered.
//
//    isAscendant
//        = self~indexableOp isSorted:ifOrdered.
//
//    Subarray : aLength at:anIndex
//        = self~indexableOp Subarray(anIndex, aLength).
//                
//    Subarray(int anIndex, int aLength)
//    [
//        auto it := T<Indexer>(self indexer).
//        
//        ^ Array new(aLength); populate(:i)<int>(it writeIndex(anIndex + i); get)
//    ]
//    
//    sequenceEqual:anArray
//    [
//        if (self length != anArray length)
//            [ ^ false ].
//            
//        T<Indexer> a := self indexer.
//        T<Indexer> b := anArray indexer.
//        
//        while (a available)
//        [
//            ifnot (a get == b get)
//                [ ^ false ].
//                
//            a appendIndex:1.
//            b appendIndex:1
//        ].
//          
//        ^ true                
//    ]
//            
//    sequenceReverse
//        = self~indexableOp sequenceReverse(self length) at:0.
//            
//    sequenceReverse:counter at:index
//    [
//        T<Indexer> it := self indexer.
//        
//        int      i := T<int>(index).
//        int      j := T<int>(counter).
//        j := j + i.
//        j := j - 1.
//        
//        while (i < j)
//        [
//            it writeIndex(i).
//            var temp1 := it get.
//            it writeIndex(j).
//            var temp2 := it get.
//            it set:temp1.
//            it writeIndex(i).
//            it set:temp2.
//            
//            i := i + 1.
//            j := j - 1
//        ].
//        
//        ^ self
//    ]    
}

//// --- stringListOp ---
//
//singleton stringController
//{
//    split(literal l, char ch)
//        = l~literalListOp split(ch).
//        
//    split(literal l, literal sp_l)
//        = l~literalListOp split(sp_l).
//        
//    split(wide l, char ch)
//        = l~wideListOp split(ch).
//        
//    split(wide l, wide sp_l)
//        = l~wideListOp split(sp_l).
//        
//}
//
//public extension stringListOp
//{
//    splitBy:aSplitter
//        = stringController split(self, aSplitter).
//}
