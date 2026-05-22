import extensions;
import system'collections;
import system'runtime;
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

singleton AsyncValueTester
{
   async Task<int> getValueAsync()
   {
      ^ 2
   }

   async Task<int> addValueAsync(int n)
   {
      int val := :await getValueAsync();

      ^ val + n
   }
}

singleton AsyncThrowTester
{
   async Task failAsync()
   {
      MyException.raise("Failed");
   }
}

singleton AsyncStressTester
{
   async Task incrementAsync(int[] counter, object gate)
   {
      :await Task.run({
         lock(gate) {
            counter[0] := counter[0] + 1;
         }
      });
   }

   async Task<int> runManyAsync(int taskCount)
   {
      auto tasks := new List<Task>();
      int[] counter := new int[](1);
      object gate := new Object();

      for (int i := 0; i < taskCount; i++) {
         tasks.append(incrementAsync(counter, gate));
      };

      :await Task.whenAll(tasks.Value);

      ^ counter[0]
   }
}

class CountingContinuation : Action<Task>
{
   int[]  _counter;
   object _gate;

   constructor(int[] counter, object gate)
   {
      _counter := counter;
      _gate := gate;
   }

   function(Task completed)
   {
      lock(_gate) {
         _counter[0] := _counter[0] + 1;
      }
   }
}

public asyncAwaitMassiveTaskCheck()
{
   int taskCount := 64;

   Assert.ifEqual(taskCount, AsyncStressTester.runManyAsync(taskCount).Result);

   Console.writeLine(".");
}

public taskRunCompletesTest() : testCase()
{
   bool completed := false;

   Task.run({ completed := true; }).Result;

   Assert.ifTrue(completed);

   Console.writeLine(".");
}

public taskRunExceptionTest() : testCase()
{
   bool failed := false;

   try
   {
      Task.run({ MyException.raise("Failed"); }).Result;
   }
   catch(MyException e)
   {
      failed := true;
   };

   Assert.ifTrue(failed);

   Console.writeLine(".");
}

public asyncValueTest() : testCase()
{
   Assert.ifEqual(5, AsyncValueTester.addValueAsync(3).Result);

   Console.writeLine(".");
}

public asyncDirectExceptionTest() : testCase()
{
   bool failed := false;

   try
   {
      AsyncThrowTester.failAsync().Result;
   }
   catch(MyException e)
   {
      failed := true;
   };

   Assert.ifTrue(failed);

   Console.writeLine(".");
}


public awaitExceptionTest() : testCase()
{
   auto tester := new AwaitExceptionTester();
   tester.invokeAsync().Result;

   Console.writeLine(".");
}

public taskWhenAllEmptyTest() : testCase()
{
   Task.whenAll(new Task[](0)).Result;

   Console.writeLine(".");
}

public taskMultipleContinuationsTest() : testCase()
{
   Task task := Task.run({});
   int[] counter := new int[](1);
   object gate := new Object();
   Action<Task> continuation := new CountingContinuation(counter, gate);

   for (int i := 0; i < 32; i++) {
      task.continueWith(continuation);
   };

   task.wait();
   Thread.sleep(1000);

   Assert.ifEqual(32, counter[0]);

   Console.writeLine(".");
}

public taskPoolStressTest() : testCase()
{
   auto tasks := new List<Task>();
   int[] counter := new int[](1);
   int taskCount := Environment.ProcessorCount * 2;

   if (taskCount < 16) {
      taskCount := 16
   };

   for (int i := 0; i < taskCount; i++) {
      tasks.append(Task.run({
         lock(tasks) {
            counter[0] := counter[0] + 1;
         }
      }));
   };

   Task.waitAll(tasks.Value);

   Assert.ifEqual(taskCount, counter[0]);

   Console.writeLine(".");
}
