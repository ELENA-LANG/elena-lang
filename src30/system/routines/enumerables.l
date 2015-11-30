// ==== ver 3.4.1 ===

#import system.
#import system'collections.

// --- literalSplitOp ---

#class(extension:literal)literalListOp
{
    #method(type:array) split &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:array_list) aList := ArrayList new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        #var(type:int)aCharLen.
        
        aChar readLength &vint:aCharLen.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &int:aStart &char:aChar &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aCharLen.
            ]
            ! [
                aList += aLiteral::literalOp Substring &int:aStart.
            ].
        ].
        
        ^ aList array.
    ]
    
    #method split &literal:aValue
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:array_list) aList := ArrayList new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        #var(type:int)aValueLen.
        #var(type:int)aLiteralLen.
        
        aValue readLength &vint:aValueLen.
        aLiteral readLength &vint:aLiteralLen.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &int:aStart &literal:aValue &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aValueLen.
            ]
            ! [
                aList += aLiteral Substring &int:aStart &int:(aLiteralLen - aStart).
            ].
        ].
        
        ^ aList array.
    ]
}

// --- wideListOp ---

#class(extension:wide)wideListOp
{
    #method(type:array) split &char:aChar
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:array_list) aList := ArrayList new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        #var(type:int)aCharLen.
        aChar readLength &vint:aCharLen.

        #var(type:int)aLiteralLen.
        aLiteral readLength &vint:aLiteralLen.
                
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &int:aStart &char:aChar &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aCharLen.
            ]
            ! [
                aList += aLiteral Substring &int:aStart &int:(aLiteralLen - aStart).
            ].
        ].
        
        ^ aList array.
    ]
    
    #method split &wide:aValue
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:array_list) aList := ArrayList new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        #var(type:int)aLiteralLen.
        aLiteral readLength &vint:aLiteralLen.
        #var(type:int)aValueLen.
        aValue readLength &vint:aValueLen.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &int:aStart &wide:aValue &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &int:aStart &int:(anIndex - aStart).
                aStart := anIndex + aValueLen.
            ]
            ! [
                aList += aLiteral Substring &int:aStart &int:(aLiteralLen - aStart).
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

    #method safeSeek : aValue
        = x [ aValue safeEqual:x ] retrieve &enumerator:(self enumerator).

    #method ifExists : aValue
        = $nil != self::enumerableOp safeSeek:aValue.

    #method top:aCounter
        = system'routines'TopFilter new:aCounter &enumerator:(self enumerator).

    #method filter &each:aFilter
        = system'routines'Filter new &each:aFilter &enumerator:(self enumerator).

    #method select &each:aSelector
        = system'routines'Selector new &each:aSelector &enumerator:(self enumerator).
        
    #method firstMember
    [
        #var(type:enumerator)anEnumerator := self enumerator.
        
        (anEnumerator next) 
            ? [ ^ anEnumerator get. ]
            ! [ ^ $nil. ].
    ]

    #method lastMember
    [
        #var(type:enumerator)anEnumerator := self enumerator.
        #var aLastItem := $nil.
        
        #loop (anEnumerator next) ?
            [ aLastItem := anEnumerator get. ].
            
        ^ aLastItem.
    ]
}

// --- indexableOp ---

#class(extension)indexableOp
{
    #method exchange : anIndex1 : anIndex2
        = self::indexableOp exchange &int:(anIndex1 int) &int:(anIndex2 int).
        
    #method(stacksafe) exchange &int:anIndex1 &int:anIndex2
    [
        #var(type:iterator)anIndexer := self iterator.
        
        anIndexer writeIndex &int:anIndex1.
        
        #var anObject1 := anIndexer get.
        
        anIndexer writeIndex &int:anIndex2.
        #var anObject2 := anIndexer get.
        
        anIndexer set:anObject1.
        
        anIndexer writeIndex &int:anIndex1.
        anIndexer set:anObject2.
    ]
    
    #method(suppress:w2) indexOf : anObject
    [
        #var(type:iterator)anIndexer := self iterator.
        
        #var(type:int)i := 0.
        #loop (anIndexer available)?
        [
            (anObject == anIndexer get)
                ? [ ^ IntNumber new &int:i. ].
            
            i := i + 1.
        ].
        
        ^ -1.
    ]
    
    #method deepClone
    [
        #var aCopy := self clone.
        
        #var(type:iterator)aIndexer := aCopy iterator.
        
        #loop (aIndexer available)?
        [
            aIndexer set:(aIndexer get clone).
            
            aIndexer append &index:1.
        ].
        
        ^ aCopy.
    ]
    
    #method set &every:anIndexFunc
        = self::indexableOp set &nfunc:(anIndexFunc nfunc).
        
    #method set &nfunc:anIndexFunc
    [
        #var(type:iterator)aIndexer := self iterator.

        #loop (aIndexer available) ?
        [
            #var(type:int)anIndex.
            aIndexer readIndex &vint:anIndex.
            
            aIndexer set:(anIndexFunc eval &int:anIndex).
            
            aIndexer append &index:1.
        ].
        
        ^ self.
    ]    

    #method sort : aCompareFunc
        = self::indexableOp sort &func2:(aCompareFunc func2).
    
    #method(suppress:w2) sort &func2:aCompareFunc
    [
        #var(type:iterator) aFormer := self iterator.
        #var(type:iterator) aLater := self iterator.

        #loop (aFormer available) ?
        [
            #var(type:vint)i.
            aFormer readIndex &vint:i.
            
            aLater writeIndex &int:(i + 1).
            
            #loop (aLater available) ?
            [
                (aCompareFunc eval:(aFormer get):(aLater get))
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
        #var(type:iterator) it := self iterator.
        
        ^ Array new &int:aLength set &nfunc: (&int:i) [ it writeIndex &int:(anIndex + i) get ].
    ]
    
    #method(suppress:w2) equal &indexable:anArray
    [
        (self length != anArray length)
            ? [ ^ false. ].
            
        #var(type:iterator) a := self iterator.
        #var(type:iterator) b := anArray iterator.
        
        #loop (a available)?
        [
            (a get == b get)
                ! [ ^ false. ].
                
            a append &index:1.
            b append &index:1.
        ].
          
        ^ true.                              
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
    #method(type:array) split &char:aChar
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
        #var(type:literal) aLiteral := self literal.
        
        ^ aSplitter cast:%eval &to:
            {
                eval &char:aChar = aLiteral::stringListOp split &char:aChar.
                
                eval &literal:aSplitter = aLiteral::stringListOp split &literal:aSplitter.
            }.
    ]
}
