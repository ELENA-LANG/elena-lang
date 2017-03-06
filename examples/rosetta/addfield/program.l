#import extensions.

class Extender :: BaseExtender
{
    object theField.
    
    constructor new : anObject
    [
        theObject := anObject.
    ]
    
    foo = theField.
    
    set &foo : aValue
    [
        theField := aValue.
    ]
}

program =
[
    var anObject := 234.
  
    // extending an object with a field
    anObject := Extender new:anObject.

    anObject set &foo:"bar".

    console writeLine:anObject:".foo=":(anObject foo).

    console readChar.
].