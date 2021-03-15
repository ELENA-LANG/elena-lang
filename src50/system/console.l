//// --- IConsoleReader ---
//
//public interface IConsoleReader
//{
//    abstract string readLine();
//    
//    abstract wide readWideLine();
//    
//    abstract read(ref char retVal);
//    
//    get abstract bool KeyAvailable();
//}

// --- IConsoleWriter ---

public interface IConsoleWriter
{
    abstract write(wide s);
    abstract write(string s);
    abstract writeLine(wide s);
    abstract writeLine(string s);
    abstract writeLine();
    
    abstract refresh();
}

// --- Console ---

/// A console
sealed class Console
{
    IConsoleWriter writer;
//    IConsoleReader reader;

    /// Creates a console
    constructor(/*reader, */writer)
    {
        this writer := writer;
//        this reader := reader
    }

    /// Writes the specified literal value, followed by the current line terminator, to the standard output stream.
    writeLine(o)
    {
        self.writeLine(o.toPrintable())
    }

    [info:"Writes the specified literal value to the standard output stream."]
    write(o)
    {
        self.write(o.toPrintable())
    }

    [info:"Writes the specified literal value to the standard output stream."]
    write(string s)
    {
        writer.write(s);
        
        writer.refresh();
    }

    [info:"Writes the specified literal value to the standard output stream."]
    write(wide s)
    {
        writer.write(s);
        
        writer.refresh()
    }

    /// the current line terminator to the standard output stream.
    writeLine(string s)
    {
        writer.writeLine(s);
        
        writer.refresh();
    }

    /// the current line terminator to the standard output stream.
    writeLine(wide s)
    {
        writer.writeLine(s);
        
        writer.refresh();
    }
    
    /// the current line terminator to the standard output stream.
    writeLine()
    {
        writer.writeLine();
        
        writer.refresh();
    }

//    /// Clears the console
//    clear()
//        => writer;
//
//    setCursorPosition(int x, int y)
//        => writer;
//
//    /// Reads the next line of characters from the standard input stream.
//    string readLine()
//        = reader.readLine();
//
//    /// Reads the next line of characters from the standard input stream.
//    wide readWideLine()
//        = reader.readWideLine();
//
//    /// Reads the next character from the input stream
//    char readChar()
//    {
//        reader.read(ref char retVal);
//        
//        ^ retVal
//    }
//
//    get bool KeyAvailable()
//        => reader;
}

/// A standard console
public static Console console = new Console(/*io'stdConsoleReader, */io'stdConsoleWriter);

