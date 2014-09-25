#define system.

// === actions ===

#symbol ifOrdered
    = (:aFormer:aLater) [ aFormer < aLater ].


// === extensions ===

//// --- misc ---
//
//#class(role)misc
//{
//    #method back : anObject = anObject.
//}

// --- indexable ---

#class(extension:indexable) indexable
{
    #method set &each:aFunction
    [
        #var anEnumerator := self enumerator.

        #loop (anEnumerator next) ?
        [
            anEnumerator set:(aFunction eval:(anEnumerator index)).
        ].
        
        ^ self.
    ]
    
    #method exchange:anIndex1:anIndex2
    [
        #var aTemp := self @ anIndex1.
        
        self setAt:anIndex1:(self @ anIndex2).
        self setAt:anIndex2:aTemp.
    ]
    
    #method exchange &int:anIndex1 &int:anIndex2
    [
        #var aTemp := self @ anIndex1.
        
        self setAt &index:anIndex1 &object:(self @ anIndex2).
        self setAt &index:anIndex2 &object:aTemp.
    ]

    #method Subarray &index:anIndex &length:aLength
        = Array new &length:aLength ~indexable set &each: i [ self@(anIndex + i) ].

    #method sort : aCondition
    [
        #var aCurrent := self enumerator.
        #var aNext := self enumerator.

        #loop (aCurrent next) ?
        [
            aNext set &index::aCurrent.
            #loop (aNext next) ?
            [
                aCondition eval:(aCurrent get):(aNext get)
                    ! [
                        #var aTmp := aCurrent get.
                        aCurrent set:(aNext get).
                        aNext set:aTmp.
                    ].
            ].
        ].
        
        ^ self.
    ]

    #method array
        = Array new &length::self ~indexable set &each: i [ self @ i ].

    #method deepClone
        = Array new &length::self ~indexable set &each: i
            [ (self @ i) clone ].
}

// --- literalOp ---

#class(role,extension:literal)literalOp
{
//    #method indexOf &literal:aLiteral
//        = self indexOf &index:0 &literal:aLiteral.

    #method Substring &index:anIndex
    [
        #var(type:int)aLength.
        self read &out'length:aLength.
        
        aLength := aLength - anIndex.
        
        ^ self Substring &index:anIndex &length:aLength.
    ]
     
    #method trimStart &char:aChar
    [
        #var(type:int)anIndex.
        #loop (self getAt &index:anIndex == aChar) 
            ? [ anIndex := anIndex + 1. ].
        
        ^ self delete &index:0 &length:anIndex.
    ]
    
    #method split &char:aChar
    [
        #var aList := system'collections'List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            self indexOf &index:aStart &char:aChar &out'int:anIndex.
            (-1 != anIndex) ?
            [
                aList += self Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + 1.
            ]
            ! [
                aList += self Substring &index:aStart &length:(self length - aStart).
            ].
        ].
        
        ^ aList~indexable array.
    ]
    
    #method split &literal:aValue
    [
        #var aList := system'collections'List new.
        #var(type:int)aStart := 0.
        #var(type:int)anIndex := 0.
        
        #loop (anIndex >= 0)?        
        [
            self indexOf &index:aStart &literal:aValue &out'int:anIndex.
            (-1 != anIndex) ?
            [
                aList += self Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + aValue length.
            ]
            ! [
                aList += self Substring &index:aStart &length:(self length - aStart).
            ].
        ].
        
        ^ aList~indexable array.
    ]

    #method replace &first_occ &literal:aReplacee &literal:aReplacer
    [
        #var(type:int)anIndex. 
        self indexOf &index:0 &literal:aReplacee &out'int:anIndex.
        
        ^ (anIndex != -1)
            ? [ ^ self delete &index:anIndex &length::aReplacee insert &index:anIndex &literal:aReplacee. ]
            ! [ self ].
    ]
        
    #method replace &all_occ &literal:aReplacee &literal:aReplacer
    [
        #var aTarget := String new.
        #var(type:int)aStart := 0.
        #var(type:int)anEnd := 0.
        self indexOf &index:aStart &literal:aReplacee &out'int:anEnd.
        
        #loop (anEnd != -1) ?
        [
            aTarget 
                append &literal::(self Substring &index:aStart &length:(anEnd - aStart))
                append &literal:aReplacer.
            
            aStart := anEnd + aReplacee length.
            self indexOf &index:aStart &literal:aReplacee &out'int:anEnd.
        ].
    
        ^ (aStart != 0)
            ? [ aTarget append:(self~literalOp Substring &index:aStart) literal ]
            ! [ self literal ].
    ]
    
    #method isEmpty
    [
        #var(type:int)aLength.
        self read &out'length:aLength.
        
        ^ aLength == 0.
    ]
    
    #method isNotEmpty
    [
        #var(type:int)aLength.
        self read &out'length:aLength.
        
        ^ aLength != 0.
    ]
}

#class(role)stringOp
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
        #var aLiteral := aLine literal.
        
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.
        
        (aLength < aWidthInt)
            ? [ self~stringOp write:aChar &length:(aWidthInt - aLength). ].
            
        self write::aLiteral.
            
        ^ self.
    ]

    #method write : aLine &paddingRight:aWidth &with:aChar
    [
        #var aLiteral := aLine literal.
        #var(type:int)aWidthInt := aWidth int.
        
        self write::aLiteral.
                    
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.
        
        (aLength < aWidthInt)
            ? [ self~stringOp write:aChar &length:(aWidthInt - aLength). ].
            
        ^ self.
    ]

    #method write : aLine &padding:aWidth &with:aChar
    [
        #var aLiteral := aLine literal.
        #var(type:int)aWidthInt := aWidth int.
        
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.        
        
        (aLength < aWidth)
            ? [
                #var(type:int)aLen := aWidthInt - aLength.
                #var(type:int)aLeftLen := aLen / 2.
                
                self~stringOp write:aChar &length:aLeftLen.
                self write::aLiteral.
                self ~stringOp write:aChar &length:(aLen - aLeftLen).
            ]
            ! [ self write::aLiteral. ].
            
        ^ self.
    ]
}

// --- enumerable ---

#class(extension:enumerable)enumerable
{
    #method run : aFunction
        = control foreach:self &do:aFunction.
        
    #method seek : aFunction
        = control foreach:self &until:aFunction.

    #method find : aValue
        = control foreach:self &until: x [ aValue equal:x ].
        
    #method getAt : anIndex
    [
        #var anEnumerator := self enumerator.
        
        control forrange &int:0 &int::anIndex &do: (&int:n)[ anEnumerator next ].
        
        ^ anEnumerator get.
    ]
    
    #method setAt : anIndex : aValue
    [
        #var anEnumerator := self enumerator.
        
        control forrange &int:0 &int::anIndex &do: (&int:n)[ anEnumerator next ].
        
        anEnumerator set:aValue.
    ]
    
    #method Last
        = self~enumerable getAt:(self length - 1).
    
    #method array
    [
        #var aListIndexer := EnumerationIterator new &enumerable:self.
        
        ^ Array new &length::self ~indexable set &each:anIndex[ aListIndexer set &index:(anIndex int) get ].
    ]
}

////// --- EEnumerator ---
////
////#class(role)EEnumerator
////{
////    #method run : aFunction
////        = control while:[ self next ] &do: [ aFunction:self. ].
////}

// === helpers ===

// --- listControl ---

#symbol listControl =
{
//    foreach:aList &do:anAction
//        = aList~enumerable run:anAction.

    find:aValue &in:aList
        = aList~enumerable find:aValue.

    seek:aList &until:anAction
        = aList~enumerable seek:anAction.

    ifExist:aValue &in:aList
        = nil != aList~enumerable find:aValue.
        
////    getAt:anIndex &in:aList
////        = aList~EList getAt:anIndex.
////        
////    toArray : aList
////        = aList~EList Array.
        
    sort : aList &with:aCondition
    [
        #var aCurrent := EnumerationIterator new &enumerable:aList.
        #var aNext := EnumerationIterator new &enumerable:aList.

        #loop (aCurrent next)?
        [
            aNext set &index::aCurrent.
            
            #loop (aNext next)?
            [
                aCondition eval:(aCurrent get):(aNext get)
                    ! [
                        #var aTmp := aCurrent get.
                        aCurrent set:(aNext get).
                        aNext set:aTmp.
                    ].
            ].
        ].
        
        ^ aList.
    ]    

    sort : aList
    [
        listControl sort:aList &with:ifOrdered.
        
        ^ aList.
    ]

    toArray : anEnumerable
        = anEnumerable ~enumerable array.
}.

// --- arrayControl ---

#symbol arrayControl =
{
    new &length:aLength &each:aFunction
        = Array new &length:aLength ~indexable set &each:aFunction.
    
    toArray : anIndexable
        = anIndexable ~indexable array.
    
//    forEach:anArray &do:aFunction
//    [
//        anArray~EArray set &function:aFunction.
//    ]
        
    exchange:anIndex1 &with:anIndex2 &in:anArray
    [
        anArray~indexable exchange:anIndex1:anIndex2.
    ]
    
    find:aValue &in:anArray
        = anArray~enumerable seek: x [ aValue equal:x ].

    seek:anArray &until:aFunction
        = anArray~enumerable seek:aFunction.
        
    ifExist:aValue &in:anArray
        = nil != anArray~enumerable seek: x [ aValue equal:x ].
        
    sort : anArray
        = anArray~indexable sort: ifOrdered.
        
    Subarray : anArray &from:anIndex &length:aLength
        = anArray~indexable Subarray &index:(anIndex int) &length:aLength.
}.

// --- literalControl ---

#symbol literalControl =
{
    split:aLiteral &by:aSplitter
        = { 
            eval &literal:aSplitter [ ^ aLiteral~literalOp split &literal:aSplitter. ] 
            
            eval &char:aSplitter [ ^ aLiteral~literalOp split &char:aSplitter. ] 
          } 
          eval::aSplitter.
    
//    trimStart:aChar &in:aString
//        = aString << aString Literal ~ELiteralOp trimStart &widechar:aChar.

    toArray : aLiteral
        = aLiteral ~indexable array.

    starting : anS &with:aSubS
        = 0 == anS indexOf &index:0 &literal::aSubS.
        
    ending : anS &with:aSubS
        = (anS length - aSubS length) == anS indexOf &index:0 &literal::aSubS.
        
    contain : aSubS &in:anS
        = -1 != anS indexOf &index:0 &literal::aSubS.
}.

// === patterns ===

// --- BasePattern ---

#class BasePattern
{
    #method foreach : anEnumerable
    [
        control foreach:anEnumerable &do:self.
    ]
}

// --- Summing ---

#class Summing : BasePattern
{
    #field theVariable.
    
    #constructor new : aVariable
    [
        theVariable := aVariable.
    ]
    
    #method eval : anItem
    [
        theVariable += anItem.
    ]
    
    #method => theVariable.
}

// --- DownCounter ---

#class DownCounter
{
    #field theFunction.
    
    #constructor new : aFunction
    [
        theFunction := aFunction.
    ]
    
    #method eval : aCounter
    [
        theFunction eval:aCounter.
    ]
    
    #method eval &int:aCounter
    [
        theFunction eval:aCounter.
    ]
    
    #method for : aCounter
    [
        $self~functionOp rewind &range &int::aCounter &int:1 &int:1.
    ]
}

