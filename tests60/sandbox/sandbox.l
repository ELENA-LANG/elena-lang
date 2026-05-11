import extensions;

sealed class X
{
   Reference<int> index;
   
   foo()
   {
      int value := *index;

      Console.printLine("foo=", value);
   }
}

public Program()
{
   new X().foo()
}
