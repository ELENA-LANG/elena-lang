// --- BasePattern ---

// --- Summing ---

public StrongSumming<type> :: Func<type,object>
{
    ref<type> result.
    
    constructor(ref<type> acc)
    [
        result := acc.
    ]

    run(Enumerator<type> e)
    [
        super run(e).
        
        ^ result value
    ]

    object closure(type item)
    [ 
        type val := result value.
        val := val + item.
        
        result value := val.
    ]
}

public StrongFilter<type> :: Enumerator<type>
{
    Func<type,bool>  filter.
    Enumerator<type> enumerator.
    
    constructor(Func<type,bool> filter, Enumerator<type> enumerator)
    [
        @filter := filter.
        @enumerator := enumerator.
    ]
    
    type get => enumerator.
    
    bool next
    [
        while (enumerator next)
        [
            if (filter(enumerator get))
                [ ^ true ]
        ].
        
        ^ false
    ]    
    
    reset => enumerator.
    
    enumerable => enumerator.
    
    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
}

// --- StrongSelector ---

public StrongSelector<type1, type2> :: Enumerator<type2>
{
    Func<type1,type2> selector.
    Enumerator<type1> enumerator.

    constructor(Func<type1,type2> selector, Enumerator<type1> enumerator)
    [
        @selector := selector.
        @enumerator := enumerator.
    ]
    
    type2 get = selector(enumerator get).
    
    bool next = enumerator next.
    
    reset => enumerator.
    
    enumerable => enumerator.
    
    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
}
