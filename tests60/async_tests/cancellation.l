import extensions;
import system'threading;
import ltests;

class CancellationTestException : Exception
{
   constructor new(string message)
      <= super new(message);
}

singleton CancellationTestCounter
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
         int value := 0;
         lock(_sync) {
            value := _value
         };

         ^ value
      }
   }
}

incrementCancellationTestCounter()
{
   CancellationTestCounter.increment()
}

recordCanceledContinuation(Task completed)
{
   Assert.ifTrue(completed.IsCanceled);
   CancellationTestCounter.increment()
}

bool waitForTaskCompletion(Task task, int timeout)
{
   ManualResetEvent completed := ManualResetEvent.new();
   task.continueWith((Task antecedent){ completed.set() });

   bool signaled := completed.wait(timeout);
   if (signaled)
      completed.close();

   ^ signaled
}

class AsyncCancellationScenarios
{
   private Task cancellableOperation(CancellationToken token, ManualResetEvent entered)
   {
      ^ Task.run(
         {
            entered.set();
            while (token.IsCancellationRequested.Inverted)
               Thread.sleep(1);

            token.throwIfCancellationRequested()
         }, token)
   }

   async Task waitAsync(CancellationToken token, ManualResetEvent entered)
   {
      :await cancellableOperation(token, entered);

      CancellationTestCounter.increment()
   }

   async Task<int> valueAsync(CancellationToken token, ManualResetEvent entered)
   {
      :await cancellableOperation(token, entered);

      CancellationTestCounter.increment();

      ^ 42
   }

   async Task<int> transformAsync(CancellationToken token, ManualResetEvent entered)
   {
      int value := :await valueAsync(token, entered);

      CancellationTestCounter.increment();

      ^ value + 1
   }

   async Task<int> nestedAsync(CancellationToken token, ManualResetEvent entered)
   {
      int value := :await transformAsync(token, entered);

      CancellationTestCounter.increment();

      ^ value + 1
   }

   async Task<int> recoverAsync(CancellationToken token, ManualResetEvent entered)
   {
      int result := 42;
      try {
         :await cancellableOperation(token, entered)
      }
      catch (TaskCanceledException e) {
         result := -1
      };

      ^ result
   }

   async Task withFinallyAsync(CancellationToken token, ManualResetEvent entered)
   {
      try {
         :await cancellableOperation(token, entered)
      }
      finally {
         CancellationTestCounter.increment()
      }
   }

   async Task<int> completeWithoutObservationAsync(Task gate, CancellationToken token)
   {
      :await gate;

      ^ 7
   }

   async Task<int> observeAfterResumeAsync(Task gate, CancellationToken token)
   {
      :await gate;
      token.throwIfCancellationRequested();

      ^ 7
   }

   async Task faultAfterResumeAsync(Task gate, CancellationToken token)
   {
      :await gate;

      CancellationTestException.raise("fault after cancellation")
   }
}

public canceledTaskTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   source.cancel();

   CancellationTestCounter.reset();
   Task canceledTask := Task.run(&incrementCancellationTestCounter, source.Token);
   Assert.ifEqual(CancellationTestCounter.Value, 0);
   Assert.ifTrue(canceledTask.IsCompleted);
   Assert.ifTrue(canceledTask.IsCanceled);
   Assert.ifFalse(canceledTask.IsFaulted);
   Assert.ifFalse(canceledTask.IsCompletedSuccessfully);

   bool canceledRaised := false;
   try {
      canceledTask.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);

   Task<int> canceledRun := class Task<int>.run(
      {
         CancellationTestCounter.increment();

         ^ 42
      }, source.Token);
   Assert.ifTrue(canceledRun.IsCanceled);
   Assert.ifEqual(CancellationTestCounter.Value, 0);

   Task<int> canceledValue := class Task<int>.fromCanceled(source.Token);
   canceledRaised := false;
   try {
      int value := canceledValue.Result
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);

   bool invalidTokenRaised := false;
   try {
      Task.fromCanceled(CancellationTokenSource.new().Token)
   }
   catch (InvalidArgumentException e) {
      invalidTokenRaised := true
   };
   Assert.ifTrue(invalidTokenRaised);

   Console.write(".")
}

public taskCancellationTokenAssociationTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   ManualResetEvent entered := ManualResetEvent.new();
   ManualResetEvent release := ManualResetEvent.new();
   Task canceled := Task.run(
      {
         entered.set();
         release.wait();
         source.Token.throwIfCancellationRequested()
      }, source.Token);

   Assert.ifTrue(entered.wait(5000));
   source.cancel();
   release.set();
   Assert.ifTrue(waitForTaskCompletion(canceled, 5000));

   bool canceledRaised := false;
   try {
      canceled.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(canceled.IsCanceled);
   entered.close();
   release.close();

   CancellationTokenSource taskSource := CancellationTokenSource.new();
   CancellationTokenSource otherSource := CancellationTokenSource.new();
   entered := ManualResetEvent.new();
   release := ManualResetEvent.new();
   Task mismatched := Task.run(
      {
         entered.set();
         release.wait();
         otherSource.Token.throwIfCancellationRequested()
      }, taskSource.Token);

   Assert.ifTrue(entered.wait(5000));
   taskSource.cancel();
   otherSource.cancel();
   release.set();
   Assert.ifTrue(waitForTaskCompletion(mismatched, 5000));

   bool faultRaised := false;
   try {
      mismatched.wait()
   }
   catch (OperationCanceledException e) {
      faultRaised := true;
      Assert.ifTrue(e.Token.equalReference(otherSource.Token))
   };
   Assert.ifTrue(faultRaised);
   Assert.ifTrue(mismatched.IsFaulted);
   Assert.ifFalse(mismatched.IsCanceled);
   entered.close();
   release.close();

   source := CancellationTokenSource.new();
   entered := ManualResetEvent.new();
   release := ManualResetEvent.new();
   Task completed := Task.run(
      {
         entered.set();
         release.wait()
      }, source.Token);

   Assert.ifTrue(entered.wait(5000));
   source.cancel();
   release.set();
   completed.wait();
   Assert.ifTrue(completed.IsCompletedSuccessfully);
   entered.close();
   release.close();

   Console.write(".")
}

public taskCancellationExceptionClassificationTest() : testCase()
{
   Task unrelatedCancellation := Task.run({ OperationCanceledException.raise() });
   bool faultRaised := false;
   try {
      unrelatedCancellation.wait()
   }
   catch (OperationCanceledException e) {
      faultRaised := true;
      Assert.ifTrue(e.Token.equalReference(CancellationToken.None))
   };
   Assert.ifTrue(faultRaised);
   Assert.ifTrue(unrelatedCancellation.IsFaulted);
   Assert.ifFalse(unrelatedCancellation.IsCanceled);

   CancellationTokenSource source := CancellationTokenSource.new();
   Task cancellationNotRequested := Task.run(
      { OperationCanceledException.withToken(source.Token).raise() }, source.Token);

   faultRaised := false;
   try {
      cancellationNotRequested.wait()
   }
   catch (OperationCanceledException e) {
      faultRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(faultRaised);
   Assert.ifTrue(cancellationNotRequested.IsFaulted);
   Assert.ifFalse(cancellationNotRequested.IsCanceled);

   source := CancellationTokenSource.new();
   ManualResetEvent entered := ManualResetEvent.new();
   ManualResetEvent release := ManualResetEvent.new();
   Task regularFailure := Task.run(
      {
         entered.set();
         release.wait();
         CancellationTestException.raise("failure after cancellation")
      }, source.Token);

   Assert.ifTrue(entered.wait(5000));
   source.cancel();
   release.set();
   Assert.ifTrue(waitForTaskCompletion(regularFailure, 5000));

   faultRaised := false;
   try {
      regularFailure.wait()
   }
   catch (CancellationTestException e) {
      faultRaised := true
   };
   Assert.ifTrue(faultRaised);
   Assert.ifTrue(regularFailure.IsFaulted);
   Assert.ifFalse(regularFailure.IsCanceled);
   entered.close();
   release.close();

   Console.write(".")
}

public cooperativeTaskCancellationTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   Task delayed := Task.sleep(1000, source.Token);
   Assert.ifFalse(delayed.IsCompleted);
   source.cancel();
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));

   bool canceledRaised := false;
   try {
      delayed.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };

   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(delayed.IsCanceled);

   Task continuation := Task.completedTask().continueWith(
      (Task completed)
      {
         OperationCanceledException.raise()
      });

   bool faultRaised := false;
   try {
      continuation.wait()
   }
   catch (OperationCanceledException e) {
      faultRaised := true
   };
   Assert.ifTrue(faultRaised);
   Assert.ifTrue(continuation.IsFaulted);
   Assert.ifFalse(continuation.IsCanceled);

   Console.write(".")
}

public nonCancelableTokenTaskTest() : testCase()
{
   Task completedDelay := Task.sleep(5, CancellationToken.None);
   completedDelay.wait();
   Assert.ifTrue(completedDelay.IsCompletedSuccessfully);

   Task completedAction := Task.run({}, CancellationToken.None);
   completedAction.wait();
   Assert.ifTrue(completedAction.IsCompletedSuccessfully);

   Task<int> completedValue := class Task<int>.run({ ^ 42 }, CancellationToken.None);
   Assert.ifEqual(completedValue.Result, 42);

   Console.write(".")
}

public canceledTaskContinuationsTest() : testCase()
{
   int continuationCount := 32;
   Task[] continuations := new Task[](continuationCount);
   CancellationTokenSource source := CancellationTokenSource.new();
   source.cancel();

   CancellationTestCounter.reset();
   for (int i := 0; i < continuationCount; i += 1)
      continuations[i] := Task.fromCanceled(source.Token).continueWith(&recordCanceledContinuation);

   Task.whenAll(continuations).wait();
   Assert.ifEqual(CancellationTestCounter.Value, continuationCount);

   Console.write(".")
}

public whenAllCancellationTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   source.cancel();

   Task[] canceledTasks := new Task[](2);
   canceledTasks[0] := Task.fromCanceled(source.Token);
   canceledTasks[1] := Task.completedTask();

   Task canceledAll := Task.whenAll(canceledTasks);
   bool canceledRaised := false;
   try {
      canceledAll.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(canceledAll.IsCanceled);

   Task[] mixedTasks := new Task[](2);
   mixedTasks[0] := Task.fromCanceled(source.Token);
   mixedTasks[1] := Task.fromException(CancellationTestException.new("failed"));

   Task mixedAll := Task.whenAll(mixedTasks);
   bool aggregateRaised := false;
   try {
      mixedAll.wait()
   }
   catch (AggregateException e) {
      aggregateRaised := true;
      Assert.ifEqual(e.InnerExceptions.Length, 1)
   };
   Assert.ifTrue(aggregateRaised);
   Assert.ifTrue(mixedAll.IsFaulted);

   ManualResetEvent gate := ManualResetEvent.new();
   Task pending := Task.run({ gate.wait() });
   Task[] pendingTasks := new Task[](2);
   pendingTasks[0] := Task.fromCanceled(source.Token);
   pendingTasks[1] := pending;

   Task pendingAll := Task.whenAll(pendingTasks);
   Assert.ifFalse(pendingAll.IsCompleted);
   gate.set();
   Assert.ifTrue(waitForTaskCompletion(pendingAll, 5000));

   canceledRaised := false;
   try {
      pendingAll.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(pendingAll.IsCanceled);
   gate.close();

   int completedCount := 128;
   Task[] completedTasks := new Task[](completedCount);
   for (int i := 0; i < completedCount; i += 1)
      completedTasks[i] := Task.fromCanceled(source.Token);

   Task completedAll := Task.whenAll(completedTasks);
   Assert.ifTrue(waitForTaskCompletion(completedAll, 5000));
   Assert.ifTrue(completedAll.IsCanceled);

   Console.write(".")
}

public asyncCancellationPropagationTest() : testCase()
{
   AsyncCancellationScenarios scenarios := new AsyncCancellationScenarios();

   CancellationTestCounter.reset();
   CancellationTokenSource source := CancellationTokenSource.new();
   ManualResetEvent entered := ManualResetEvent.new();
   Task task := scenarios.waitAsync(source.Token, entered);
   Assert.ifTrue(entered.wait(5000));
   Assert.ifFalse(task.IsCompleted);
   source.cancel();
   Assert.ifTrue(waitForTaskCompletion(task, 5000));

   bool canceledRaised := false;
   try {
      task.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(task.IsCanceled);
   Assert.ifEqual(CancellationTestCounter.Value, 0);
   entered.close();

   source := CancellationTokenSource.new();
   entered := ManualResetEvent.new();
   Task<int> valueTask := scenarios.nestedAsync(source.Token, entered);
   Assert.ifTrue(entered.wait(5000));
   Assert.ifFalse(valueTask.IsCompleted);
   source.cancel();
   Assert.ifTrue(waitForTaskCompletion(valueTask, 5000));

   canceledRaised := false;
   try {
      int value := valueTask.Result
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(valueTask.IsCanceled);
   Assert.ifEqual(CancellationTestCounter.Value, 0);
   entered.close();

   Console.write(".")
}

public asyncCancellationControlFlowTest() : testCase()
{
   AsyncCancellationScenarios scenarios := new AsyncCancellationScenarios();

   CancellationTokenSource source := CancellationTokenSource.new();
   ManualResetEvent entered := ManualResetEvent.new();
   Task<int> recovered := scenarios.recoverAsync(source.Token, entered);
   Assert.ifTrue(entered.wait(5000));
   Assert.ifFalse(recovered.IsCompleted);
   source.cancel();
   Assert.ifTrue(waitForTaskCompletion(recovered, 5000));
   Assert.ifEqual(recovered.Result, -1);
   Assert.ifTrue(recovered.IsCompletedSuccessfully);
   entered.close();

   CancellationTestCounter.reset();
   source := CancellationTokenSource.new();
   entered := ManualResetEvent.new();
   Task finalizing := scenarios.withFinallyAsync(source.Token, entered);
   Assert.ifTrue(entered.wait(5000));
   Assert.ifFalse(finalizing.IsCompleted);
   source.cancel();
   Assert.ifTrue(waitForTaskCompletion(finalizing, 5000));

   bool canceledRaised := false;
   try {
      finalizing.wait()
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(finalizing.IsCanceled);
   Assert.ifEqual(CancellationTestCounter.Value, 1);
   entered.close();

   Console.write(".")
}

public asyncCancellationObservationTest() : testCase()
{
   AsyncCancellationScenarios scenarios := new AsyncCancellationScenarios();

   CancellationTokenSource source := CancellationTokenSource.new();
   ManualResetEvent gate := ManualResetEvent.new();
   Task blocker := Task.run({ gate.wait() });
   Task<int> unobserved := scenarios.completeWithoutObservationAsync(blocker, source.Token);
   Assert.ifFalse(unobserved.IsCompleted);
   source.cancel();
   gate.set();
   Assert.ifTrue(waitForTaskCompletion(unobserved, 5000));
   Assert.ifEqual(unobserved.Result, 7);
   Assert.ifTrue(unobserved.IsCompletedSuccessfully);
   gate.close();

   source := CancellationTokenSource.new();
   gate := ManualResetEvent.new();
   blocker := Task.run({ gate.wait() });
   Task<int> observed := scenarios.observeAfterResumeAsync(blocker, source.Token);
   Assert.ifFalse(observed.IsCompleted);
   source.cancel();
   gate.set();
   Assert.ifTrue(waitForTaskCompletion(observed, 5000));

   bool canceledRaised := false;
   try {
      int value := observed.Result
   }
   catch (TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(observed.IsCanceled);
   gate.close();

   source := CancellationTokenSource.new();
   gate := ManualResetEvent.new();
   blocker := Task.run({ gate.wait() });
   Task faulted := scenarios.faultAfterResumeAsync(blocker, source.Token);
   Assert.ifFalse(faulted.IsCompleted);
   source.cancel();
   gate.set();
   Assert.ifTrue(waitForTaskCompletion(faulted, 5000));

   bool faultRaised := false;
   try {
      faulted.wait()
   }
   catch (CancellationTestException e) {
      faultRaised := true
   };
   Assert.ifTrue(faultRaised);
   Assert.ifTrue(faulted.IsFaulted);
   Assert.ifFalse(faulted.IsCanceled);
   gate.close();

   Console.write(".")
}
