// --- Operations ---

//#class(role)EMathOp
//{
//    #method mod : anOperand
//        = self - (self / anOperand * anOperand).
//}

#symbol mathOp =
{
    Exp &type'real:aValue &out'type'real:aRetVal
        <r_exp>.
        
    Ln &type'real:aValue &out'type'real:aRetVal
        <r_ln>.
        
    Abs &type'real:aValue &out'type'real:aRetVal
        <r_abs>.
        
    Round &type'real:aValue &out'type'real:aRetVal
        <r_round>.
}.
            
#class(role)EReal
{
    #method Exp
    [
        #var(type:real)theRetVal := mathOp Exp &type'real::self.
        
        ^ RealNumber new &type'real:theRetVal.
    ]
    
    #method Ln
    [
        #var(type:real)theRetVal := mathOp Ln &type'real::self.
        
        ^ RealNumber new &type'real:theRetVal.
    ]

    #method Rounded
    [
        #var(type:real)theRetVal := mathOp Round &type'real::self.
        
        ^ RealNumber new &type'real:theRetVal.
    ]

    #method Abs
    [
        #var(type:real)theRetVal := mathOp Abs &type'real::self.
        
        ^ RealNumber new &type'real:theRetVal.
    ]
}

