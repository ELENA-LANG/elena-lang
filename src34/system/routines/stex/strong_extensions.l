public extension strongEnumerableOp<type>
{
    forEach(Func<type,object> f)
    [
        Enumerator<type> e := self enumerator.
        
        while (e next)
        [
            f(e get).
        ]
    ]
    
    system'Enumerator<type> filterBy(Func<type,bool> f)
        = StrongFilter<type>(f, system'Enumerator<type>(self enumerator)).
        
    system'Enumerator<type> selectBy(Func<type,type> f)
        = StrongSelector<type,type>(f, system'Enumerator<type>(self enumerator)).
        
    system'Enumerator<object> selectBy(Func<type,object> f)
        = StrongSelector<type,object>(f, system'Enumerator<type>(self enumerator)).
}

public extension strongEnumerableOp<type1,type2>
{
    system'Enumerator<type2> selectBy(Func<type1,type2> f)
        = StrongSelector<type1,type2>(f, system'Enumerator<type1>(self enumerator)).    
}

public extension strongEnumerableOp<type1,type2,type3>
{
    system'Enumerator<type3> join(object list) on(Func<type1,type2,bool> filter_f) selectBy(Func<type1,type2,type3> select_f)
        = StrongJoinEnumerator<type1,type2,type3>(filter_f, select_f, system'Enumerator<type1>(self enumerator), system'Enumerator<type2>(list enumerator)).        
}

public extension strongEnumerableOp2<type>
{
    stackunsafe type summarize(ref<type> retVal)
    [
        auto p := StrongSumming<type>(retVal).
        
        p run(system'Enumerator<type>(self enumerator)).
        
        ^ retVal value
    ]
}