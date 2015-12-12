#import system.

#class TokenIdentifier
{
    #field line.
    #field value.
    
    #constructor new &name:aVal &line:aLine
    [
        line := aLine.
        value := aVal.
    ]
    
    #method literal
    [
        ^(value literal).
    ]
    
    #method Value = value.
    #method Value:aValue [ value := aValue. ]
    
    #method Line = line.
}