import extensions;

sealed class X
{
   Reference</*int*/string> index;
   
   constructor() { index := /*3*/"3" }

   foo()
   {
      /*int*/var value := *index;

      Console.printLine("foo=", value);
      //Console.printLine("foo=", *index);
   }
}

public Program()
{
   new X().foo()
}
