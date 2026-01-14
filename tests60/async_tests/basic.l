import extensions;
import ltests;

class MyException : Exception
{
    constructor new(s)
        <= super new(s);
}

class AwaitExceptionTester
{
   bool failed;

   constructor()
   {
      failed := false;
   }

   async Task doSomethingAsync()
   {
      MyException.raise("Failed");
   }

   async Task invokeAsync()
   {
      try
      {
         :await doSomethingAsync();
      }
      catch(MyException e)
      {
         failed := true;
      }
   }

   validate()
   {
      Assert.ifTrue(failed);
   }
}


public awaitExceptionTest() : testCase()
{
   auto tester := new AwaitExceptionTester();
   tester.invokeAsync().Result;
   tester.validate();

   Console.writeLine(".");
}
