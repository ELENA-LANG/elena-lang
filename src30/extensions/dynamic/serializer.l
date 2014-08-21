//#define system.
//#define system'text.
//#define system'dynamic.
//
//// --- ETextSerializer ---
//
//#class(role) textserializable
//{
//    #method literal
//    [
//        #var aResult := TextBuffer new.
//        #var anObject := self.
//
//        aResult << anObject~classOp Name.
//        #var(type:int) aCounter := anObject~classOp length.
//        (0 < aCounter) ?
//        [       
//            aResult << "{".
//
//            control forrange &int:0 &int:(aCounter-1) &do: (&int:i)
//            [
//                (0 == i) 
//                    ? [ aResult << "," ].
//                
//                #var aMember := anObject~classOp getAt:i.
//
//                aResult << aMember~textserializable literal.
//            ].
//
//            aResult << "}".
//        ].
//
//        ^ aResult literal.
//    ]
//}