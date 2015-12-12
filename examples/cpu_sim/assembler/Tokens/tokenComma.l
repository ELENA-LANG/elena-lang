#import system.

#class TokenComma
{
    #field line.
    
    #constructor new &line:aLine
    [
        line := aLine.
    ]
    
    #method literal
    [
        ^(",").
    ]
    #method Line = line.
}
