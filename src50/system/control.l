// === Basic prototypes ===

// --- Enumerator ---

[info:"An enumerator prototype"]
public interface Enumerator
{
    [info:"Returns the current object"]
    get abstract Value();
    
    [info:"Goes to the next enumeration member and returns true if it is not the last member"]
    abstract bool next();
       
    [info:"Resets the enumerator"]
    abstract reset();
    
    [info:"returns the enumerator owner"]
    abstract enumerable();
    
    Enumerator enumerator() = self;
}

// --- enumerator<> ---

public interface Enumerator<T>
{
    abstract bool next();
    
    get abstract T Value();
    
    /// Resets the enumerator
    abstract reset();
    
    /// returns the enumerator owner
    abstract enumerable();

    abstract Enumerator cast();
    
    Enumerator<T> enumerator() = self;
}

public abstract Enumerable
{
    abstract Enumerator enumerator();
}

public abstract Enumerable<T>
{
    abstract Enumerator<T> enumerator();
}

// --- Indexer ---

/// An indexer prototype
public interface Indexer
{
    /// Returns false if the index out of range
    get abstract bool Available();

    /// Returns the current member
    get abstract Value();
    
    /// Sets the current member
    set abstract Value(object);
    
    abstract int Index
    {
        /// Reads the current index
        get();
        
        /// Sets the index by specified value
        set(retVal);
    }

    abstract appendIndex(int anIndex);
   
    abstract int Length
    {
        get();
    }
}

public interface Indexer<T>
{
    /// Returns false if the index out of range
    get abstract bool Available();

    /// Returns the current member
    get abstract T Value();
    
    /// Sets the current member
    set abstract Value(T object);

    abstract int Index
    {
        /// Reads the current index
        get();
        
        /// Sets the index by specified value
        set(retVal);
    }

    abstract appendIndex(int anIndex);
                                
    abstract int Length
    {
        get();
    }
    
    abstract Indexer cast();
}

public abstract Indexable
{
   abstract Indexer indexer();
}

public abstract Indexable<T>
{
   abstract Indexer<T> indexer();
}

// --- MatrixIndexer ---

/// An indexer prototype
public interface Indexer2D
{
    /// Returns false if the index out of range
    get abstract bool Available();

    /// Returns the current member
    get abstract Value();
    
    /// Sets the current member
    set abstract Value(object);
    
    abstract int Index1
    {
        /// Reads the current index
        get();
        
        /// Sets the index by specified value
        set(value);
    }
    
    abstract int Index2
    {
        /// Reads the current index
        get();
        
        /// Sets the index by specified value
        set(value);
    }
    
    /// Increaments the index by specified value
    abstract appendIndex1(int offs);
    
    /// Increaments the index by specified value
    abstract appendIndex2(int offs);

    abstract int Length1
    {
        get();
    }

    abstract int Length2
    {
        get();
    }
}

// --- base functions ---

[info:"A base action"]
public interface Func
{
    /// Executes the action
    abstract function();
    
    /// Executes the action while the condition is true
    sealed doWhile(b)
    {
        while (b)
        {
            self()
        }
    }

    /// Executes the action while the condition is false
    ///
    /// Executes at least once
    sealed doUntil(b)
    {
        do
        {
            self()
        }
        until(b);
    }
}

/// A base function
public interface Func1
{
    /// Executes the action
    abstract function(param);
    
    [info:"Executes the function for every enumeration item"]
    run(Enumerator en)
    {
        while(en.next())
        {
            self(*en)
        }
    }
    
    /// Executes the function for every enumeration item until it returns true
    retrieve(Enumerator en)
    {
        object item := nil;
        while (en.next())
        {
            item := *en;

            if(self(item))
                 { ^ item }
        };
        
        ^ nil
    }
}

public interface Func<T, TResult>
{
    /// Executes the action
    abstract TResult function(T param);
    
    Func1 cast()
        = (x => self(x));
    
    run(Enumerator<T> e)
    {
        while(e.next())
        {
            self(*e)
        }
    }
}

/// A base two variable function
public interface Func2
{
    /// Executes the action
    abstract function(param1, param2);
    
    /// Executes the function for parameters and every enumeration item
    sealed run(object param, Enumerator enumerator)
    {
        while(enumerator.next())
        {
            self(param,*enumerator)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true
    sealed retrieve(object param, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(param, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2);
    
    Func2 cast()
        = (x,y => self(x,y));
}

/// A base three variable function
public interface Func3
{
    /// Executes the function
    abstract function(param1, param2, param3);
    
    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3);
    
    Func3 cast()
        = (x,y,z => self(x,y,z));
}

/// A base three variable function
public interface Func4
{
    /// Executes the function
    abstract function(param1, param2, param3, param4);
    
    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4);
    
    Func4 cast()
        = (a,b,c,d => self(a,b,c,d));
}

/// A base five variable function
public interface Func5
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5);

    Func5 cast()
        = (a,b,c,d,e => self(a,b,c,d,e));
}

/// A base six variable function
public interface Func6
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5, param6);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, object p5, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,p5,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, object p5, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, p5, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,T6,TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6);

    Func6 cast()
        = (a,b,c,d,e,f => self(a,b,c,d,e,f));
}

/// A base seven variable function
public interface Func7
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5, param6, param7);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, object p5, object p6, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,p5,p6,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, object p5, object p6, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, p5, p6, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,T6,T7, TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7);

    Func7 cast()
        = (a,b,c,d,e,f,g => self(a,b,c,d,e,f,g));
}

/// A base eight variable function
public interface Func8
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5, param6, param7, param8);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, object p5, object p6, object p7, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,p5,p6,p7,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, object p5, object p6, object p7, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, p5, p6, p7, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,T6,T7,T8, TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8);

    Func8 cast()
        = (a,b,c,d,e,f,g,h => self(a,b,c,d,e,f,g,h));
}

/// A base nine variable function
public interface Func9
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, object p5, object p6, object p7, object p8, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,p5,p6,p7,p8,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, object p5, object p6, object p7, object p8, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, p5, p6, p7, p8, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,T6,T7,T8,T9, TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8, T9 param9);
    
    Func9 cast()
        = (a,b,c,d,e,f,g,h,i => self(a,b,c,d,e,f,g,h,i));
}

/// A base ten variable function
public interface Func10
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, object p5, object p6, object p7, object p8, object p9, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,p5,p6,p7,p8,p9,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, object p5, object p6, object p7, object p8, 
                    object p9, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, p5, p6, p7, p8, p9, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10, TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8,
                            T9 param9, T10 param10);
    
    Func10 cast()
        = (a,b,c,d,e,f,g,h,i,j => self(a,b,c,d,e,f,g,h,i,j));
}

/// A base eleven variable functions
public interface Func11
{
    /// Executes the function
    abstract function(param1, param2, param3, param4, param5, param6, param7, param8, param9, param10, param11);

    /// Executes the function for parameters and every enumeration item
    sealed run(object p1, object p2, object p3, object p4, object p5, object p6, object p7, object p8, 
                object p9, object p10, Enumerator e)
    {
        while (e.next())
        {
            self(p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,*e)
        }
    }
    
    /// Executes the function for parameters and every enumeration item until it returns true    
    sealed retrieve(object p1, object p2, object p3, object p4, object p5, object p6, object p7, object p8, 
                    object p9, object p10, Enumerator enumerator)
    {
        while (enumerator.next())
        {
            var item := *enumerator;
            
            if (self(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, item))
                { ^ item }
        };        
        
        ^ nil
    }
}

public interface Func<T1,T2,T3,T4,T5,T6,T7,T8,T9,T10,T11,TResult>
{
    /// Executes the action
    abstract TResult function(T1 param1, T2 param2, T3 param3, T4 param4, T5 param5, T6 param6, T7 param7, T8 param8,
                            T9 param9, T10 param10, T11 param11);

    Func11 cast()
        = (a,b,c,d,e,f,g,h,i,j,k => self(a,b,c,d,e,f,g,h,i,j,k));
}

// --- base lazy expression ---

public abstract BaseLazyExpression
{
    abstract function();
    
    dispatch() => intern system'core_routines'__lazyexpression;
}

// --- BaseEnumerator<type> ---

public abstract class BaseEnumerator<T> : Enumerator<T>
{
    ref<int> theIndex;
    int      theLength;
    
    bool next()
    {
        theIndex.append(1);
        
        ^ (cast int(theIndex)) < theLength
    }
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// --- BaseIndexer ---

public abstract class BaseIndexer : Indexer
{
    int theIndex;
    int theLength;
    
    bool Available = (theIndex < theLength);

    int Index
    {
        /// Reads the current i-index into an output parameter
        get()
        {
            ^ theIndex
        }
        
        /// Sets the i-index by specified value
        set(index)
        { 
            theIndex := index
        }
    }    
    
    appendIndex(int index)
    { 
        theIndex += index
    }
    
    int Length
    {
        get()
        {
            ^ theLength
        }
    }
}

public abstract BaseIndexer<T> : Indexer<T>
{
    int theIndex;
    int theLength;
    
    /// Returns false if the index out of range
    bool Available = (theIndex < theLength);

    int Index
    {    
        /// Reads the current i-index into an output parameter
        get()
        {
            ^ theIndex
        }    
        
        /// Sets the i-index by specified value
        set(index)
        {
            theIndex := index
        }
    } 
    
    appendIndex(int anIndex)
    { 
        theIndex := theIndex + anIndex
    }     
    
    int Length
    {
        get()
        {
            ^ theLength;
        }
    }
    
    Indexer cast() = new Indexer { embeddable dispatch() => self; };
}

// --- Range --

class IntRangeEnumerator : Enumerator<int>
{
    int theIndex;
    int theStart;
    int theEnd;

    constructor(IntNumber start, IntNumber end)
    {
        theStart := start;
        theEnd := end;
        
        self.reset();
    }

    get int Value() = theIndex;
            
    /// Returns false if the index out of range
    bool next()
    {
        theIndex += 1;
        
        ^ theIndex < theEnd
    }
    
    /// Sets the i-index by specified value
    reset()
    {
        theIndex := theStart - 1;
    } 
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
    
    enumerable() = new Range(theStart, theEnd - theStart);
}

public sealed class Range : Enumerable<int>
{
    int theStart;
    int theEnd;
    
    constructor(int start, int count)
    {
        theStart := start;
        theEnd := start + count
    }
    
    constructor new(int start, int count)
    {
        theStart := start;
        theEnd := start + count
    }
    
    constructor for(int start, int end)
        <= new(start, end - start);
    
    int Length
    {
        get()
        {
            int l := theEnd - theStart;
            
            ^ l
        }
    }
    
    Enumerator<int> enumerator() = new IntRangeEnumerator(theStart, theEnd);
    
    Enumerable cast() = new Enumerable { Enumerator enumerator() => self; };
}
