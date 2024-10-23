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
      int val := :await getValueAsync();

      val := val + n;

      ^ val
   }
}

sample2()
{
   int n := Tester.addValueAsync(3).Result;
   Console.printLine("the result is ", n);
}
