#import system.

#symbol(const,type:int)$C_Always               = 0.
#symbol(const,type:int)$C_Sign                 = 1.
#symbol(const,type:int)$C_Zero                 = 2.
#symbol(const,type:int)$C_Carry                = 3.
#symbol(const,type:int)$C_Overflow             = 4.
#symbol(const,type:int)$C_Equal                = 5.
#symbol(const,type:int)$C_NotEqual             = 6.
#symbol(const,type:int)$C_GreaterThan          = 7.
#symbol(const,type:int)$C_GreaterThanOrEqual   = 8. 
#symbol(const,type:int)$C_LessThan             = 9.
#symbol(const,type:int)$C_LessThanOrEqual      = 10.
#symbol(const,type:int)$C_Trap                 = 11.

#symbol ConditionCode =
{
    #method Always             = $C_Always.
    #method Sign               = $C_Sign.
    #method Zero               = $C_Zero.
    #method Carry              = $C_Carry.
    #method Overflow           = $C_Overflow.
    #method Equal              = $C_Equal.
    #method NotEqual           = $C_NotEqual.
    #method GreaterThan        = $C_GreaterThan.
    #method GreaterThanOrEqual = $C_GreaterThanOrEqual. 
    #method LessThan           = $C_LessThan.
    #method LessThanOrEqual    = $C_LessThanOrEqual.
    #method Trap               = $C_Trap.
}.