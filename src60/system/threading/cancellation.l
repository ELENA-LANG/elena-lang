import system'collections;

public sealed class CancellationTokenRegistration
{
   CancellationTokenSource? _source;
   Func?                    _action;

   internal constructor new()
   {
   }

   internal constructor new(CancellationTokenSource source, Func action)
   {
      _source := source;
      _action := action
   }

   internal Func? takeAction()
   {
      Func? action := nil;
      lock(self) {
         action := _action;
         _action := nil;
         _source := nil
      };

      ^ action
   }

   close()
   {
      CancellationTokenSource? source := nil;
      lock(self) {
         source := _source;
         _source := nil;
         _action := nil
      };

      if:not:nil(source)
         source!.unregister(self)
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
      lock(self) {
         if:not (_cancellationRequested) {
            _cancellationRequested := true;
            registrations := _registrations;
            _registrations := nil;
         }
      };

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
            }
         }
      };

      if:not:nil(exceptions)
         AggregateException.new(exceptions!.Value).raise()
   }
}

public class TaskCanceledException : OperationCanceledException
{
   constructor new()
      <= super new();

   constructor withToken(CancellationToken cancellationToken)
      <= super withToken(cancellationToken);
}
