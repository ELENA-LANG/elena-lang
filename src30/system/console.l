// --- Console ---
#class $Console
{
    #field theWriter.
    #field theReader.
    #field theNewLineConstant.

    #constructor new &output:aWriter &input:aReader
    [
        theWriter := aWriter.
        theReader := aReader.
        theNewLineConstant := system'io'newLine.
    ]

    #method read &Line
        => theReader.

    #method read &Line:aTarget
        = aTarget << $self read &Line.

    #method read &Char
        = theReader read &type'char.

    #method read &Key
        = theReader read &type'int.

    #method write &Line : anObject
    [
        theWriter write::(anObject Literal) write::theNewLineConstant.
        
        ^ self.
    ]

    #method write : anObject
    [
        theWriter write::(anObject Literal).
        
        ^ self.
    ]
    
    #method writeLine : aLine
        <= write &Line:aLine.
        
    #method writeLine
    [
        theWriter write::theNewLineConstant.
        
        ^ self.
    ]
        
    #method readLine
        = $self read &Line.
        
    #method readLine : aTarget
        = $self read &Line:aTarget.
        
    #method readChar
        = $self read &Char.
}

#static console = $Console new &output:system'io'stdConsoleWriter &input:system'io'stdConsoleReader.
