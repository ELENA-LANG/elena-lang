// === basic memory classes ===

// --- BaseByteArray ---

#class BaseByteArray
{
    #field(itemsize:1)theData.

    #constructor(explicit)new &length : aLength
        <bs_create>.
}

// --- ByteArray ---

#class(sealed,type:bytearray) ByteArray : BaseByteArray
{
    #method bytearray = $self.

    #method write &index:anIndex &short:anInt
        <bs_writeword>.

    #method write &index:anIndex &int:anInt
        <bs_writedword>.

    #method write &index:anIndex &long:aLong
        <bs_writeqword>.

    #method write &index:anIndex &length:aLength &bytearray:aDump
        <bs_write>.

//    #method write &index:anIndex &literal:aLiteral
//        <bs_writestr>.

    #method read &index:anIndex &out'short:aChar
        <bs_readword>.

    #method read &index:anIndex &out'int:anInt
        <bs_readdword>.

    #method read &index:anIndex &out'long:aLong
        <bs_readqword>.

    #method read &index:anIndex &length:aLength &bytearray:aDump
        <bs_read>.

//    #method indexOf &index:anIndex &length:aLength &byte:aByte &out'int:aRetVal
//        <bs_findbyte>.me
//
//    #method indexOf &index:anIndex &length:aLength &short:aWord &out'int:aRetVal
//        <bs_findshort>.

    #method equal &index:anIndex &short:aValue
        <bs_equalword>.

    #method equal &index:anIndex &int:aValue
        <bs_equalint>.

    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method read &out'length:aLength
        <bs_getlen>.
}


// --- ShortArray ---

#class(sealed,type:shortarray) ShortArray
{
    #field(itemsize:2)theData.
    
    #constructor(explicit)new &length : aLength
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
        
    #method read &index:anIndex &length:aLength &shortarray:anArray
        <sa_read>.
        
    #method length
    [
        #var(type:int)aLength.
        $self read &out'length:aLength.
        
        ^ IntNumber new &int:aLength.
    ]

    #method read &out'length:aLength
        <sa_getlen>.        
}
