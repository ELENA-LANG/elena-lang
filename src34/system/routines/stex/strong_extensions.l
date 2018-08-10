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
}