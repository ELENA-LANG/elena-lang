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
    #constructor new &length : aLength
        => system'internal'bs_create.
    
    #constructor new &length:aLength &bytearray:anArray
        => system'internal'bs_copy.
    
    #constructor new &length:aLength &index:anIndex &bytearray:anArray
        => system'internal'bs_copysubarr.
    
    #method bytearray = $self.

    #method readLength &vint:aLength
        => system'internal'bs_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method fill &index:anIndex &length:aLength &byte:aValue
    [
        #var(type:int)aTotalLength.
        $self readLength &vint:aTotalLength.
        
        #var(type:int)i := anIndex.
        #var(type:int)m := anIndex + aLength.
        
        (i > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
        
        (m > aTotalLength)
            ? [ #throw system'InvalidArgumentException new. ].
            
        #loop (i < m)?
        [
            $self write &index:i &byte:aValue.
            
            i := i + 1.
        ].
    ]

    #method read &index:anIndex &length:aLength &bytearray:aDump
        => system'internal'bs_read.

    #method write &index:anIndex &length:aLength &bytearray:aDump
        => system'internal'bs_write.

    #method write &index:anIndex &byte:anInt
        => system'internal'bs_writebyte.

    #method read &index:anIndex &vbyte:anInt
        => system'internal'bs_readbyte.

    #method write &index:anIndex &short:anInt
        => system'internal'bs_writeword.

    #method read &index:anIndex &vshort:aChar
        => system'internal'bs_readword.

    #method write &index:anIndex &char:anInt
        => system'internal'bs_writedword.

    #method read &index:anIndex &vchar:aChar
        => system'internal'bs_readdword.

    #method write &index:anIndex &int:anInt
        => system'internal'bs_writedword.

    #method read &index:anIndex &vint:anInt
        => system'internal'bs_readdword.

    #method write &index:anIndex &long:aLong
        => system'internal'bs_writeqword.

    #method read &index:anIndex &vlong:aLong
        => system'internal'bs_readqword.

    #method write &index:anIndex &real:aLong
        => system'internal'bs_writeqword.

    #method read &index:anIndex &vreal:aLong
        => system'internal'bs_readqword.

    #method getAt : anIndex
    [
        #var(type:byte)aRetVal.
        $self read &index:(anIndex index) &vbyte:aRetVal.
        
        ^ ByteNumber new &byte:aRetVal.
    ]        

    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &byte:(aValue byte).
    ]
                        
    #method clone = ByteArray new &length:($self length) &bytearray:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &bytearray:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &bytearray:$self.
}

// --- ShortArray ---

#class(sealed,stringof:2) ShortArray
{
    #constructor new &length : aLength
        => system'internal'sa_create.
    
    #constructor new &length:aLength &shortarray:anArray
        => system'internal'sa_copy.
    
    #constructor new &length:aLength &index:anIndex &shortarray:anArray
        => system'internal'sa_copysubarr.

    #method shortarray = $self.

    #method readLength &vint:aLength
        => system'internal'sa_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method read &index:anIndex &length:aLength &shortarray:anArray
        => system'internal'sa_read.
        
    #method write &index:anIndex &length:aLength &shortarray:anArray
        => system'internal'sa_write.

    #method write &index:anIndex &short:aValue
        => system'internal'sa_writeword.

    #method read &index:anIndex &vshort:aValue
        => system'internal'sa_readword.

    #method getAt : anIndex
    [
        #var(type:short)aRetVal.
        $self read &index:(anIndex index) &vshort:aRetVal.
        
        ^ ShortNumber new &short:aRetVal.
    ]        

    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &short:(aValue short).
    ]

    #method clone = ShortArray new &length:($self length) &shortarray:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &shortarray:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &shortarray:$self.
}

// --- BaseIntArray ---

#class(stringof:4) BaseIntArray
{
    #constructor new &length : aLength
        => system'internal'ns_create.
}

// --- IntArray ---

#class(sealed) IntArray :: BaseIntArray
{
    #constructor new &length : aLength
        => system'internal'ns_create.

    #constructor new &length:aLength &intarray:anArray
        => system'internal'ns_copy.
    
    #constructor new  &length:aLength &index:anIndex &intarray:anArray
        => system'internal'ns_copysubarr.
        
    #method readLength &vint:aLength
        => system'internal'ns_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method intarray = $self.

    #method read &index:anIndex &length:aLength &intarray:anArray
        => system'internal'ns_read.

    #method write &index:anIndex &length:aLength &intarray:anArray
        => system'internal'ns_write.

    #method write &index:anIndex &int:aValue
        => system'internal'ns_writedword.

    #method read &index:anIndex &vint:anInt
        => system'internal'ns_readdword.

    #method getAt : anIndex
    [
        #var(type:int)aRetVal.
        $self read &index:(anIndex index) &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]        

    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &int:(aValue int).
    ]

    #method clone = IntArray new &length:($self length) &intarray:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &intarray:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &intarray:$self.    
}

// --- IntArray ---

#class(sealed) CharArray :: BaseIntArray
{
    #constructor new &length : aLength
        => system'internal'ns_create.

    #constructor new  &length:aLength &index:anIndex &chararray:anArray
        => system'internal'ns_copysubarr.
        
    #method readLength &vint:aLength
        => system'internal'ns_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method chararray = $self.

    #method read &index:anIndex &length:aLength &chararray:anArray
        => system'internal'ns_read.

    #method write &index:anIndex &length:aLength &chararray:anArray
        => system'internal'ns_write.

    #method write &index:anIndex &char:aValue
        => system'internal'ns_writedword.

    #method read &index:anIndex &vchar:anInt
        => system'internal'ns_readdword.

    #method move &index:anIndex &length:aLength &offset:anOffset
        => system'internal'ns_move.

    #method getAt : anIndex
    [
        #var(type:char)aRetVal.
        $self read &index:(anIndex index) &vchar:aRetVal.
        
        ^ CharValue new &char:aRetVal.
    ]        

    #method setAt : anIndex : aValue
    [
        $self write &index:(anIndex int) &char:(aValue char).
    ]

    #method clone = CharArray new &length:($self length) &index:0 &chararray:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &chararray:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &chararray:$self.    
}
        
// --- emptyliteralvalue ---

#symbol(const,type:literal) emptyLiteralValue = "".

// --- LiteralValue ---

#class(sealed,string) LiteralValue :: BaseValue
{
    #field(type:bytearray)theValue.

    #constructor min
        => system'internal's_empty.
        
    #constructor new &length:aSize 
        => system'internal's_create.

    #constructor new &length:aSize &literal:aLiteral
        => system'internal's_copy.
           
    #constructor new &length:aSize &char:aChar
        => system'internal's_fill.

    #constructor new &literal:aLiteral
        <= %new &length:(aLiteral length) &literal:aLiteral.
        
    #constructor new &length:aSize &index:anIndex &chararray:anArray
        => system'internal's_copysubarr.

    #constructor $new &length:aSize &index:anIndex &bytearray:anArray
        => system'internal's_copybuf.

    #method literal = $self.
    
    #method wide = wideConvertor convert &literal:$self.

    #method(type:bool) equal &literal : aLiteral
        => system'internal's_equal.

    #method(type:bool) less &literal : aLiteral
        => system'internal's_less.

    #method(type:bool) equal &char : aChar
        <= %equal &literal:(aChar literal).

    #method(type:bool) less &char : aChar
        <= %less &literal:(aChar literal).

    #method(type:literal) add &literal : aLiteral
        => system'internal's_add.

    #method(type:literal) add &char:aChar
        = $self add &literal:(aChar literal).

    #method(type:literal) add : anObject
        = $self add &literal:(anObject literal).

    #method getAt : aParam
        <= %getAt &index:(aParam int).

    #method read &index:anIndex &vchar:aChar
        => system'internal's_getat.

    #method read &index:anIndex &vbyte:anInt
        => system'internal'bs_readbyte.
        
    #method(type:char) getAt &index:anIndex
    [
        #var(type:char)aChar.
        $self read &index:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    #method indexOf &index:anIndex &literal:aLiteral &vint:aRetVal
        => system'internal's_indexof.

    #method indexOf &index:anIndex &char:aChar &vint:aRetVal
        => system'internal's_indexof_char.        

    #method indexOf &index:anIndex &char:aChar
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &literal:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(type:literal) insert &index:anIndex &literal:aLiteral
        => system'internal's_insert.

    #method(type:literal) insert &index:anIndex &char:aChar
        = $self insert &index:anIndex &literal:(aChar literal).

    #method(type:literal) delete &index:anIndex &length:aLength
        => system'internal's_delete.

    #method(type:literal) Substring &index:anIndex &length:aLength
        => system'internal's_subs.

    #method readLength &vint:aLength
        => system'internal's_getlen.

    #method save &index:anIndex &chararray:anArray &vlength:aRetVal
        => system'internal's_save.

    #method $save &index:anIndex &bytearray:anArray &length:aRetVal
        => system'internal's_saveraw.

    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = LiteralEnumerator new &literal:$self.
    
    #method iterator = LiteralIterator new &literal:$self.
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &literal:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &literal:$self.
    
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
    
    #constructor new &char:aValue
    [
        theValue := aValue literal.
    ]
    
    #method write : anOperand
    [
        theValue := anOperand literal.
    ]
    
    #method write &literal:aLiteral
    [
        theValue := aLiteral.
    ]
    
    #method write &char:aChar
    [
        theValue := aChar literal.
    ]
    
    #method append : anOperand
    [
        theValue := theValue add &literal:(anOperand literal).
    ]
    
    #method append &literal:anOperand
    [
        theValue := theValue add &literal:anOperand.
    ]
    
    #method append &char:anOperand
    [
        theValue := theValue add &literal:(anOperand literal).
    ]
    
    #method insert &index:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &literal:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &literal:aLiteral. ].
    ]
    
    #method insert &index:anIndex &char:aChar
    [
        (anIndex == 0)
            ? [ theValue := aChar literal add &literal:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &literal:(aChar literal). ].
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
                
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &literal:theValue.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &literal:theValue.
    
    #method literal = theValue.
    
    #method value = theValue.
    
    #method => theValue.
}

// --- emptyWideLiteralValue ---

#symbol(const,type:wide) emptyWideLiteralValue = #0 wide. // !! temporal

// --- WideLiteralValue ---

#class(sealed,widestring) WideLiteralValue :: BaseValue
{
    #field(type:shortarray)theValue.

    #constructor min
        => system'internal'ws_empty.
        
    #constructor new &length:aSize 
        => system'internal'ws_create.

    #constructor new &length:aSize &wide:aLiteral
        => system'internal'ws_copy.
    
    #constructor new &length:aSize &char:aChar
        => system'internal'ws_fill.

    #constructor new &wide:aLiteral
        <= %new &length:(aLiteral length) &wide:aLiteral.
        
    #constructor new &length:aSize &index:anIndex &chararray:anArray
        => system'internal'ws_copysubarr.

    #constructor $new &length:aSize &index:anIndex &shortarray:anArray
        => system'internal'ws_copybuf.

    #method wide = $self.

    #method literal = literalConvertor convert &wide:$self.

    #method(type:bool) equal &wide : aLiteral
        => system'internal'ws_equal.

    #method(type:bool) less &wide : aLiteral
        => system'internal'ws_less.

    #method(type:bool) equal &char : aChar
        <= %equal &wide:(aChar wide).

    #method(type:bool) less &char : aChar
        <= %less &wide:(aChar wide).

    #method(type:wide) add &wide : aLiteral
        => system'internal'ws_add.

    #method(type:wide) add &char:aChar
        = $self add &wide:(aChar wide).

    #method(type:wide) add : anObject
        = $self add &wide:(anObject wide).

    #method getAt : aParam
        <= %getAt &index:(aParam int).

    #method read &index:anIndex &vchar:aChar
        => system'internal'ws_getat.

    #method read &index:anIndex &vshort:aValue
        => system'internal'sa_readword.
        
    #method(type:char) getAt &index:anIndex
    [
        #var(type:char)aChar.
        $self read &index:anIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]

    #method indexOf &index:anIndex &wide:aLiteral &vint:aRetVal
        => system'internal'ws_indexof.

    #method indexOf &index:anIndex &char:aChar &vint:aRetVal
        => system'internal'ws_indexof_char.

    #method indexOf &index:anIndex &char:aChar
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &char:aChar &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method indexOf &index:anIndex &wide:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &wide:aLiteral &vint:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method(type:wide) insert &index:anIndex &wide:aLiteral
        => system'internal'ws_insert.

    #method(type:wide) insert &index:anIndex &char:aChar
        = $self insert &index:anIndex &wide:(aChar wide).

    #method(type:wide) delete &index:anIndex &length:aLength
        => system'internal'ws_delete.

    #method(type:wide) Substring &index:anIndex &length:aLength
        => system'internal'ws_subs.

    #method readLength &vint:aLength
        => system'internal'ws_getlen.

    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method save &index:anIndex &chararray:anArray &vlength:aLength
        => system'internal'ws_save.

    #method $save &index:anIndex &shortarray:anArray &length:aRetVal
        => system'internal'ws_saveraw.

    #method enumerator = WideLiteralEnumerator new &wide:$self.
    
    #method iterator = WideLiteralIterator new &wide:$self.
    
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &wide:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &wide:$self.
    
    #method clone = WideLiteralValue new &wide:$self.
}

// --- WideString ---

#class WideString :: BaseValue
{
    #field(type:wide)theValue.

    #constructor new
    [
        theValue := emptyWideLiteralValue.
    ]
            
    #constructor new : aValue
    [
        theValue := aValue wide.
    ]
    
    #constructor new &wide:aValue
    [
        theValue := aValue.
    ]
    
    #constructor new &char:aValue
    [
        theValue := aValue wide.
    ]
    
    #method write : anOperand
    [
        theValue := anOperand wide.
    ]
    
    #method write &wide:aLiteral
    [
        theValue := aLiteral.
    ]
    
    #method write &char:aChar
    [
        theValue := aChar wide.
    ]
    
    #method append : anOperand
    [
        theValue := theValue add &wide:(anOperand wide).
    ]
    
    #method append &wide:anOperand
    [
        theValue := theValue add &wide:anOperand.
    ]
    
    #method append &char:anOperand
    [
        theValue := theValue add &wide:(anOperand wide).
    ]
    
    #method insert &index:anIndex &wide:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral add &wide:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &wide:aLiteral. ].
    ]
    
    #method insert &index:anIndex &char:aChar
    [
        (anIndex == 0)
            ? [ theValue := aChar wide add &wide:theValue. ]
            ! [ theValue := theValue insert &index:anIndex &wide:(aChar wide). ].
    ]
            
    #method delete &index:anIndex &length:aLength
    [
        theValue := theValue delete &index:anIndex &length:aLength.
    ]
                        
    #method clear
    [
        theValue := emptyWideLiteralValue.
    ]
    
    #method clone = WideString new &wide:theValue.
                
    #method cast:aVerb &to:aTarget = aTarget::aVerb eval &wide:theValue.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &wide:theValue.
    
    #method wide = theValue.
    
    #method value = theValue.
    
    #method => theValue.
}

// --- Variant ---

#class Variant :: String
{
    #method byte
    [
        #var(type:byte)retVal.
        byteConvertor convert &literal:theValue &vbyte:retVal.
        
        ^ ByteNumber new &byte:retVal.
    ]

    #method short
    [
        #var(type:short)retVal.
        shortConvertor convert &literal:theValue &vshort:retVal.
        
        ^ ShortNumber new &short:retVal.
    ]

    #method char
    [
        #var(type:char)retVal.
        charConvertor $convert &index:0 &literal:theValue &vchar:retVal.
        
        ^ CharValue new &char:retVal.
    ]

    #method int
    [
        #var(type:int)retVal.
        intConvertor convert &literal:theValue &vint:retVal.
        
        ^ IntNumber new &int:retVal.
    ]

    #method long
    [
        #var(type:long)retVal.
        longConvertor convert &literal:theValue &vlong:retVal.
        
        ^ LongNumber new &long:retVal.
    ]

    #method real
    [
        #var(type:real)retVal.
        realConvertor convert &literal:theValue &vreal:retVal.
        
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
    #constructor min
        => system'internal'arr_empty.
    
    #constructor new &length:aLength
        => system'internal'arr_create.
    
    #constructor new &length:aLength &array:anArray
        <= (%new &length:aLength)
    [
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            $self setAt &index:anIndex &object:(anArray getAt &index:anIndex).
            
            anIndex := anIndex + 1.
        ].
    ]
    
    #constructor new &length:aLength &index:anIndex &array:anArray
        <= (%new &length:aLength)
    [
        #var(type:int)i := 0.
        #loop (i < aLength) ?
        [
            $self setAt &index:i &object:(anArray getAt &index:(anIndex + i)).
            
            i := i + 1.
        ].
    ]
    
    #constructor new &array:anArray
        <= %new &length:(anArray length) &array:anArray.
        
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
        #var(type:int)aLength1.
        #var(type:int)aLength2.
        
        $self readLength &vint:aLength1.        
        anArray readLength &vint:aLength2.
        
        #var(type:array)aRetVal := Array new &length:(aLength1 + aLength2).

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

    #method readLength &vint:aLength
        => system'internal'arr_getlen.

    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = ArrayEnumerator new &array:$self.

    #method iterator = ArrayIterator new &array:$self.

    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &array:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &array:$self.

    #method clone 
        = Array new &array:$self.
}

// --- OpenArgs ---

#class(sealed,dynamic)OpenArgs
{
    #method readLength &vint:aLength
        => system'internal'args_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
}

// === Enumerators / Indexers

// --- LiteralEnumerator ---

#class LiteralEnumerator :: Enumerator
{
    #field(type:literal) theLiteral.
    #field(type:vint)    theIndex.
    #field(type:vint)    theDiff.
    #field(type:char)    theChar.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
        theDiff := Integer new &int:1.
        theChar := #0.
    ]

    #method reset
    [
        theIndex write &int:-1.
        theDiff write &int:1.
    ]

    #method next
    [
        #var(type:int)aLength.
        
        theLiteral readLength &vint:aLength.
        
        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &index:theIndex.                
                
                theChar readLength &vint:theDiff.
                
                ^ true.
            ].

        theDiff write &int:0.
        theChar := #0.

        ^ false.
    ]
    
    #method get = theChar.
}

// --- LiteralIterator ---

#class LiteralIterator :: Iterator
{
    #field(type:literal) theLiteral.
    #field(type:vint)    theIndex.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theLiteral readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
    [
        #var(type:char)aChar.
        theLiteral read &index:theIndex &vchar:aChar.
        
        ^ CharValue new &char:aChar.
    ]
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    #method readIndex &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- LiteralEnumerator ---

#class WideLiteralEnumerator :: Enumerator
{
    #field(type:wide) theLiteral.
    #field(type:vint) theIndex.
    #field(type:vint) theDiff.
    #field(type:char) theChar.
    
    #constructor new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new &int:(-1).
        theDiff := Integer new &int:1.
        theChar := #0.
    ]

    #method reset
    [
        theIndex write &int:-1.
        theDiff write &int:1.
    ]

    #method next
    [
        #var(type:int)aLength.
        
        theLiteral readLength &vint:aLength.

        theIndex += theDiff.
        
        (theIndex < aLength)
            ? [
                theChar := theLiteral getAt &index:theIndex.
                
                theChar readLength &vint:theDiff.
                theDiff += 1.
                theDiff /= 2.
                
                ^ true.
            ].

        theDiff write &int:0.
        theChar := #0.

        ^ false.
    ]
    
    #method get = theChar.
}

// --- WideLiteralIterator ---

#class WideLiteralIterator :: Iterator
{
    #field(type:wide) theLiteral.
    #field(type:vint) theIndex.
    
    #constructor new &wide:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theLiteral readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
        = theLiteral getAt &index:theIndex.
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    #method readIndex &vint:anIndex
    [
        anIndex << theIndex.
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
    #field(type:vint)  theIndex.

    #constructor new &array:anArray
    [
        theArray := anArray.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
        = theArray getAt &index:theIndex.
    
    #method set : anObject
    [
        theArray setAt &index:theIndex &object:anObject.
    ]
    
    #method readIndex  &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- ArrayEnumerator ---

#class ArrayEnumerator :: Enumerator
{
    #field(type:array) theArray.
    #field(type:vint)  theIndex.
    
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
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        theIndex append &int:1.

        ^ theIndex < aLength.
    ]
    
    #method get
        = theArray getAt &index:theIndex.
}
