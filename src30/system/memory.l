// --- ByteArray ---

#class ByteArray
{
    #field(type:bytearray)theData.
    
    #constructor new &type'length : aLength
    [
    ]
    
    #method type'bytearray = $self.

    #method write &type'index:anIndex &type'length:aLength &type'bytearray:aDump
        <bs_write>.

    #method write &type'index:anIndex &type'widestr:aLiteral
        <bs_writestr>.

    #method read &type'index:anIndex &type'length:aLength &type'bytearray:aDump
        <bs_read>.

    #method indexOf &type'index:anIndex &type'length:aLength &type'byte:aByte &out'type'int:aRetVal
        <bs_findbyte>.

    #method indexOf &type'index:anIndex &type'length:aLength &type'short:aWord &out'type'int:aRetVal
        <bs_findshort>.

    #method Length 
        <= type'length.

    #method type'length
    [
        #var(type:int)aLength := theData type'length.
        
        ^ IntNumber new &type'int:aLength.
    ]

    #method <= &&type'dump:$self.
}

