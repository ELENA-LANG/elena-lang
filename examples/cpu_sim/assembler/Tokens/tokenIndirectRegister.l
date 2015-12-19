#import system.

#class TokenIndirectRegister :: BaseTokenValue
{
    #field theOffset.
    
    #method tokenType = %indirect_register.
    
    #method set &offset:value
    [
        theOffset := value int.
    ]
    
    #method offset = theOffset.
}