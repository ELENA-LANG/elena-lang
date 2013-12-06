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
//        aResult += anObject~EReflector ClassName.
//        #var aCounter := anObject~EReflector Count.
//        (0 < aCounter) ?
//        [       
//            aResult += "{".
//
//            control from:0 &till:aCounter &do: &&:i
//            [
//                (0 == i) 
//                    ? [ aResult += "," ].
//                
//                #var aMember := anObject~EReflector getAt &type'index:i.
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