// --- Operations ---

#class(role) mathOp
{
    #method Exp
    [
        #var(type:real)aRetVal.
        mathPrimitive Exp &real:(realConvertor convert::self) &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]
    
    #method Ln
    [
        #var(type:real)aRetVal.
        mathPrimitive Ln &real:(realConvertor convert::self) &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method Rounded
    [
        #var(type:real)aRetVal.
        mathPrimitive Round &real:(realConvertor convert::self) &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method Int
    [
        #var(type:real)aRetVal.
        mathPrimitive Truncate &real:(realConvertor convert::self) &out'real:aRetVal.
        
        ^ RealNumber new &real:aRetVal.
    ]

    #method Frac
    [
        #var(type:real)aRetVal.
        mathPrimitive Truncate &real:(realConvertor convert::self) &out'real:aRetVal.
        
        ^ RealNumber new &real:(self real - aRetVal).
    ]
    
    #method Abs =
    { 
        real : n 
        [
            #var(type:real)aRetVal.
            mathPrimitive Abs &real:n &out'real:aRetVal.
            
            ^ RealNumber new &real:aRetVal.
        ]
        
        int : n
            = n < 0 ? [ -1 * n ] ! [ n ].
        
        long : n
            = n < 0l ? [ -1l * n ] ! [ n ].
    } 
    eval::self.

    #method isZero
        = self == 0.
        
    #method isPositive
        = self > 0.
        
    #method isNegative
        = self < 0.
        
    #method isNonnegative
        = self <= 0.
}

// --- intOp ---

#class(role) intOp
{
    #method mod : anOperand
        = self - (self / anOperand * anOperand).
                
    #method isOdd
        = self~intOp mod:2 != 0.
        
    #method isEven
        = self~intOp mod:2 == 0.
        
    #method HiWord
    [
        #var(type:int)aWord. 
        primitive read &bytearray:(self int) &index:2 &out'short:aWord.
        
        ^ IntNumber new &int:aWord.
    ]
    
    #method LoWord
    [
        #var(type:int)aWord. 
        primitive read &bytearray:(self int) &index:0 &out'short:aWord.
        
        ^ IntNumber new &int:aWord.
    ]
    
    #method HiByte
    [
        #var(type:int)aWord. 
        primitive read &bytearray:(self int) &index:1 &out'short:aWord.
        
        ^ IntNumber new &int:(aWord ^& 0FFh).
    ]
        
    #method LoByte
        = self int and &int:0FFh.
}

//#class(role)EIntNumber
//{
//    #method power : aPower
//    [
//        #var(type:int)aValue := self type'int.
//        #var(type:int)aRetVal := 1.
//        #var(type:int)aCounter := aPower type'int.
//        
//        control while:[ aCounter > 0 ] &do: [ aRetVal := aRetVal * aValue. aCounter := aCounter - 1. ].
//        
//        ^ aRetVal.
//    ]
//}

