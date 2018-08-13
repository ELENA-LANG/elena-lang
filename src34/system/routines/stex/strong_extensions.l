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
    
//    filterBy(Func<type,bool> f)
//        = StrongFilter<type> new(aSelector, Enumerator<type>(self enumerator)).
}