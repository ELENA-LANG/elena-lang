#import system.

#class TokenHash
{
    #field line.
    
    #constructor new &line:aLine
    [
        line := aLine.
    ]
    
    #method literal
    [
        ^("#").
    ]
    #method Line = line.
}