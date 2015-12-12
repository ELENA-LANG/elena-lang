#import system.

#class TokenRegister
{
    #field line.
    #field register.
    
    #constructor new &register:aRegister &line:aLine
    [
        register := aRegister.
        line := aLine.
    ]
    #method Line = line.
    #method Register = register.
}