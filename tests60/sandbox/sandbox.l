import system'collections;
import extensions;
import system'threading;
import extensions'threading;
import system'runtime;

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
 
public Program()
{
   churnRounds(8, 4, 1);

   Console.write(".")
}