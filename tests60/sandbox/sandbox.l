import extensions;
import system'threading;
import ltests;

//class MyException : Exception
//{
//    constructor new(s)
//        <= super new(s);
//}
//
//class AwaitExceptionTester
//{
//   async Task doSomethingAsync()
//   {
//      MyException.raise("Failed");
//   }
//
//   async Task invokeAsync()
//   {
//      bool failed := false;
//      try
//      {
//         :await doSomethingAsync();
//      }
//      catch(MyException e)
//      {
//         failed := true;
//      };
//
//      Assert.ifTrue(failed);
//   }
//}
//
//
//public awaitExceptionTest() : testCase()
//{
//   auto tester := new AwaitExceptionTester();
//   tester.invokeAsync().Result;
//
//   Console.writeLine(".");
//}
//
//waitForAsyncGate(gate)
//{
//   gate.wait()
//}

class AsyncAwaitScenarios
{
   
   async Task<int> sumAwaitedValuesAsync(int value)
   {
/*      Task<int> v := :await delayedValueAsync(value);
      Task<int> v2 := :await delayedValueAsync(value + 1);
      
      ^ v.Result + v.Result*/
      
      ^ (:await delayedValueAsync(value)) + (:await delayedValueAsync(value + 1))
   }
   
   
/*   async Task<int> resumeWithValueAsync(Task blocker, int value)
   {
      :await blocker;

      ^ value
   }
*/
   async Task<int> delayedValueAsync(int value)
   {
      :await Task.sleep(2);

      ^ value
   }
/*
   async Task<int> awaitValueThenReturnAsync(Task<int> blocker)
   {
      int result := :await blocker;

      ^ result
   }

   async Task<int> returnAwaitedValueAsync(Task<int> blocker)
   {
      ^ :await blocker
   }

   async Task<int> sumAwaitedValuesViaLocalAsync(int value)
   {
      int result := (:await delayedValueAsync(value)) + (:await delayedValueAsync(value + 1));

      ^ result
   }
*/
/*
   async Task<int> sumThreeAwaitedValuesAsync(int value)
   {
      ^ (:await delayedValueAsync(value)) + (:await delayedValueAsync(value + 1)) + (:await delayedValueAsync(value + 2))
   }

   async Task<int> chainedAsync(int value)
   {
      int first := :await delayedValueAsync(value);
      int second := :await delayedValueAsync(value + 1);

      ^ first + second
   }

   async Task failAfterAsync(Task blocker)
   {
      :await blocker;

      MyException.raise("failed after suspension")
   }

   async Task incrementAfterSuspensionAsync()
   {
      :await Task.sleep(1);

      AsyncTaskCounter.increment()
   }*/
}

public Program()
{
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();

   // The first result must survive while the second await is suspended.
//   Assert.ifEqual(scenarios.sumAwaitedValuesViaLocalAsync(20).Result, 41);

   Task<int> direct := scenarios.sumAwaitedValuesAsync(20);
   
   int r := direct.Result;
   
/*   ManualResetEvent completed := ManualResetEvent.new();
   direct.continueWith((Task task){ completed.set() });

   Assert.ifTrue(completed.wait(5000));
   Assert.ifEqual(direct.Result, 41);
*/
   Console.write(r)
}


//public singleton AsyncTaskCounter
//{
//   static object _sync := new object();
//   static int _value := 0;
//
//   reset()
//   {
//      lock(_sync) {
//         _value := 0
//      }
//   }
//
//   increment()
//   {
//      lock(_sync) {
//         _value := _value + 1
//      }
//   }
//
//   int Value
//   {
//      get()
//      {
//         int current := 0;
//         lock(_sync) {
//            current := _value
//         };
//
//         ^ current
//      }
//   }
//}
