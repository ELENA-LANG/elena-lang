class Console
{
   write(s)
   {
      var writer := new io'ConsoleWriter();
      writer.write(s);
   }
}

public symbol console = new Console();
