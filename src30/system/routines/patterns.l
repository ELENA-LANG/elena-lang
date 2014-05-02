#define system.

// === extensions ===

//// --- misc ---
//
//#class(role)misc
//{
//    #method back : anObject = anObject.
//}

// --- indexable ---

#class(role) indexable
{
    #method set &each:aFunction
    [
        #var anEnumerator := self enumerator.
        
        control while:(anEnumerator next) &do:[ anEnumerator set:(aFunction eval:(anEnumerator index)) ].
        
        ^ self.
    ]
    
    #method exchange:anIndex1:anIndex2
    [
        #var aTemp := self @ anIndex1.
        
        self setAt:anIndex1:(self @ anIndex2).
        self setAt:anIndex2:aTemp.
    ]

    #method Subarray &index:anIndex &length:aLength
        = Array new &length:aLength ~indexable set &each: i [ self@(anIndex + i) ].

    #method sort : aCondition
    [
        #var aCurrent := self enumerator.
        #var aNext := self enumerator.

        control while:[ aCurrent next ] &do:
        [
            aNext set &index::aCurrent.
            control while:[ aNext next ] &do:
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
            [ (self @ i) ~indexable array ].
}

// --- literalOp ---

#class(role)literalOp
{
    #method indexOf &literal:aLiteral
        = self indexOf &index:0 &literal:aLiteral.

    #method Substring &index:anIndex
    [
        #var(type:int)aLength := self length.
        aLength := aLength - anIndex.
        
        ^ self Substring &index:anIndex &length:aLength.
    ]
     
    #method trimStart &char:aChar
    [
        #var theIndex := Integer new.
        control while:[self getAt &index:theIndex == aChar ] &do:[ theIndex += 1. ].
        
        ^ self delete &index:0 &length:theIndex.
    ]
    
    #method split &char:aChar
    [
        #var aList := system'collections'List new.
        #var aStart := 0.
        #var anIndex := 0.
        
        control do:
        [
            anIndex := self indexOf &index:aStart &char:aChar.
            (-1 != anIndex) ?
            [
                aList += self Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + 1.
            ]
            ! [
                aList += self Substring &index:aStart &length:(self length - aStart).
            ].
        ]        
        &until:[anIndex == -1].
        
        ^ aList~indexable array.
    ]
    
    #method split &literal:aValue
    [
        #var aList := system'collections'List new.
        #var aStart := 0.
        #var anIndex := 0.
        
        control do:
        [
            anIndex := self indexOf &index:aStart &literal:aValue.
            (-1 != anIndex) ?
            [
                aList += self Substring &index:aStart &length:(anIndex - aStart).
                aStart := anIndex + aValue length.
            ]
            ! [
                aList += self Substring &index:aStart &length:(self length - aStart).
            ].
        ]        
        &until:[anIndex == -1].
        
        ^ aList~indexable array.
    ]
}

#class(role)textOp
{
    #method write : aLiteral &length:aCounter
    [
        #var(type:int)aNumber := aCounter.
        control while:[aNumber > 0] &do:
            [ self write:aLiteral. aNumber := aNumber - 1. ].
            
        ^ self.
    ]
    
    #method write : aLine &paddingLeft:aWidth &with:aChar
    [
        #var aLiteral := aLine literal.
        #var aLength := aLiteral length.        
        (aLength < aWidth)
            ? [ self~textOp write:aChar &length:(aWidth - aLength). ].
            
        self write::aLiteral.
            
        ^ self.
    ]

    #method write : aLine &paddingRight:aWidth &with:aChar
    [
        #var aLiteral := aLine literal.
        
        self write::aLiteral.
        
        #var aLength := aLiteral length.        
        (aLength < aWidth)
            ? [ self~textOp write:aChar &length:(aWidth - aLength). ].
            
        ^ self.            
    ]

    #method write : aLine &padding:aWidth &with:aChar
    [
        #var aLiteral := aLine literal.
        #var aLength := aLiteral length.        
        (aLength < aWidth)
            ? [
                #var aLen := aWidth - aLength.
                #var aLeftLen := aLen / 2.
                
                self~textOp write:aChar &length:aLeftLen.
                self write::aLiteral.
                self ~textOp write:aChar &length:(aLen - aLeftLen).
            ]
            ! [ self write::aLiteral. ].
            
        ^ self.
    ]
}

// --- enumerable ---

#class(role)enumerable
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
        
        control from:0 &to:anIndex &do: n[ anEnumerator next ].
        
        ^ anEnumerator get.
    ]
    
    #method setAt : anIndex : aValue
    [
        #var anEnumerator := self enumerator.
        
        control from:0 &to:anIndex &do: n[ anEnumerator next ].
        
        anEnumerator set:aValue.
    ]
    
    #method Last
        = self~enumerable getAt:(self length - 1).
    
    #method array
    [
        #var aListIndexer := EnumerationIterator new &enumerable:self.
        
        ^ Array new &length::self ~indexable set &each:anIndex[ aListIndexer set &index:anIndex get ].
    ]
}

//// --- EEnumerator ---
//
//#class(role)EEnumerator
//{
//    #method run : aFunction
//        = control while:[ self next ] &do: [ aFunction:self. ].
//}

// === helpers ===

// --- listControl ---

#symbol listControl =
{
    foreach:aList &do:anAction
        = aList~enumerable run:anAction.

    find:aValue &in:aList
        = aList~enumerable find:aValue.

    seek:aList &until:anAction
        = aList~enumerable seek:anAction.

    ifExist:aValue &in:aList
        = nil != aList~enumerable seek: x [ aValue equal:x ].
        
//    getAt:anIndex &in:aList
//        = aList~EList getAt:anIndex.
//        
//    toArray : aList
//        = aList~EList Array.
        
    sort : aList &with:aCondition
    [
        #var aCurrent := EnumerationIterator new &enumerable:aList.
        #var aNext := EnumerationIterator new &enumerable:aList.

        control while:[ aCurrent next ] &do:
        [
            aNext set &index::aCurrent.
            control while:[ aNext next ] &do:
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
        listControl sort:aList &with:(:aFormer:aLater) [ aFormer < aLater ].
        
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
        = anArray~indexable sort: (:aFormer:aLater) [ aFormer < aLater ].        
        
    Subarray : anArray &from:anIndex &length:aLength
        = anArray~indexable Subarray &index::anIndex &length:aLength.
}.

// --- literalControl ---

#symbol literalControl =
{
    split:aLiteral &by:aSplitter
        = { 
            eval &literal:aSplitter [ ^ aLiteral~literalOp split &literal:aSplitter. ] 
            
            eval &char:aSplitter [ ^ aLiteral~literalOp split &char:aSplitter. ] 
          } eval::aSplitter.
    
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
    
    #method for : aCounter
    [
        $self~functionOp rewind &range &int:aCounter &int:1 &int:1.
    ]
}

