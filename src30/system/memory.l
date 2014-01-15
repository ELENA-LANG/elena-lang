// --- ByteArray ---

#class ByteArray
{
    #field(type:bytearray)theData.
    
    #constructor new &length : aLength
    [
    ]
    
    #method bytearray = $self.

    #method write &index:anIndex &length:aLength &bytearray:aDump
        <bs_write>.

    #method write &index:anIndex &literal:aLiteral
        <bs_writestr>.

    #method read &index:anIndex &length:aLength &bytearray:aDump
        <bs_read>.

    #method indexOf &index:anIndex &length:aLength &byte:aByte &out'int:aRetVal
        <bs_findbyte>.

    #method indexOf &index:anIndex &length:aLength &short:aWord &out'int:aRetVal
        <bs_findshort>.

    #method length
    [
        #var(type:int)aLength := theData length.
        
        ^ IntNumber new &int:aLength.
    ]

    #method <= &bytearray:$self.
}

