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
    
    filterBy(Func<type,bool> f)
        = StrongFilter<type>(f, system'Enumerator<type>(self enumerator)).
        
    selectBy(Func<type,type> f)
        = StrongSelector<type,type>(f, system'Enumerator<type>(self enumerator)).
        
    selectBy(Func<type,object> f)
        = StrongSelector<type,object>(f, system'Enumerator<type>(self enumerator)).
}
