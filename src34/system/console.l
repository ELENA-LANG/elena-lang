// --- Console ---

/// A console
class Console
{
    object               theWriter.
    T<io'IConsoleReader> theReader.
    literal              theNewLineConstant.

    /// Creates a console
    constructor new(object aReader, object aWriter)
    [
        theWriter := aWriter.
        theReader := aReader.
        theNewLineConstant := forwards'newLine.
    ]

    /// Writes the specified literal value, followed by the current line terminator, to the standard output stream.
    writeLine : anObject
    [
        theWriter write(anObject literal).
        
        theWriter write(theNewLineConstant)
    ]

    /// Writes the specified literal value to the standard output stream.
    write : anObject
    [
        self write(anObject literal).
    ]

    /// Writes the specified literal value to the standard output stream.
    write(literal aLiteral)
    [
        theWriter write(aLiteral).
        
        theWriter refresh.
    ]

    /// Writes the specified literal value to the standard output stream.
    write(wide aLiteral)
    [
        theWriter write(aLiteral).
        
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

//    stacksafe setCursorPosition int:x int:y
//        => theWriter.

    /// Reads the next line of characters from the standard input stream.
    literal readLine
        = theReader readLiteral.

    /// Reads the next line of characters from the standard input stream.
    wide readWideLine
        = theReader readWide.

    /// Reads the next character from the input stream
    readChar
    [
        char ch := $0.
        theReader read(&ch).
        
        ^  ch.
    ]

//    bool isKeyAvailable
//        => theReader.

    /// Reads the next key code from the input stream
    readKey
    [
        int key := 0.
        theReader read(&key).
        
        ^ key
    ]
}

/// A standard console
public static console = Console new(io'stdConsoleReader, io'stdConsoleWriter).
