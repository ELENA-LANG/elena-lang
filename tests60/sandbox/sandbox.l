import extensions;

public extension commonOpX
{
   back(o)
      = o;
}

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