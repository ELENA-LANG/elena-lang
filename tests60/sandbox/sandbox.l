import extensions;

A;

public singleton Tester
{
   printMe(/*object? */o)
   {
      Console.printLine(o)
   }
}

public program()
{
   Tester.printMe(nil)
}