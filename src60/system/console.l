class Console
{
   write(s)
   {
      var writer := new io'ConsoleWriter();
      writer.write(s.toPrintable());
   }

   writeLine(s)
   {
      var writer := new io'ConsoleWriter();
      writer.write(s.toPrintable());
      writer.write(newLine);
   }
}

public static console = new Console();
