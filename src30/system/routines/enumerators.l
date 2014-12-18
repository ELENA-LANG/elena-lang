#define system.
#define system'collections.

#define(const)enumerable = system'routines'enumerable.

// --- arrayOp ---

#class(extension:array)arrayOp
{
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

////    #method indexOf &index:anIndex &object:anObject &out'int:aRetVal
////        <arr_indexOf>.
////        
////    #method reallocate &length:aNewLength
////        <arr_realloc>.
////
////    #method getAt &index:anIndex
////        <arrptr_get>.
////
////    #method setAt &index:anIndex &object:anObject
////        <arrptr_set>.
////        
////    #method read &out'length:aLength
////        <arrptr_getLen>.        
////        
////    #method length
////    [
////        #var(type:int)aCount.
////        self~arrayOp read &out'length:aCount.
////        
////        ^ IntNumber new &int:aCount.
////    ]
}

// --- literalOp ---

#class(extension:literal)literalOp
{
//    #method indexOf &literal:aLiteral
//        = self indexOf &index:0 &literal:aLiteral.

    #method Substring &index:anIndex
    [
        #var(type:literal)aLiteral := self literal.        
        #var(type:int)aLength := aLiteral length.
        
        aLength := aLength - anIndex.
        
        ^ aLiteral Substring &index:anIndex &length:aLength.
    ]
     
//    #method trimLeft &char:aChar
//    [
//        #var(type:int)anIndex := 0.
//        #loop (self getAt &index:anIndex == aChar) 
//            ? [ anIndex := anIndex + 1. ].
//        
//        ^ self delete &index:0 &length:anIndex.
//    ]
//     
//    #method trimRight &char:aChar
//    [
//        #var(type:int)aLength.
//        self read &out'length:aLength.
//        
//        #var(type:int)anIndex := aLength - 1.
//        
//        #loop (self getAt &index:anIndex == aChar) 
//            ? [ anIndex := anIndex - 1. ].
//        
//        ^ self delete &index:anIndex &length:(aLength - anIndex).
//    ]
//    
//    #method trim &char:aChar
//        = (self~literalOp trimLeft &char:aChar)~literalOp trimRight &char:aChar.
//    
//    #method padLeft &char:aChar &length:aLength
//    [
//        #var aLiteral := self literal.
//        
//        #var(type:int)aCurrentLength.
//        aLiteral read &out'length:aCurrentLength.
//
//        #loop (aLength > aCurrentLength)?
//            [ aLiteral := aChar literal + aLiteral. aCurrentLength := aCurrentLength + 1. ].
//            
//        ^ aLiteral.
//    ]
//    
//    #method padRight &char:aChar &length:aLength
//    [
//        #var aLiteral := self literal.
//        
//        #var(type:int)aCurrentLength.
//        aLiteral read &out'length:aCurrentLength.
//
//        #loop (aLength > aCurrentLength)?
//            [ aLiteral := aLiteral + aChar. aCurrentLength := aCurrentLength + 1. ].
//            
//        ^ aLiteral.
//    ]
    
    #method split &char:aChar
    [
        #var(type:literal)aLiteral := self literal.        
        #var aList := system'collections'List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &index:aStart &char:aChar &out'int:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + 1.
            ]
            ! [
                aList += aLiteral Substring &index:aStart &length:(self length - aStart).
            ].
        ].
        
        ^ aList~literalOp array.
    ]
    
    #method split &literal:aValue
    [
        #var(type:literal)aLiteral := self literal.        
        #var aList := system'collections'List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            aLiteral indexOf &index:aStart &literal:aValue &out'int:anIndex.
            (-1 != anIndex) ?
            [
                aList += aLiteral Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + aValue length.
            ]
            ! [
                aList += aLiteral Substring &index:aStart &length:(aLiteral length - aStart).
            ].
        ].
        
        ^ aList~enumerable array.
    ]

////    #method replace &first_occ &literal:aReplacee &literal:aReplacer
////    [
////        #var(type:int)anIndex. 
////        self indexOf &index:0 &literal:aReplacee &out'int:anIndex.
////        
////        ^ (anIndex != -1)
////            ? [ ^ self delete &index:anIndex &length::aReplacee insert &index:anIndex &literal:aReplacee. ]
////            ! [ self ].
////    ]
////        
////    #method replace &all_occ &literal:aReplacee &literal:aReplacer
////    [
////        #var aTarget := String new.
////        #var(type:int)aStart := 0.
////        #var(type:int)anEnd := 0.
////        self indexOf &index:aStart &literal:aReplacee &out'int:anEnd.
////        
////        #loop (anEnd != -1) ?
////        [
////            aTarget 
////                append &literal::(self Substring &index:aStart &length:(anEnd - aStart))
////                append &literal:aReplacer.
////            
////            aStart := anEnd + aReplacee length.
////            self indexOf &index:aStart &literal:aReplacee &out'int:anEnd.
////        ].
////    
////        ^ (aStart != 0)
////            ? [ aTarget append:(self~literalOp Substring &index:aStart) literal ]
////            ! [ self literal ].
////    ]
////
////    #method UpperCase
////        = characterController toUpperCase &literal::self.
////
////    #method LowerCase
////        = characterController toLowerCase &literal::self.

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

//    #method array
//    [
//        #var(type:int)aLength := self length.
//        #var(type:array)anArray := Array new &length:aLength.
//        
//        anArray set &each: (&index:i) [ self getAt &index:i ].
//        
//        ^ anArray.
//    ]
}

// --- Enumerable ---

#class(extension:enumerable) enumerable
{
//    #method run : aFunction
//        = control foreach:self &do:aFunction.
        
//    #method seek : aFunction
//        = control run:self &seeking:aFunction.

//    #method find : aValue
//        = control run:self &seeking: x [ aValue equal:x ].

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

    #method array
    [
        #var(type:list)aList := List new.
        control run:self &forEach: anItem [ aList append:anItem ].
        
        #var(type:array) anArray := Array new &length::aList.
        #var(type:enumerator)anEnumerator := aList enumerator.
        
        anArray set &every: (&index:n) [ anEnumerator next. ^ anEnumerator get. ].

        ^ anArray.
    ]
}

// --- indexable ---

#class(extension:indexable) indexable
{
//    #method set &each:anIndexFunc
//    [
//        #var anEnumerator := self enumerator.
//
//        #loop (anEnumerator next) ?
//        [
//            anEnumerator set:(anIndexFunc eval &index::anEnumerator).
//        ].
//        
//        ^ self.
//    ]
    
    #method exchange:anIndex1:anIndex2
    [
        self~indexable exchange &index:(anIndex1 int) &index:(anIndex2 int).
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

//    #method Subarray &index:anIndex &length:aLength
//        = Array new &length:aLength ~indexable set &each: i [ self@(anIndex + i) ].

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

//    #method array
//    [
//        #var(type:array)anArray := Array new &length::self.
//        
//        anArray set &each: (&index:i) [ self getAt &index:i ].
//        
//        ^ anArray.
//    ]
//
}

//// --- EnumerationIterator ---
//
//#class EnumerationIterator
//{
//    #field theList.
//    #field theEnumerator.
//    #field(type:intvar) theIndex.
//
//    #constructor new &enumerable:aList
//    [
//        theList := aList.
//        theEnumerator := aList enumerator.
//        
//        theIndex:= Integer new &int:-1.
//    ]
//
//    #method index = theIndex int.
//
//    #method read &out'index : anIndex
//    [
//        theIndex read &out'int:anIndex.
//    ]
//    
//    #method set &index:anIndex
//    [
//        #var(type:int)aCurrent.
//        theIndex read &out'int:aCurrent.
//        
//        anIndex < aCurrent ?
//        [
//            aCurrent := -1.
//            theEnumerator := theList enumerator.
//        ].
//        
//        #loop (aCurrent < anIndex)?
//        [
//            theEnumerator next
//                ! [ #throw OutOfRangeException new:"Index out of the range". ].
//            
//            aCurrent := aCurrent + 1.
//        ].
//        
//        theIndex write &int:aCurrent.
//    ]
//            
//    #method get
//    [
//        ^ theEnumerator get.
//    ]
//    
//    #method set : anObject
//    [
//        theEnumerator set:anObject        
//    ]
//
//    #method next
//    [
//        theIndex append &int:1.
//        
//        ^ theEnumerator next.
//    ]
//}
//
////// --- TopEnumerator ---
////
////#class TopEnumerator
////{
////    #field theEnumerator.
////    #field theCounter.
////    #field theMaxValue.
////    
////    #constructor new : anEnumerable &max:aMaxValue
////    [
////        theEnumerator := anEnumerable enumerator.
////        theCounter := Integer new:aMaxValue.
////        theMaxValue := aMaxValue.
////    ]
////    
////    #method reset
////    [
////        $super reset.
////        
////        theCounter << theMaxValue.
////    ]
////    
////    #method next
////        = (theCounter > 0)
////            ? [ theCounter -= 1. ^ theEnumerator next. ]
////            ! [ false ].
////    
////    #method => theEnumerator.
////}