#define system.

// === helpers ===

// --- EList ---

#class(role)EList
{
    #method run : aFunction
        = control foreach:self &do:aFunction.
        
    #method seek : aFunction
        = control foreach:self &until:aFunction.

    #method Enumerator
        = IndexedListEnumerator new:self.

    #method getAt : anIndex
    [
        #var anEnumerator := self Enumerator.
        
        control from:0 &to:anIndex &do: &&:n[ anEnumerator next ].
        
        ^ anEnumerator get.
    ]
    
    #method setAt : anIndex : aValue
    [
        #var anEnumerator := self Enumerator.
        
        control from:0 &to:anIndex &do: &&:n[ anEnumerator next ].
        
        anEnumerator set:aValue.
    ]
    
    #method Last
        = self~EList getAt:(self type'length - 1).
    
    #method Array
    [
        ^ system'Array new &type'length::self &function: &&:anIndex [ self~EList getAt:anIndex ].
    ]
}

// --- EArray ---

#class(role) EArray
{
    #method set &function:aFunction
    [
        #var anEnumerator := self Enumerator.
        
        control while:(anEnumerator next) &do:[ anEnumerator set:(aFunction eval:(anEnumerator Index)) ].
    ]
    
    #method exchange:anIndex1:anIndex2
    [
        #var aTemp := self getAt &type'index::anIndex1.
        
        self setAt:anIndex1:(self getAt &type'index::anIndex2).
        self setAt:anIndex2:aTemp.
    ]

    #method Subarray &type'index:anIndex &type'length:aLength
        = Array new &type'length:aLength &function: &&:i [ self@(anIndex + i) ].

    #method sort : aCondition
    [
        #var aCurrent := self Enumerator.
        #var aNext := self Enumerator.

        control while:[ aCurrent next ] &do:
        [
            aNext set &type'index::aCurrent.
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
    
    #method DeepClone
        = Array new &type'length::self &function: &&:i
            [ (self @ i) Clone ].
}

// --- EEnumerator ---

#class(role)EEnumerator
{
    #method run : aFunction
        = control while:[ self next ] &do: [ aFunction:self. ].
}

// --- listControl ---

#symbol listControl =
{
    foreach:aList &do:anAction
        = aList~EList run:anAction.
        
    find:aValue &in:aList
        = aList~EList seek: &&:x [ aValue equal:x ].
        
    seek:aList &for:anAction
        = aList~EList seek:anAction.
        
    ifExist:aValue &in:aList
        = nil != aList~EList seek: &&:x [ aValue equal:x ].
        
    getAt:anIndex &in:aList
        = aList~EList getAt:anIndex.
        
    toArray : aList
        = aList~EList Array.
        
    sort : aList &with:aCondition
    [
        Wrap(EList, aList)~EArray sort:aCondition.
        
        ^ aList.
    ]    
}.

// --- arrayControl ---

#symbol arrayControl =
{
    forEach:anArray &do:aFunction
    [
        anArray~EArray set &function:aFunction.
    ]
        
    exchange:anIndex1 &with:anIndex2 &in:anArray
    [
        anArray~EArray exchange:anIndex1:anIndex2.
    ]
    
    find:aValue &in:anArray
        = anArray~EList seek: &&:x [ aValue equal:x ].
        
    seek:anArray &for:aFunction
        = anArray~EList seek:aFunction.
        
    ifExist:aValue &in:anArray
        = nil != anArray~EList seek: &&:x [ aValue equal:x ].
        
    sort : anArray
        = anArray~EArray sort: &&:aFormer:aLater [ aFormer < aLater ].        
        
    copy : anArray &from:anIndex &length:aLength
        = anArray~EArray Subarray &type'index::anIndex &type'length:aLength.
}.

// --- ELiteralOp ---

#class(role)ELiteralOp
{
    #method trimStart &widechar:aChar
    [
        #var theIndex := Integer new.
        control while:[self getAt &type'index:theIndex == aChar ] &do:[ theIndex += 1. ].
        
        ^ self delete &type'index:0 &type'length:theIndex.
    ]
    
    #method split &widechar:aChar
    [
        #var aList := system'collections'List new.
        #var aStart := 0.
        #var anIndex := 0.
        
        control do:
        [
            anIndex := self indexOf &type'index:aStart &widechar:aChar.
            (-1 != anIndex) ?
            [
                aList += self Substring &type'index:aStart &type'length:(anIndex - aStart).
                aStart := anIndex + 1.
            ]
            ! [
                aList += self Substring &type'index:aStart &type'length:(self type'length - aStart).
            ].
        ]        
        &until:[anIndex == -1].
        
        ^ aList~EList Array.
    ]
    
    #method split &type'widestr:aValue
    [
        #var aList := system'collections'List new.
        #var aStart := 0.
        #var anIndex := 0.
        
        control do:
        [
            anIndex := self indexOf &type'index:aStart &type'widestr:aValue.
            (-1 != anIndex) ?
            [
                aList += self Substring &type'index:aStart &type'length:(anIndex - aStart).
                aStart := anIndex + aValue type'length.
            ]
            ! [
                aList += self Substring &type'index:aStart &type'length:(self type'length - aStart).
            ].
        ]        
        &until:[anIndex == -1].
        
        ^ aList~EList Array.
    ]
    
    #method Array
        = self~EList Array.
}

// --- stringControl ---

#symbol stringControl =
{
    split:aLiteral &by:aChar
        = { 
            eval &type'widestr:aSplitter [ ^ aLiteral~ELiteralOp split &type'widestr:aSplitter. ] 
            
            eval &widechar:aSplitter [ ^ aLiteral~ELiteralOp split &widechar:aSplitter. ] 
          } eval::aChar.
    
    trimStart:aChar &in:aString
        = aString << aString Literal ~ELiteralOp trimStart &widechar:aChar.
        
    toArray : aLiteral
        = aLiteral~ELiteralOp Array.
        
    starting : anS &with:aSubS
        = 0 == anS indexOf &type'index:0 &type'widestr::aSubS.
        
    ending : anS &with:aSubS
        = (anS Length - aSubS Length) == anS indexOf &type'index:0 &type'widestr::aSubS.
        
    contain : aSubS &in:anS
        = -1 != anS indexOf &type'index:0 &type'widestr::aSubS.
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
        $self~EFunction reverse &for:(Integer new) &from:0 &till:aCounter &by:1.
    ]
}

