// ==== ver 3.4.1 ===

//#define system.
//#define system'math.
//
//#symbol(const) daysInMonths     
//    = Array(31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31).
//
//// --- dateOp ---
//
//#class(extension:datetime) dateOp
//{
//    #method daysInMonth
//    [
//        #var(type:int)aMonth := self Month int.
//        ^ (aMonth == 2)
//            ? [ 
//                #var(type:int)days := daysInMonths getAt &index:(aMonth - 1) int.
//                
//                self~dateOp isLeapYear 
//                    ? [ days := days + 1. ].
//               
//               ^ IntNumber new &int:days.
//            ]
//            ! [ daysInMonths getAt &index:(aMonth - 1) ].
//    ]
//    
//    #method isLeapYear
//    [
//        #var(type:int)aYear := self Year int.
//        
//        ^ (aYear mod:4 == 0) and:((aYear mod:100 != 0)or:(aYear mod:400 == 0)).
//    ]
//}