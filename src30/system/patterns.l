#define system.

// === helpers ===

// --- IndexedListEnumerator ---

#class IndexedListEnumerator
{
    #field theList.
    #field theEnumerator.
    #field theIndex.
    
    #constructor new : aList
    [
        theEnumerator := aList Enumerator.
        theIndex:= Integer new &type'int:-1.
    ]
    
    #method reset
    [
        theIndex << -1.
        
        theEnumerator reset.
    ]
    
    #method next
    [
        theIndex += 1.
        
        ^ theEnumerator next.
    ] 

    #method get => theEnumerator.
            
    #method set : aValue
    [
        theEnumerator set:aValue.
    ]
            
    #method set &Index:anIndex
    [
        theIndex > anIndex ?
        [
            $self reset.
        ].
        
        control while:[ theIndex < anIndex ] &do:
        [
            theEnumerator next
                ! [ #throw OutOfRangeException new:"Index out of the range". ].
            
            theIndex += 1.
        ].
    ]
    
    #method Index
        = theIndex Number.
}

// --- EList ---

#class(role)EList
{
    #method run : anAction
        = control foreach:self &do:anAction.
        
    #method seek : anAction
        = control foreach:self &until:anAction.

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
        = self~EList getAt:(self Count - 1).
    
    #method Array
    [
        ^ system'Array new &type'length:(self Length) &action: &&:anIndex [ self~EList getAt:anIndex ].
    ]
}

// --- EArray ---

#class(role) EArray
{
    #method set &action:anAction
    [
        #var anEnumerator := self Enumerator.
        
        control while:(anEnumerator next) &do:[ anEnumerator set:(anAction eval:(anEnumerator Index)) ].
    ]
    
    #method exchange:anIndex1:anIndex2
    [
        #var aTemp := self getAt &type'index::anIndex1.
        
        self setAt:anIndex1:(self getAt &type'index::anIndex2).
        self setAt:anIndex2:aTemp.
    ]

    #method sort : aCondition
    [
        #var aCurrent := self Enumerator.
        #var aNext := self Enumerator.

        control while:[ aCurrent next ] &do:
        [
            aNext set &Index:(aCurrent Index).
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
        = Array new &type'length::self &action: &&:i
            [ (self @ i) Clone ].
}

// --- listControl ---

#symbol listControl =
{
    foreach:aList &do:anAction
        = aList~EList run:anAction.
        
    foreach : aList &top:aMaxValue &do:anAction
    [
        #var aCounter := Integer new::aMaxValue.
        
        control foreach:aList &until:[ aCounter -= 1. ^ aCounter > -1. ] &do:anAction.
    ]
    
    find:aValue &in:aList
        = aList~EList seek: &&:x [ aValue equal:x ].
        
    seek:aList &for:anAction
        = aList~EList seek:anAction.
        
    exist:aValue &in:aList
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
    forEach:anArray &do:anAction
    [
        anArray~EArray set &action:anAction.
    ]
        
    exchange:anIndex1 &with:anIndex2 &in:anArray
    [
        anArray~EArray exchange:anIndex1:anIndex2.
    ]
    
    sort : anArray
        = anArray~EArray sort: &&:aFormer:aLater [ aFormer < aLater ].
}.

// --- ELiteralOp ---

#class(role)ELiteralOp
{
    #method trimStart &type'char:aChar
    [
        #var theIndex := Integer new.
        control while:[self getAt &type'index:theIndex == aChar ] &do:[ theIndex += 1. ].
        
        ^ self delete &type'index:0 &type'length:theIndex.
    ]
    
    #method split &type'char:aChar
    [
        #var aList := system'collections'List new.
        #var aStart := 0.
        #var anIndex := 0.
        
        control do:
        [
            anIndex := self indexOf &type'char:aChar &type'index:aStart.
            (-1 != anIndex) ?
            [
                aList += self Substring &type'index:aStart &type'length:(anIndex - aStart).
                aStart := anIndex + 1.
            ]
            ! [
                aList += self Substring &type'index:aStart &type'length:(self type'length - aStart).
            ].
        ]        
        &until:[anIndex != -1].
        
        ^ aList~EList Array.
    ]
    
    #method split &type'widestr:aValue
    [
        #var aList := system'collections'List new.
        #var aStart := 0.
        #var anIndex := 0.
        
        control do:
        [
            anIndex := self indexOf &type'widestr:aValue &type'index:aStart.
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
            
            eval &type'widechar:aSplitter [ ^ aLiteral~ELiteralOp split &type'widechar:aSplitter. ] 
          } eval::aChar.
    
    trimStart:aChar &in:aString
        = aString << aString Literal ~ELiteralOp trimStart &type'char:aChar.
        
    toArray : aLiteral
        = aLiteral~ELiteralOp Array.
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