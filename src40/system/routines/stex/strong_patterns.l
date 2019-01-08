//// --- BasePattern ---
//
//// --- Summing ---
//
//public StrongSumming<type> :: Func<type,object>
//{
//    ref<type> result.
//    
//    constructor(ref<type> acc)
//    [
//        result := acc.
//    ]
//
//    run(Enumerator<type> e)
//    [
//        super run(e).
//        
//        ^ result value
//    ]
//
//    object closure(type item)
//    [ 
//        type val := result value.
//        val := val + item.
//        
//        result value := val.
//    ]
//}
//
//public StrongFilter<type> :: Enumerator<type>
//{
//    Func<type,bool>  filter.
//    Enumerator<type> enumerator.
//    
//    constructor(Func<type,bool> filter, Enumerator<type> enumerator)
//    [
//        @filter := filter.
//        @enumerator := enumerator.
//    ]
//    
//    type get => enumerator.
//    
//    bool next
//    [
//        while (enumerator next)
//        [
//            if (filter(enumerator get))
//                [ ^ true ]
//        ].
//        
//        ^ false
//    ]    
//    
//    reset => enumerator.
//    
//    enumerable => enumerator.
//    
//    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
//}
//
//// --- StrongSelector ---
//
//public StrongSelector<type1, type2> :: Enumerator<type2>
//{
//    Func<type1,type2> selector.
//    Enumerator<type1> enumerator.
//
//    constructor(Func<type1,type2> selector, Enumerator<type1> enumerator)
//    [
//        @selector := selector.
//        @enumerator := enumerator.
//    ]
//    
//    type2 get = selector(enumerator get).
//    
//    bool next = enumerator next.
//    
//    reset => enumerator.
//    
//    enumerable => enumerator.
//    
//    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
//}
//
//// --- StrongJoinEnumerator ---
//
//public StrongJoinEnumerator<type1, type2, type3> :: Enumerator<type3>
//{
//    Enumerator<type1> _left.
//    Enumerator<type2> _right.
//
//    Func<type1,type2,bool>  _filter.
//    Func<type1,type2,type3> _selector.
//    
//    constructor(Func<type1,type2,bool> filter_f, Func<type1,type2,type3> select_f, Enumerator<type1> left_e, Enumerator<type2> right_e)
//    [
//        _left := left_e.
//        _right := right_e.
//        
//        _filter := filter_f.
//        _selector := select_f.
//    ]
//    
//    reset
//    [
//        _left reset.
//        _right reset.
//    ]
//    
//    bool next
//    [
//        while(_left next)
//        [
//            _right reset.
//            
//            while (_right next)
//            [
//                if(_filter(_left get, _right get)) 
//                [
//                    ^ true.
//                ]
//            ]
//        ].
//        
//        ^ false.
//    ]
//    
//    type3 get = _selector(_left get, _right get).
//    
//    enumerable => _left.    
//    
//    cast T<Enumerator> = Enumerator::{ embeddable dispatch => self. }.
//}
