// === basic memory classes ===

// --- BaseByteArray ---

#class(stringof:1) BaseByteArray
{
    #constructor new &length : aLength
        <bs_create>.
}

// --- ByteArray ---

#class(sealed,type:bytearray) ByteArray : BaseByteArray
{
    #method bytearray = $self.

//    #method write &index:anIndex &short:anInt
//        <bs_writeword>.
//
//    #method write &index:anIndex &int:anInt
//        <bs_writedword>.
//
//    #method write &index:anIndex &long:aLong
//        <bs_writeqword>.
//
//    #method write &index:anIndex &length:aLength &bytearray:aDump
//        <bs_write>.
//
////    #method write &index:anIndex &literal:aLiteral
////        <bs_writestr>.

    #method read &index:anIndex &out'short:aChar
        <bs_readword>.

    #method read &index:anIndex &out'int:anInt
        <bs_readdword>.

//    #method read &index:anIndex &out'long:aLong
//        <bs_readqword>.
//
//    #method read &index:anIndex &length:aLength &bytearray:aDump
//        <bs_read>.

//    #method indexOf &index:anIndex &length:aLength &byte:aByte &out'int:aRetVal
//        <bs_findbyte>.me
//
//    #method indexOf &index:anIndex &length:aLength &short:aWord &out'int:aRetVal
//        <bs_findshort>.

    #method equal &index:anIndex &short:aValue
        <bs_equalword>.

    #method equal &index:anIndex &int:aValue
        <bs_equalint>.

    #method read &out'length:aLength
        <bs_getlen>.
        
    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
}

// --- ShortArray ---

#class(sealed,type:shortarray,stringof:2) ShortArray
{
    #constructor new &length : aLength
        <sa_create>.
    
    #method equal &index:anIndex &short:aValue
        <ws_equalword>.
        
    #method indexOf &index:anIndex &length:aLength &short:aLiteral &out'int:aRetVal
        <sa_indexof>.
        
    #method write &index:anIndex &short:aValue
        <sa_writeword>.
        
    #method write &index:anIndex &length:aLength &literal:aLiteral
        <sa_write>.
        
    #method write &index:anIndex &length:aLength &shortarray:anArray
        <sa_write>.
        
//    #method read &index:anIndex &length:aLength &shortarray:anArray
//        <sa_read>.
        
    #method read &out'length:aLength
        <sa_getlen>.
        
    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method shortarray = $self.
}

// --- emptyliteralvalue ---

#symbol(const) emptyLiteralValue = "".

// --- LiteralValue ---

#class(type:literal,sealed) LiteralValue : BaseValue
{
    #field(type:shortarray)theValue.

    #constructor new &length:aSize 
        <ws_create>.

    #constructor new &length:aSize &literal:aLiteral
        <ws_copy>.

    #constructor new &length:aSize &shortarray:anArray
        <ws_copyarr>.

    #constructor new &length:aSize &index:anIndex &shortarray:anArray
        <ws_copysubarr>.

    #constructor new &length:aSize &short:aChar
        <ws_fill>.

    #constructor new &literal:aLiteral
        <= %new &length::aLiteral &literal:aLiteral.

    #method literal = $self.

    #method equal &literal : aLiteral
        <ws_equal>.

    #method less &literal : aLiteral
        <ws_less>.

    #method add : aLiteral
        = $self add &literal::aLiteral.

    #method add &literal : aLiteral
        <ws_add>.

    #method getAt : aParam
        <= %getAt &index:(aParam int).

    #method read &index:anIndex &out'short:aChar
        <ws_getat>.

    #method getAt &index:anIndex
    [
        #var(type:short)aChar.
        $self read &index:anIndex &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]

    #method indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal
        <ws_indexof>.

    #method indexOf &index:anIndex &literal:aLiteral
    [
        #var(type:int)aRetVal.
        $self indexOf &index:anIndex &literal:aLiteral &out'int:aRetVal.
        
        ^ IntNumber new &int:aRetVal.
    ]

    #method indexOf &index:anIndex &char:aChar &out'int:aRetVal
        <= %indexOf &index:anIndex &literal::aChar &out'int:aRetVal.

    #method indexOf &index:anIndex &char:aChar
        <= %indexOf &index:anIndex &literal::aChar.

    #method insert &index:anIndex &literal:aLiteral
        <ws_insert>.

    #method delete &index:anIndex &length:aLength
        <ws_delete>.

    #method Substring &index:anIndex &length:aLength
        <ws_subs>.

    #method read &out'length:aLength
        <ws_getlen>.

    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = system'LiteralEnumerator new &literal:$self.
    
    #method iterator = system'LiteralIterator new &literal:$self.
    
    #method cast : aTypecaster = aTypecaster literal:$self.
    
    #method clone = LiteralValue new &literal:$self.
    
    #method value = $self.
}

// --- String ---

#class String : BaseValue
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
        theValue := theValue + anOperand.
    ]
    
    #method append &literal:anOperand
    [
        theValue := theValue add &literal:anOperand.
    ]
    
    #method insert &index:anIndex &literal:aLiteral
    [
        (anIndex == 0)
            ? [ theValue := aLiteral + theValue. ]
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
                
    #method cast : aTypecaster = aTypecaster literal:theValue.
    
    #method literal = theValue.
    
    #method value = theValue.
    
    #method => theValue.
}

// --- Variant ---

#class Variant : String
{
    #method int
    [
        #var(type:int)retVal.
        intConvertor convert &literal:theValue &out'int:retVal.
        
        ^ IntNumber new &int:retVal.
    ]

    #method long
    [
        #var(type:long)retVal.
        longConvertor convert &literal:theValue &out'long:retVal.
        
        ^ LongNumber new &long:retVal.
    ]

    #method real
    [
        #var(type:real)retVal.
        realConvertor convert &literal:theValue &out'real:retVal.
        
        ^ RealNumber new &real:retVal.
    ]

    #method literal = theValue.
}

// --- BaseArray ---

#class(dynamic) BaseArray
{
    #constructor new &length:aLength
        <arr_create>.
}

// --- Array ---

#class(sealed,type:array) Array : BaseArray
{
    #method getAt : anIndex
        = $self getAt &index:(anIndex int).

    #method getAt &index:anIndex
        <arr_getat>.

    #method setAt:anIndex:anObject
    [
        $self setAt &index:(anIndex int) &object:anObject.
    ]
    
    #method setAt &index:anIndex &object:anObject
        <arr_setat>.

//    #method add : anObject
//        = $self add &array::anObject.

//    #method add &array:anArray
//    [
//        #var(type:int)aLength1. 
//        $self read &out'length:aLength1.
//        
//        #var(type:int)aLength2.
//        anArray read &out'length:aLength2.
//        
//        #var aRetVal := Array new &length:(aLength1 + aLength2).
//
//        #var(type:int)anIndex := 0.
//        #loop (anIndex < aLength1) ?
//        [
//            aRetVal setAt &index:anIndex &object:(self getAt &index:anIndex).
//            
//            anIndex := anIndex + 1.
//        ].
//
//        anIndex := 0.
//        #loop (anIndex < aLength2) ?
//        [
//            aRetVal setAt &index:(aLength1 + anIndex) &object:(anArray getAt &index:anIndex).
//            
//            anIndex := anIndex + 1.
//        ].
//            
//        ^ aRetVal.
//    ]

    #method array = $self.

    #method read &out'length:aLength
        <arr_getlen>.

    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method enumerator = system'ArrayEnumerator new &array:$self.

    #method iterator = system'ArrayIterator new &array:$self.

    #method cast : aTypeCaster = aTypeCaster array:$self.

    #method clone 
    [
        #var(type:int)aLength. 
        $self read &out'length:aLength.
        
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

#class(sealed,type:params,dynamic)OpenArgs
{
    #method read &out'length:aLength
        <args_getlen>.
        
    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
}

// === Enumerators / Indexers

// --- LiteralEnumerator ---

#class LiteralEnumerator : Enumerator
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
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        #var(type:short)aChar.
        theLiteral read &index:anIndex &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]
}

// --- LiteralIterator ---

#class LiteralIterator : Iterator
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
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        #var(type:short)aChar.
        theLiteral read &index:anIndex &out'short:aChar.
        
        ^ CharValue new &short:aChar.
    ]
    
    #method set : anObject 
    [
        #throw system'NotSupportedException new.
    ]
    
    #method read &out'index:anIndex 
    [
        theIndex read &out'int:anIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- ArrayIterator ---

#class ArrayIterator : Iterator
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
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        ^ theArray getAt &index:anIndex.
    ]
    
    #method set : anObject
    [
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        theArray setAt &index:anIndex &object:anObject.
    ]
    
    #method read &out'index : anIndex
    [
        theIndex read &out'int:anIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}


// --- ArrayEnumerator ---

#class ArrayEnumerator : Enumerator
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
        #var(type:int)anIndex.
        theIndex read &out'int:anIndex.
        
        ^ theArray getAt &index:anIndex.
    ]
}
