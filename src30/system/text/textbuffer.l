#define system. 
//#define system'routines. 
//#define system'io. 

#symbol(const)$TextPageSize = 128.

// --- TextBuffer ---

#class(sealed) TextBuffer
{
    #field theBuffer.
    #field theLength.

    #constructor new
    [
        theBuffer := ShortArray new &length:($TextPageSize int).
        theLength := Integer new &int:0.
    ]

    #method clear
    [
        theLength write &int:0.
    ]

    #method length
        = theLength int.

    #method read &out'length:aLength
    [
        theLength read &out'int:aLength.
    ]

    #method set &length:aLength
    [
        theLength write &int:aLength.
    ]

    #method write : aLine
    [
        $self write &literal::aLine.
    ]
                                    
    #method write &char:aChar
    [
        $self write &literal::aChar.
    ]
    
    #method reserve &length:aLength
    [
        #var(type:int)anOldLength.
        theLength read &out'int:anOldLength.
        
        #var(type:int)aNewLength := aLength.
        aNewLength := aNewLength + anOldLength.
        
        #var(type:int)aBufferLength.
        theBuffer read &out'length:aBufferLength.
        
        (aNewLength > aBufferLength) ?
        [
            aNewLength := ((aNewLength / $TextPageSize + 1) * $TextPageSize) int.
            
            #var aNewBuffer := ShortArray new &length:aNewLength.
            
            aNewBuffer write &index:0 &length:aBufferLength &shortarray::theBuffer.
            
            theBuffer := aNewBuffer.
        ].
    ]

    #method write &literal:aLiteral
    [
        #var(type:int)anOldLength.
        theLength read &out'int:anOldLength.
        
        #var(type:int)aLength.
        aLiteral read &out'length:aLength.
        
        (0 == aLength) !
        [
            $self reserve &length:aLength.
            
            theBuffer write &index:anOldLength &length:aLength &literal:aLiteral.
            
            theLength append &int:aLength.
        ].
    ]

//    #method indexOf &index:aPosition &char:aChar
//    [
//        #var(type:int) anIndex.
//        theBuffer indexOf &index:(2 * aPosition) &short:aChar &out'int:anIndex.
//        
//        (-1 == anIndex) 
//            ! [ anIndex := anIndex / 2. ].
//            
//        ^ anIndex.
//    ]
//
//    #method Substring &index:anIndex &length:aLength
//        = (aLength == 0)
//            ? [ emptyLiteralValue ]
//            ! [
//                #var aLiteral := LiteralValue new &length:aLength.
//                
//                theBuffer bytearray read &index:(anIndex * 2) &length:(aLength * 2) &bytearray:aLiteral.
//                
//                literalPrimitiveOp refresh &literal:aLiteral.
//                
//                ^ aLiteral.        
//            ].
//
//    #method delete &index:anIndex &length:aLength
//    [
//        (anIndex == 0)
//            ? [
//                #var aRightLiteral := $self~literalOp Substring &index:aLength.
//                
//                $self clear write &literal:aRightLiteral.
//            ]
//            ! [
//                #var aLeftLiteral := $self Substring &index:0 &length:anIndex.
//                #var aRightLiteral := $self~literalOp Substring &index:(anIndex + aLength).
//                
//                $self clear write &literal:aLeftLiteral write &literal:aRightLiteral.                
//            ].
//    ]

    #method literal
    [
        #var(type:int)aLength.
        theLength read &out'int:aLength.
        
        ^ LiteralValue new &length:aLength &shortarray::theBuffer.
    ]
}
