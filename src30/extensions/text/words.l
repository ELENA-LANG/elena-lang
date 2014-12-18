#define system.
#define system'text.

#class WordEnumerator : Enumerator
{
    #field(type:literal)    theLiteral.
    #field(type:intvar)     theIndex.
    #field(type:textbuffer) theBuffer.
    
    #constructor new &literal:aLiteral
    [
        theLiteral := aLiteral.
        theIndex := Integer new.
        theBuffer := TextBuffer new.
    ]    
    
    #method reset 
    [
        theIndex write &int:0.
    ]
    
//    #method next &space
//    [
//        #var(type:short)aChar.
//        theIndexer read &out'short:aChar.
//        
//        (aChar == 32)
//            ? [ ^ theIndexer next. ]
//            ! [ theRole := %word. ^ true. ].
//    ]
//    
//    #method next &word
//    [
//        #var(type:short)aChar.
//        theIndexer read &out'short:aChar.
//        
//        (aChar == 32)
//            ? [ ^ false. ]
//            ! [ theWord write &short:aChar. ^ theIndexer next. ].
//    ]
    
    #method next
    [
        theBuffer clear.

        #var(type:int)aLength := theLiteral length.
        #var(type:int)aCurrent := theIndex int.
        #var(type:short)aChar.
        #var(type:bool)aSpaceMode := true.
        
        #loop (aCurrent < aLength) ?
        [
            theLiteral read &index:aCurrent &out'short:aChar.
            (aChar == 32)
                ? [
                    aSpaceMode ! [ theIndex write &int:aCurrent. ^ true. ].                    
                ]
                ! [
                    aSpaceMode ? [ aSpaceMode := false. ].
                        
                    theBuffer write &short:aChar.
                ].
                
            aCurrent := aCurrent + 1.
        ].

        theIndex write &int:aCurrent.

        ^ aSpaceMode not.
    ]
    
    #method get = theBuffer literal.
}

