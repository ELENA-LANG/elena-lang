import extensions;
import system'threading;
import system'runtime;

// MTA test harness
//
// Four runtime limitations shape everything here
//  * Thread.join is unusable on Linux, handlers.l:180 passes one argument to
//    pthread_join, so Latch stands in for it
//  * Thread.sleep is a no-op below one second, handlers.l:203, nothing may use it to
//    order events
//  * no wait with a deadline exists, events.l:114-117, so the only watchdog is the
//    external timeout in the runtests script
//  * thread table slots are never recycled, elenartmachine.cpp:353-370, so workers are
//    created once and repeat inside instead of one set per round

public singleton MtaLoad
{
   // more workers than cores, so the scheduler is forced to preempt
   int WorkerCount
   {
      get()
      {
         int n := 8;

         if (n < 4)
            { n := 4 };

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
         // the lock template declares an intermediate named v, operations.l:207, so a
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

            // toPrintable carries the call stack, exceptions.l:33-38
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
