sealed class Console
{
   field reader;
   field writer;

   constructor(reader, writer)
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

    char readChar()
    {
        char retVal := reader.read();
        
        ^ retVal
    }
}

public static console = new Console(io'stdConsoleReader, io'stdConsoleWriter);
