//#define system.
////#define system'routines.
//
//// --- Bookmark ---
//
//#class Bookmark
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
//    #constructor new &literal:aText
//    [
//        theLiteral := aText.
//        theIndex := 0.
//        theStart := 0.
//    ]
//
//    #constructor new &literal:aText &index:anIndex
//    [
//        theLiteral := aText.
//        theIndex := anIndex.
//        theStart := 0.
//    ]
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
//    #method append :aValue
//        => &append::aValue.
//    
//    #method append &literal:aValue
//    [
//        theLiteral := (theLiteral + aValue) literal.
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
////    #method Marked
////        = (-1 == theIndex)
////            ? [ theLiteral~literalOp Substring &index:theStart ]
////            ! [ theLiteral Substring &index:theStart &length:(theIndex - theStart) ].
//    
//    #method literal = theLiteral.
//    
//    #method length = theLiteral length.
//    
//    #method index = theIndex.
//}
