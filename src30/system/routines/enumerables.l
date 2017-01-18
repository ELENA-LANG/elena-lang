#import system.
#import system'collections.

// --- literalSplitOp ---

#class(extension:literal)literalListOp
{
    #method(array) split &char:aChar
    [
        #var(array_list) aList := ArrayList new.
        #var(int)aStart := 0.
        #var(int)anIndex := 0.
        #var(int)aCharLen := aChar length.
        
        #loop (anIndex >= 0)?        
        [
            self indexOf &int:aStart &char:aChar &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += self Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aCharLen.
            ]
            ! [
                aList += self::literalOp Substring &int:aStart.
            ].
        ].
        
        ^ aList array.
    ]
    
    #method split &literal:aValue
    [
        #var(array_list) aList := ArrayList new.
        #var(int)aStart := 0.
        #var(int)anIndex := 0.
        #var(int)aValueLen := aValue length.
        #var(int)aLiteralLen := self length.
        
        #loop (anIndex >= 0)?        
        [
            self indexOf &int:aStart &literal:aValue &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += self Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aValueLen.
            ]
            ! [
                aList += self Substring &int:aStart &int:(aLiteralLen - aStart).
            ].
        ].
        
        ^ aList array.
    ]
}

// --- wideListOp ---

#class(extension:wide)wideListOp
{
    #method(array) split &char:aChar
    [
        #var(array_list) aList := ArrayList new.
        #var(int)aStart := 0.
        #var(int)anIndex := 0.
        #var(int)aCharLen := aChar length.

        #var(int)aLiteralLen := self length.
                
        #loop (anIndex >= 0)?        
        [
            self indexOf &int:aStart &char:aChar &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += self Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aCharLen.
            ]
            ! [
                aList += self Substring &int:aStart &int:(aLiteralLen - aStart).
            ].
        ].
        
        ^ aList array.
    ]
    
    #method split &wide:aValue
    [
        #var(array_list) aList := ArrayList new.
        #var(int)aStart := 0.
        #var(int)anIndex := 0.
        #var(int)aLiteralLen := self length.
        #var(int)aValueLen := aValue length.
        
        #loop (anIndex >= 0)?        
        [
            self indexOf &int:aStart &wide:aValue &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += self Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aValueLen.
            ]
            ! [
                aList += self Substring &int:aStart &int:(aLiteralLen - aStart).
            ].
        ].
        
        ^ aList array.
    ]
}

// --- enumerableOp ---

#class(extension)enumerableOp
{
    #method run &each:aFunction
      = aFunction run &enumerator:(self enumerator).
      
    #method run &eachPair:aFunction
      = aFunction run:self &enumerator:(self enumerator).

    #method run:aParam &eachPair:aFunction
      = aFunction run:aParam &enumerator:(self enumerator).

    #method seek &each:aFunction
      = aFunction retrieve &enumerator:(self enumerator).

    #method seek:param &eachPair:function
      = function retrieve:param &enumerator:(self enumerator).

    #method safeSeek : aValue
        = x [ aValue safeEqual:x ] retrieve &enumerator:(self enumerator).

    #method seek : aValue
        = x [ aValue equal:x ] retrieve &enumerator:(self enumerator).

    #method(bool) ifExists : aValue
        = $nil != self::enumerableOp safeSeek:aValue.

//    #method equal &enumerable:list
//    [
//        (self length == list length)
//            ! [ ^ false. ].
//            
//        #var(enumerator) it1 := self enumerator.
//        #var(enumerator) it2 := list enumerator.
//        
//        #loop (it1 next)?
//        [
//            it2 next.
//            
//            (it1 get safeEqual:(it2 get) bool)
//                ! [ ^ false. ].
//        ].
//        
//        ^ true.
//    ]
//             
//    #method sequenceEqual:list
//        = self::enumerableOp equal &enumerable:list.
                                                  
    #method firstMember
    [
        #var(enumerator)anEnumerator := self enumerator.
        
        (anEnumerator next) 
            ? [ ^ anEnumerator get. ]
            ! [ ^ $nil. ].
    ]

    #method lastMember
    [
        #var(enumerator)anEnumerator := self enumerator.
        #var aLastItem := $nil.
        
        #loop (anEnumerator next) ?
            [ aLastItem := anEnumerator get. ].
            
        ^ aLastItem.
    ]
    
    #method(int,w3) count
    [
        #var(enumerator)en := self enumerator.
        #var(int)counter := 0.
        #loop (en next)?
        [
            counter += 1.
        ].
        
        ^ counter.
    ]
}

// --- indexableOp ---

#class(extension)indexableOp
{
    #method exchange : anIndex1 : anIndex2
        = self::indexableOp exchange &int:(anIndex1 int) &int:(anIndex2 int).
        
    #method(stacksafe) exchange &int:anIndex1 &int:anIndex2
    [
        #var(iterator)anIndexer := self iterator.
        
        anIndexer writeIndex &int:anIndex1.
        
        #var anObject1 := anIndexer get.
        
        anIndexer writeIndex &int:anIndex2.
        #var anObject2 := anIndexer get.
        
        anIndexer set:anObject1.
        
        anIndexer writeIndex &int:anIndex1.
        anIndexer set:anObject2.
    ]
    
//    #method run &every:nfunc
//    [
//        #var(iterator)it := self iterator.
//        
//        #loop (it available)?
//        [
//            nfunc eval &index:(it index).
//            
//            it append &index:1.
//        ].
//    ]
//    
//    #method(w2) indexOf : anObject
//    [
//        #var(iterator)anIndexer := self iterator.
//        
//        #loop (anIndexer available)?
//        [
//            (anObject == anIndexer get)
//                ? [ ^ anIndexer index. ].
//            
//            anIndexer append &index:1.
//        ].
//        
//        ^ -1.
//    ]
//    
//    #method deepClone
//    [
//        #var aCopy := self clone.
//        
//        #var(iterator)aIndexer := aCopy iterator.
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
    
    #method set &every:anIndexFunc
        = self::indexableOp set &nfunc:(anIndexFunc nfunc).
        
    #method set &nfunc:anIndexFunc
    [
        #var(iterator)aIndexer := self iterator.

        #loop (aIndexer available) ?
        [
            #var(int)anIndex.
            aIndexer readIndex &vint:anIndex.
            
            aIndexer set:(anIndexFunc eval &int:anIndex).
            
            aIndexer append &index:1.
        ].
        
        ^ self.
    ]    

    #method sort : aCompareFunc
        = self::indexableOp sort &func2:(aCompareFunc func2).
    
    #method(w2) sort &func2:aCompareFunc
    [
        #var(iterator) aFormer := self iterator.
        #var(iterator) aLater := self iterator.

        #loop (aFormer available) ?
        [
            #var(int)i := aFormer index.
            
            aLater writeIndex &int:(i + 1).
            
            #loop (aLater available) ?
            [
                (aCompareFunc eval:(aFormer get):(aLater get) bool)
                    ! [
                        #var aTmp := aFormer get.
                        aFormer set:(aLater get).
                        aLater set:aTmp.
                    ].
                    
                aLater append &index:1.
            ].
            aFormer append &index:1.
        ].
        
        ^ self.
    ]
    
    #method ascendant
        = (self clone)::indexableOp sort:ifOrdered.

    #method Subarray : aLength &at:anIndex
        = self::indexableOp Subarray &int:(anIndex int) &int:(aLength int).
                
    /// obsolete
    #method Subarray &index:anIndex &length:aLength
        = self::indexableOp Subarray &int:(anIndex int) &int:(aLength int).
                
    #method Subarray &int:anIndex &int:aLength
    [
        #var(iterator) it := self iterator.
        
        ^ Array new &int:aLength set &nfunc: (&int:i) [ it writeIndex &int:(anIndex + i) get ].
    ]
    
    #method(w2) equal &indexable:anArray
    [
        (self length != anArray length)
            ? [ ^ false. ].
            
        #var(iterator) a := self iterator.
        #var(iterator) b := anArray iterator.
        
        #loop (a available)?
        [
            (a get == b get)
                ! [ ^ false. ].
                
            a append &index:1.
            b append &index:1.
        ].
          
        ^ true.                              
    ]
            
    #method reverse
        = self::indexableOp reverse:(self length) &at:0.
            
    #method reverse:counter &at:index
    [
        #var(iterator) it := self iterator.
        
        #var(int)i := index int.
        #var(int)j := counter int.
        j := j + i.
        j := j - 1.
        
        #loop (i < j)?
        [
            it writeIndex &int:i.
            #var temp1 := it get.
            it writeIndex &int:j.
            #var temp2 := it get.
            it set:temp1.
            it writeIndex &int:i.
            it set:temp2.
            
            i += 1.
            j -= 1.
        ].
        
        ^ self.
    ]    
}

// --- stringListOp ---

#symbol stringListOpDispatcher =
{
    eval &literal:aLiteral = literalListOp.
    
    eval &wide:aLiteral = wideListOp.
}.

#class(extension) stringListOp
{
    #method(array) split &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringListOpDispatcher.
        
        ^ self::dispatcher split &char:aChar array.
    ]
    
    #method split &literal:aValue
    [
        #var dispatcher := self cast:%eval &to:stringListOpDispatcher.
        
        ^ self::dispatcher split &literal:aValue.
    ]
    
    #method split &by:aSplitter
    [
        #var(literal) aLiteral := self literal.
        
        ^ aSplitter cast:%eval &to:
            {
                eval &char:aChar = aLiteral::stringListOp split &char:aChar.
                
                eval &literal:aSplitter = aLiteral::stringListOp split &literal:aSplitter.
            }.
    ]
}
