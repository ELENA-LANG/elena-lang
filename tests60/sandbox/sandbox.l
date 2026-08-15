//import system'collections;
//import extensions;
//import system'threading;
//import extensions'threading;
//import extensions'runtime;
//import system'runtime;
//
//// RoundBarrier
////
//// Releases every worker at once, which is the only way to force overlap here. Gates are
//// pre-allocated one pair per round rather than reset, since resetting an event while a
//// worker may still be inside wait is the kind of race a harness must not have
//public sealed class RoundBarrier
//{
//   Array _arrivals;
//   Array _gates;
//   int   _rounds;
//
//   constructor new(int count, int rounds)
//   {
//      _rounds := rounds;
//      _arrivals := Array.allocate(rounds);
//      _gates := Array.allocate(rounds);
//
//      for (int i := 0; i < rounds; i += 1)
//      {
//         _arrivals[i] := CountDownEvent.new(count);
//         _gates[i] := ManualResetEvent.new()
//      }
//   }
//
//   // worker side
//   arrive(int round)
//   {
//      auto arrival := _arrivals[round];
//      auto gate := _gates[round];
//
//      arrival.signal();
//      gate.wait()
//   }
//
//   // coordinator side
//   releaseAll(int round)
//   {
//      auto arrival := _arrivals[round];
//      auto gate := _gates[round];
//
//      arrival.wait();
//      gate.set()
//   }
//
//   close()
//   {
//      for (int i := 0; i < _rounds; i += 1)
//      {
//         auto arrival := _arrivals[i];
//         auto gate := _gates[i];
//
//         arrival.close();
//         gate.close()
//      }
//   }
//}
//
//class TlsWorkerState
//{
//   RoundBarrier Barrier   : prop;
//   Array        Matches   : prop;
//   Array        Completed : prop;
//   int          Rounds    : prop;
//   ErrorBox     Errors    : prop;
//}
//
//class TlsWorkerArg
//{
//   int            Index : prop;
//   TlsWorkerState State : prop;
//}
//
//public singleton MtaLoad
//{
//   // more workers than cores, so the scheduler is forced to preempt
//   int WorkerCount
//   {
//      get()
//      {
//         int n := 8;
//
///*         if (n < 4)
//            { n := 4 };*/
//
//         ^ n
//      }
//   }
//
//   // one run samples one interleaving, so every case repeats its body
//   int Repetitions
//      = 20;
//}
//
//// SharedCounter
////
//// The increment is a deliberate read-modify-write, so a lock that does not exclude
//// loses updates and the total comes out short
//public sealed class SharedCounter
//{
//   int _n;
//
//   constructor new()
//   {
//      _n := 0
//   }
//
//   increment()
//   {
//      lock(self)
//      {
//         _n := _n + 1
//      }
//   }
//
//   int Value
//   {
//      get()
//      {
//         // the lock template declares an intermediate named v, so a
//         // local of that name here would be shadowed
//         int current := 0;
//
//         lock(self)
//         {
//            current := _n
//         };
//
//         ^ current
//      }
//   }
//}
//
//// Latch, stands in for Thread.join
//public sealed class Latch
//{
//   CountDownEvent _done;
//
//   constructor new(int count)
//   {
//      _done := CountDownEvent.new(count)
//   }
//
//   done()
//   {
//      _done.signal()
//   }
//
//   await()
//   {
//      _done.wait()
//   }
//
//   close()
//   {
//      _done.close()
//   }
//}
//
//class ChurnState
//{
//   int      Allocations : prop;
//   Latch    Done        : prop;
//   ErrorBox Errors      : prop;
//}
//
//// ErrorBox
////
//// An assertion failing inside a worker aborts the process without saying which case
//// failed. Workers record here and the coordinator re-raises after the latch
//public sealed class ErrorBox
//{
//   bool   _failed;
//   string _message;
//
//   constructor new()
//   {
//      _failed := false;
//      _message := ""
//   }
//
//   record(Exception e)
//   {
//      lock(self)
//      {
//         if:not (_failed)
//         {
//            _failed := true;
//
//            // toPrintable carries the call stack
//            _message := e.toPrintable()
//         }
//      }
//   }
//
//   rethrowIfAny()
//   {
//      if (_failed)
//         { AssertException.raise(_message) }
//   }
//}
//
//churnWorker(state)
//{
//   Console.printLineConcurrent("churnWorker");
//
//   int n := state.Allocations;
//
//   try
//   {
//      for (int i := 0; i < n; i += 1)
//      {
//         auto tmp := new ArrayList();
//         tmp.append(i)
//      }
//   }
//   catch(Exception e)
//   {
//      state.Errors.record(e)
//   };
//
//   state.Done.done()
//
//   // and then it returns, which is the point, leaving a null slot behind while the
//   // other waves are still running
//}
// 
//churnRounds(int waves, int workersPerWave, int whenCollect)
//{
//   // has to come through every wave unchanged
//   auto survivors := new ArrayList();
//   for (int i := 0; i < 200; i += 1)
//      { survivors.append(i * 13) };
//
//   auto errors := ErrorBox.new();
//
//   for (int w := 0; w < waves; w += 1)
//   {
//      auto state := new ChurnState();
//      state.Allocations := 5000;
//      state.Done := Latch.new(workersPerWave);
//      state.Errors := errors;
//
//      for (int i := 0; i < workersPerWave; i += 1)
//      {
//         auto worker := Thread.assign(&churnWorker);
//
//         worker.start(state)
//      };
//
//      // collect while the wave finishes, so one lands in the window where some workers
//      // have exited and others have not
//      if (whenCollect == 1)
//         { GCManager.collectFull() }
//      else
//         { GCManager.collectMinor() };
//
//      state.Done.await();
//      state.Done.close();
//
//      if (whenCollect == 2)
//         { GCManager.collectFull() }
//   };
//
//   errors.rethrowIfAny();
//
//   // element by element, a partial relocation leaves the length right
//   Assert.ifEqual(survivors.Length, 200);
//   for (int i := 0; i < 200; i += 1)
//      { Assert.ifEqual(survivors[i], i * 13) }
//}
//
//public Program()
//{
////   churnRounds(50, 15, 1);
//  churnRounds(50, 9, 1);
//
//   Console.write(".");
//
//   SystemMonitor.printMemoryStatistics();
//}

import extensions;
 
class U0 : Exception
{
   constructor new() 
      <= super new("U0 exception");
}
 
class U1 : Exception
{
   constructor new() 
      <= super new("U1 exception");
}
 
singleton Exceptions
{
   static int i;
 
   bar()
      <= baz();
 
   baz()
   {
      if (i == 0)
      {
         U0.raise()
      }
      else
      {
         U1.raise()
      }
   }
 
   foo()
   {
      for(i := 0; i < 2; i := i + 1)
      {
         try
         {
            self.bar()
         }
         catch(U0 e)
         {
            Console.printLine("U0 Caught")
         }
      }
    }    
}
 
public Program()
{
    Exceptions.foo()
}