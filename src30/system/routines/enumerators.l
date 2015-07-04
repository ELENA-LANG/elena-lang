#define system.
#define system'collections.

// --- arrayOp ---

#class(extension:intarray)intArrayOp
{
    #method exchange &index:anIndex1 &index:anIndex2
    [
        #var(type:intarray)a := self intarray.

        #var(type:int)n1.
        #var(type:int)n2.

        a read &index:anIndex1 &vint:n1.
        a read &index:anIndex2 &vint:n2.
        
        a write &index:anIndex1 &int:n2.
        a write &index:anIndex2 &int:n1.
    ]
    
    #method set &every:anIndexFunc
    [
        #var(type:intarray)anArray := self intarray.
        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
        #loop (anIndex < aLength) ?
        [            
            anArray write &index:anIndex &int:(anIndexFunc eval &index:anIndex int).
            
            anIndex := anIndex + 1.
        ].
        
        ^ self.
    ]
}

// --- arrayOp ---

#class(extension:array)arrayOp
{
    #method reallocate &length:aNewLength
        => system'internal'arr_realloc.

    #method exchange : anIndex1 : anIndex2
        = self::arrayOp exchange &index:(anIndex1 int) &index:(anIndex2 int).
        
    #method exchange &index:anIndex1 &index:anIndex2
    [
        #var(type:array) anArray := self array.

        #var aTemp := anArray getAt &index:anIndex1.
        
        anArray setAt &index:anIndex1 &object:(anArray getAt &index:anIndex2).
        anArray setAt &index:anIndex2 &object:aTemp.
    ]
    
    #method indexOf &index:anIndex &object:anObject
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.
        
        anArray readLength &vint:aLength.
        
        #var(type:int)i := anIndex.
        #loop (i < aLength)?
        [
            #var anArrayItem := anArray getAt &index:i.
            
            (anObject == anArrayItem)
                ? [ ^ IntNumber new &int:i. ].
            
            i := i + 1.
        ].
        
        ^ -1.
    ]
    
    #method indexOf : anObject
        = self::arrayOp indexOf &index:0 &object:anObject.
        
    #method safeSeek : aValue
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.                
        anArray readLength &vint:aLength.
        
        #var(type:int)i := 0.
        #loop (i < aLength)?
        [
            #var anArrayItem := anArray getAt &index:i.
            
            (aValue safeEqual:anArrayItem)
                ? [ ^ anArrayItem. ].
            
            i := i + 1.
        ].
        
        ^ $nil.
    ]
        
    #method deepClone
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
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
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
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
    
    #method ascendant
        = (self clone)::arrayOp sort:ifOrdered.
    
    #method is &empty
    [
        #var(type:array)anArray := self array.
        #var(type:int)aLength.
        anArray readLength &vint:aLength.
        
        ^ aLength == 0.
    ]
    
    #method Subarray &index:anIndex &length:aLength
    [
        #var(type:iterator) it := self iterator.
        
        ^ Array new &length:aLength set &every: (&index:i) [ it write &index:(anIndex + i) get ].
    ]
}

// --- literalOp ---

#class(extension:literal)literalOp
{
    #method indexOf : anObject
        = self literal indexOf:0:anObject.

    #method indexOf &literal:aLiteral
        = self literal indexOf &index:0 &literal:aLiteral.

    #method indexOf &char:aChar
        = self literal indexOf &index:0 &char:aChar.
        
    #method(type:literal) Substring &index:anIndex
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &index:anIndex &length:aLength.
    ]        
    
    #method trimLeft &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen.
        aChar readLength &vint:aCharLen.
        
        #loop (anIndex < aLength) ?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            
            (aCurrent == aChar)
                ! [ 
                    (anIndex == 0)
                        ? [ ^ aLiteral. ]
                        ! [^ aLiteral delete &index:0 &length:anIndex. ].
                ].
            
            anIndex := anIndex + aCharLen.
        ].
        
        ^ emptyLiteralValue.
    ]
    
    #method trimLeft
        = self::literalOp trimLeft &char:#32.    
       
    #method trimRight &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength.
        aLiteral readLength &vint:aLength.
        #var(type:int)anIndex := aLength - 1.
        #var(type:char)aCurrent.
        #var(type:int)aCharLen.
        aChar readLength &vint:aCharLen.
        
        #loop (anIndex >= 0) ?
        [
            aLiteral read &index:anIndex &vchar:aCurrent.
            
            (aCurrent equal &char:aChar)
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
        #var(type:int)aCurrentLength.
        aLiteral readLength &vint:aCurrentLength.
        
        (aCurrentLength < aLength)
            ? [ ^ LiteralValue new &length:(aLength - aCurrentLength) &char:aChar add &literal:aLiteral. ].
            
        ^ self.            
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aCurrentLength.
        aLiteral readLength &vint:aCurrentLength.

        (aCurrentLength < aLength)
            ? [ ^ aLiteral add &literal:(LiteralValue new &length:(aLength - aCurrentLength) &char:aChar). ].
            
        ^ self.            
    ]

    #method startingWith:s
        = self::literalOp startingWith &literal:(s literal).
        
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
        #var(type:int)l1.
        #var(type:int)l2.
        aLiteral readLength &vint:l1.
        s readLength &vint:l2.

        #var(type:vint)n.
        aLiteral indexOf &index:(l1 - l2) &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method endingWith:s
        = self::literalOp endingWith &literal:(s literal).
        
    #method containing &literal:s
    [
        #var(type:literal)aLiteral := self literal.
        #var(type:int)l1.
        #var(type:int)l2.
        aLiteral readLength &vint:l1.
        s readLength &vint:l2.

        #var(type:vint)n.
        aLiteral indexOf &index:0 &literal:s &vint:n.
        
        ^ n != -1.
    ]

    #method containing:s
        = self::literalOp containing &literal:(s literal).
    
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
        #var(type:array_list) aList := ArrayList new.
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
        
        ^ aList array.
    ]
    
    #method split &literal:aValue
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:array_list) aList := ArrayList new.
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
        
        ^ aList array.
    ]
}

// --- wideOp ---

#class(extension:wide)wideOp
{
    #method indexOf : anObject
        = self wide indexOf:0:anObject.
        
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
            
            (aCurrent equal &char:aChar)
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
            
            (aCurrent equal &char:aChar)
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
            
        ^ self.            
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)aCurrentLength.
        aLiteral readLength &vint:aCurrentLength.

        (aCurrentLength < aLength)
            ? [ ^ aLiteral add &wide:(WideLiteralValue new &length:(aLength - aCurrentLength) &char:aChar). ].
            
        ^ self.            
    ]

    #method startingWith &wide:s
    [
        #var(type:wide)aLiteral := self wide.        
        #var(type:int)n.
        aLiteral indexOf &index:0 &wide:s &vint:n.
        
        ^ 0 == n.
    ]
    
    #method startingWith:s
        = self::wideOp startingWith &wide:(s wide).

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
    
    #method endingWith:s
        = self::wideOp endingWith &wide:(s wide).

    #method containing &wide:s
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:int)l1.
        #var(type:int)l2.
        #var(type:int)n.
        
        aLiteral readLength &vint:l1.
        s readLength &vint:l2.
        
        aLiteral indexOf &index:0 &wide:s &vint:n.
        
        ^ n != -1.
    ]
    
    #method containing:s
        = self::wideOp containing &wide:(s wide).

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
        #var(type:array_list) aList := ArrayList new.
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
        
        ^ aList array.
    ]
    
    #method split &wide:aValue
    [
        #var(type:wide)aLiteral := self wide.
        #var(type:array_list) aList := ArrayList new.
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
        
        ^ aList array.
    ]
}

// --- enumerableOp ---

#class(extension)enumerableOp
{
    #method run &each:aFunction
      = aFunction run &enumerator:(self enumerator).
      
    #method run:aParam &eachPair:aFunction
      = aFunction run:aParam &enumerator:(self enumerator).
      
    #method seek &each:aFunction
      = aFunction retrieve &enumerator:(self enumerator).

    #method safeSeek : aValue
        = x [ aValue safeEqual:x ] retrieve &enumerator:(self enumerator).

    #method ifExists : aValue
        = $nil != self::enumerableOp seek:aValue.

    #method top:aCounter
        = system'routines'TopFilter new &length:(aCounter int) &enumerator:(self enumerator).

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
        = self::indexableOp exchange &index:(anIndex1 int) &index:(anIndex2 int).
        
    #method exchange &index:anIndex1 &index:anIndex2
    [
        #var(type:iterator)anIndexer := self iterator.
        
        anIndexer set &index:anIndex1.
        
        #var anObject1 := anIndexer get.
        
        anIndexer set &index:anIndex2.
        #var anObject2 := anIndexer get.
        
        anIndexer set:anObject1.
        
        anIndexer set &index:anIndex1.
        anIndexer set:anObject2.
    ]
    
    #method indexOf : anObject
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
    [
        #var(type:iterator)aIndexer := self iterator.

        #loop (aIndexer available) ?
        [
            #var(type:int)anIndex.
            aIndexer readIndex &vint:anIndex.
            
            aIndexer set:(anIndexFunc eval &index:anIndex).
            
            aIndexer append &index:1.
        ].
        
        ^ self.
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
    
    #method ascendant
        = (self clone)::indexableOp sort:ifOrdered.
        
    #method Subarray &index:anIndex &length:aLength
    [
        #var(type:iterator) it := self iterator.
        
        ^ Array new &length:aLength set &every: (&index:i) [ it write &index:(anIndex + i) get ].
    ]
}

// --- stringOp ---

#symbol stringOpDispatcher =
{
    eval &literal:aLiteral = literalOp.
    
    eval &wide:aLiteral = wideOp.
}.

#class(extension) stringOp
{
    #method Substring : anIndex
        = self Substring &index:(anIndex int) &length:(self length - anIndex int).
            
    #method Substring : anIndex : aLength
        = self Substring &index:(anIndex int) &length:(aLength int).
            
    #method trimLeft &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimLeft &char:aChar.
    ]

    #method trimRight &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimRight &char:aChar.
    ]

    #method trim &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trim &char:aChar.
    ]

    #method trim
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trim.
    ]

    #method trimLeft
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimLeft.
    ]

    #method trimRight
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher trimRight.
    ]
    
    #method padLeft &char:aChar &length:aLength
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher padLeft &char:aChar &length:aLength.
    ]
    
    #method padRight &char:aChar &length:aLength
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher padRight &char:aChar &length:aLength.
    ]
    
    #method startingWith : s
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher startingWith:s.
    ]
    
    #method endingWith : s
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher endingWith:s.
    ]
    
    #method containing : s
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher containing:s.
    ]
    
    #method replaceFirst &literal:aReplacee &literal:aReplacer
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher replaceFirst &literal:aReplacee &literal:aReplacer.
    ]
    
    #method replace &literal:aReplacee &literal:aReplacer
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher replace &literal:aReplacee &literal:aReplacer.
    ]
    
    #method split &char:aChar
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher split &char:aChar.
    ]
    
    #method split &literal:aValue
    [
        #var dispatcher := self cast:%eval &to:stringOpDispatcher.
        
        ^ self::dispatcher split &literal:aValue.
    ]
    
    #method split &by:aSplitter
    [
        #var(type:literal) aLiteral := self literal.
        
        ^ aSplitter cast:%eval &to:
            {
                eval &char:aChar = aLiteral::stringOp split &char:aChar.
                
                eval &literal:aSplitter = aLiteral::stringOp split &literal:aSplitter.
            }.
    ]
    
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
        #var aPaddedStr := aLine padLeft &char:(aChar char) &length:(aWidth int).

        self write:aPaddedStr.
            
        ^ self.
    ]

    #method write : aLine &paddingRight:aWidth &with:aChar
    [
        #var aPaddedStr := aLine padRight &char:(aChar char) &length:(aWidth int).

        self write:aPaddedStr.
            
        ^ self.
    ]
    
    #method write : aLine &padding:aWidth &with:aChar
    [
        #var(type:int)aLength := aWidth int.
        #var(type:int)aLeftPadding := aLength / 2.
        #var(type:int)aRightPadding := aLength - aLeftPadding.
        
        #var aPaddedStr := aLine 
            padLeft &char:(aChar char) &length:aLeftPadding
            padRight &char:(aChar char) &length:aRightPadding.

        self write:aPaddedStr.
            
        ^ self.
    ]
    
}

// === actions ===

#symbol ifOrdered
    = (:aFormer:aLater) [ aFormer < aLater ].
