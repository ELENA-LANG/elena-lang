// --- ByteArray ---

#class ByteArray
{
    #field(type:bytearray)theData.
    
    #constructor new &type'length : aLength
    [
    ]
    
    #method type'bytearray = $self.

    #method write &type'index:anIndex &type'length:aLength &type'bytearray:aDump
        = system'external'CoreAPI write &out'type'bytearray:theData &type'index:anIndex &type'length:aLength &type'bytearray:aDump 
            | raise:(InvalidArgumentException new:"Invalid argument").

    #method write &type'index:anIndex &type'widestr:aLiteral
        = system'external'CoreAPI write &out'type'bytearray:theData &type'index:anIndex &type'widestr:aLiteral
            | raise:(InvalidArgumentException new:"Invalid argument").

    #method read &type'index:anIndex &type'length:aLength &type'bytearray:aDump
        = system'external'CoreAPI read &type'bytearray:theData &type'index:anIndex &type'length:aLength &out'type'bytearray:aDump 
            | raise:(InvalidArgumentException new:"Invalid argument").

    #method indexOf &type'index:anIndex &type'length:aLength &type'byte:aByte
        = IntNumber new &type'int:(system'external'CoreAPI find &type'bytearray:theData &type'index:anIndex &type'length:aLength &type'byte:aByte).

    #method indexOf &type'index:anIndex &type'length:aLength &type'short:aWord
        = IntNumber new &type'int:(system'external'CoreAPI find &type'bytearray:theData &type'index:anIndex &type'length:aLength &type'short:aWord).

    #method Length 
        = IntNumber new &type'int:(system'external'CoreAPI lengthOf &type'bytearray:theData).

    #method type'length
        = IntNumber new &type'int:(system'external'CoreAPI lengthOf &type'bytearray:theData).

    #method <= &&type'dump:$self.
}

