import extensions;

class Test
{
   static test()
   {
      var o := X;
      
      Console.printLine("X=", o)
   }

   static int X
      = 2;
}

public program()
{
   Test.test()
}
