#import system.
#import system'collections.

#class TokenItem
{
    #field theToken.
    
    #constructor new &token:aToken
    [
        theToken := aToken.
    ]    
  
    #method(generic)is
    [
        ^ theToken tokenType equal &subject:$subject.
    ]
        
    #method => theToken.        
}




#symbol(const,type:int)$TK_IntLiteral       = 0.
#symbol(const,type:int)$TK_EOL              = 1.
#symbol(const,type:int)$TK_StringLiteral    = 2.
#symbol(const,type:int)$TK_Identifier       = 3.
#symbol(const,type:int)$TK_Register         = 4.
#symbol(const,type:int)$TK_IndirectRegister = 5.
#symbol(const,type:int)$TK_Comma            = 6.
#symbol(const,type:int)$TK_Hash             = 7.
#symbol(const,type:int)$TK_Colon            = 8.

#symbol TokenType =
{
    #method IntLiteral       = $TK_IntLiteral.
    #method EOL              = $TK_EOL.
    #method StringLiteral    = $TK_StringLiteral.
    #method Identifier       = $TK_Identifier.
    #method Register         = $TK_Register.
    #method IndirectRegister = $TK_IndirectRegister.
    #method Comma            = $TK_Comma.
    #method Hash             = $TK_Hash.
    #method Colon            = $TK_Colon.
}.
