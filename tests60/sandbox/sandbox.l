import system'threading;

class Sample1
{
   async static Task wait()
   {
      Exception.raise("error");

      ^ 2
   }

   async static Task someMethod()
   {
      Console.writeLine("Some Method Started......");
      :await wait();
      //int n := :await wait();
      Console.writeLine("Some Method End");      
   }
}

public Program()
{
   Sample1.someMethod().Result;
}