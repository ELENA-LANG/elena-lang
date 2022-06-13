import system;

class A
{
   nameMe()
   {
      console.writeLine($name self);
   }
}

singleton Tester
{
   run()
   {
       new A().nameMe()

//      for (int n := 0, n < 5, n := n + 1) {
//         console.write("*");
//      }
   }
}

public symbol program = Tester.run();
