import extensions;
import system'threading;
import ltests;

class CancellationCallbackException : Exception
{
   constructor new(string message)
      <= super new(message);
}

class CallbackOrder
{
   int  _next;
   bool _valid;

   constructor new(int count)
   {
      _next := count;
      _valid := true
   }

   record(int position)
   {
      lock(self)
      {
         if (_next != position)
            _valid := false;

         _next := _next - 1
      }
   }

   bool IsValid
   {
      get()
      {
         bool valid := false;
         lock(self)
         {
            valid := _valid && _next == 0
         };

         ^ valid
      }
   }
}

class RegistrationHolder
{
   CancellationTokenRegistration Registration : prop;
   SharedCounter Counter : prop;

   invoke()
   {
      Registration.close();
      Counter.increment()
   }
}

public cancellationTokenTest() : testCase()
{
   CancellationToken none := CancellationToken.None;
   Assert.ifFalse(none.CanBeCanceled);
   Assert.ifFalse(none.IsCancellationRequested);

   OperationCanceledException defaultCancellation := OperationCanceledException.new();
   Assert.ifTrue(defaultCancellation.Token.equalReference(none));

   CancellationTokenSource source := CancellationTokenSource.new();
   CancellationToken token := source.Token;
   Assert.ifTrue(token.CanBeCanceled);
   Assert.ifFalse(token.IsCancellationRequested);

   source.cancel();
   source.cancel();
   Assert.ifTrue(source.IsCancellationRequested);
   Assert.ifTrue(token.IsCancellationRequested);

   bool canceled := false;
   try
   {
      token.throwIfCancellationRequested()
   }
   catch(OperationCanceledException e)
   {
      canceled := true;
      Assert.ifTrue(e.Token.equalReference(token))
   };

   Assert.ifTrue(canceled);

   Console.write(".")
}

public cancellationRegistrationTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   SharedCounter counter := SharedCounter.new();
   CallbackOrder order := CallbackOrder.new(3);

   source.Token.register({ order.record(1); counter.increment() });
   source.Token.register({ order.record(2); CancellationCallbackException.raise("first callback") });
   source.Token.register({ order.record(3); CancellationCallbackException.raise("second callback") });

   bool aggregateRaised := false;
   try
   {
      source.cancel()
   }
   catch(AggregateException e)
   {
      aggregateRaised := true;
      Assert.ifEqual(e.InnerExceptions.Length, 2)
   };

   Assert.ifTrue(aggregateRaised);
   Assert.ifTrue(order.IsValid);
   Assert.ifEqual(counter.Value, 1);

   source.cancel();
   Assert.ifEqual(counter.Value, 1);

   bool lateRaised := false;
   try
   {
      source.Token.register({ CancellationCallbackException.raise("late callback") })
   }
   catch(CancellationCallbackException e)
   {
      lateRaised := true
   };
   Assert.ifTrue(lateRaised);

   CancellationTokenRegistration late := source.Token.register({ counter.increment() });
   Assert.ifEqual(counter.Value, 2);
   late.close();

   source := CancellationTokenSource.new();
   CancellationTokenRegistration removed := source.Token.register({ counter.increment() });
   removed.close();
   removed.close();
   source.cancel();
   Assert.ifEqual(counter.Value, 2);

   source := CancellationTokenSource.new();
   bool scopeRaised := false;
   try
   {
      using(CancellationTokenRegistration registration := source.Token.register({ counter.increment() }))
      {
         CancellationCallbackException.raise("registration scope")
      }
   }
   catch(CancellationCallbackException e)
   {
      scopeRaised := true
   };

   Assert.ifTrue(scopeRaised);
   source.cancel();
   Assert.ifEqual(counter.Value, 2);

   Console.write(".")
}

public cancellationRegistrationReentrancyTest() : testCase()
{
   CancellationTokenSource source := CancellationTokenSource.new();
   SharedCounter counter := SharedCounter.new();
   CancellationTokenRegistration first := source.Token.register({ counter.increment() });

   source.Token.register(
      {
         first.close();
         source.cancel()
      });

   source.cancel();
   Assert.ifEqual(counter.Value, 0);

   source := CancellationTokenSource.new();
   RegistrationHolder holder := new RegistrationHolder();
   holder.Counter := counter;
   holder.Registration := source.Token.register({ holder.invoke() });
   source.cancel();
   Assert.ifEqual(counter.Value, 1);

   Console.write(".")
}

class RegisterRaceState
{
   Array Sources : prop;
   Array Counters : prop;
   Array Completed : prop;
   RoundBarrier Barrier : prop;
   int Rounds : prop;
   ErrorBox Errors : prop;
}

registerRaceWorker(state)
{
   auto sources := state.Sources;
   auto counters := state.Counters;
   auto completedItems := state.Completed;

   for (int round := 0; round < state.Rounds; round += 1)
   {
      auto source := sources[round];
      auto counter := counters[round];
      auto completed := completedItems[round];

      try
      {
         state.Barrier.arrive(round);
         source.Token.register({ counter.increment() })
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      completed.done()
   }
}

public cancellationRegisterRaceTest() : testCase()
{
   int workers := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;
   auto sources := Array.allocate(rounds);
   auto counters := Array.allocate(rounds);
   auto completed := Array.allocate(rounds);

   for (int round := 0; round < rounds; round += 1)
   {
      sources[round] := CancellationTokenSource.new();
      counters[round] := SharedCounter.new();
      completed[round] := Latch.new(workers)
   };

   RegisterRaceState state := new RegisterRaceState();
   state.Sources := sources;
   state.Counters := counters;
   state.Completed := completed;
   state.Barrier := RoundBarrier.new(workers, rounds);
   state.Rounds := rounds;
   state.Errors := ErrorBox.new();

   for (int i := 0; i < workers; i += 1)
   {
      Thread worker := Thread.assign(&registerRaceWorker);
      worker.start(state)
   };

   for (int round := 0; round < rounds; round += 1)
   {
      auto source := sources[round];
      auto counter := counters[round];
      auto done := completed[round];

      state.Barrier.releaseAll(round);
      source.cancel();
      done.await();

      Assert.ifEqual(counter.Value, workers)
   };

   state.Errors.rethrowIfAny();
   state.Barrier.close();
   for (int round := 0; round < rounds; round += 1)
   {
      auto done := completed[round];
      done.close()
   };

   Console.write(".")
}

class CancelRaceState
{
   Array Sources : prop;
   Array Completed : prop;
   RoundBarrier Barrier : prop;
   int Rounds : prop;
   ErrorBox Errors : prop;
}

cancelRaceWorker(state)
{
   auto sources := state.Sources;
   auto completedItems := state.Completed;

   for (int round := 0; round < state.Rounds; round += 1)
   {
      auto source := sources[round];
      auto completed := completedItems[round];

      try
      {
         state.Barrier.arrive(round);
         source.cancel()
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      completed.done()
   }
}

public cancellationConcurrentCancelTest() : testCase()
{
   int workers := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;
   auto sources := Array.allocate(rounds);
   auto counters := Array.allocate(rounds);
   auto completed := Array.allocate(rounds);

   for (int round := 0; round < rounds; round += 1)
   {
      auto source := CancellationTokenSource.new();
      auto counter := SharedCounter.new();
      for (int i := 0; i < workers; i += 1)
         source.Token.register({ counter.increment() });

      sources[round] := source;
      counters[round] := counter;
      completed[round] := Latch.new(workers)
   };

   CancelRaceState state := new CancelRaceState();
   state.Sources := sources;
   state.Completed := completed;
   state.Barrier := RoundBarrier.new(workers, rounds);
   state.Rounds := rounds;
   state.Errors := ErrorBox.new();

   for (int i := 0; i < workers; i += 1)
   {
      Thread worker := Thread.assign(&cancelRaceWorker);
      worker.start(state)
   };

   for (int round := 0; round < rounds; round += 1)
   {
      auto done := completed[round];
      auto counter := counters[round];

      state.Barrier.releaseAll(round);
      done.await();

      Assert.ifEqual(counter.Value, workers)
   };

   state.Errors.rethrowIfAny();
   state.Barrier.close();
   for (int round := 0; round < rounds; round += 1)
   {
      auto done := completed[round];
      done.close()
   };

   Console.write(".")
}

class OnceCallback : interface<Func>
{
   int  _calls;
   bool _duplicated;

   constructor new()
   {
      _calls := 0;
      _duplicated := false
   }

   invoke()
   {
      lock(self)
      {
         _calls := _calls + 1;
         if (_calls > 1)
            _duplicated := true
      }
   }

   function()
   {
      invoke()
   }

   int Calls
   {
      get()
      {
         int calls := 0;
         lock(self)
         {
            calls := _calls
         };

         ^ calls
      }
   }

   bool Duplicated
   {
      get()
      {
         bool duplicated := false;
         lock(self)
         {
            duplicated := _duplicated
         };

         ^ duplicated
      }
   }
}

class CloseRaceState
{
   Array Registrations : prop;
   Array Completed : prop;
   RoundBarrier Barrier : prop;
   int Rounds : prop;
   ErrorBox Errors : prop;
}

class CloseRaceWorkerArg
{
   int Index : prop;
   CloseRaceState State : prop;
}

closeRaceWorker(arg)
{
   int index := arg.Index;
   auto state := arg.State;
   auto registrationRows := state.Registrations;
   auto completedItems := state.Completed;

   for (int round := 0; round < state.Rounds; round += 1)
   {
      auto registrations := registrationRows[round];
      auto registration := registrations[index];
      auto completed := completedItems[round];

      try
      {
         state.Barrier.arrive(round);
         registration.close()
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      completed.done()
   }
}

public cancellationCloseRaceTest() : testCase()
{
   int workers := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;
   auto sources := Array.allocate(rounds);
   auto registrationRows := Array.allocate(rounds);
   auto callbackRows := Array.allocate(rounds);
   auto completed := Array.allocate(rounds);

   for (int round := 0; round < rounds; round += 1)
   {
      auto source := CancellationTokenSource.new();
      auto registrations := Array.allocate(workers);
      auto callbacks := Array.allocate(workers);

      for (int i := 0; i < workers; i += 1)
      {
         auto callback := OnceCallback.new();
         callbacks[i] := callback;
         registrations[i] := source.Token.register(callback)
      };

      sources[round] := source;
      registrationRows[round] := registrations;
      callbackRows[round] := callbacks;
      completed[round] := Latch.new(workers)
   };

   CloseRaceState state := new CloseRaceState();
   state.Registrations := registrationRows;
   state.Completed := completed;
   state.Barrier := RoundBarrier.new(workers, rounds);
   state.Rounds := rounds;
   state.Errors := ErrorBox.new();

   for (int i := 0; i < workers; i += 1)
   {
      CloseRaceWorkerArg arg := new CloseRaceWorkerArg();
      arg.Index := i;
      arg.State := state;

      Thread worker := Thread.assign(&closeRaceWorker);
      worker.start(arg)
   };

   for (int round := 0; round < rounds; round += 1)
   {
      auto source := sources[round];
      auto done := completed[round];
      auto callbacks := callbackRows[round];

      state.Barrier.releaseAll(round);
      try
      {
         source.cancel()
      }
      catch(AggregateException e)
      {
         Exception[] failures := e.InnerExceptions;
         failures[0].raise()
      };
      done.await();

      for (int i := 0; i < workers; i += 1)
      {
         auto callback := callbacks[i];
         Assert.ifTrue(callback.Calls <= 1);
         Assert.ifFalse(callback.Duplicated)
      }
   };

   state.Errors.rethrowIfAny();
   state.Barrier.close();
   for (int round := 0; round < rounds; round += 1)
   {
      auto done := completed[round];
      done.close()
   };

   Console.write(".")
}
