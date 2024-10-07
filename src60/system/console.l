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

public sealed class ConsoleHelperImpl
{
   IConsoleReader reader;
   IConsoleWriter writer;

   constructor new(IConsoleReader reader, IConsoleWriter writer)
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
      ConsoleHelper.write(s.toPrintable());
      ConsoleHelper.write(NewLineConstant);
   }

   writeLine()
   {
      writer.write(NewLineConstant);
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

static ConsoleHelperImpl ConsoleHelper = ConsoleHelperImpl.new(io'stdConsoleReader, io'stdConsoleWriter);

public singleton Console
{
   write(s) 
            : info("Writes the specified literal value to the standard output stream.")
      => ConsoleHelper;

   writeLine(s)
            : info("Writes the specified literal value, followed by the current line terminator, to the standard output stream.")
      => ConsoleHelper;

   writeLine()
      => ConsoleHelper;

   char readChar() : info("Reads the next character from the input stream")
      => ConsoleHelper;

   get bool KeyAvailable()
      => ConsoleHelper;   

   setCursorPosition(int x, int y)
      => ConsoleHelper;

   clear()
      => ConsoleHelper;

   string readLine()
            : info("Reads the next line of characters from the standard input stream.")
      => ConsoleHelper;

   wide readWideLine()
            : info("Reads the next line of characters from the standard input stream.")
      => ConsoleHelper;
}

// obsolete
public ConsoleHelperImpl console = ConsoleHelper;
