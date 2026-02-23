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

public sealed class ConsoleHelperImpl : info("A class implementing Console functionality")
{
   IConsoleReader reader;
   IConsoleWriter writer;

   constructor new(IConsoleReader reader, IConsoleWriter writer) : info("Initializes the class with the specified input <b>reader</b> and output <b>writer</b>")
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

   writeLine() : info("Writes the current line terminator")
   {
      writer.write(NewLineConstant);
   }

   char readChar()
      : info("Reads the next character from the input stream")
   {
      char retVal := reader.read();
        
      ^ retVal
   }

   get bool KeyAvailable() : info("Checks if there is a record in the input stream")
      => reader;   

   setCursorPosition(int x, int y) : info("Sets the console cursor at the specified position<br/>Cannot be supported in the file output mode")
      => writer;

   clear() : info("Clear the console buffer<br/>Cannot be supported in the file output mode")
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

public singleton Console : info("Console singleton")
{
   write(s) 
            : info("Writes the specified literal value to the standard output stream.")
      => ConsoleHelper;

   writeLine(s)
            : info("Writes the specified literal value, followed by the current line terminator, to the standard output stream.")
      => ConsoleHelper;

   writeLine() : info("Writes the current line terminator")
      => ConsoleHelper;

   char readChar() : info("Reads the next character from the input stream")
      => ConsoleHelper;

   get bool KeyAvailable() : info("Checks if there is a record in the input stream")
      => ConsoleHelper;   

   setCursorPosition(int x, int y) : info("Sets the console cursor at the specified position<br/>Cannot be supported in the file output mode")
      => ConsoleHelper;

   clear() : info("Clear the console buffer<br/>Cannot be supported in the file output mode")
      => ConsoleHelper;

   string readLine()
            : info("Reads the next line of characters from the standard input stream.")
      => ConsoleHelper;

   wide readWideLine()
            : info("Reads the next line of characters from the standard input stream.")
      => ConsoleHelper;
}

// obsolete
public ConsoleHelperImpl console : info("A console symbol<br/>Obsolete. Please use Console") = ConsoleHelper;
