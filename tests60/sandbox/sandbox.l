import extensions;
import system'threading;

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

class ThreadCurrentWorkerArg
{
   int                Index   : prop;
   Array              Threads : prop;
   CountDownEvent     Ready   : prop;
   ManualResetEvent   Release : prop;
   Latch              Done    : prop;
   ErrorBox           Errors  : prop;
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

threadCurrentWorker(arg)
{
   try
   {
      Thread current := Thread.Current;

      Assert.ifTrue(current.equalReference(Thread.Current));

      Array threads := arg.Threads;
      int index := arg.Index;
      threads[index] := current;

      arg.Ready.signal();
      arg.Release.wait();

      Assert.ifTrue(current.equalReference(Thread.Current))
   }
   catch(Exception e)
   {
      arg.Errors.record(e)
   };

   arg.Done.done()
}

public Program()
{
   int count := 4;
   Array threads := Array.allocate(count);
   CountDownEvent ready := CountDownEvent.new(count);
   ManualResetEvent release := ManualResetEvent.new();
   Latch done := Latch.new(count);
   ErrorBox errors := ErrorBox.new();

   for (int i := 0; i < count; i += 1)
   {
      ThreadCurrentWorkerArg arg := new ThreadCurrentWorkerArg();
      arg.Index := i;
      arg.Threads := threads;
      arg.Ready := ready;
      arg.Release := release;
      arg.Done := done;
      arg.Errors := errors;

      Thread worker := Thread.assign(&threadCurrentWorker);
      worker.start(arg)
   };

   ready.wait();

   bool unique := true;
   for (int i := 0; i < count; i += 1)
   {
      for (int j := i + 1; j < count; j += 1) {
         if (threads[i].equalReference(threads[j]))
            unique := false
      }
   };

   release.set();
   done.await();
   errors.rethrowIfAny();

   Assert.ifTrue(unique);

   ready.close();
   release.close();
   done.close();

   Console.write(".")
}
