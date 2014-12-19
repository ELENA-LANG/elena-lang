#define system.

// --- Operations ---

// --- intOp ---

#class(extension:int) intOp
{
    #method modulus
    [
        #var(type:int)n := self int.
        
        ^ n < 0 ? [ -1 * n ] ! [ n ].
    ]
    
    #method mod : anOperand
        = self - (self / anOperand * anOperand).
                
//    #method isOdd
//        = self~intOp mod:2 != 0.
//        
//    #method isEven
//        = self~intOp mod:2 == 0.
//        
//    #method isZero
//        = self equal &int:0.
//
//    #method equal &object:aValue
//        <intptr_equal>.
//                        
////    #method HiWord
////    [
////        #var(type:int)aWord. 
////        byteArrayPrimitiveOp read &bytearray:(self int) &index:2 &out'short:aWord.
////        
////        ^ IntNumber new &int:aWord.
////    ]
////    
////    #method LoWord
////    [
////        #var(type:int)aWord. 
////        byteArrayPrimitiveOp read &bytearray:(self int) &index:0 &out'short:aWord.
////        
////        ^ IntNumber new &int:aWord.
////    ]
////    
////    #method HiByte
////    [
////        #var(type:int)aWord. 
////        byteArrayPrimitiveOp read &bytearray:(self int) &index:1 &out'short:aWord.
////        
////        ^ IntNumber new &int:(aWord ^& 0FFh).
////    ]
////        
////    #method LoByte
////        = self int and &int:0FFh.
//
////    #method power : aPower
////    [
////        #var(type:int)aValue := self type'int.
////        #var(type:int)aRetVal := 1.
////        #var(type:int)aCounter := aPower type'int.
////        
////        control while:[ aCounter > 0 ] &do: [ aRetVal := aRetVal * aValue. aCounter := aCounter - 1. ].
////        
////        ^ aRetVal.
////    ]
}

// --- realOp ---
#class(extension:real) realOp
{
//    #method Exp &out'real:aRetVal
//        <r_exp>.
//        
//    #method Ln &out'real:aRetVal
//        <r_ln>.
        
    #method modulus
    [
        #var(type:real)n := self real.
        
        #var(type:real)aRetVal.        
        n modulus &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]        
        
//    #method Round &out'real:aRetVal
//        <r_round>.
//        
//    #method Truncate &out'real:aRetVal
//        <r_truncate>.
}

//// --- mathOp ---
//
//#class(role) mathOp
//{
//    #method Exp
//    [
//        #var(type:real)aRetVal.
//        
//        realConvertor convert::self ~realOp Exp &out'real:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//    
//    #method Ln
//    [
//        #var(type:real)aRetVal.
//        
//        realConvertor convert::self ~realOp Ln &out'real:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    #method Rounded
//    [
//        #var(type:real)aRetVal.
//        
//        realConvertor convert::self ~realOp Round &out'real:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    #method Int
//    [
//        #var(type:real)aRetVal.
//        
//        realConvertor convert::self ~realOp Truncate &out'real:aRetVal.
//        
//        ^ RealNumber new &real:aRetVal.
//    ]
//
//    #method Frac
//    [
//        #var aValue := realConvertor convert::self real.
//        #var(type:real)aRetVal.
//        
//         aValue~realOp Truncate &out'real:aRetVal.
//        
//        ^ RealNumber new &real::(aValue subtract &real:aRetVal).
//    ]
//    
//    #method Abs =
//    { 
//        real : n 
//        [
//        ]
//        
//        int : n
//            = n < 0 ? [ -1 * n ] ! [ n ].
//        
//        long : n
//            = n < 0l ? [ -1l * n ] ! [ n ].
//    } 
//    eval::(self value).
//
//    #method isZero
//        = self == 0.
//        
//    #method isPositive
//        = self > 0.
//        
//    #method isNegative
//        = self < 0.
//        
//    #method isNonnegative
//        = self <= 0.
//}
