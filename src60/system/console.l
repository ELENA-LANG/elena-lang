// --- IConsoleReader ---

public interface IConsoleReader
{
    abstract string readLine();
    
    abstract wide readWideLine();
    
    abstract char read();
    
    get abstract bool KeyAvailable();
}

// --- IConsoleWriter ---

public interface IConsoleWriter
{
   abstract write(wide s);
   abstract write(string s);
    
   abstract clear();

   abstract setCursorPosition(int x, int y);

   abstract refresh();
}

public sealed class Console
{
   IConsoleReader reader;
   IConsoleWriter writer;

   internal constructor new(IConsoleReader reader, IConsoleWriter writer)
   {
      this reader := reader;
      this writer := writer;
   }

   write(s)
      : info("Writes the specified literal value to the standard output stream.")
   {
      writer.write(s.toPrintable());
   }

   writeLine(s)
      : info("Writes the specified literal value, followed by the current line terminator, to the standard output stream.")
   {
      writer.write(s.toPrintable());
      writer.write(newLineConstant);
   }

   writeLine()
   {
      writer.write(newLineConstant);
   }

   char readChar()
      : info("Reads the next character from the input stream")
   {
      char retVal := reader.read();
        
      ^ retVal
   }

   get bool KeyAvailable()
      => reader;   

   setCursorPosition(int x, int y)
      => writer;

   clear()
   {
      writer.clear()
   }

   string readLine()
            : info("Reads the next line of characters from the standard input stream.")
      = reader.readLine();   

   wide readWideLine()
            : info("Reads the next line of characters from the standard input stream.")
      = reader.readWideLine();   
}

public static Console console = Console.new(io'stdConsoleReader, io'stdConsoleWriter);
