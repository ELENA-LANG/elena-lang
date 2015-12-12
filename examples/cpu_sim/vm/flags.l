#import system.

#symbol(const,type:int)$F_Sign     = 001h.
#symbol(const,type:int)$F_Zero     = 002h.
#symbol(const,type:int)$F_Carry    = 004h.
#symbol(const,type:int)$F_Overflow = 008h.

#symbol Flags = 
{
    #method Sign = $F_Sign.
    #method Zero = $F_Zero.
    #method Carry = $F_Carry.
    #method Overflow = $F_Overflow.
}.