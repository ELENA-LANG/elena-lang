import extensions;
import system'threading;
import system'runtime;
import ltests;

class TimerAsyncScenarios
{
   async Task<int> delayedValueAsync(int value, CancellationToken token)
   {
      :await Task.sleep(20, token);

      ^ value
   }

   async Task<int> nestedDelayAsync(CancellationToken token)
   {
      int first := :await delayedValueAsync(20, token);
      int second := :await delayedValueAsync(22, token);

      ^ first + second
   }
}

public taskSleepTimerTest() : testCase()
{
   long started := Environment.TickCount64;
   Task delayed := Task.sleep(30);
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifTrue(delayed.IsCompletedSuccessfully);
   Assert.ifTrue(Environment.TickCount64 - started >= 30l);

   Task immediate := Task.sleep(0);
   Assert.ifTrue(immediate.IsCompletedSuccessfully);

   Task infinite := Task.sleep(-1);
   Assert.ifFalse(infinite.IsCompleted);

   bool invalidDelayRaised := false;
   try {
      Task.sleep(-2)
   }
   catch(InvalidArgumentException e) {
      invalidDelayRaised := true
   };
   Assert.ifTrue(invalidDelayRaised);

   Console.write(".")
}

public asyncTaskSleepTest() : testCase()
{
   TimerAsyncScenarios scenarios := new TimerAsyncScenarios();
   CancellationTokenSource source := CancellationTokenSource.new();
   Task<int> result := scenarios.nestedDelayAsync(source.Token);
   Assert.ifTrue(waitForTaskCompletion(result, 5000));
   Assert.ifEqual(result.Result, 42);

   source := CancellationTokenSource.new();
   result := scenarios.nestedDelayAsync(source.Token);
   source.cancelAfter(20);
   Assert.ifTrue(waitForTaskCompletion(result, 5000));

   bool canceledRaised := false;
   try {
      int value := result.Result
   }
   catch(TaskCanceledException e) {
      canceledRaised := true;
      Assert.ifTrue(e.Token.equalReference(source.Token))
   };
   Assert.ifTrue(canceledRaised);
   Assert.ifTrue(result.IsCanceled);

   Console.write(".")
}

public cancellationTokenCancelAfterTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   Task delayed := Task.sleep(-1, source.Token);
   Assert.ifFalse(delayed.IsCompleted);
   source.cancel();
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifTrue(delayed.IsCanceled);

   source := CancellationTokenSource.new();
   source.cancelAfter(30);
   delayed := Task.sleep(5000, source.Token);
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifTrue(source.IsCancellationRequested);
   Assert.ifTrue(delayed.IsCanceled);

   source := CancellationTokenSource.new();
   source.cancelAfter(20);
   source.cancelAfter(-1);
   delayed := Task.sleep(60);
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifFalse(source.IsCancellationRequested);
   source.cancel();

   source := CancellationTokenSource.new();
   source.cancelAfter(20);
   source.cancelAfter(100);
   delayed := Task.sleep(50);
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifFalse(source.IsCancellationRequested);
   delayed := Task.sleep(5000, source.Token);
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifTrue(delayed.IsCanceled);

   source := CancellationTokenSource.new();
   source.cancelAfter(0);
   delayed := Task.sleep(5000, source.Token);
   Assert.ifTrue(waitForTaskCompletion(delayed, 5000));
   Assert.ifTrue(delayed.IsCanceled);

   bool invalidTimeoutRaised := false;
   try {
      CancellationTokenSource.new().cancelAfter(-2)
   }
   catch(InvalidArgumentException e) {
      invalidTimeoutRaised := true
   };
   Assert.ifTrue(invalidTimeoutRaised);

   Console.write(".")
}

public sharedTimerSchedulerStressTest() : testCase()
{
   int count := 256;
   Task[] delays := new Task[](count);
   for (int i := 0; i < count; i += 1)
      delays[i] := Task.sleep(100);

   Task all := Task.whenAll(delays);
   Assert.ifTrue(waitForTaskCompletion(all, 5000));
   Assert.ifTrue(all.IsCompletedSuccessfully);
   Assert.ifTrue(ThreadPool.WorkerCount <= 4);

   Console.write(".")
}
