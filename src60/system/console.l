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

//    abstract refresh();
}

public sealed class Console
{
   IConsoleReader reader;
   IConsoleWriter writer;

   internal constructor(IConsoleReader reader, IConsoleWriter writer)
   {
      this reader := reader;
      this writer := writer;
   }

   write(s)
   {
      writer.write(s.toPrintable());
   }

   writeLine(s)
   {
      writer.write(s.toPrintable());
      writer.write(newLineConstant);
   }

   writeLine()
   {
      writer.write(newLineConstant);
   }

   char readChar()
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
}

public static Console console = new Console(io'stdConsoleReader, io'stdConsoleWriter);
