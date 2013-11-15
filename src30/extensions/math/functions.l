#define system.
#define extensions.

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
        
    #method Abs
        = RealNumber new &type'real:(system'external'CoreAPI abs &type'real::self).
}

// --- Functions ---

#symbol modulus = &&:x:y
    [ (extensions'realValue:x)~EMathOp mod:y ].

#symbol exp = &&:x
    [ (realValue:x)~EReal Exp ].

#symbol ln = &&:x
    [ (realValue:x)~EReal Ln ].
    
#symbol power = &&:x:y
    [ exp:((ln:(realValue:x)) * y) ].

#symbol round = &&:x
    [ (realValue:x)~EReal Rounded ].
    
#symbol abs = &&:x
  [ (realValue:x)~EReal Abs ].
