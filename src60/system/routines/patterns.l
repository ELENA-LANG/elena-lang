import system;

// --- Selecting ---

public class Selector : Enumerator
{
   Func1      _selector;
   Enumerator _enumerator;

   constructor new(Func1 selector, Enumerator enumerator)
   {
      _selector := selector;
      _enumerator := enumerator
   }
    
   constructor new(ExtensionMessage extension, Enumerator enumerator)
   {
      _selector := (x => extension(x));
      _enumerator := enumerator
   }
    
   get Value() = _selector(*_enumerator);
    
   bool next() = _enumerator.next();
    
   reset()
   {
      _enumerator.reset()
   }    
    
   enumerable() => _enumerator;
}

// === patterns ===

// --- BasePattern ---

public abstract class BasePattern : Func1
{
   object _result;

   run(Enumerator en)
   {
      super.run(en);

      ^ _result
   }
}

// --- Summing ---

public class Summing : BasePattern
{
   constructor(variable)
   {
      _result := variable
   }

   function(param) { _result.append(param) }
}

// --- Filtering ---

public class Filter : Enumerator
{
    Func1      _filter;
    Enumerator _enumerator;

    constructor new(Func1 selector, Enumerator enumerator)
    {
        _filter := selector;
        _enumerator := enumerator
    }
    
    constructor new(ExtensionMessage extension, Enumerator enumerator)
    {
        _filter := (x => extension(x));
        _enumerator := enumerator
    }
    
    get Value() = *_enumerator;
    
    bool next()
    {
        while (_enumerator.next())
        {
            if (_filter(*_enumerator))
                { ^ true }
        };
        
        ^ false
    }
    
    reset () => _enumerator;
    
    enumerable() => _enumerator;
}
