#define system.

// --- Operations ---

#class(role)EMathOp
{
    #method mod : anOperand
        = self - (self / anOperand * anOperand).
}
    
#class(role)EReal
{
    #method Exp
        = RealNumber new &type'real:(system'external'CoreAPI exp &type'real::self).
                
    #method Ln
        = RealNumber new &type'real:(system'external'CoreAPI ln &type'real::self).
        
    #method Rounded
        = RealNumber new &type'real:(system'external'CoreAPI round &type'real::self).
}

// --- Functions ---

#symbol modulus = &&:x:y
    [ x~EMathOp mod:y ].

#symbol exp = &&:x
    [ x~EReal Exp ].

#symbol ln = &&:x
    [ x~EReal Ln ].
    
#symbol power = &&:x:y
    [ exp:((ln:x) * y) ].

#symbol round = &&:x
    [ x~EReal Rounded ].