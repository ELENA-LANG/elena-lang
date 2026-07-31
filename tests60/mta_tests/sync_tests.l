import extensions;
import system'threading;
import ltests;

// Layer C, synchronisation
//
// These come first because the harness is built on CountDownEvent and
// ManualResetEvent. If those are broken, every later failure is the harness lying

// C6, CountDownEvent

class CountDownState
{
   Array    Counters : prop;
   Array    Latches  : prop;
   int      Rounds   : prop;
   ErrorBox Errors   : prop;
}

countDownWorker(state)
{
   int rounds := state.Rounds;
   auto counters := state.Counters;
   auto latches := state.Latches;

   for (int k := 0; k < rounds; k += 1)
   {
      auto counter := counters[k];
      auto latch := latches[k];

      try
      {
         counter.increment()
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      latch.done()
   }
}

// Regression, ManualResetEvent
//
// signal decrements under lock and only opens the event at zero, and the _counter > 0
// guard keeps extra signals harmless
public countDownEventTest() : testCase()
{
   int t := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;

   auto counters := Array.allocate(rounds);
   auto latches := Array.allocate(rounds);

   for (int k := 0; k < rounds; k += 1)
   {
      counters[k] := SharedCounter.new();
      latches[k] := Latch.new(t)
   };

   auto state := new CountDownState();
   state.Counters := counters;
   state.Latches := latches;
   state.Rounds := rounds;
   state.Errors := ErrorBox.new();

   // created once and repeating inside, slots are never recycled
   for (int i := 0; i < t; i += 1)
   {
      auto worker := Thread.assign(&countDownWorker);

      worker.start(state)
   };

   // the latch must not open before the last signal, and an early open shows up as a
   // short count. Value > 0 would pass either way
   for (int k := 0; k < rounds; k += 1)
   {
      auto latch := latches[k];
      auto counter := counters[k];

      latch.await();

      Assert.ifEqual(counter.Value, t)
   };

   state.Errors.rethrowIfAny();

   Console.write(".");

   // extra signals past zero must leave the event open
   auto spare := CountDownEvent.new(1);
   spare.signal();
   spare.signal();
   spare.signal();
   spare.wait();
   spare.close();

   for (int k := 0; k < rounds; k += 1)
   {
      auto latch := latches[k];

      latch.close()
   };

   Console.write(".")
}

// C1, ManualResetEvent

class BroadcastState
{
   RoundBarrier Barrier   : prop;
   Array        Woken     : prop;
   Array        Completed : prop;
   int          Rounds    : prop;
   ErrorBox     Errors    : prop;
}

broadcastWorker(state)
{
   int rounds := state.Rounds;
   auto barrier := state.Barrier;
   auto woken := state.Woken;
   auto completed := state.Completed;

   for (int k := 0; k < rounds; k += 1)
   {
      auto counter := woken[k];
      auto latch := completed[k];

      try
      {
         barrier.arrive(k);

         counter.increment()
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      latch.done()
   }
}

// Regression, CountDownEvent
//
// One set must release every waiter, and an already signalled event must not block.
// With pthread_cond_signal instead of broadcast only one waiter wakes and the rest
// hang, so this one fails as the runner timeout rather than as an assertion
public manualResetEventTest() : testCase()
{
   // already signalled, wait must go straight through
   auto ready := ManualResetEvent.new(true);
   ready.wait();
   ready.close();

   Console.write(".");

   int t := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;

   auto woken := Array.allocate(rounds);
   auto completed := Array.allocate(rounds);

   for (int k := 0; k < rounds; k += 1)
   {
      woken[k] := SharedCounter.new();
      completed[k] := Latch.new(t)
   };

   auto state := new BroadcastState();
   state.Barrier := RoundBarrier.new(t, rounds);
   state.Woken := woken;
   state.Completed := completed;
   state.Rounds := rounds;
   state.Errors := ErrorBox.new();

   for (int i := 0; i < t; i += 1)
   {
      auto worker := Thread.assign(&broadcastWorker);

      worker.start(state)
   };

   for (int k := 0; k < rounds; k += 1)
   {
      auto latch := completed[k];
      auto counter := woken[k];

      // everyone is inside wait before the single set
      state.Barrier.releaseAll(k);

      latch.await();

      Assert.ifEqual(counter.Value, t)
   };

   state.Errors.rethrowIfAny();

   state.Barrier.close();

   for (int k := 0; k < rounds; k += 1)
   {
      auto latch := completed[k];

      latch.close()
   };

   Console.write(".")
}

// C2, AutoResetEvent
//
// The only user of the auto reset mode, nothing else in the suite creates one

class PingPongState
{
   AutoResetEvent Req    : prop;
   AutoResetEvent Rsp    : prop;
   SharedCounter  Turns  : prop;
   int            Rounds : prop;
   Latch          Done   : prop;
   ErrorBox       Errors : prop;
}

pingWorker(state)
{
   int rounds := state.Rounds;
   auto req := state.Req;
   auto rsp := state.Rsp;
   auto turns := state.Turns;

   try
   {
      for (int k := 0; k < rounds; k += 1)
      {
         req.wait();

         turns.increment();

         rsp.set()
      }
   }
   catch(Exception e)
   {
      state.Errors.record(e)
   };

   state.Done.done()
}

class TokenState
{
   AutoResetEvent Gate   : prop;
   AutoResetEvent Ack    : prop;
   SharedCounter  Woken  : prop;
   Latch          Done   : prop;
   ErrorBox       Errors : prop;
}

tokenWorker(state)
{
   try
   {
      state.Gate.wait();

      state.Woken.increment();

      state.Ack.set()
   }
   catch(Exception e)
   {
      state.Errors.record(e)
   };

   state.Done.done()
}

// Regression, the CreateEventA argument order and the pthread auto reset mode
//
// The ping pong is fully serialised : rsp.wait returning means the worker finished that
// turn, so the count is exact on every round, not just at the end. An event created
// signalled by mistake lets the coordinator run ahead and the first assertion sees 0.
// A lost wakeup, the old pthread branch on a spurious return or a set landing between
// waits, parks the worker forever and this fails as the runner timeout.
//
// The token phase releases n waiters with n tokens, one ack consumed per token. A set
// that releases more than one waiter shows up as an early count ahead of k + 1
public autoResetEventTest() : testCase()
{
   int rounds := MtaLoad.Repetitions;

   auto state := new PingPongState();
   state.Req := AutoResetEvent.new();
   state.Rsp := AutoResetEvent.new();
   state.Turns := SharedCounter.new();
   state.Rounds := rounds;
   state.Done := Latch.new(1);
   state.Errors := ErrorBox.new();

   auto worker := Thread.assign(&pingWorker);
   worker.start(state);

   for (int k := 0; k < rounds; k += 1)
   {
      state.Req.set();
      state.Rsp.wait();

      Assert.ifEqual(state.Turns.Value, k + 1)
   };

   state.Done.await();
   state.Errors.rethrowIfAny();

   state.Done.close();
   state.Req.close();
   state.Rsp.close();

   Console.write(".");

   int t := 4;

   auto tokens := new TokenState();
   tokens.Gate := AutoResetEvent.new();
   tokens.Ack := AutoResetEvent.new();
   tokens.Woken := SharedCounter.new();
   tokens.Done := Latch.new(t);
   tokens.Errors := ErrorBox.new();

   for (int i := 0; i < t; i += 1)
   {
      auto waiter := Thread.assign(&tokenWorker);

      waiter.start(tokens)
   };

   for (int k := 0; k < t; k += 1)
   {
      tokens.Gate.set();
      tokens.Ack.wait();

      Assert.ifEqual(tokens.Woken.Value, k + 1)
   };

   tokens.Done.await();
   tokens.Errors.rethrowIfAny();

   tokens.Done.close();
   tokens.Gate.close();
   tokens.Ack.close();

   Console.write(".")
}
