public extension strongEnumerableOp<T>
{
    forEach(Func<T,object> f)
    {
        Enumerator<T> e := self.enumerator();
        
        while (e.next())
        {
            f(e.get())
        }
    }
    
    Enumerator<T> filterBy(Func<T,bool> f)
        = new StrongFilter<T>(f, cast Enumerator<T>(self.enumerator()));
        
    Enumerator<T> selectBy(Func<T,T> f)
        = new StrongSelector<T,T>(f, cast Enumerator<T>(self.enumerator()));
        
    Enumerator<T> selectBy(Func<T,object> f)
        = new StrongSelector<T,object>(f, cast Enumerator<T>(self.enumerator()));
}

public extension strongEnumerableOp<T1,T2>
{
    Enumerator<T2> selectBy(Func<T1,T2> f)
        = new StrongSelector<T1,T2>(f, cast Enumerator<T1>(self.enumerator()));    
}

public extension strongEnumerableOp<T1,T2,T3>
{
    Enumerator<T3> joinBy(object list, Func<T1,T2,bool> filter_f, Func<T1,T2,T3> select_f)
        = new StrongJoinEnumerator<T1,T2,T3>(filter_f, select_f, 
                cast Enumerator<T1>(self.enumerator()), cast Enumerator<T2>(list.enumerator()));
}

public extension strongEnumerableOp2<T>
{
    /*stackunsafe */T summarize(T defValue)
    {
        auto p := new StrongSumming<T>(defValue);
        
        ^ p.run(cast Enumerator<T>(self.enumerator()))
    }
}