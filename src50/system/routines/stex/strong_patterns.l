// --- BasePattern ---

// --- Summing ---

public StrongSumming<T> : Func<T,object>
{
    ref<T> result;
    
    constructor(T defValue)
    {
        result := defValue
    }

//    constructor()
//    {
//        result := T.Default
//    }

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

////// --- StrongJoinEnumerator ---
////
////public StrongJoinEnumerator<type1, type2, type3> :: Enumerator<type3>
////{
////    Enumerator<type1> _left.
////    Enumerator<type2> _right.
////
////    Func<type1,type2,bool>  _filter.
////    Func<type1,type2,type3> _selector.
////    
////    constructor(Func<type1,type2,bool> filter_f, Func<type1,type2,type3> select_f, Enumerator<type1> left_e, Enumerator<type2> right_e)
////    [
////        _left := left_e.
////        _right := right_e.
////        
////        _filter := filter_f.
////        _selector := select_f.
////    ]
////    
////    reset
////    [
////        _left reset.
////        _right reset.
////    ]
////    
////    bool next
////    [
////        while(_left next)
////        [
////            _right reset.
////            
////            while (_right next)
////            [
////                if(_filter(_left get, _right get)) 
////                [
////                    ^ true.
////                ]
////            ]
////        ].
////        
////        ^ false.
////    ]
////    
////    type3 get = _selector(_left get, _right get).
////    
////    enumerable => _left.    
////    
////    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
////}
