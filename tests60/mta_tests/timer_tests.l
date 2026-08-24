import extensions;
import system'threading;
import ltests;

class CloseWaitState
{
   CancellationTokenRegistration Registration : prop;
   ManualResetEvent Started : prop;
   ManualResetEvent Returned : prop;
   ErrorBox Errors : prop;
}

closeRegistrationWorker(state)
{
   try
   {
      state.Started.set();
      state.Registration.close();
   }
   catch(Exception e)
   {
      state.Errors.record(e)
   };

   state.Returned.set()
}

public cancellationRegistrationCloseWaitTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   ManualResetEvent callbackStarted := ManualResetEvent.new();
   ManualResetEvent callbackRelease := ManualResetEvent.new();
   ManualResetEvent cancellationReturned := ManualResetEvent.new();
   ManualResetEvent firstCloseStarted := ManualResetEvent.new();
   ManualResetEvent firstCloseReturned := ManualResetEvent.new();
   ManualResetEvent secondCloseStarted := ManualResetEvent.new();
   ManualResetEvent secondCloseReturned := ManualResetEvent.new();
   ErrorBox errors := ErrorBox.new();

   CancellationTokenRegistration registration := source.Token.register(
      {
         callbackStarted.set();
         callbackRelease.wait()
      });

   Thread.start(
      {
         try
         {
            source.cancel()
         }
         catch(Exception e)
         {
            errors.record(e)
         };

         cancellationReturned.set()
      });

   Assert.ifTrue(callbackStarted.wait(5000));

   CloseWaitState firstState := new CloseWaitState();
   firstState.Registration := registration;
   firstState.Started := firstCloseStarted;
   firstState.Returned := firstCloseReturned;
   firstState.Errors := errors;
   Thread firstCloser := Thread.assign(&closeRegistrationWorker);
   firstCloser.start(firstState);

   CloseWaitState secondState := new CloseWaitState();
   secondState.Registration := registration;
   secondState.Started := secondCloseStarted;
   secondState.Returned := secondCloseReturned;
   secondState.Errors := errors;
   Thread secondCloser := Thread.assign(&closeRegistrationWorker);
   secondCloser.start(secondState);

   Assert.ifTrue(firstCloseStarted.wait(5000));
   Assert.ifTrue(secondCloseStarted.wait(5000));
   Assert.ifFalse(firstCloseReturned.wait(50));
   Assert.ifFalse(secondCloseReturned.wait(50));

   callbackRelease.set();
   Assert.ifTrue(cancellationReturned.wait(5000));
   Assert.ifTrue(firstCloseReturned.wait(5000));
   Assert.ifTrue(secondCloseReturned.wait(5000));
   errors.rethrowIfAny();

   callbackStarted.close();
   callbackRelease.close();
   cancellationReturned.close();
   firstCloseStarted.close();
   firstCloseReturned.close();
   secondCloseStarted.close();
   secondCloseReturned.close();

   Console.write(".")
}

class CancelAfterRaceState
{
   CancellationTokenSource Source : prop;
   RoundBarrier Barrier : prop;
   int Rounds : prop;
   int Index : prop;
   ErrorBox Errors : prop;
   CountDownEvent Completed : prop;
}

cancelAfterRaceWorker(state)
{
   try
   {
      for (int round := 0; round < state.Rounds; round += 1)
      {
         state.Barrier.arrive(round);
         state.Source.cancelAfter(20 + state.Index)
      }
   }
   catch(Exception e)
   {
      state.Errors.record(e)
   };

   state.Completed.signal()
}

public cancellationTokenCancelAfterRaceTest() : testCase()
{
   int workers := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;
   CancellationTokenSource source := CancellationTokenSource.new();
   RoundBarrier barrier := RoundBarrier.new(workers, rounds);
   ErrorBox errors := ErrorBox.new();
   CountDownEvent completed := CountDownEvent.new(workers);
   SharedCounter callbacks := SharedCounter.new();
   source.Token.register({ callbacks.increment() });

   for (int i := 0; i < workers; i += 1)
   {
      CancelAfterRaceState state := new CancelAfterRaceState();
      state.Source := source;
      state.Barrier := barrier;
      state.Rounds := rounds;
      state.Index := i;
      state.Errors := errors;
      state.Completed := completed;

      Thread worker := Thread.assign(&cancelAfterRaceWorker);
      worker.start(state)
   };

   for (int round := 0; round < rounds; round += 1)
      barrier.releaseAll(round);

   Assert.ifTrue(completed.wait(5000));
   ManualResetEvent taskCompleted := ManualResetEvent.new();
   Task canceled := Task.sleep(5000, source.Token);
   canceled.continueWith((Task task){ taskCompleted.set() });
   Assert.ifTrue(taskCompleted.wait(5000));

   errors.rethrowIfAny();
   Assert.ifTrue(source.IsCancellationRequested);
   Assert.ifTrue(canceled.IsCanceled);
   Assert.ifEqual(callbacks.Value, 1);

   barrier.close();
   completed.close();
   taskCompleted.close();

   Console.write(".")
}

public cancelAfterReplacesQueuedTimerTest() : testCase()
{
   int workerCount := 4;
   CountDownEvent workersStarted := CountDownEvent.new(workerCount);
   ManualResetEvent releaseWorkers := ManualResetEvent.new();
   for (int i := 0; i < workerCount; i += 1)
   {
      ThreadPool.queueAction(
         {
            workersStarted.signal();
            releaseWorkers.wait()
         })
   };

   Assert.ifTrue(workersStarted.wait(5000));

   CancellationTokenSource source := CancellationTokenSource.new();
   source.cancelAfter(0);
   Thread.sleep(50);

   ManualResetEvent rescheduleStarted := ManualResetEvent.new();
   ManualResetEvent rescheduleReturned := ManualResetEvent.new();
   ErrorBox errors := ErrorBox.new();
   Thread.start(
      {
         rescheduleStarted.set();
         try
         {
            source.cancelAfter(5000)
         }
         catch(Exception e)
         {
            errors.record(e)
         };

         rescheduleReturned.set()
      });

   Assert.ifTrue(rescheduleStarted.wait(5000));
   bool returnedBeforeRelease := rescheduleReturned.wait(500);
   releaseWorkers.set();
   Assert.ifTrue(rescheduleReturned.wait(5000));
   Assert.ifTrue(returnedBeforeRelease);

   ManualResetEvent probeCompleted := ManualResetEvent.new();
   Task.sleep(50).continueWith((Task task){ probeCompleted.set() });
   Assert.ifTrue(probeCompleted.wait(5000));
   errors.rethrowIfAny();
   Assert.ifFalse(source.IsCancellationRequested);
   source.cancelAfter(-1);

   workersStarted.close();
   releaseWorkers.close();
   rescheduleStarted.close();
   rescheduleReturned.close();
   probeCompleted.close();

   Console.write(".")
}
