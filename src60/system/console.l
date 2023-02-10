// --- IConsoleReader ---

public interface IConsoleReader
{
    abstract string readLine();
    
//    abstract wide readWideLine();
    
    abstract char read();
    
//    get abstract bool KeyAvailable();
}

// --- IConsoleWriter ---

public interface IConsoleWriter
{
    abstract write(wide s);
    abstract write(string s);
    abstract writeLine(wide s);
    abstract writeLine(string s);
    abstract writeLine();
    
//    abstract refresh();
}

sealed class Console
{
   /*IConsoleReader*/object reader;
   /*IConsoleWriter*/object writer;

   constructor(/*IConsoleReader*/ reader, /*IConsoleWriter*/ writer)
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

   string readLine()
            : info("Reads the next line of characters from the standard input stream.")
      = reader.readLine();   
}

public static console = new Console(io'stdConsoleReader, io'stdConsoleWriter);
