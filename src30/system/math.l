// --- Operations ---

#class(role) mathOp
{
    #method Exp
    [
        #var(type:real)theRetVal := mathPrimitive Exp &real::self.
        
        ^ RealNumber new &real:theRetVal.
    ]
    
    #method Ln
    [
        #var(type:real)theRetVal := mathPrimitive Ln &real::self.
        
        ^ RealNumber new &real:theRetVal.
    ]

    #method Rounded
    [
        #var(type:real)theRetVal := mathPrimitive Round &real::self.
        
        ^ RealNumber new &real:theRetVal.
    ]

    #method Int
    [
        #var(type:real)theRetVal := mathPrimitive Truncate &real::self.
        
        ^ RealNumber new &real:theRetVal.        
    ]

    #method Frac
    [
        #var(type:real)theRetVal := mathPrimitive Truncate &real::self.
        
        ^ RealNumber new &real:(self real - theRetVal).
    ]
    
    #method Abs
    [
        #var(type:real)theRetVal := mathPrimitive Abs &real::self.
        
        ^ RealNumber new &real:theRetVal.
    ]

    #method mod : anOperand
        = self - (self / anOperand * anOperand).
}

// --- intOp ---

#class(role) intOp
{
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

