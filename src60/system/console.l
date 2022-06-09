class Console
{
   write(s)
   {
      var writer := new io'ConsoleWriter();
      writer.write(s);
   }

   writeLine(s)
   {
      var writer := new io'ConsoleWriter();
      writer.write(s);
      writer.write(newLine);
   }
}

public symbol console = new Console();
