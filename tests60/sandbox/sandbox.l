import extensions;

public singleton Tester
{
   writeLine(s)
   {
      console.writeLine(s)
   }
}

public program()
{
   var c := new Symbol("sandbox'Tester");
   c.writeLine("Hello");
}