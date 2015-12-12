#import system.

#class TokenIndirectRegister
{
    #field register.
    #field line.
    #field offset.
    
    #constructor new &register:aRegister &offset:aOffset &line:aLine
    [
        offset := aOffset.
        register := aRegister.
        line := aLine.
    ]
    
    #constructor new &register:aRegister &line:aLine
    [
        register := aRegister.
        line := aLine.
        offset := 0.
    ]
    #method Line = line.
    #method Register = register.
    #method Offset = offset.
}