//#define system.
//#define system'text.
//#define system'dynamic.
//
//// --- ETextSerializer ---
//
//#class(role) ETextSerializer
//{
//    #method Serialized
//    [
//        #var aResult := TextBuffer new.
//        #var anObject := self.
//
//        aResult += anObject~EClass Name.
//        #var aCounter := anObject~EClass Count.
//        (0 < aCounter) ?
//        [       
//            aResult += "{".
//
//            control from:0 &till:aCounter &do: &&:i
//            [
//                (0 == i) 
//                    ? [ aResult += "," ].
//                
//                #var aMember := anObject~EClass getAt:i.
//
//                aResult += aMember~ETextSerializer Serialized.
//            ].
//
//            aResult += "}".
//        ].
//
//        ^ aResult Literal.
//    ]
//}