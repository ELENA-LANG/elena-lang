//closed A
//{
//   indexed internal getResultUnsafe()
//      = nil;
//}
//
//sealed B : A
//{
//   indexed internal int getResultUnsafe()
//      = 2;
//}
//
//public program()
//{
//   A a := new B();
//
//   int n := a.getResultUnsafe();
//}

import extensions;
import system'threading;

singleton Tester
{
   async Task<int> getValueAsync()
   {
      ^ 2
   }

   async Task<int> addValueAsync(int n)
   {
      int val := $await getValueAsync();

      val := val + n;

      ^ val
   }
}

public program()
{
   int n := Tester.addValueAsync(3).Result;
   Console.printLine("the result is ", n);
}
