import extensions;

struct Y
{
   int val := 2;

   int Value = val;

   foo()
   {
      int n := *self;
   }
}

sealed class X
{
   Reference<int> index;
//   Reference</*int*/string> index;
   
   constructor() { index := 3 }
//   constructor() { index := /*3*/"3" }
//   constructor() { index := new Reference<string>("3") }

   foo()
   {
//       index.append(2);

      /*int*/var value := *index;

      Console.printLine("foo=", value);
//      Console.printLine("foo=", *index);
   }
}

public Program()
{
   new X().foo();
   new Y().foo();
}
