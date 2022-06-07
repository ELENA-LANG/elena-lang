import system;

singleton Tester
{
   run()
   {
      for (int n := 0, n < 5, n := n + 1) {
         console.write("*");
      }
   }
}

public symbol program = Tester.run();
