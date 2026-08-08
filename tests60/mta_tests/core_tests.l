import extensions;
import system'threading;
import system'collections;
import system'runtime;
import ltests;

// Layer A, MTA core

// A4, mutual exclusion

class LockWorkerState
{
   RoundBarrier Barrier    : prop;
   Array        Counters   : prop;
   Array        Completed  : prop;
   int          Rounds     : prop;
   int          Increments : prop;
   ErrorBox     Errors     : prop;
}

lockWorker(state)
{
   int rounds := state.Rounds;
   int increments := state.Increments;
   auto barrier := state.Barrier;
   auto counters := state.Counters;
   auto completed := state.Completed;

   for (int k := 0; k < rounds; k += 1)
   {
      auto counter := counters[k];
      auto latch := completed[k];

      try
      {
         // released together, so the increments actually overlap
         barrier.arrive(k);

         for (int i := 0; i < increments; i += 1)
            { counter.increment() }
      }
      catch(Exception e)
      {
         state.Errors.record(e)
      };

      latch.done()
   }
}

// Regression, the trylock and freelock templates plus the __enterCritical spin loop at
// __enterCritical in system.core_routines.esm
//
// A lock that does not exclude loses updates and the total comes up short, never long.
// Vacuous under STA, where trylock is xor eax,eax, so this must not move to
// system_tests
public mtaCriticalSectionTest() : testCase()
{
   int t := MtaLoad.WorkerCount;
   int rounds := MtaLoad.Repetitions;
   int increments := 1000;

   auto counters := Array.allocate(rounds);
   auto completed := Array.allocate(rounds);

   for (int k := 0; k < rounds; k += 1)
   {
      counters[k] := SharedCounter.new();
      completed[k] := Latch.new(t)
   };

   auto state := new LockWorkerState();
   state.Barrier := RoundBarrier.new(t, rounds);
   state.Counters := counters;
   state.Completed := completed;
   state.Rounds := rounds;
   state.Increments := increments;
   state.Errors := ErrorBox.new();

   for (int i := 0; i < t; i += 1)
   {
      auto worker := Thread.assign(&lockWorker);

      worker.start(state)
   };

   for (int k := 0; k < rounds; k += 1)
   {
      auto latch := completed[k];
      auto counter := counters[k];

      state.Barrier.releaseAll(k);

      latch.await();

      // the exact total, a broken lock still produces a large number
      Assert.ifEqual(counter.Value, t * increments)
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

// A3, thread table churn

class ChurnState
{
   int      Allocations : prop;
   Latch    Done        : prop;
   ErrorBox Errors      : prop;
}

churnWorker(state)
{
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

// Runs waves of workers that allocate and exit, checking that a list built before the
// first wave comes through every one of them intact.
//
// whenCollect places the collection, and the bisection found that the position is the
// only thing that matters. Neither the volume nor the kind of collection does : a tiny
// young generation collecting hundreds of times never corrupted anything, while a
// single collection landing right after the threads are created did. Full moves more
// objects than minor, so it corrupts more from the same position, and with one worker
// per wave it stops happening at all
//   0 - minor right after the wave starts
//   1 - full right after the wave starts, the worst case measured
//   2 - minor at the start and full once the wave has joined
churnRounds(int waves, int workersPerWave, int whenCollect)
{
   // has to come through every wave unchanged
   auto survivors := new ArrayList();
   for (int i := 0; i < 200; i += 1)
      { survivors.append(i * 13) };

   auto errors := ErrorBox.new();

   for (int w := 0; w < waves; w += 1)
   {
      auto state := new ChurnState();
      state.Allocations := 5000;
      state.Done := Latch.new(workersPerWave);
      state.Errors := errors;

      for (int i := 0; i < workersPerWave; i += 1)
      {
         auto worker := Thread.assign(&churnWorker);

         worker.start(state)
      };

      // collect while the wave finishes, so one lands in the window where some workers
      // have exited and others have not
      if (whenCollect == 1)
         { GCManager.collectFull() }
      else
         { GCManager.collectMinor() };

      state.Done.await();
      state.Done.close();

      if (whenCollect == 2)
         { GCManager.collectFull() }
   };

   errors.rethrowIfAny();

   // element by element, a partial relocation leaves the length right
   Assert.ifEqual(survivors.Length, 200);
   for (int i := 0; i < 200; i += 1)
      { Assert.ifEqual(survivors[i], i * 13) }
}

// Regression, the labNext scan of GC_COLLECT and GC_ALLOCPERM
//
// clearThreadEntry leaves null slots in the middle of the table,
// ELENARTMachine::clearThreadEntry, and the scan has to walk past them. With the cursor
// advance inside the non-null branch a null slot pins the loop on the same entry and
// every higher index thread keeps running while the collector relocates objects
//
// Measured on i386, four threads with one exiting : 8 SIGSEGV in 10 before the fix,
// 0 in 10 after, 5 in 10 with only that line put back
//
// The failure is corruption, so the signal is surviving with live data intact rather
// than an assertion firing where the bug is
public mtaThreadTableChurnTest() : testCase()
{
   int workersPerWave := 4;

   churnRounds(8, workersPerWave, 2);

   Console.write(".");

   auto errors := ErrorBox.new();

   // the table has to still be usable
   auto after := Latch.new(2);
   auto tail := new ChurnState();
   tail.Allocations := 100;
   tail.Done := after;
   tail.Errors := errors;

   for (int i := 0; i < 2; i += 1)
   {
      auto worker := Thread.assign(&churnWorker);

      worker.start(tail)
   };

   after.await();
   after.close();
   errors.rethrowIfAny();

   Console.write(".")
}

// Regression, the system 8 safe point of thread_start
//
// The collector only walks the thread table under gc_lock, so a thread reaching its
// system 3 after that walk is invisible to the collection already running. It used to
// leave thread_start and read StartHelper.Argument out of the heap while the collector
// was compacting, which shows up as a nil Argument or as one pointing at some other
// object
//
// Both cases put the collection right where the wave starts, which is the position the
// bisection isolated. Full is the worse of the two because it moves more : measured at
// 20 failures in 40 before the fix against 9 in 40 for minor
public mtaCollectFullOnThreadStartTest() : testCase()
{
   churnRounds(8, 4, 1);

   Console.write(".")
}

public mtaCollectMinorOnThreadStartTest() : testCase()
{
   churnRounds(8, 4, 0);

   Console.write(".")
}
