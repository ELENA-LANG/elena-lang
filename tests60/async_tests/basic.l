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

waitForAsyncGate(gate)
{
   gate.wait()
}

class AsyncAwaitScenarios
{
   async Task<int> resumeWithValueAsync(Task blocker, int value)
   {
      :await blocker;

      ^ value
   }

   async Task<int> delayedValueAsync(int value)
   {
      :await Task.sleep(2);

      ^ value
   }

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

   async Task<int> sumAwaitedValuesAsync(int value)
   {
      ^ (:await delayedValueAsync(value)) + (:await delayedValueAsync(value + 1))
   }

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
   }
}

public awaitSuspensionAndResultTest() : testCase()
{
   ManualResetEvent gate := ManualResetEvent.new();
   Task blocker := Task.run(&waitForAsyncGate, gate);
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();

   Task<int> pending := scenarios.resumeWithValueAsync(blocker, 42);

   // A real await must return control to the caller while the antecedent is
   // incomplete, then resume the generated state machine after it completes.
   Assert.ifFalse(pending.IsCompleted);
   gate.set();
   Assert.ifEqual(pending.Result, 42);
   Assert.ifTrue(pending.IsCompletedSuccessfully);

   gate.close();

   Console.write(".")
}

public awaitValueThenReturnTest() : testCase()
{
   ManualResetEvent gate := ManualResetEvent.new();
   Task<int> blocker := class Task<int>.run(
      {
         gate.wait();

         ^ 42
      });
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();

   Task<int> pending := scenarios.awaitValueThenReturnAsync(blocker);
   Task<int> directPending := scenarios.returnAwaitedValueAsync(blocker);
   ManualResetEvent directCompleted := ManualResetEvent.new();
   directPending.continueWith((Task task){ directCompleted.set() });

   Assert.ifFalse(pending.IsCompleted);
   Assert.ifFalse(directPending.IsCompleted);

   gate.set();
   Assert.ifEqual(pending.Result, 42);
   Assert.ifTrue(pending.IsCompletedSuccessfully);
   Assert.ifTrue(directCompleted.wait(5000));
   Assert.ifEqual(directPending.Result, 42);
   Assert.ifTrue(directPending.IsCompletedSuccessfully);

   // A completed antecedent follows the synchronous fast path.
   Task<int> completed := class Task<int>.fromResult(7);
   Assert.ifEqual(scenarios.awaitValueThenReturnAsync(completed).Result, 7);
   Assert.ifEqual(scenarios.returnAwaitedValueAsync(completed).Result, 7);

   directCompleted.close();
   gate.close();

   Console.write(".")
}

public multipleAwaitsInExpressionTest() : testCase()
{
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();

   // The first result must survive while the second await is suspended.
   Assert.ifEqual(scenarios.sumAwaitedValuesViaLocalAsync(20).Result, 41);

   Task<int> direct := scenarios.sumAwaitedValuesAsync(20);
   ManualResetEvent completed := ManualResetEvent.new();
   direct.continueWith((Task task){ completed.set() });

   Assert.ifTrue(completed.wait(5000));
   Assert.ifEqual(direct.Result, 41);

   completed.reset();

   Task<int> directThree := scenarios.sumThreeAwaitedValuesAsync(20);
   directThree.continueWith((Task task){ completed.set() });

   Assert.ifTrue(completed.wait(5000));
   Assert.ifEqual(directThree.Result, 63);

   completed.close();

   Console.write(".")
}

public chainedAwaitTest() : testCase()
{
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();

   // Exercises two suspension/resume cycles and generic result extraction.
   Assert.ifEqual(scenarios.chainedAsync(20).Result, 41);

   Console.write(".")
}

public awaitExceptionAfterSuspensionTest() : testCase()
{
   ManualResetEvent gate := ManualResetEvent.new();
   Task blocker := Task.run(&waitForAsyncGate, gate);
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();
   Task faulted := scenarios.failAfterAsync(blocker);

   Assert.ifFalse(faulted.IsCompleted);
   gate.set();

   bool failed := false;
   try {
      faulted.Result
   }
   catch (MyException e) {
      failed := true
   };

   Assert.ifTrue(failed);
   Assert.ifTrue(faulted.IsFaulted);

   gate.close();

   Console.write(".")
}

public asyncAwaitStressTest() : testCase()
{
   int rounds := 20;
   int batchSize := 50;
   AsyncAwaitScenarios scenarios := new AsyncAwaitScenarios();

   AsyncTaskCounter.reset();

   for (int round := 0; round < rounds; round += 1) {
      Task[] tasks := new Task[](batchSize);

      for (int i := 0; i < batchSize; i += 1)
         tasks[i] := scenarios.incrementAfterSuspensionAsync();

      Task.whenAll(tasks).wait();
      Assert.ifEqual(AsyncTaskCounter.Value, (round + 1) * batchSize)
   };

   Assert.ifEqual(AsyncTaskCounter.Value, rounds * batchSize);

   Console.write(".")
}

public singleton AsyncTaskCounter
{
   static object _sync := new object();
   static int _value := 0;

   reset()
   {
      lock(_sync) {
         _value := 0
      }
   }

   increment()
   {
      lock(_sync) {
         _value := _value + 1
      }
   }

   int Value
   {
      get()
      {
         int current := 0;
         lock(_sync) {
            current := _value
         };

         ^ current
      }
   }
}

recordSuccessfulContinuation(Task completed)
{
   Assert.ifTrue(completed.IsCompletedSuccessfully);
   AsyncTaskCounter.increment()
}

incrementAsyncTaskCounter()
{
   AsyncTaskCounter.increment()
}

public taskStateAndResultTest() : testCase()
{
   Task task := Task.assign({});
   Assert.ifTrue(task.Status == TaskStatus.Created);

   task.start();
   task.wait();

   Assert.ifTrue(task.IsCompleted);
   Assert.ifTrue(task.IsCompletedSuccessfully);
   Assert.ifTrue(task.Status == TaskStatus.RanToCompletion);

   bool secondStartFailed := false;
   try {
      task.start()
   }
   catch (InvalidOperationException e) {
      secondStartFailed := true
   };
   Assert.ifTrue(secondStartFailed);

   Task<int> valueTask := class Task<int>.run({ ^ 42 });
   Assert.ifEqual(valueTask.Result, 42);

   Console.write(".")
}

public multipleContinuationsTest() : testCase()
{
   int continuationCount := 32;

   ManualResetEvent gate := ManualResetEvent.new();
   AsyncTaskCounter.reset();
   Task antecedent := Task.run({ gate.wait() });
   Task[] continuations := new Task[](continuationCount);

   for (int i := 0; i < continuationCount; i += 1) {
      continuations[i] := antecedent.continueWith(&recordSuccessfulContinuation)
   };

   gate.set();
   try {
      Task.whenAll(continuations).wait()
   }
   catch (AggregateException e) {
      Exception[] failures := e.InnerExceptions;
      failures[0].raise()
   };

   Assert.ifEqual(AsyncTaskCounter.Value, continuationCount);
   gate.close();

   Task late := antecedent.continueWith(&recordSuccessfulContinuation);
   late.wait();
   Assert.ifEqual(AsyncTaskCounter.Value, continuationCount + 1);

   Console.write(".")
}

public continuationExceptionTest() : testCase()
{
   Task continuation := Task.completedTask().continueWith(
      (Task completed)
      {
         MyException.raise("continuation failed")
      });

   bool failed := false;
   try {
      continuation.wait()
   }
   catch (MyException e) {
      failed := true
   };

   Assert.ifTrue(failed);
   Assert.ifTrue(continuation.IsFaulted);

   Console.write(".")
}

public whenAllTest() : testCase()
{
   Task[] empty := new Task[](0);
   Task emptyResult := Task.whenAll(empty);
   Assert.ifTrue(emptyResult.IsCompletedSuccessfully);

   Task[] tasks := new Task[](4);
   for (int i := 0; i < tasks.Length; i += 1)
      tasks[i] := Task.run({});

   Task all := Task.whenAll(tasks);
   all.wait();
   Assert.ifTrue(all.IsCompletedSuccessfully);

   Task[] failedTasks := new Task[](3);
   failedTasks[0] := Task.fromException(MyException.new("first"));
   failedTasks[1] := Task.completedTask();
   failedTasks[2] := Task.fromException(MyException.new("second"));

   bool aggregateRaised := false;
   try {
      Task.whenAll(failedTasks).wait()
   }
   catch (AggregateException e) {
      aggregateRaised := true;
      Assert.ifEqual(e.InnerExceptions.Length, 2)
   };

   Assert.ifTrue(aggregateRaised);

   Console.write(".")
}

public eventSemanticsTest() : testCase()
{
   ManualResetEvent manual := ManualResetEvent.new();
   Assert.ifFalse(manual.wait(10));
   manual.set();
   Assert.ifTrue(manual.wait(100));
   manual.reset();
   Assert.ifFalse(manual.wait(10));
   manual.close();

   AutoResetEvent automatic := AutoResetEvent.new();
   automatic.set();
   Assert.ifTrue(automatic.wait(100));
   Assert.ifFalse(automatic.wait(10));
   automatic.close();

   CountDownEvent completed := CountDownEvent.new(0);
   Assert.ifTrue(completed.wait(10));
   completed.close();

   Console.write(".")
}

class ThreadPoolGrowthState
{
   CountDownEvent _entered;
   ManualResetEvent _release;

   constructor new(int workerCount)
   {
      _entered := CountDownEvent.new(workerCount);
      _release := ManualResetEvent.new()
   }

   block()
   {
      _entered.signal();
      _release.wait()
   }

   bool waitForWorkers(int timeout)
      = _entered.wait(timeout);

   release()
      = _release.set();

   close()
   {
      _entered.close();
      _release.close()
   }
}

threadPoolBlockingWorkItem(state)
{
   state.block()
}

public threadPoolGrowthTest() : testCase()
{
   int workerCount := 4;

   ThreadPoolGrowthState state := ThreadPoolGrowthState.new(workerCount);
   Task[] blockers := new Task[](workerCount);

   for (int i := 0; i < workerCount; i += 1) {
      blockers[i] := Task.run(&threadPoolBlockingWorkItem, state)
   };

   bool allWorkersEntered := state.waitForWorkers(5000);
   state.release();
   Task.whenAll(blockers).wait();

   Assert.ifTrue(allWorkersEntered);
   Assert.ifEqual(ThreadPool.WorkerCount, workerCount);

   state.close();

   Console.write(".")
}

public taskBatchStressTest() : testCase()
{
   int rounds := 20;
   int batchSize := 100;

   AsyncTaskCounter.reset();

   for (int round := 0; round < rounds; round += 1) {
      Task[] tasks := new Task[](batchSize);

      for (int i := 0; i < batchSize; i += 1) {
         tasks[i] := Task.run(&incrementAsyncTaskCounter)
      };

      Task.whenAll(tasks).wait();
      Assert.ifEqual(AsyncTaskCounter.Value, (round + 1) * batchSize)
   };

   Assert.ifEqual(AsyncTaskCounter.Value, rounds * batchSize);

   Console.write(".")
}

public continuationStressTest() : testCase()
{
   int rounds := 25;
   int continuationCount := 40;

   AsyncTaskCounter.reset();

   for (int round := 0; round < rounds; round += 1) {
      Task antecedent := Task.run({});
      Task[] callbacks := new Task[](continuationCount);

      for (int i := 0; i < continuationCount; i += 1) {
         callbacks[i] := antecedent.continueWith(&recordSuccessfulContinuation)
      };

      Task.whenAll(callbacks).wait()
   };

   Assert.ifEqual(AsyncTaskCounter.Value, rounds * continuationCount);

   Console.write(".")
}
