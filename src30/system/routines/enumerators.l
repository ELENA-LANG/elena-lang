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
        anArray setAt &index:anIndex2 &object:aTemp.
    ]

    #method indexOf : anObject
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.
        
        anArray readLength &vint:aLength.
        
        #var(type:int)i := 0.
        #loop (i < aLength)?
        [
            #var anArrayItem := anArray getAt &index:i.
            
            (anObject == anArrayItem)
                ? [ ^ IntNumber new &int:i. ].
            
            i := i + 1.
        ].
        
        ^ -1.
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
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        #var aCurrent := $nil.
        #var aNext := $nil.
        
        #loop (i < aLength) ?
        [
            aCurrent := anArray getAt &index:i.
            
            j := i + 1.
            #loop (j < aLength) ?
            [
                aNext := anArray getAt &index:j.
                (aCompareFunc eval:aCurrent:aNext)
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
            ! [ ^ $nil. ].
    ]

    #method last
    [
        #var(type:enumerator)anEnumerator := self enumerator.
        #var aLastItem := $nil.
        
        #loop (anEnumerator next) ?
            [ aLastItem := anEnumerator get. ].
            
        ^ aLastItem.
    ]

    #method(type:enumerable) top:aCounter
        = system'routines'TopFilter new &length:(aCounter int) &enumerator:(self enumerator).

    #method(type:enumerable) filter &each:aFilter
        = system'routines'Filter new &each:aFilter &enumerator:(self enumerator).

    #method(type:enumerable) select &each:aSelector
        = system'routines'Selector new &each:aSelector &enumerator:(self enumerator).

    #method array
    [
        #var(type:list)aList := List new.
        
        self::enumerableOp run &each:anItem [ aList append:anItem ].

        #var(type:int)aLength.
        aList readLength &vint:aLength.
        
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
        = self literal indexOf &index:0 &literal:aLiteral.

    #method indexOf &char:aChar
        = self literal indexOf &index:0 &char:aChar.

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
        #var(type:int)aLength := aLiteral length.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen := aChar length.
        
        #loop (anIndex < aLength) ?
        [
            aCurrent := aLiteral getAt &index:anIndex.
            
            (aChar equal &char:aChar)
                ! [ ^ aLiteral delete &index:0 &length:anIndex. ].
            
            anIndex := anIndex + aCharLen.
        ].
        
        ^ emptyLiteralValue.
    ]
    
    #method trimLeft
        = self::literalOp trimLeft &char:#32.
       
    #method trimRight &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.        
        #var(type:int)anIndex := aLength - 1.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen := aChar length.
        
        #loop (anIndex >= 0) ?
        [
            aCurrent := aLiteral getAt &index:anIndex.
            
            (aChar equal &char:aChar)
                ! [ ^ aLiteral delete &index:anIndex &length:(aLength - anIndex). ].
            
            anIndex := anIndex - aCharLen.
        ].
        
        ^ emptyLiteralValue.
    ]
    
    #method trimRight
        = self::literalOp trimRight &char:#32.
       
    #method trim &char:aChar
        = self::literalOp trimLeft &char:aChar trimRight &char:aChar.
    
    #method trim
        = self::literalOp trimLeft &char:#32 trimRight &char:#32.
    
    #method padLeft &char:aChar &length:aLength
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:int)aCurrentLength := aLiteral length.
        
        (aCurrentLength < aLength)
            ? [ ^ LiteralValue new &length:(aLength - aCurrentLength) &char:aChar add &literal:aLiteral. ].
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aCurrentLength := aLiteral length.

        (aCurrentLength < aLength)
            ? [ ^ aLiteral add &literal:(LiteralValue new &length:(aLength - aCurrentLength) &char:aChar). ].
    ]

    #method startingWith &literal:s
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:vint)n.
        aLiteral indexOf &index:0 &literal:s &vint:n.
        
        ^ 0 == n.
    ]

    #method endingWith &literal:s
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)l1 := aLiteral length.        
        #var(type:int)l2 := s length.        
        
        #var(type:vint)n.
        aLiteral indexOf &index:(l1 - l2) &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method replaceFirst &literal:aReplacee &literal:aReplacer
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:vint) aReplaceeLen.
        #var(type:vint)anIndex.
        
        aReplacee readLength &vint:aReplaceeLen.
        aLiteral indexOf &index:0 &literal:aReplacee &vint:anIndex.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &index:anIndex &length:aReplaceeLen insert &index:anIndex &literal:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]
        
    #method replace &literal:aReplacee &literal:aReplacer
    [
        #var(type:literal)aLiteral := self literal.        
        #var aTarget := String new.
        #var(type:int)aStart := 0.
        #var(type:int)anEnd.
        
        aLiteral indexOf &index:aStart &literal:aReplacee &vint:anEnd.
        
        #loop (anEnd != -1) ?
        [
            aTarget 
                append &literal::(aLiteral Substring &index:aStart &length:(anEnd - aStart))
                append &literal:aReplacer.
            
            aStart := anEnd + aReplacee length.
            
            aLiteral indexOf &index:aStart &literal:aReplacee &vint:anEnd.
        ].
    
        ^ (aStart != 0)
            ? [ aTarget append:(aLiteral::literalOp Substring &index:aStart) literal ]
            ! [ self literal ].
    ]

    #method upperCase
    [
        #var(type:literal)aLiteral := self literal.
        
        #var(type:literal)aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.
        
        aLiteral readLength &vint:aLength.
        
        #loop (anIndex < aLength)?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            aCurrent readLength &vint:aCurrentLen.
            
            characterController toUpperCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(type:literal)aLiteral := self literal.
        
        #var(type:literal)aRetVal := emptyLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.
        
        aLiteral readLength &vint:aLength.
        
        #loop (anIndex < aLength)?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            aCurrent readLength &vint:aCurrentLen.
            
            characterController toLowerCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method is &empty
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        
        ^ aLength == 0.
    ]

    #method(type:array) split &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:list) aList := List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        #var(type:int)aCharLen.
        
        aChar readLength &vint:aCharLen.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &index:aStart &char:aChar &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + aCharLen.
            ]
            ! [
                aList += aLiteral::literalOp Substring &index:aStart.
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
            aLiteral indexOf &index:aStart &literal:aValue &vint:anIndex.
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

    #method split &by:aSplitter
    [
        #var(type:literal) aLiteral := self literal.
        
        ^ aSplitter cast:%eval &to:
            {
                eval &char:aChar = aLiteral::literalOp split &char:aChar.
                
                eval &literal:aSplitter = aLiteral::literalOp split &literal:aSplitter.
            }.
    ]
}

// --- wideOp ---

#class(extension:wide)wideOp
{
    #method indexOf &wide:aLiteral
        = self wide indexOf &index:0 &wide:aLiteral.

    #method indexOf &char:aChar
        = self wide indexOf &index:0 &char:aChar.

    #method(type:wide) Substring &index:anIndex
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &index:anIndex &length:aLength.
    ]
     
    #method trimLeft &char:aChar
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen.
        
        aLiteral readLength &vint:aLength.
        aChar readLength &vint:aCharLen.
        
        #loop (anIndex < aLength) ?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            
            (aChar equal &char:aChar)
                ! [ ^ aLiteral delete &index:0 &length:anIndex. ].
            
            anIndex := anIndex + aCharLen.
        ].
        
        ^ emptyWideLiteralValue.
    ]
    
    #method trimLeft
        = self::wideOp trimLeft &char:#32.
       
    #method trimRight &char:aChar
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aLength.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen.

        aLiteral readLength &vint:aLength.
        aChar readLength &vint:aCharLen.
                
        #var(type:int)anIndex := aLength - 1.
        #loop (anIndex >= 0) ?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            
            (aChar equal &char:aChar)
                ! [ ^ aLiteral delete &index:anIndex &length:(aLength - anIndex). ].
            
            anIndex := anIndex - aCharLen.
        ].
        
        ^ emptyWideLiteralValue.
    ]
    
    #method trimRight
        = self::wideOp trimRight &char:#32.
       
    #method trim &char:aChar
        = self::wideOp trimLeft &char:aChar trimRight &char:aChar.
    
    #method trim
        = self::wideOp trimLeft &char:#32 trimRight &char:#32.
    
    #method padLeft &char:aChar &length:aLength
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)aCurrentLength.
        aLiteral readLength &vint:aCurrentLength.
        
        (aCurrentLength < aLength)
            ? [ ^ WideLiteralValue new &length:(aLength - aCurrentLength) &char:aChar add &wide:aLiteral. ].
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aCurrentLength.
        aLiteral readLength &vint:aCurrentLength.

        (aCurrentLength < aLength)
            ? [ ^ aLiteral add &wide:(WideLiteralValue new &length:(aLength - aCurrentLength) &char:aChar). ].
    ]

    #method startingWith &wide:s
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)n.
        aLiteral indexOf &index:0 &wide:s &vint:n.
        
        ^ 0 == n.
    ]

    #method endingWith &wide:s
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)l1.
        #var(type:int)l2.
        #var(type:int)n.
        
        aLiteral readLength &vint:l1.
        s readLength &vint:l2.
        
        aLiteral indexOf &index:(l1 - l2) &wide:s &vint:n.
        
        ^ n != -1.
    ]

    #method replaceFirst &wide:aReplacee &wide:aReplacer
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int) aReplaceeLen.
        #var(type:int)anIndex.
        
        aReplacee readLength &vint:aReplaceeLen.
        
        aLiteral indexOf &index:0 &wide:aReplacee &vint:anIndex.
        
        (anIndex != -1)
            ? [ ^ aLiteral delete &index:anIndex &length:aReplaceeLen insert &index:anIndex &wide:aReplacee. ]
            ! [ ^ aLiteral. ].
    ]
        
    #method replace &wide:aReplacee &wide:aReplacer
    [
        #var(type:wide)aLiteral := self wide.
        
        #var aTarget := String new.
        #var(type:int)aStart := 0.
        #var(type:int)anEnd.
        aLiteral indexOf &index:aStart &wide:aReplacee &vint:anEnd.
        
        #loop (anEnd != -1) ?
        [
            aTarget 
                append &wide::(aLiteral Substring &index:aStart &length:(anEnd - aStart))
                append &wide:aReplacer.
            
            aStart := anEnd + aReplacee length.
            
            aLiteral indexOf &index:aStart &wide:aReplacee &vint:anEnd.
        ].
    
        ^ (aStart != 0)
            ? [ aTarget append:(aLiteral::wideOp Substring &index:aStart) wide ]
            ! [ aLiteral ].
    ]

    #method upperCase
    [
        #var(type:wide)aLiteral := self wide.
        
        #var(type:wide)aRetVal := emptyWideLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.

        aLiteral readLength &vint:aLength.
                
        #loop (anIndex < aLength)?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            aCurrent readLength &vint:aCurrentLen.
            
            characterController toUpperCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method lowerCase
    [
        #var(type:wide)aLiteral := self wide.
        
        #var(type:wide)aRetVal := emptyWideLiteralValue.        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:char)aCurrent.
        #var(type:char)aConverted.
        #var(type:int)aCurrentLen.
        
        aLiteral readLength &vint:aLength.
        
        #loop (anIndex < aLength)?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            aCurrent readLength &vint:aCurrentLen.
            
            characterController toLowerCase &char:aCurrent &vchar:aConverted.
            
            aRetVal := aRetVal add &char:aConverted.
            
            anIndex := anIndex + aCurrentLen.
        ].
            
        ^ aRetVal.
    ]

    #method is &empty
    [
        #var(type:int)aLength.
        #var(type:wide)aLiteral := self wide.        
        aLiteral readLength &vint:aLength.
        
        ^ aLength == 0.
    ]

    #method(type:array) split &char:aChar
    [
        #var(type:wide)aLiteral := self wide.        
        #var aList := List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        #var(type:int)aCharLen.
        aChar readLength &vint:aCharLen.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &index:aStart &char:aChar &vint:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + aCharLen.
            ]
            ! [
                aList += aLiteral Substring &index:aStart &length:(self length - aStart).
            ].
        ].
        
        ^ aList::enumerableOp array.
    ]
    
    #method split &wide:aValue
    [
        #var(type:wide)aLiteral := self wide.
        #var aList := List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &index:aStart &wide:aValue &vint:anIndex.
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

    #method split &by:aSplitter
    [
        #var(type:wide) aLiteral := self wide.
        
        ^ aSplitter cast:%eval &to:
            {
                eval &char:aChar = aLiteral::wideOp split &char:aChar.
                
                eval &wide:aSplitter = aLiteral::wideOp split &wide:aSplitter.
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
    [
        #var(type:iterator) it := self iterator.
        
        ^ Array new &length:aLength set &every: (&index:i) [ it write &index:(anIndex + i) get ].
    ]

    #method sort : aCondition
    [
        #var(type:iterator) aFormer := self iterator.
        #var(type:iterator) aLater := self iterator.

        #loop (aFormer available) ?
        [
            #var(type:vint)i.
            aFormer readIndex &vint:i.
            
            aLater write &index:(i + 1).
            
            #loop (aLater available) ?
            [
                (aCondition eval:(aFormer get):(aLater get))
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

#class(extension:string) stringOp
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
        
        (aLength < aWidthInt)
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
