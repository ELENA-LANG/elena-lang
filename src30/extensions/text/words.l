//#define system.
//#define system'text.
//
//#class WordEnumerator :: Enumerator
//{
//    #field(type:literal)    theLiteral.
//    #field(type:intvar)     theIndex.
//    #field(type:textbuffer) theBuffer.
//    
//    #constructor new &literal:aLiteral
//    [
//        theLiteral := aLiteral.
//        theIndex := Integer new.
//        theBuffer := TextBuffer new.
//    ]    
//    
//    #method reset 
//    [
//        theIndex write &int:0.
//    ]
//    
//    #method next
//    [
//        theBuffer clear.
//
//        #var(type:int)aLength := theLiteral length.
//        #var(type:int)aCurrent := theIndex int.
//        #var(type:char)aChar.
//        #var(type:bool)aSpaceMode := true.
//        
//        #loop (aCurrent < aLength) ?
//        [
//            aChar := theLiteral read &index:aCurrent.
//            (aChar == 32)
//                ? [
//                    aSpaceMode ! [ theIndex write &int:aCurrent. ^ true. ].                    
//                ]
//                ! [
//                    aSpaceMode ? [ aSpaceMode := false. ].
//                        
//                    theBuffer write &char:aChar.
//                ].
//                
//            aCurrent := aCurrent + 1.
//        ].
//
//        theIndex write &int:aCurrent.
//
//        ^ aSpaceMode not.
//    ]
//    
//    #method get = theBuffer literal.
//}
//
