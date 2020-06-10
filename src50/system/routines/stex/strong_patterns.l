// --- BasePattern ---

// --- Summing ---

public StrongSumming<T> : Func<T,object>
{
    ref<T> result;
    
    constructor(T defValue)
    {
        result := defValue
    }

    run(Enumerator<T> e)
    {
        super.run(e);
        
        ^ result.Value
    }

    object function(T item)
    { 
        T val := result.Value;
        val := val + item;
        
        result.Value := val
    }
}

public StrongFilter<T> : Enumerator<T>
{
    Func<T,bool>  filter;
    Enumerator<T> enumerator;
    
    constructor(Func<T,bool> filter, Enumerator<T> enumerator)
    {
        this filter := filter;
        this enumerator := enumerator;
    }
    
    T get() => enumerator;
    
    bool next()
    {
        while (enumerator.next())
        {
            if (filter(enumerator.get()))
                { ^ true }
        };
        
        ^ false
    }    
    
    reset() => enumerator;
    
    enumerable() => enumerator;
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// --- StrongSelector ---

public StrongSelector<T1, T2> : Enumerator<T2>
{
    Func<T1,T2> selector;
    Enumerator<T1> enumerator;

    constructor(Func<T1,T2> selector, Enumerator<T1> enumerator)
    {
        this selector := selector;
        this enumerator := enumerator;
    }
    
    T2 get() = selector(enumerator.get());
    
    bool next() = enumerator.next();
    
    reset() => enumerator;
    
    enumerable() => enumerator;
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}

// --- StrongJoinEnumerator ---

public StrongJoinEnumerator<T1, T2, T3> : Enumerator<T3>
{
    Enumerator<T1> left;
    Enumerator<T2> right;

    Func<T1,T2,bool> filter;
    Func<T1,T2,T3>   selector;
    
    constructor(Func<T1,T2,bool> filter_f, Func<T1,T2,T3> select_f, Enumerator<T1> left_e, Enumerator<T2> right_e)
    {
        left := left_e;
        right := right_e;
        
        filter := filter_f;
        selector := select_f
    }
    
    reset()
    {
        left.reset();
        right.reset()
    }
    
    bool next()
    {
        while(left.next())
        {
            right.reset();
            
            while (right.next())
            {
                if(filter(left.get(), right.get())) 
                {
                    ^ true
                }
            }
        };
        
        ^ false
    }
    
    T3 get() = selector(left.get(), right.get());
    
    enumerable() => left;    
    
    Enumerator cast() = new Enumerator { embeddable dispatch() => self; };
}
