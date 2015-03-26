//#define system.
//#define system'routines.
////#define system'io.
////#define system'text.
//
//// --- StringBookmark ---
//
//#class StringBookmark
//{
//    #field(type:literal) theLiteral.
//    #field(type:int)     theIndex.
//    #field(type:int)     theStart.
//    
//    #constructor new
//    [
//        theLiteral := emptyLiteralValue literal.
//        theIndex := 0.
//        theStart := 0.
//    ]
//
//    #constructor new : anObject
//        <= %new &literal:(anObject literal).
//
//    #constructor new &literal:aText
//        <= ( %new )
//    [
//        theLiteral := aText.
//    ]
//
//    #constructor new &literal:aText &index:anIndex
//        <= ( %new &literal:aText )
//    [
//        theIndex := anIndex.
//    ]
// 
//    #method find : anObject
//        = anObject cast:%eval &to:
//        {
//            eval &int : anIndex = self find &index:anIndex.
//            
//            eval &literal : aLiteral = self find &literal:aLiteral.
//        }.
//
//    #method append : anObject
//        = anObject cast:%eval &to:
//        {
//            eval &int : anIndex = self append &index:anIndex.
//            
//            eval &literal : aLiteral = self append &literal:aLiteral.
//        }.
//
//    #method find &index:anIndex
//    [
//        theIndex := anIndex.
//    ]
//            
//    #method find &literal:aValue
//    [
//        theIndex := theLiteral indexOf &index:theIndex &literal:aValue int.
//        
//        ^ (-1 != theIndex).
//    ]
//    
//    #method mark
//    [
//        (-1 == theIndex)
//            ? [
//                $self clear.
//            ]
//            ! [
//                theIndex := 1 + theIndex.
//                theStart := theIndex.
//            ].
//        
//    ]
//    
//    #method append &literal:aValue 
//    [
//        theLiteral := (theLiteral + aValue) literal.
//    ]
//    
//    #method append &index:aValue 
//    [
//        theIndex := theIndex + aValue.
//    ]
//    
//    #method insert &literal:aValue
//    [
//        theLiteral := theLiteral insert &index:theIndex &literal:aValue literal.
//        
//        theIndex := theIndex + aValue length.
//    ]
//    
//    #method delete &length:aLength
//    [
//        theLiteral := theLiteral delete &index:theIndex &length:aLength literal.
//    ]
//
//    #method clear
//    [
//        theLiteral := emptyLiteralValue literal.
//        theIndex := 0.
//        theStart := 0.
//    ]
//
//    #method marked
//        = (-1 == theIndex)
//            ? [ theLiteral Substring &index:theStart ]
//            ! [ theLiteral Substring &index:theStart &length:(theIndex - theStart) ].
//    
//    #method literal = theLiteral.
//    
//    #method length = theLiteral length.
//    
//    #method index = theIndex.
//    
//    #method cast : aVerb &to:aDispatcher = aDispatcher::aVerb eval &literal:theLiteral.
//}
//
////// --- StringReader ---
////
////#class StringReader : TextReader
////{
////    #field(type:literal) theLiteral.
////    #field(type:int)     theLength.
////    #field theIndex.
////
////    #constructor new &literal:aLiteral
////    [
////        theLiteral := aLiteral.
////        theIndex := Integer new:0.
////        theLength := aLiteral length.
////
////        theOutput := TextBuffer new.
////        theOpen := true.
////    ]
////    
////    #method read &shortarray:anOutput &out'length:aLength
////    [
////        #var(type:int)anIndex.
////        theIndex read &out'int:anIndex.
////        
////        (aLength > theLength)
////            ? [ aLength := theLength - theIndex. ].
////        
////        anOutput write &index:anIndex &length:aLength &literal:theLiteral.
////        
////        theIndex append &int:aLength.
////    ]
////    
////    #method close
////    [
////        theLiteral := emptyLiteralValue literal.
////    ]
////}