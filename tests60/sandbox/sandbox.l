import extensions;

sealed class X
{
   Reference<int> index;
   
//   constructor() { index := 3 }

   foo()
   {
      int value := *index;
   }
}

public Program()
{
   new X().foo();
}
