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

// --- RangeEnumerator ---

public class RangeEnumerator : Enumerator
{
    object _variable;
    object _start;
    object _end;
    object _step;
    
    bool   _forward;
    
    constructor new(var,start,end,step)
    {
        _variable := var;
        _start := start;
        _end := end;
        _step := step;
        
        _forward := (start < end);
        
        self.reset()
    }

    constructor new(start, end, step)
        <= new(new Variable(), start, end, step);

    constructor new(start, end)
        <= new(new Variable(), start, end, 1);
        
    reset() 
    {
        _variable.Value := _start;

        if(_forward)
        {
            _variable.reduce:_step 
        }
        else
        {
            _variable.append:_step
        }
    }    
    
    get Value() = _variable.Value;
    
    bool next()
    {
        if(_forward)
            { _variable.append:_step; ^ (_variable <= _end) };
        
        _variable.reduce:_step; 
        
        ^ (_variable >= _end)
    }
    
    enumerable() = _variable;
}
