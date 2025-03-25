import system'threading;

class Sample1
{
   async static Task wait()
   {
      :await Task.sleep(10000);
      Console.writeLine("10 Seconds wait Completed\n");
   }

   async static Task someMethod()
   {
      Console.writeLine("Some Method Started......");
      :await wait();
      Console.writeLine("Some Method End");      
   }

   static run()
   {
      Console.writeLine("Main Method Started......");
      someMethod();
      Console.writeLine("Main Method End");
      Console.readChar();
   }
}

sample1()
{
   Sample1.run();
}