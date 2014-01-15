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

    #method writeLine : anObject
    [
        theWriter write &literal::anObject write &literal:theNewLineConstant.
    ]

    #method write : anObject
    [
        theWriter write &literal::anObject.
    ]

    #method write &literal:aLiteral
    [
        theWriter write &literal:aLiteral.
    ]
            
    #method writeLine
    [
        theWriter write &literal:theNewLineConstant.
    ]
        
    #method readLine
        = theReader read &Line.
        
    #method readChar
        = theReader read &char.

    #method readKey
        = theReader read &int.
}

#static console = $Console new &output:system'io'stdConsoleWriter &input:system'io'stdConsoleReader.
