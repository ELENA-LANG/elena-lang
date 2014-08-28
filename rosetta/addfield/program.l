#define system.
#define extensions.

#class Extender
{
    #field theObject.
    #field theField.
    
    #constructor new : anObject
    [
        theObject := anObject.
    ]
    
    #method foo = theField.
    
    #method set &foo : aValue
    [
        theField := aValue.
    ]
    
    #method => theObject.
}

#symbol program =
[
    #var anObject := 234.
  
    // adding a field
    anObject := Extender new:anObject.

    anObject set &foo:"bar".

    consoleEx write:anObject:".foo=":(anObject foo).

    console readChar.
].