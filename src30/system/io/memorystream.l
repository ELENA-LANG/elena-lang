#define system.

// --- MemoryStream ---

#class MemoryStream
{
    #field theByteArray.
    #field thePosition.
    
    #constructor new &size:aLength
    [
        theByteArray := ByteArray new &type'length:aLength.
        thePosition := Integer new.
    ]

    #method clear
    [
        thePosition write &type'int:0.
    ]

    #method Length => %type'length.

    #method type'length = thePosition Number.

    #method type'bytearray = theByteArray.

    #method set &type'length:aLength
    [
        thePosition write &type'int::aLength.
    ]

    #method Size = theByteArray Length.

    #method append &Size:aSize
    [
        $self expand::aSize.
    ]

    #method expand &type'int:aSize
    [
        #var aTemp := theByteArray.

        theByteArray := ByteArray new &type'length:(aSize + aTemp type'length).

        theByteArray write &type'index:0 &type'length::aTemp &type'bytearray:aTemp.
    ]

    #method indexOf &type'index:anIndex &type'byte:aByte
        = theByteArray indexOf &type'index:anIndex &type'length:thePosition &type'byte:aByte.

    #method indexOf &type'index:anIndex &type'short:aWord
        = theByteArray indexOf &type'index:anIndex &type'length:thePosition &type'short:aWord.

    #method write &type'bytearray:aDump &type'length:aLength
    [
        theByteArray write &type'index:thePosition &type'length:aLength &type'bytearray:aDump.

        thePosition append &type'int:aLength.
    ]

    #method read &type'bytearray:aDump &type'length:aLength
    [
        thePosition reduce &type'int:aLength.
        
        theByteArray read &type'index:thePosition &type'length:aLength &type'bytearray:aDump.        
    ]
}
