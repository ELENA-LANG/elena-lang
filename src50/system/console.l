//// --- IConsoleReader ---
//
//public interface IConsoleReader
//{
//    abstract string readLine() {}
//    
//    abstract wide readWideLine() {}
//    
////    abstract read(ref<int> retVal) []
//    
//    abstract read(ref char retVal) {}
//    
//    abstract get bool KeyAvailable() {}
//}

// --- Console ---

/// A console
sealed class Console
{
    object         writer;
    /*IConsoleReader*/object reader;

    /// Creates a console
    constructor(reader, writer)
    {
        this writer := writer;
        this reader := reader
    }

//    /// Writes the specified literal value, followed by the current line terminator, to the standard output stream.
//    writeLine(o)
//    {
//        self.writeLine(o.Printable)
//    }
//
//    /// Writes the specified literal value to the standard output stream.
//    write(o)
//    {
//        self.write(o.Printable)
//    }

    /// Writes the specified literal value to the standard output stream.
    write(string s)
    {
        writer.write(s);
        
//        theWriter.refresh();
    }

    /// Writes the specified literal value to the standard output stream.
    write(wide s)
    {
        writer.write(s);
        
//        theWriter.refresh()
    }

    /// the current line terminator to the standard output stream.
    writeLine(string s)
    {
        writer.writeLine(s);
        
//        theWriter.refresh();
    }

    /// the current line terminator to the standard output stream.
    writeLine(wide s)
    {
        writer.writeLine(s);
        
//        theWriter.refresh();
    }
    
    /// the current line terminator to the standard output stream.
    writeLine()
    {
        writer.writeLine();
        
//        theWriter.refresh();
    }

//    /// Clears the console
//    clear()
//        => theWriter;
//
//    setCursorPosition(int x, int y)
//        => theWriter;
//
//    /// Reads the next line of characters from the standard input stream.
//    string readLine()
//        = theReader.readLine();
//
////    /// Reads the next line of characters from the standard input stream.
////    wide readWideLine
////        = theReader readWide.

    /// Reads the next character from the input stream
    char readChar()
    {
        reader.read(ref char retVal);
        
        ^ retVal
    }

//    get bool KeyAvailable()
//        => theReader;
//
////    /// Reads the next key code from the input stream
////    readKey
////    [
////        int key := 0.
////        theReader read(&key).
////        
////        ^ key
////    ]
}

/// A standard console
public static Console console = new Console(io'stdConsoleReader, io'stdConsoleWriter);

