// === basic memory classes ===

#type(itemsize:1,role:ByteArray)bytearray.
#type(itemsize:2,role:ShortArray)shortarray.

// --- ByteArray ---

#class(sealed,type:bytearray) ByteArray
{
    #field(itemsize:1)theData.

    #constructor => <bs_create>.
    
    #constructor new &length : aLength
    [
    ]
    
    #method bytearray = $self.

//    #method write &index:anIndex &length:aLength &bytearray:aDump
//        <bs_write>.
//
//    #method write &index:anIndex &literal:aLiteral
//        <bs_writestr>.

    #method read &index:anIndex &out'short:aChar
        <bs_readword>.

//    #method read &index:anIndex &length:aLength &bytearray:aDump
//        <bs_read>.
//
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

    #method read&length &out'length:aLength
        <bs_getlen>.
}


// --- ShortArray ---

#class(sealed,type:shortarray) ShortArray
{
    #field(itemsize:2)theData.
    
    #constructor => <sa_create>.
    
    #constructor new &length : aLength
    [
    ]
    
    #method equal &index:anIndex &short:aValue
        <ws_equalword>.
        
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
