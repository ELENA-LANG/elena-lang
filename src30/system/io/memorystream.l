#define system.

// --- memoryOp ---

#class(role) memoryOp
{
    #method write &literal:aLiteral
    [
        self write &bytearray:aLiteral &length:(aLiteral length * 2).
    ]    
    
    #method read &literal &length:aLength
    [
        #var aLiteral := LiteralValue new &length:aLength.
        
        self read &bytearray:aLiteral &length:(aLength * 2).
        
        literalPrimitiveOp refresh &literal:aLiteral.
        
        ^ aLiteral.
    ]
}

// --- MemoryStream ---

#class MemoryStream
{
    #field theByteArray.
    #field thePosition.
    
    #constructor new &size:aLength
    [
        theByteArray := ByteArray new &length:aLength.
        thePosition := Integer new.
    ]

    #method clear
    [
        thePosition write &int:0.
    ]

    #method length = thePosition number.

    #method bytearray = theByteArray.

    #method set &length:aLength
    [
        thePosition write &int:aLength.
    ]

    #method size = theByteArray length.

    #method set &size:aSize
    [
        #var aTemp := theByteArray.

        theByteArray := ByteArray new &length:aSize.

        theByteArray write &index:0 &length::aTemp &bytearray:aTemp.
    ]

    #method append &size:aSize
    [
        #var(type:int)aNewSize := aSize.
        aNewSize := aNewSize + theByteArray length.
        
        $self set &size:aNewSize
    ]

    #method indexOf &index:anIndex &byte:aByte
    [
        #var(type:int)anIndex := theByteArray indexOf &index:anIndex &length:thePosition &byte:aByte.
        
        ^ IntNumber new &int:anIndex.
    ]

    #method indexOf &index:anIndex &short:aWord
    [
        #var(type:int)anIndex.
        theByteArray indexOf &index:anIndex &length:thePosition &short:aWord &out'int:anIndex.
        
        ^ IntNumber new &int:anIndex.
    ]

    #method indexOf &index:anIndex &short:aWord &out'int:aRetVal
    [  
        theByteArray indexOf &index:anIndex &length:thePosition &short:aWord &out'int:aRetVal.
    ]

    #method write &bytearray:aDump &length:aLength
    [
        theByteArray write &index:thePosition &length:aLength &bytearray:aDump.

        thePosition append &int:aLength.
    ]

    #method read &bytearray:aDump &length:aLength
    [
        thePosition reduce &int:aLength.
        
        theByteArray read &index:thePosition &length:aLength &bytearray:aDump.        
    ]
}
