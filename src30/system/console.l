// --- Console ---
#class $Console
{
    #field theWriter.
    #field theReader.
    #field(type:literal)theNewLineConstant.

    #constructor new &reader:aReader &writer:aWriter
    [
        theWriter := aWriter.
        theReader := aReader.
        theNewLineConstant := system'io'newLine literal.
    ]

    #method writeLine : anObject
    [
        theWriter write &literal:anObject write &literal:theNewLineConstant.
    ]

    #method write : anObject
    [
        theWriter write &literal:anObject.
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
        = theReader read &line.

    #method readChar
    [
        #var(type:short)aRetVal := theReader short.
      
        ^ CharValue new &short:aRetVal.
    ]

    #method readKey
        = theReader int.
}

#static console = $Console new &reader:system'io'stdConsoleReader &writer:system'io'stdConsoleWriter.
