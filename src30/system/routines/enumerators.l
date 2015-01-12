#define system.
#define system'collections.

// --- arrayOp ---

#class(extension:array)arrayOp
{
    #method reallocate &length:aNewLength
        => system'internal'arr_realloc.

    #method exchange &index:anIndex1 &index:anIndex2
    [
        #var(type:array) anArray := self array.

        #var aTemp := anArray getAt &index:anIndex1.
        
        anArray setAt &index:anIndex1 &object:(anArray getAt &index:anIndex2).
        self setAt &index:anIndex2 &object:aTemp.
    ]

    #method deepClone
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength := anArray length.
        
        #var(type:array)aCopy := Array new &length:aLength.
                
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            aCopy setAt &index:anIndex &object:(anArray getAt &index:anIndex clone).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]

    #method set &every:anIndexFunc
    [
        #var(type:array)anArray := self array.
        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength := anArray length.
        
        #loop (anIndex < aLength) ?
        [            
            anArray setAt &index:anIndex &object:(anIndexFunc eval &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]
    
    #method sort : aCompareFunc
    [
        #var(type:array)anArray := self array.
        
        #var(type:int)i := 0.
        #var(type:int)j := 0.
        #var(type:int)aLength := anArray length.
        #var aCurrent := nil.
        #var aNext := nil.
        
        #loop (i < aLength) ?
        [
            aCurrent := anArray getAt &index:i.
            
            j := i + 1.
            #loop (j < aLength) ?
            [
                aNext := anArray getAt &index:j.
                aCompareFunc eval:aCurrent:aNext
                    ! [                        
                        anArray setAt &index:i &object:aNext.
                        anArray setAt &index:j &object:aCurrent.
                        
                        aCurrent := aNext.
                    ].
                    
                j := j + 1.
            ].
            
            i := i + 1.
        ].
        
        ^ self.
    ]
}

// --- Enumerable ---

#class(extension:enumerable) enumerableOp
{
    #method run &each:aFunction
      = aFunction run &enumerator:(self enumerator).

    #method seek &each:aFunction
      = aFunction retrieve &enumerator:(self enumerator).

    #method find : aValue
        = x [ aValue safeEqual:x ] retrieve &enumerator:(self enumerator).

    #method first
    [
        #var(type:enumerator)anEnumerator := self enumerator.
        
        (anEnumerator next) 
            ? [ ^ anEnumerator get. ]
            ! [ ^ nil. ].
    ]

    #method last
    [
        #var(type:enumerator)anEnumerator := self enumerator.
        #var aLastItem := nil.
        
        #loop (anEnumerator next) ?
            [ aLastItem := anEnumerator get. ].
            
        ^ aLastItem.
    ]

    #method top:aCounter
        = system'routines'TopFilter new &length:(aCounter int) &enumerator:(self enumerator).

    #method filter &each:aFilter
        = system'routines'Filter new &each:aFilter &enumerator:(self enumerator).

    #method select &each:aSelector
        = system'routines'Selector new &each:aSelector &enumerator:(self enumerator).

    #method array
    [
        #var(type:list)aList := List new.
        
        self::enumerableOp run &each:anItem [ aList append:anItem ].

        #var(type:int)aLength := aList length.
        #var(type:array) anArray := Array new &length:aLength.
        #var(type:enumerator)anEnumerator := aList enumerator.
        
        anArray set &every: (&index:n) [ anEnumerator next. ^ anEnumerator get. ].

        ^ anArray.
    ]
}

// --- literalOp ---

#class(extension:literal)literalOp
{
    #method indexOf &literal:aLiteral
        = self indexOf &index:0 &literal:aLiteral.

    #method(type:literal) Substring &index:anIndex
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &index:anIndex &length:aLength.
    ]
     
    #method trimLeft &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)anIndex := 0.
        #loop (aLiteral getAt &index:anIndex equal &char:aChar) 
            ? [ anIndex := anIndex + 1. ].
        
        ^ aLiteral delete &index:0 &length:anIndex.
    ]
     
    #method trimRight &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        #var(type:int)anIndex := aLength - 1.
        
        #loop (aLiteral getAt &index:anIndex equal &char:aChar) 
            ? [ anIndex := anIndex - 1. ].
        
        ^ aLiteral delete &index:anIndex &length:(aLength - anIndex).
    ]
    
    #method trim &char:aChar
        = self::literalOp trimLeft &char:aChar trimRight &char:aChar.
    
    #method padLeft &char:aChar &length:aLength
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:literal)padding := aChar literal.
        
        #var(type:int)aCurrentLength := aLiteral length.

        #loop (aLength > aCurrentLength)?
            [ aLiteral := padding add &literal:aLiteral. aCurrentLength := aCurrentLength + 1. ].
            
        ^ aLiteral.
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var(type:literal)aLiteral := self literal.
        
        #var(type:int)aCurrentLength := aLiteral length.

        #loop (aLength > aCurrentLength)?
            [ aLiteral := aLiteral add &char:aChar. aCurrentLength := aCurrentLength + 1. ].
            
        ^ aLiteral.
    ]

    #method startingWith &literal:s
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)n := aLiteral indexOf &index:0 &literal:s.
        
        ^ 0 == n.
    ]

    #method endingWith &literal:s
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)l1 := aLiteral length.        
        #var(type:int)l2 := s length.        
        
        #var(type:int)n := aLiteral indexOf &index:(l1 - l2) &literal:s.
        
        ^ n != -1.
    ]

    #method replace &first &literal:aReplacee &literal:aReplacer
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:int) aReplaceeLen := aReplacee length.
        
        #var(type:int)anIndex := aLiteral indexOf &index:0 &literal:aReplacee.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &index:anIndex &length:aReplaceeLen insert &index:anIndex &literal:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]
        
    #method replace &literal:aReplacee &literal:aReplacer
    [
        #var(type:literal)aLiteral := self literal.
        
        #var aTarget := String new.
        #var(type:int)aStart := 0.
        #var(type:int)anEnd := aLiteral indexOf &index:aStart &literal:aReplacee.
        
        #loop (anEnd != -1) ?
        [
            aTarget 
                append &literal::(aLiteral Substring &index:aStart &length:(anEnd - aStart))
                append &literal:aReplacer.
            
            aStart := anEnd + aReplacee length.
            
            anEnd := aLiteral indexOf &index:aStart &literal:aReplacee.
        ].
    
        ^ (aStart != 0)
            ? [ aTarget append:(aLiteral::literalOp Substring &index:aStart) literal ]
            ! [ self literal ].
    ]

    #method upperCase
    [
        #var(type:literal)aLiteral := self literal.
        
        #var aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:shortarray,size:128)aTemp.
        #var(type:int)aLength := aLiteral length.
        
        #loop (aLength > 0)?
        [
            (aLength > 127)
                ? [
                    aTemp write &index:anIndex &length:127 &literal:aLiteral.
                
                    characterController toUpperCase &shortarray:aTemp &length:127.
                    
                    aRetVal := aRetVal add &literal:(LiteralValue new &length:127 &shortarray:aTemp).                            
                    anIndex := anIndex + 127.
                    aLength := aLength - 127.
                ]
                ! [
                    aTemp write &index:anIndex &length:aLength &literal:aLiteral.
                
                    characterController toUpperCase &shortarray:aTemp &length:aLength.
                    
                    aRetVal := aRetVal add &literal:(LiteralValue new &length:aLength &shortarray:aTemp).                            
                    aLength := 0.
                ].
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(type:literal)aLiteral := self literal.
        
        #var aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:shortarray,size:128)aTemp.
        #var(type:int)aLength := aLiteral length.
        
        #loop (aLength > 0)?
        [
            (aLength > 127)
                ? [
                    aTemp write &index:anIndex &length:127 &literal:aLiteral.
                
                    characterController toLowerCase &shortarray:aTemp &length:127.
                    
                    aRetVal := aRetVal add &literal:(LiteralValue new &length:127 &shortarray:aTemp).                            
                    anIndex := anIndex + 127.
                    aLength := aLength - 127.
                ]
                ! [
                    aTemp write &index:anIndex &length:aLength &literal:aLiteral.
                
                    characterController toLowerCase &shortarray:aTemp &length:aLength.
                    
                    aRetVal := aRetVal add &literal:(LiteralValue new &length:aLength &shortarray:aTemp).                            
                    aLength := 0.
                ].
        ].
            
        ^ aRetVal.
    ]

    #method isEmpty
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        ^ aLength == 0.
    ]
    
    #method isNotEmpty
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        ^ aLength != 0.
    ]

    #method split &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var aList := List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            anIndex := aLiteral indexOf &index:aStart &char:aChar.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + 1.
            ]
            ! [
                aList += aLiteral Substring &index:aStart &length:(self length - aStart).
            ].
        ].
        
        ^ aList::enumerableOp array.
    ]
    
    #method split &literal:aValue
    [
        #var(type:literal)aLiteral := self literal.        
        #var aList := List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            anIndex := aLiteral indexOf &index:aStart &literal:aValue.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + aValue length.
            ]
            ! [
                aList += aLiteral Substring &index:aStart &length:(aLiteral length - aStart).
            ].
        ].
        
        ^ aList::enumerableOp array.
    ]

    #method split &by:aChar
    [
        #var(type:literal) aLiteral := self literal.
        
        ^ aChar cast:%eval &to:
            {
                eval &char:aChar = aLiteral::literalOp split &char:aChar.
                
                eval &literal:aSplitter = aLiteral::literalOp split &literal:aSplitter.
            }.
    ]
}

// --- indexable ---

#class(extension:indexable) indexableOp
{
    #method set &every:anIndexFunc
    [
        #var anEnumerator := self enumerator.

        #loop (anEnumerator next) ?
        [
            #var(type:int)anIndex := anEnumerator index.
            
            anEnumerator set:(anIndexFunc eval &index:anIndex).
        ].
        
        ^ self.
    ]
    
    #method exchange:anIndex1:anIndex2
    [
        self::indexableOp exchange &index:(anIndex1 int) &index:(anIndex2 int).
    ]
    
    #method exchange &index:anIndex1 &index:anIndex2
    [
        #var(type:iterator) it := self iterator.
        
        it write &index:anIndex1.
        #var anObject1 := it get.
                
        it write &index:anIndex2.
        #var anObject2 := it get.        
        it set:anObject1.
        
        it write &index:anIndex1.
        it set:anObject2.
    ]

    #method Subarray &index:anIndex &length:aLength
        = Array new &length:aLength set &every: (&index:i) [ self@(anIndex + i) ].

    #method sort : aCondition
    [
        #var(type:iterator) aFormer := self iterator.
        #var(type:iterator) aLater := self iterator.

        #loop (aFormer available) ?
        [
            #var(type:int)i := aFormer index.
            
            aLater write &index:(i + 1).
            
            #loop (aLater available) ?
            [
                aCondition eval:(aFormer get):(aLater get)
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

    #method array
    [
        #var(type:array)aRetVal := Array new &length:(self length).
        #var(type:iterator)it := self iterator.
        
        aRetVal set &every: (&index:i) [ it write &index:i get ].
        
        ^ aRetVal.
    ]
}

#class(extension:textio)stringOp
{
    #method write : aLiteral &length:aCounter
    [
        #var(type:int)aNumber := aCounter.
        
        #loop (aNumber > 0) ?
        [
            self write:aLiteral. 
            
            aNumber := aNumber - 1.
        ].
            
        ^ self.
    ]
    
    #method write : aLine &paddingLeft:aWidth &with:aChar
    [
        #var(type:int)aWidthInt := aWidth int.
        #var(type:literal) aLiteral := aLine literal.
        
        #var(type:int)aLength := aLiteral length.
        
        (aLength < aWidthInt)
            ? [ self::stringOp write:aChar &length:(aWidthInt - aLength). ].
            
        self write:aLiteral.
            
        ^ self.
    ]

    #method write : aLine &paddingRight:aWidth &with:aChar
    [
        #var(type:literal) aLiteral := aLine literal.
        #var(type:int)aWidthInt := aWidth int.
        
        self write:aLiteral.
                    
        #var(type:int)aLength := aLiteral length.
        
        (aLength < aWidthInt)
            ? [ self::stringOp write:aChar &length:(aWidthInt - aLength). ].
            
        ^ self.
    ]

    #method write : aLine &padding:aWidth &with:aChar
    [
        #var(type:literal) aLiteral := aLine literal.
        #var(type:int)aWidthInt := aWidth int.
        
        #var(type:int)aLength := aLiteral length.        
        
        (aLength < aWidth)
            ? [
                #var(type:int)aLen := aWidthInt - aLength.
                #var(type:int)aLeftLen := aLen / 2.
                
                self::stringOp write:aChar &length:aLeftLen.
                self write:aLiteral.
                self::stringOp write:aChar &length:(aLen - aLeftLen).
            ]
            ! [ self write:aLiteral. ].
            
        ^ self.
    ]
}

// === actions ===

#symbol ifOrdered
    = (:aFormer:aLater) [ aFormer < aLater ].
