// ==== ver 3.4.1 ===

//#define system.
//#define system'math.
//
//// --- Functions ---
//
//#symbol mathControl =
//{
//    exp : x
//        = x real exp.
//            
//    ln : x
//        = x real ln.
//            
//    round : x
//        = x real rounded.
//            
//    roundInt : x
//        = intConvertor convert &real:(x real rounded).
//            
//    sin : x
//        = x real sin.
//            
//    cos : x
//        = x real cos.
//            
//    arctan : x
//        = x real arctan.
//            
//    abs : x
//        = x cast:%eval &to:
//            {
//                eval &int : aValue = aValue absolute.
//                
//                eval &long : aValue = aValue absolute.
//                
//                eval &byte : aValue = aValue absolute.
//                
//                eval &short : aValue = aValue absolute.
//                
//                eval &real : aValue = aValue absolute.
//            }.
//            
//    mod :x :y
//        = x - (x / y * y).
//        
//    power : x : y
//       = x real ln multiply:y real exp.
//
//    sqrt : x       
//        = x real sqrt.
//                     
//    pi
//    [
//        #var(type:real)r.
//        realOpController pi &vreal:r.
//        
//        ^ RealNumber new &real:r.
//    ]       
//}.
