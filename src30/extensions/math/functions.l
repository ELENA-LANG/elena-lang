//#define system.
//#define system'math.
//
//// --- Functions ---
//
//#symbol exp = (:x)
//    [ x::mathOp exp ].
//
//#symbol ln = (:x)
//    [ x::mathOp ln ].
//    
//#symbol power = (:x:y)
//    [ exp: ((ln:x) * y) ].
//
//#symbol round = (:x)
//    [ x~mathOp Rounded ].
//    
//#symbol abs = (:x)
//[
//    ^ x cast:
//        {            
//            eval &int:n = n~intOp modulus.
//            
//            eval &real:r = r~realOp modulus.
//        }.
//].
//
//#symbol mod = (:x:y)
//    [ x - (x / y * y) ].
