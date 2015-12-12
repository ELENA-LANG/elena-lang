#import system.

#class TokenStringLiteral
{
    #field line.
    #field value.
    
    #constructor new &value:aVal &line:aLine
    [
        line := aLine.
        value := aVal.
    ]
    
    #method literal
    [
        ^(value literal).
    ]
    #method Line = line.
    #method Value = value.
}