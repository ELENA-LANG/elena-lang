public extension strongEnumerableOp<T>
{
//    forEach(Func<T,object> f)
//    {
//        Enumerator<T> e := self.enumerator();
//        
//        while (e.next())
//        {
//            f(e.get())
//        }
//    }
    
    Enumerator<T> filterBy(Func<T,bool> f)
        = new StrongFilter<T>(f, cast Enumerator<T>(self.enumerator()));
        
////    Enumerator<T> selectBy(Func<T,T> f)
////        = new StrongSelector<T,T>(f, cast Enumerator<T>(self.enumerator()));
//        
////    system'Enumerator<object> selectBy(Func<type,object> f)
////        = StrongSelector<type,object>(f, system'Enumerator<type>(self enumerator)).
}

//public extension strongEnumerableOp<T1,T2>
//{
//    Enumerator<T2> selectBy(Func<T1,T2> f)
//        = new StrongSelector<T1,T2>(f, cast Enumerator<T1>(self.enumerator()));    
//}
//
////public extension strongEnumerableOp<type1,type2,type3>
////{
////    system'Enumerator<type3> join(object list) on(Func<type1,type2,bool> filter_f) selectBy(Func<type1,type2,type3> select_f)
////        = StrongJoinEnumerator<type1,type2,type3>(filter_f, select_f, system'Enumerator<type1>(self enumerator), system'Enumerator<type2>(list enumerator)).        
////}
//
//public extension strongEnumerableOp2<T>
//{
//    /*stackunsafe */T summarize(T defValue)
//    {
//        auto p := new StrongSumming<T>(defValue);
//        
//        ^ p.run(cast Enumerator<T>(self.enumerator()))
//    }
//}