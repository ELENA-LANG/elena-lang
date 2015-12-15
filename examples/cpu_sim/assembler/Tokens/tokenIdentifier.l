#import system.

#class BaseToken
{
    #field theLineNumber.
    
    #method line_number = theLineNumber.
    
    #constructor new &line_number:lineNumber
    [
        theLineNumber := lineNumber.
    ]
}

#class BaseTokenValue :: BaseToken
{
    #field theValue.
    
    #method Value = theValue.
    
    #constructor new &value:aValue &line_number:lineNumber
        <= (new &line_number:lineNumber)
    [
        theValue := aValue.
    ]
}

#class TokenIdentifier :: BaseTokenValue
{
    #method tokenType = %identifier.
    
//    #method literal
//    [
//        ^(value literal).
//    ]
//    
//    #method Value = value.
//    #method Value:aValue [ value := aValue. ]
//    
//    #method Line = line.
}