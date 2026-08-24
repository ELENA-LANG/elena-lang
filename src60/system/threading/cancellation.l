import system'collections;

public sealed class CancellationTokenRegistration
{
   CancellationTokenSource? _source;
   Func?                    _action;
   Thread?                  _executingThread;
   ManualResetEvent?        _completion;
   Reference<int>           _waiterCount;

   internal constructor new()
   {
      _executingThread := nil;
      _waiterCount := 0
   }

   internal constructor new(CancellationTokenSource source, Func action)
   {
      _source := source;
      _action := action;
      _executingThread := nil;
      _waiterCount := 0
   }

   internal Func? takeAction()
   {
      Func? action := nil;
      lock(self) {
         action := _action;
         if:not:nil(action) {
            _action := nil;
            _source := nil;
            _executingThread := Thread.Current;
         }
      };

      ^ action
   }

   internal sealed completeAction()
   {
      ManualResetEvent? completion := nil;
      lock(self) {
         _executingThread := nil;
         completion := _completion
      };

      if:not:nil(completion)
         completion!.set()
   }

   close()
   {
      CancellationTokenSource? source := nil;
      ManualResetEvent? completion := nil;
      lock(self) {
         source := _source;
         _source := nil;
         _action := nil;

         Thread? executingThread := _executingThread;
         if:not:nil(executingThread) {
            if:not (executingThread.equalReference(Thread.Current)) {
               if (_completion == nil)
                  _completion := ManualResetEvent.new();

               _waiterCount.append(1);
               completion := _completion
            }
         }
      };

      if:not:nil(source)
         source!.unregister(self);

      if:not:nil(completion) {
         completion!.wait();

         bool closeCompletion := false;
         lock(self) {
            _waiterCount.reduce(1);
            if (*_waiterCount == 0) {
               _completion := nil;
               closeCompletion := true
            }
         };

         if (closeCompletion)
            completion!.close()
      }
   }
}

public sealed class CancellationToken
{
   CancellationTokenSource? _source;

   static CancellationToken _none;

   static constructor()
   {
      _none := new CancellationToken()
   }

   internal constructor new()
   {
   }

   internal constructor new(CancellationTokenSource source)
   {
      _source := source
   }

   static CancellationToken None
      = _none;

   bool CanBeCanceled
      = _source != nil;

   bool IsCancellationRequested
      = _source?.IsCancellationRequested ?? false;

   CancellationTokenRegistration register(Func action)
   {
      if (action == nil)
         InvalidArgumentException.raise("action");

      CancellationTokenSource? source := _source;
      if (source == nil)
         { ^ CancellationTokenRegistration.new() };

      ^ source!.register(action)
   }

   throwIfCancellationRequested()
   {
      if (self.IsCancellationRequested)
         OperationCanceledException.withToken(self).raise()
   }
}

public class OperationCanceledException : Exception
{
   CancellationToken _cancellationToken;

   constructor new()
      <= withToken(CancellationToken.None);

   constructor withToken(CancellationToken cancellationToken)
   {
      if (cancellationToken == nil)
         InvalidArgumentException.raise("cancellationToken");

      initialize("The operation was canceled");
      _cancellationToken := cancellationToken
   }

   CancellationToken Token
      = _cancellationToken;
}

public sealed class CancellationTokenSource
{
   bool                                 _cancellationRequested;
   CancellationToken                    _token;
   List<CancellationTokenRegistration>? _registrations;
   TimerRegistration?                   _timer;

   constructor new()
   {
      _cancellationRequested := false;
      _token := CancellationToken.new(self)
   }

   CancellationToken Token
      = _token;

   bool IsCancellationRequested
   {
      get()
      {
         bool requested := false;
         lock(self) {
            requested := _cancellationRequested
         };

         ^ requested
      }
   }

   internal CancellationTokenRegistration register(Func action)
   {
      CancellationTokenRegistration? registration := nil;
      bool invokeNow := false;

      lock(self) {
         if (_cancellationRequested) {
            invokeNow := true
         }
         else {
            registration := CancellationTokenRegistration.new(self, action);
            if (_registrations == nil)
               _registrations := new List<CancellationTokenRegistration>();

            _registrations!.append(registration!)
         }
      };

      if (invokeNow) {
         action();

         ^ CancellationTokenRegistration.new()
      };

      ^ registration!
   }

   internal unregister(CancellationTokenRegistration registration)
   {
      lock(self) {
         if:not:nil(_registrations) {
            List<CancellationTokenRegistration> registrations := _registrations!;
            int index := -1;
            int len := registrations.Length;
            for (int i := 0; i < len && index < 0; i += 1) {
               if (registrations[i].equalReference(registration))
                  index := i
            };

            if (index >= 0)
               registrations.remove(index)
         }
      }
   }

   cancel()
   {
      List<CancellationTokenRegistration>? registrations := nil;
      TimerRegistration? timer := nil;
      lock(self) {
         if:not (_cancellationRequested) {
            _cancellationRequested := true;
            registrations := _registrations;
            _registrations := nil;
            timer := _timer;
            _timer := nil;
         }
      };

      if:not:nil(timer)
         timer!.close();

      List<Exception>? exceptions := nil;
      if:not:nil(registrations) {
         List<CancellationTokenRegistration> items := registrations!;
         for (int i := items.Length - 1; i >= 0; i -= 1) {
            CancellationTokenRegistration item := items[i];
            Func? action := item.takeAction();
            if:not:nil(action) {
               try {
                  action()
               }
               catch (Exception e) {
                  if (exceptions == nil)
                     exceptions := new List<Exception>();

                  exceptions!.append(e)
               }
               finally {
                  item.completeAction()
               }
            }
         }
      };

      if:not:nil(exceptions)
         AggregateException.new(exceptions!.Value).raise()
   }

   cancelAfter(int milliseconds)
   {
      if (milliseconds < -1)
         InvalidArgumentException.raise("milliseconds");

      if:not (self.IsCancellationRequested) {
         TimerRegistration? replacement := nil;
         if (milliseconds >= 0)
            replacement := TimerQueue.schedule(milliseconds, { self.cancel() });

         TimerRegistration? previous := nil;
         bool discardReplacement := false;
         lock(self) {
            if (_cancellationRequested) {
               discardReplacement := true
            }
            else {
               previous := _timer;
               _timer := replacement
            }
         };

         if:not:nil(previous)
            previous!.close();

         if (discardReplacement && replacement != nil)
            replacement!.close()
      }
   }
}

public class TaskCanceledException : OperationCanceledException
{
   constructor new()
      <= super new();

   constructor withToken(CancellationToken cancellationToken)
      <= super withToken(cancellationToken);
}
