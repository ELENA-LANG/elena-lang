import extensions.

class Extender :: BaseExtender
{
    object foo :: prop(theField).
    
    constructor new : anObject
    [
        theObject := anObject.
    ]
}

program =
[
    var anObject := 234.
  
    // extending an object with a field
    anObject := Extender new:anObject.

    anObject foo := "bar".

    console printLine:anObject:".foo=":(anObject foo).

    console readChar.
].