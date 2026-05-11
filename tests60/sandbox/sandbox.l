import extensions;

sealed class X
{
   Reference<int> index;
   
   foo()
   {
      /*int*/var value := *index;

      Console.printLine("foo=", value);
   }
}

public Program()
{
   new X().foo()
}
