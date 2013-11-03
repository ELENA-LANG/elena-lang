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

    #method read &type'widechar
        = theReader read &type'widechar.

    #method readKey
        = theReader read &type'int.

    #method writeLine : anObject
    [
        theWriter write::(anObject Literal) write::theNewLineConstant.
        
        ^ self.
    ]

    #method write : anObject
    [
        theWriter write::(anObject Literal).
        
        ^ self.
    ]
    
    #method writeLine
    [
        theWriter write::theNewLineConstant.
        
        ^ self.
    ]
        
    #method readLine
        = theReader read &Line.
        
    #method readLine : aTarget
        = aTarget << theReader read &Line.
        
    #method readChar
        = theReader read &type'widechar.
}

#static console = $Console new &output:system'io'stdConsoleWriter &input:system'io'stdConsoleReader.
