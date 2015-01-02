// === basic memory classes ===

// --- BaseByteArray ---

#class(stringof:1) BaseByteArray
{
    #constructor new &length : aLength
        => system'internal'bs_create.
}

// --- ByteArray ---

#class(sealed) ByteArray :: BaseByteArray
{
    #method bytearray = $self.

    #method write &index:anIndex &short:anInt
        => system'internal'bs_writeword.

    #method write &index:anIndex &int:anInt
        => system'internal'bs_writedword.

    #method write &index:anIndex &long:aLong
        => system'internal'bs_writeqword.

    #method write &index:anIndex &length:aLength &bytearray:aDump
        => system'internal'bs_write.

    #method read &index:anIndex &out'short:aChar
        => system'internal'bs_readword.

    #method read &index:anIndex &out'int:anInt
        => system'internal'bs_readdword.

    #method read &index:anIndex &out'long:aLong
        => system'internal'bs_readqword.

    #method read &index:anIndex &length:aLength &bytearray:aDump
        => system'internal'bs_read.

    #method read &out'length:aLength
        => system'internal'bs_getlen.

    #method equal &index:anIndex &int:aValue
    [
        #var(type:int)anInt := $self read &index:anIndex.
        
        ^ anInt equal &int:aValue.
    ]

    #method equal &index:anIndex &short:aValue
    [
        #var(type:short)anInt := $self read &index:anIndex.
        
        ^ anInt equal &short:aValue.
    ]
        
    #method length
    [
        #var(type:int)aLength := $self length.
        
        ^ IntNumber new &int:aLength.
    ]
}

// --- ShortArray ---

#class(sealed,stringof:2) ShortArray
{
    #constructor new &length : aLength
        => system'internal'sa_create.
    
    #method equal &index:anIndex &short:aValue
        => system'internal'ws_equalword.
        
    #method indexOf &index:anIndex &length:aLength &short:aLiteral &out'int:aRetVal
        => system'internal'sa_indexof.
        
    #method write &index:anIndex &short:aValue
        => system'internal'sa_writeword.
        
    #method write &index:anIndex &length:aLength &literal:aLiteral
        => system'internal'sa_write.
        
    #method write &index:anIndex &length:aLength &shortarray:anArray
        => system'internal'sa_write.
        
    #method read &index:anIndex &length:aLength &shortarray:anArray
        => system'internal'sa_read.
        
    #method read &out'length:aLength
        => system'internal'sa_getlen.
        
    #method read &index:anIndex &out'short:aValue
        => system'internal'sa_readword.
        
    #method length
    [
        #var(type:int)aLength := $self length.
        
        ^ IntNumber new &int:aLength.
    ]

    #method shortarray = $self.
}

// --- emptyliteralvalue ---

#symbol(const,type:literal) emptyLiteralValue = "".

// --- LiteralValue ---

#class(sealed) LiteralValue :: BaseValue
{
    #field(type:shortarray)theValue.

    #constructor new &length:aSize 
        => system'internal'ws_create.

    #constructor new &length:aSize &literal:aLiteral
        => system'internal'ws_copy.

    #constructor new &length:aSize &shortarray:anArray
        => system'internal'ws_copyarr.

    #constructor new &length:aSize &index:anIndex &shortarray:anArray
        => system'internal'ws_copysubarr.

    #constructor new &length:aSize &short:aChar
        => system'internal'ws_fill.

    #constructor new &literal:aLiteral
        <= %new &length:(aLiteral length) &literal:aLiteral.

    #method literal = $self.

    #method equal &literal : aLiteral
        => system'internal'ws_equal.

    #method less &literal : aLiteral
        => system'internal'ws_less.

    #method(type:literal) add &literal : aLiteral
        => system'internal'ws_add.

    #method(type:literal) add &char:aChar
        = $self add &literal:(aChar literal).

    #method add : anObject
        = $self add &literal:(anObject literal).

    #method getAt : aParam
        <= %getAt &index:(aParam int).

    #method read &index:anIndex &out'short:aChar
        => system'internal'ws_getat.

    #method getAt &index:anIndex
    [
        #var(type:short)aChar := $self read &index:anIndex.
        
        ^ CharValue new &short:aChar.
    ]

    #method indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal
        => system'internal'ws_indexof.

    #method indexOf &index:anIndex &char:aChar &out'int:aRetVal
    [
        #var(type:int)anIndex := $self indexOf &index:anIndex &literal:(aChar literal).
        
        aRetVal := anIndex.
    ]

    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal := $self indexOf &index:anIndex &literal:aLiteral.
        
        ^ IntNumber new &int:aRetVal.
    ]

//    #method indexOf &index:anIndex &char:aChar &out'int:aRetVal
//        <= %indexOf &index:anIndex &literal:aChar &out'int:aRetVal.

    #method indexOf &index:anIndex &char:aChar
        <= %indexOf &index:anIndex &literal:(aChar literal).

    #method(type:literal) insert &index:anIndex &literal:aLiteral
        => system'internal'ws_insert.

    #method(type:literal) delete &index:anIndex &length:aLength
        => system'internal'ws_delete.

    #method(type:literal) Substring &index:anIndex &length:aLength
        => system'internal'ws_subs.

    #method read &out'length:aLength
        => system'internal'ws_getlen.

    #method length
    [
        #var(type:int)aLength := $self length.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = system'LiteralEnumerator new &literal:$self.
    
    #method iterator = system'LiteralIterator new &literal:$self.
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb literal:$self.
    
    #method clone = LiteralValue new &literal:$self.
}

// --- String ---

#class String :: BaseValue
{
    #field(type:literal)theValue.

    #constructor new
    [
        theValue := emptyLiteralValue.
    ]
            
    #constructor new : aValue
    [
        theValue := aValue literal.
    ]
    
    #constructor new &literal:aValue
    [
        theValue := aValue.
    ]
    
    #method write : anOperand
    [
        theValue := anOperand literal.
    ]
    
    #method write &literal:aLiteral
    [
        theValue := aLiteral.
    ]
    
    #method append : anOperand
    [
        theValue := theValue add &literal:(anOperand literal).
    ]
    
    #method append &literal:anOperand
    [
        theValue := theValue add &literal:anOperand.
    ]
    
    #method insert &index:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &literal:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &literal:aLiteral. ].
    ]
            
    #method delete &index:anIndex &length:aLength
    [
        theValue := theValue delete &index:anIndex &length:aLength.
    ]
                        
    #method clear
    [
        theValue := emptyLiteralValue.
    ]
    
    #method clone = String new &literal:theValue.
                
    #method cast:aVerb &to:aTarget = aTarget::aVerb literal:theValue.
    
    #method literal = theValue.
    
    #method value = theValue.
    
    #method => theValue.
}

// --- Variant ---

#class Variant :: String
{
    #method int
    [
        #var(type:int)retVal := intConvertor convert &literal:theValue.
        
        ^ IntNumber new &int:retVal.
    ]

    #method long
    [
        #var(type:long)retVal := longConvertor convert &literal:theValue.
        
        ^ LongNumber new &long:retVal.
    ]

    #method real
    [
        #var(type:real)retVal := realConvertor convert &literal:theValue.
        
        ^ RealNumber new &real:retVal.
    ]

    #method literal = theValue.
}

// --- BaseArray ---

#class(dynamic) BaseArray
{
    #constructor new &length:aLength
        => system'internal'arr_create.
}

// --- Array ---

#class(sealed) Array :: BaseArray
{
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    #method getAt &index:anIndex
        => system'internal'arr_getat.

    #method setAt:anIndex:anObject
    [
        $self setAt &index:(anIndex int) &object:anObject.
    ]
    
    #method setAt &index:anIndex &object:anObject
        => system'internal'arr_setat.

    #method add : anObject
        = $self add &array:(anObject array).

    #method add &array:anArray
    [
        #var(type:int)aLength1 := $self length.
        
        #var(type:int)aLength2 := anArray length.
        //anArray read &out'length:aLength2.
        
        #var aRetVal := Array new &length:(aLength1 + aLength2).

        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength1) ?
        [
            aRetVal setAt &index:anIndex &object:($self getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].

        anIndex := 0.
        #loop (anIndex < aLength2) ?
        [
            aRetVal setAt &index:(aLength1 + anIndex) &object:(anArray getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]

    #method array = $self.

    #method read &out'length:aLength
        => system'internal'arr_getlen.

    #method length
    [
        #var(type:int)aLength := $self length.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = system'ArrayEnumerator new &array:$self.

    #method iterator = system'ArrayIterator new &array:$self.

    #method cast : aVerb &to:aTarget = aTarget::aVerb array:$self.

    #method clone 
    [
        #var(type:int)aLength := $self length.
        
        #var aRetVal := Array new &length:aLength.
        
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            aRetVal setAt &index:anIndex &object:($self getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
            
        ^ aRetVal.
    ]
}

// --- OpenArgs ---

#class(sealed,dynamic)OpenArgs
{
    #method read &out'length:aLength
        => system'internal'args_getlen.
        
    #method length
    [
        #var(type:int)aLength := $self length.
        
        ^ IntNumber new &int:aLength.
    ]
}

// === Enumerators / Indexers

// --- LiteralEnumerator ---

#class LiteralEnumerator :: Enumerator
{
    #field(type:literal) theLiteral.
    #field(type:intvar)  theIndex.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
    ]

    #method reset
    [
        theIndex write &int:-1.
    ]

    #method next
    [
        #var(type:int)aLength := theLiteral length.
        
        theIndex append &int:1.

        ^ theIndex less &int:aLength.
    ]
    
    #method get
    [
        #var(type:int)anIndex := theIndex int.
        
        #var(type:short)aChar := theLiteral read &index:anIndex.
        
        ^ CharValue new &short:aChar.
    ]
}

// --- LiteralIterator ---

#class LiteralIterator :: Iterator
{
    #field(type:literal) theLiteral.
    #field(type:intvar)  theIndex.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength := theLiteral length.
        
        ^ theIndex less &int:aLength.
    ]

    #method get
    [
        #var(type:int)anIndex := theIndex int.        
        #var(type:short)aChar := theLiteral read &index:anIndex.
        
        ^ CharValue new &short:aChar.
    ]
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    #method read &out'index:anIndex
    [
        #var(type:int)aValue := theIndex int.
        
        anIndex := aValue.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- ArrayIterator ---

#class ArrayIterator :: Iterator
{
    #field(type:array) theArray.
    #field(type:intvar) theIndex.

    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength := theArray length.
        
        ^ theIndex less &int:aLength.
    ]

    #method get
    [
        #var(type:int)anIndex := theIndex int.
        
        ^ theArray getAt &index:anIndex.
    ]
    
    #method set : anObject
    [
        #var(type:int)anIndex := theIndex int.
        
        theArray setAt &index:anIndex &object:anObject.
    ]
    
    #method read &out'index : anIndex
    [
        #var(type:int)aValue := theIndex int.
        
        anIndex := aValue.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- ArrayEnumerator ---

#class ArrayEnumerator :: Enumerator
{
    #field(type:array)  theArray.
    #field(type:intvar) theIndex.
    
    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new &int:(-1).
    ]

    #method reset
    [
        theIndex write &int:-1.
    ]

    #method next
    [
        #var(type:int)aLength := theArray length.
        
        theIndex append &int:1.

        ^ theIndex less &int:aLength.
    ]
    
    #method get
    [
        #var(type:int)anIndex := theIndex int.
        
        ^ theArray getAt &index:anIndex.
    ]
}
