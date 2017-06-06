// --- Console ---

/// A console
class $Console
{
    object  theWriter.
    object  theReader.
    literal theNewLineConstant.

    /// Creates a console
    constructor new reader:aReader writer:aWriter
    [
        theWriter := aWriter.
        theReader := aReader.
        theNewLineConstant := 'newLine.
    ]

    /// Writes the specified literal value, followed by the current line terminator, to the standard output stream.
    writeLine : anObject
    [
        theWriter write literal:anObject.
        
        theWriter write literal:theNewLineConstant
    ]

    /// Writes the specified literal value to the standard output stream.
    write : anObject
    [
        anObject cast:%write to:$self.
    ]

    /// Writes the object literal presentation to the standard output stream.
    generic write : anObject
    [
        theWriter write literal:anObject.
        
        theWriter refresh.
    ]

    /// Writes the specified literal value to the standard output stream.
    write literal:aLiteral
    [
        theWriter write literal:aLiteral.
        
        theWriter refresh.
    ]

    /// Writes the specified literal value to the standard output stream.
    write wide:aLiteral
    [
        theWriter write wide:aLiteral.
        
        theWriter refresh.
    ]

    /// the current line terminator to the standard output stream.
    writeLine
    [
        self write:theNewLineConstant.
    ]

    /// Clears the console
    clear 
        => theWriter.

    stacksafe setCursorPosition int:x int:y
        => theWriter.

    /// Reads the next line of characters from the standard input stream.
    readLine
        = theReader read literal.

    /// Reads the next line of characters from the standard input stream.
    read literal
        => theReader.

    /// Reads the next line of characters from the standard input stream.
    read wide
        => theReader.

    /// Reads the next character from the input stream
    readChar
        = theReader char.

    bool is keyAvailable
        => theReader.

    /// Reads the next key code from the input stream
    readKey
        = theReader int.
}

/// A standard console
static console = $Console new reader:system'io'stdConsoleReader writer:system'io'stdConsoleWriter.
