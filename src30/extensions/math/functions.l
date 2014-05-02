//#define system.
//#define extensions.
//
//// --- Functions ---
//
//#symbol exp = (:x)
//    [ (realValue:x)~mathOp Exp ].
//
//#symbol ln = (:x)
//    [ (realValue:x)~mathOp Ln ].
//    
//#symbol power = (:x:y)
//    [ exp:((ln:(realValue:x)) * y) ].
//
//#symbol round = (:x)
//    [ (realValue:x)~mathOp Rounded ].
//    
//#symbol abs = (:x)
//  [ (realValue:x)~mathOp Abs ].
//
//#symbol modulus = (:x:y)
//    [ x - (x / y * y) ].
