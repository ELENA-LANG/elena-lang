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
        
    system'Enumerator<type> selectBy(Func<type,object> f)
        = StrongSelector<type,object>(f, system'Enumerator<type>(self enumerator)).
        
    summarize
    [
        auto p := StrongSumming<type>().
        
        ^ p run(system'Enumerator<type>(self enumerator)).
    ]    
//        = StrongSumming<type>() run(system'Enumerator<type>(self enumerator)).
}
