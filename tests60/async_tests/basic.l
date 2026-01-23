import extensions;
import system'threading;
import ltests;

class MyException : Exception
{
    constructor new(s)
        <= super new(s);
}

class AwaitExceptionTester
{
   async Task doSomethingAsync()
   {
      MyException.raise("Failed");
   }

   async Task invokeAsync()
   {
      bool failed := false;
      try
      {
         :await doSomethingAsync();
      }
      catch(MyException e)
      {
         failed := true;
      };

      Assert.ifTrue(failed);
   }
}


public awaitExceptionTest() : testCase()
{
   auto tester := new AwaitExceptionTester();
   tester.invokeAsync().Result;

   Console.writeLine(".");
}
