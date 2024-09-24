import system'threading;

class Sample1
{
   async static Task wait()
   {
      $await Task.sleep(10000);
      console.writeLine("10 Seconds wait Completed\n");
   }

   async static Task someMethod()
   {
      console.writeLine("Some Method Started......");
      $await wait();
      console.writeLine("Some Method End");      
   }

   static run()
   {
      console.writeLine("Main Method Started......");
      someMethod();
      console.writeLine("Main Method End");
      console.readChar();
   }
}

sample1()
{
   Sample1.run();
}