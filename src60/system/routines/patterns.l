import system;
import system'collections;

idleFunc1 = (x){};

// --- Toping ---

public class TopFilter : Enumerator
{
   Reference<int>   _offset;
   Reference<int>   _counter;
   Enumerator       _enumerator;
    
   constructor new(int counter, Enumerator enumerator)
   {
       _counter := counter;
      _offset := 0;
      _enumerator := enumerator;
   }
    
   constructor new(int counter, int offset, Enumerator enumerator)
   {
      _counter := counter;
      _offset := offset;
        
      _enumerator := enumerator;
   }
    
   get Value() = *_enumerator;
    
   bool next()
   {
      if (_counter != 0)
      {
         while (_enumerator.next())
         {
            if (_offset == 0)
            {
               _counter.reduce(1); ^ true
            }
            else
            {
               _offset.reduce(1)
            }
         }
      };
        
      ^ false
   }
    
   reset() => _enumerator;
    
   enumerable() => _enumerator;
}

// --- SkipFilter ---

public class SkipFilter : Enumerator
{
   int        _counter;    
   bool       _skipped;    
   Enumerator _enumerator;
    
   constructor new(int counter, Enumerator en)
   {
      _counter := counter;
      _skipped := false;
      _enumerator := en
   }
    
   get Value() = *_enumerator;
    
   bool next()
   {
      ifnot (_skipped)
      {
         _skipped := true;
            
         int i := _counter;
         while (i > 0)            
         {
            ifnot (_enumerator.next())
            {
               ^ false
            };
                
            i -= 1
         }
      };
        
      ^ _enumerator.next()
   }
    
   reset()
   {
      _enumerator.reset();
      _skipped := false
   }
    
   enumerable() => _enumerator;
}

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

// --- OrderingEnumerator ---

public class OrderingEnumerator : Enumerator
{
    Enumerator _enumerator;
    Func2      _orderFunc;   
    Enumerator _listEnumerator;
    
    constructor new(Func2 orderFunc, Enumerator enumerator)
    {
        _orderFunc := orderFunc;
        _enumerator := enumerator;
        
        self.reset();
    }
    
    constructor new(Func1 orderVal, Enumerator enumerator)
    {
        _orderFunc := (p,n => orderVal(p) < orderVal(n) );
        _enumerator := enumerator;
        
        self.reset()
    }
    
    constructor new(ExtensionMessage fn, Enumerator enumerator)
        <= new((x,y => fn(x, y)), enumerator);
    
    reset()
    {
        _enumerator.reset();
        
        Array list := new Summing(new ArrayList()).run(_enumerator);
        list.sort(_orderFunc);
        
        _listEnumerator := list.enumerator();
    }
    
    get Value() = *_listEnumerator;
    
    bool next()
        = _listEnumerator.next();
        
    enumerable()
        => _enumerator;
}

// --- GroupingEnumerator ---

public class GroupingEnumerator : Enumerator
{
   Dictionary _dictionary;
   Enumerator _enumerator;
   Func1      _groupFunc;   
   Enumerator _groupEnumerator;
    
   constructor new(Func1 grouper, Enumerator enumerator)
   {
      _groupFunc := grouper;
      _dictionary := Dictionary.new();
      _enumerator := enumerator;
        
      self.reset()
   }
    
   constructor new(ExtensionMessage fn, Enumerator enumerator)
      <= new((x => fn(x)), enumerator);
    
   reset()
   {
      _enumerator.reset();
        
      while (_enumerator.next())
      {
         var item := *_enumerator;
         var key := _groupFunc(item);
            
         var groupItem := _dictionary[key];
         if (nil == groupItem)
         {
            groupItem := new ArrayList();
                
            _dictionary[key] := groupItem
         };
            
         groupItem.append:item
      };
        
      _groupEnumerator := _dictionary.enumerator();
   }
    
   get Value() = *_groupEnumerator;
    
   bool next()
      = _groupEnumerator.next();
        
   enumerable()
      => _enumerator;
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

// --- Accumulating ---

public class Accumulating : BasePattern
{
   Func2 _operation;
    
   constructor new(object variable, Func2 operation)
   {
      _result := variable;
      _operation := operation
   }

   constructor new(object variable, ExtensionMessage operation)
   {
      _result := variable;
      _operation := (a,b){ operation(a, b) }
   }

   function(param) { _result.Value := _operation(_result.Value, param) }
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

// --- ZipEnumerator ---

public class ZipEnumerator : Enumerator
{
   Enumerator _enumerator1;
   Enumerator _enumerator2;
    
   Func2      _zipper;
    
   constructor new(Func2 fn, Enumerator enum1, Enumerator enum2)
   {
      _zipper := fn;
        
      _enumerator1 := enum1;
      _enumerator2 := enum2;
   }
    
   constructor new(ExtensionMessage ext, Enumerator enum1, Enumerator enum2)
      <= new((n,m => ext(n,m)), enum1, enum2);
    
   get Value() = _zipper(_enumerator1.Value, _enumerator2.Value);
    
   bool next() = _enumerator1.next() && _enumerator2.next();
    
   reset()
   {
      _enumerator1.reset();
      _enumerator2.reset();
   }

   enumerable() => _enumerator1;
}

// --- patternOp ---

public extension patternOp
{
   skipping(int counter)
      = SkipFilter.new(counter, cast Enumerator(self.enumerator()));

   last(int counter)
      = TopFilter.new(counter, self.Length - counter, cast Enumerator(self.enumerator()));
}

