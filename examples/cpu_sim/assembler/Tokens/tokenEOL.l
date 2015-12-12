#import system.

#class TokenEOL
{
    #field line.
    
    #constructor new &line:aLine
    [
        line := aLine.
    ]
    
    #method literal
    [
        ^("End of line").
    ]
    
    #method Line = line.
}