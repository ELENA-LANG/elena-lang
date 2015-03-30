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
        theNewLineConstant := 'newLine literal.
    ]

    #method writeLine : anObject
    [
        theWriter write &literal:(anObject literal).
        
        $self write:theNewLineConstant.
    ]

    #method write : anObject
    [
        anObject cast:%write &to:$self.
    ]

    #method(generic) write : anObject
    [
        theWriter write &literal:(anObject literal).
    ]

    #method write &literal:aLiteral
    [
        theWriter write &literal:aLiteral.
    ]

    #method writeLine
    [
        theWriter write:theNewLineConstant.
    ]

    #method readLine
        = theReader read &line.

    #method readChar
    [
        #var(type:char)aRetVal := theReader char.
      
        ^ CharValue new &char:aRetVal.
    ]

    #method readKey
        = theReader int.
}

#static console = $Console new &reader:system'io'stdConsoleReader &writer:system'io'stdConsoleWriter.
