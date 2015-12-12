#import system.
#import system'collections.

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

#class Token
{
    #field t_token.
    #field t_tokenType.
    
    #constructor new &token:aToken &tokenType:aTkType
    [
        t_token := aToken.
        t_tokenType := aTkType.
    ]
    
    #method token = t_token.
    #method tokenType = t_tokenType.
}