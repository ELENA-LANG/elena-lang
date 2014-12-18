#define system.

// === actions ===

#symbol ifOrdered
    = (:aFormer:aLater) [ aFormer < aLater ].

//// === extensions ===
//
//// --- objectOp ---
//
//#class(role,extension)objectOp
//{
////    #method back : anObject = anObject.
//
//    #method isNil
//        = nil == self.
//        
//    #method isLiteral
//        = self cast:literalDetector.
//        
//    #method isInteger
//        = self cast:intDetector.
//        
//    #method is &numeric
//        = self cast:numericDetector.
//}

////#class(role)stringOp
////{
////    #method write : aLiteral &length:aCounter
////    [
////        #var(type:int)aNumber := aCounter.
////        
////        #loop (aNumber > 0) ?
////        [
////            self write:aLiteral. 
////            
////            aNumber := aNumber - 1.
////        ].
////            
////        ^ self.
////    ]
////    
////    #method write : aLine &paddingLeft:aWidth &with:aChar
////    [
////        #var(type:int)aWidthInt := aWidth int.
////        #var aLiteral := aLine literal.
////        
////        #var(type:int)aLength.
////        aLiteral read &out'length:aLength.
////        
////        (aLength < aWidthInt)
////            ? [ self~stringOp write:aChar &length:(aWidthInt - aLength). ].
////            
////        self write::aLiteral.
////            
////        ^ self.
////    ]
////
////    #method write : aLine &paddingRight:aWidth &with:aChar
////    [
////        #var aLiteral := aLine literal.
////        #var(type:int)aWidthInt := aWidth int.
////        
////        self write::aLiteral.
////                    
////        #var(type:int)aLength.
////        aLiteral read &out'length:aLength.
////        
////        (aLength < aWidthInt)
////            ? [ self~stringOp write:aChar &length:(aWidthInt - aLength). ].
////            
////        ^ self.
////    ]
////
////    #method write : aLine &padding:aWidth &with:aChar
////    [
////        #var aLiteral := aLine literal.
////        #var(type:int)aWidthInt := aWidth int.
////        
////        #var(type:int)aLength.
////        aLiteral read &out'length:aLength.        
////        
////        (aLength < aWidth)
////            ? [
////                #var(type:int)aLen := aWidthInt - aLength.
////                #var(type:int)aLeftLen := aLen / 2.
////                
////                self~stringOp write:aChar &length:aLeftLen.
////                self write::aLiteral.
////                self ~stringOp write:aChar &length:(aLen - aLeftLen).
////            ]
////            ! [ self write::aLiteral. ].
////            
////        ^ self.
////    ]
////}
//
//// --- enumerable ---
//
//#class(extension:enumerable)enumerable
//{
////    #method getAt : anIndex
////    [
////        #var anEnumerator := self enumerator.
////        
////        control forrange &int:0 &int::anIndex &do: (&int:n)[ anEnumerator next ].
////        
////        ^ anEnumerator get.
////    ]
////    
////    #method setAt : anIndex : aValue
////    [
////        #var anEnumerator := self enumerator.
////        
////        control forrange &int:0 &int::anIndex &do: (&int:n)[ anEnumerator next ].
////        
////        anEnumerator set:aValue.
////    ]
////    
////    #method Last
////        = self~enumerable getAt:(self length - 1).
//    
//    #method array
//    [
//        #var aListIndexer := EnumerationIterator new &enumerable:self.
//        
//        #var(type:array)anArray := Array new &length::self.
//
//        anArray set &each: (&index:anIndex) [ aListIndexer set &index:anIndex get ].
//        
//        ^ anArray.
//    ]
//}
//
////// --- EEnumerator ---
////
////#class(role)EEnumerator
////{
////    #method run : aFunction
////        = control while:[ self next ] &do: [ aFunction:self. ].
////}

// === helpers ===

// --- listControl ---

#class(extension:control) listControl
{
////    foreach:aList &do:anAction
////        = aList~enumerable run:anAction.

    #method find:aValue &in:aList
        = control run:aList &seeking: x [ aValue equal:x ].

//    seek:aList &until:anAction
//        = aList~enumerable seek:anAction.

    #method ifExist:aValue &in:aList
        = nil != (control run:aList &seeking: x [ aValue equal:x ]).

////    getAt:anIndex &in:aList
////        = aList~EList getAt:anIndex.
////        
////    toArray : aList
////        = aList~EList Array.
//        
//    sort : aList &with:aCondition
//    [
//        #var aCurrent := EnumerationIterator new &enumerable:aList.
//        #var aNext := EnumerationIterator new &enumerable:aList.
//
//        #loop (aCurrent next)?
//        [
//            aNext set &index::aCurrent.
//            
//            #loop (aNext next)?
//            [
//                aCondition eval:(aCurrent get):(aNext get)
//                    ! [
//                        #var aTmp := aCurrent get.
//                        aCurrent set:(aNext get).
//                        aNext set:aTmp.
//                    ].
//            ].
//        ].
//        
//        ^ aList.
//    ]    
//
//    sort : aList
//    [
//        listControl sort:aList &with:ifOrdered.
//        
//        ^ aList.
//    ]
}

// --- arrayControl ---

#symbol arrayControl =
{
    new &length:aLength &every:aFunction
        = Array new &length:aLength ~arrayOp set &every:aFunction.
    
//    toArray : anIndexable
//        = anIndexable ~indexable array.
//    
////    forEach:anArray &do:aFunction
////    [
////        anArray~EArray set &function:aFunction.
////    ]
//        
//    exchange:anIndex1 &with:anIndex2 &in:anArray
//    [
//        anArray~indexable exchange:anIndex1:anIndex2.
//    ]
//    
//    find:aValue &in:anArray
//        = anArray~enumerable seek: x [ aValue equal:x ].
//
//    seek:anArray &until:aFunction
//        = anArray~enumerable seek:aFunction.
//        
//    ifExist:aValue &in:anArray
//        = nil != anArray~enumerable seek: x [ aValue equal:x ].
//        
//    sort : anArray
//        = anArray~indexable sort: ifOrdered.
//        
//    Subarray : anArray &from:anIndex &length:aLength
//        = anArray~indexable Subarray &index:(anIndex int) &length:aLength.
}.

// --- literalControl ---

#symbol literalControl =
{
    split:aLiteral &by:aSplitter
        = aSplitter cast:
            { 
                is &numeric = false.
            
                eval &literal:aSplitter [ ^ aLiteral~literalOp split &literal:aSplitter. ]
            
                eval &char:aSplitter [ ^ aLiteral~literalOp split &char:aSplitter. ] 
            }.
    
//    trimStart:aChar &in:aString
//        = aString << aString Literal ~ELiteralOp trimStart &widechar:aChar.

//    toArray : aLiteral
//        = aLiteral ~indexable array.
//
//    starting : anS &with:aSubS
//        = 0 == anS indexOf &index:0 &literal::aSubS.
//        
//    ending : anS &with:aSubS
//        = (anS length - aSubS length) == anS indexOf &index:0 &literal::aSubS.
//        
//    contain : aSubS &in:anS
//        = -1 != anS indexOf &index:0 &literal::aSubS.
}.

////// === patterns ===
////
////// --- BasePattern ---
////
////#class BasePattern
////{
////    #method foreach : anEnumerable
////    [
////        control foreach:anEnumerable &do:self.
////    ]
////}
////
////// --- Summing ---
////
////#class Summing : BasePattern
////{
////    #field theVariable.
////    
////    #constructor new : aVariable
////    [
////        theVariable := aVariable.
////    ]
////    
////    #method eval : anItem
////    [
////        theVariable += anItem.
////    ]
////    
////    #method => theVariable.
////}
////
////// --- DownCounter ---
////
////#class DownCounter
////{
////    #field theFunction.
////    
////    #constructor new : aFunction
////    [
////        theFunction := aFunction.
////    ]
////    
////    #method eval : aCounter
////    [
////        theFunction eval:aCounter.
////    ]
////    
////    #method eval &int:aCounter
////    [
////        theFunction eval:aCounter.
////    ]
////    
////    #method for : aCounter
////    [
////        $self~functionOp rewind &range &int::aCounter &int:1 &int:1.
////    ]
////}
////
