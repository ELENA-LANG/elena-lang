import extensions;
import system'threading;
import ltests;

// Layer A, thread local storage
//
// Separate file because declaring a __threadvar makes this the one part of the suite
// that cannot be compiled against an STA target, compiling.cpp:882-886

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

// Regression, peektls and storetls plus the per thread root range the collector builds
// from env_tls_size
//
// Expected to fail on Linux, and the failure is the point. The image writes the TLS
// section as [ThreadContent | var0 ...], jitcompiler.cpp:3385-3398, and the collector
// agrees, but the _LNX branch of peektls and storetls puts the variables below the
// ThreadContent. Confirm with readelf -lW, PT_TLS MemSiz against tt_size
public mtaThreadLocalStorageTest() : testCase()
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
