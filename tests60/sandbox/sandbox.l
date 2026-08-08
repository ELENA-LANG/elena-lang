import system'collections;
import extensions;
import system'threading;
import extensions'threading;
import system'runtime;

// RoundBarrier
//
// Releases every worker at once, which is the only way to force overlap here. Gates are
// pre-allocated one pair per round rather than reset, since resetting an event while a
// worker may still be inside wait is the kind of race a harness must not have
public sealed class RoundBarrier
{
   Array _arrivals;
   Array _gates;
   int   _rounds;

   constructor new(int count, int rounds)
   {
      _rounds := rounds;
      _arrivals := Array.allocate(rounds);
      _gates := Array.allocate(rounds);

      for (int i := 0; i < rounds; i += 1)
      {
         _arrivals[i] := CountDownEvent.new(count);
         _gates[i] := ManualResetEvent.new()
      }
   }

   // worker side
   arrive(int round)
   {
      auto arrival := _arrivals[round];
      auto gate := _gates[round];

      arrival.signal();
      gate.wait()
   }

   // coordinator side
   releaseAll(int round)
   {
      auto arrival := _arrivals[round];
      auto gate := _gates[round];

      arrival.wait();
      gate.set()
   }

   close()
   {
      for (int i := 0; i < _rounds; i += 1)
      {
         auto arrival := _arrivals[i];
         auto gate := _gates[i];

         arrival.close();
         gate.close()
      }
   }
}

class TlsWorkerState
{
   RoundBarrier Barrier   : prop;
   Array        Matches   : prop;
   Array        Completed : prop;
   int          Rounds    : prop;
   ErrorBox     Errors    : prop;
}

class TlsWorkerArg
{
   int            Index : prop;
   TlsWorkerState State : prop;
}

public singleton MtaLoad
{
   // more workers than cores, so the scheduler is forced to preempt
   int WorkerCount
   {
      get()
      {
         int n := 8;

/*         if (n < 4)
            { n := 4 };*/

         ^ n
      }
   }

   // one run samples one interleaving, so every case repeats its body
   int Repetitions
      = 20;
}

// SharedCounter
//
// The increment is a deliberate read-modify-write, so a lock that does not exclude
// loses updates and the total comes out short
public sealed class SharedCounter
{
   int _n;

   constructor new()
   {
      _n := 0
   }

   increment()
   {
      lock(self)
      {
         _n := _n + 1
      }
   }

   int Value
   {
      get()
      {
         // the lock template declares an intermediate named v, so a
         // local of that name here would be shadowed
         int current := 0;

         lock(self)
         {
            current := _n
         };

         ^ current
      }
   }
}

// Latch, stands in for Thread.join
public sealed class Latch
{
   CountDownEvent _done;

   constructor new(int count)
   {
      _done := CountDownEvent.new(count)
   }

   done()
   {
      _done.signal()
   }

   await()
   {
      _done.wait()
   }

   close()
   {
      _done.close()
   }
}

class ChurnState
{
   int      Allocations : prop;
   Latch    Done        : prop;
   ErrorBox Errors      : prop;
}

// ErrorBox
//
// An assertion failing inside a worker aborts the process without saying which case
// failed. Workers record here and the coordinator re-raises after the latch
public sealed class ErrorBox
{
   bool   _failed;
   string _message;

   constructor new()
   {
      _failed := false;
      _message := ""
   }

   record(Exception e)
   {
      lock(self)
      {
         if:not (_failed)
         {
            _failed := true;

            // toPrintable carries the call stack
            _message := e.toPrintable()
         }
      }
   }

   rethrowIfAny()
   {
      if (_failed)
         { AssertException.raise(_message) }
   }
}

churnWorker(state)
{
   Console.printLineConcurrent("churnWorker");

   int n := state.Allocations;

   try
   {
      for (int i := 0; i < n; i += 1)
      {
         auto tmp := new ArrayList();
         tmp.append(i)
      }
   }
   catch(Exception e)
   {
      state.Errors.record(e)
   };

   state.Done.done()

   // and then it returns, which is the point, leaving a null slot behind while the
   // other waves are still running
}

// The only __threadvar in the tree other than Thread._currentThread
public sealed class TlsProbe
{
   __threadvar TlsProbe _current;

   int _id;

   constructor new(int id)
   {
      _id := id
   }

   int Id = _id;

   static store(TlsProbe probe)
   {
      _current := probe
   }

   static TlsProbe Current
   {
      get()
         = _current;
   }
}

tlsWorker(arg)
{
   int index := arg.Index;
   auto state := arg.State;

   int rounds := state.Rounds;
   auto barrier := state.Barrier;
   auto matches := state.Matches;
   auto completed := state.Completed;

   for (int k := 0; k < rounds; k += 1)
   {
      auto counter := matches[k];
      auto latch := completed[k];

      try
      {
         int expected := index * 1000 + k;

         TlsProbe.store(TlsProbe.new(expected));

         // everyone has written before anyone reads back, which turns a shared slot
         // from probably wrong into certainly wrong
         barrier.arrive(k);

         auto seen := TlsProbe.Current;

         if (seen.Id == expected)
            { counter.increment() }
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      latch.done()
   }
}

public mtaThreadLocalStorageTest()
{
   int t := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;

   auto matches := Array.allocate(rounds);
   auto completed := Array.allocate(rounds);

   for (int k := 0; k < rounds; k += 1)
   {
      matches[k] := SharedCounter.new();
      completed[k] := Latch.new(t)
   };

   auto state := new TlsWorkerState();
   state.Barrier := RoundBarrier.new(t, rounds);
   state.Matches := matches;
   state.Completed := completed;
   state.Rounds := rounds;
   state.Errors := ErrorBox.new();

   for (int i := 0; i < t; i += 1)
   {
      auto arg := new TlsWorkerArg();
      arg.Index := i;
      arg.State := state;

      auto worker := Thread.assign(&tlsWorker);

      worker.start(arg)
   };

   for (int k := 0; k < rounds; k += 1)
   {
      auto latch := completed[k];
      auto counter := matches[k];

      state.Barrier.releaseAll(k);

      latch.await();

      // one shared slot would leave at most one match per round
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
 
public Program()
{
   mtaThreadLocalStorageTest();

   Console.write(".")
}