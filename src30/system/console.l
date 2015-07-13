// --- Console ---

/// A console
#class $Console
{
    #field theWriter.
    #field theReader.
    #field(type:literal)theNewLineConstant.

    /// Creates a console
    #constructor new &reader:aReader &writer:aWriter
    [
        theWriter := aWriter.
        theReader := aReader.
        theNewLineConstant := 'newLine literal.
    ]

    /// Writes the specified literal value, followed by the current line terminator, to the standard output stream.
    #method writeLine : anObject
    [
        theWriter write &literal:(anObject literal).
        
        theWriter write &literal:theNewLineConstant.
    ]

    /// Writes the specified literal value to the standard output stream.
    #method write : anObject
    [
        anObject cast:%write &to:$self.
    ]

    /// Writes the object literal presentation to the standard output stream.
    #method(generic) write : anObject
    [
        theWriter write &literal:(anObject literal).
        
        theWriter refresh.
    ]

    /// Writes the specified literal value to the standard output stream.
    #method write &literal:aLiteral
    [
        theWriter write &literal:aLiteral.
        
        theWriter refresh.
    ]

    /// the current line terminator to the standard output stream.
    #method writeLine
    [
        self write:theNewLineConstant.
    ]

    /// Clears the console
    #method clear 
        => theWriter.

    /// Reads the next line of characters from the standard input stream.
    #method readLine
        = theReader read &literal.

    /// Reads the next line of characters from the standard input stream.
    #method read &literal
        = theReader read &literal.

    /// Reads the next line of characters from the standard input stream.
    #method read &wide
        = theReader read &wide.

    /// Reads the next character from the input stream
    #method readChar
        = theReader char.

    /// Reads the next key code from the input stream
    #method readKey
        = theReader int.
}

/// A standard console
#static console = $Console new &reader:system'io'stdConsoleReader &writer:system'io'stdConsoleWriter.
