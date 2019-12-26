
// --- variables ---

public sealed class Integer : BaseVariable, ref<int>
{
    constructor()
    {
        value := 0
    }
    
    constructor(int n) 
    {
        value := n
    }
    
    constructor new()
    {
    }
    
////    constructor new : n
////        <= new(T<int>(n)).
//        
////    append : n
////        <= append(T<int>(n)).
////
////    reduce : n
////        <= reduce(T<int>(n)).
//        
//    append(int n)
//    {
//        int v := theValue;
//        v := v + n;
//        
//        theValue := v
//    }
//    
//    reduce(int n)
//    {
//        int v := theValue;
//        v := v - n;
//        
//        theValue := v
//    }
    
    dispatch() => value;
}

//public sealed class Real : BaseVariable, ref<real>
//{
//    constructor()
//    {
//        theValue := 0.0r
//    }
//    
//    constructor(real n) 
//    {
//        theValue := n
//    }
//    
//    constructor new()
//        <= ();
//        
////    constructor new : n
////        <= new(T<real>(n)).
//        
//    append(real n)
//    {
//        real v := theValue;
//        v := v + n;
//        
//        theValue := v
//    }
//    
//    append(n)
//        <= append(cast real(n));
//    
//    reduce(real n)
//    {
//        real v := theValue;
//        v := v - n;
//        
//        theValue := v
//    }
//    
//    reduce(n)
//        <= reduce(cast real(n));
//        
//    dispatch() => theValue;
//}
//
//public sealed class LongInteger : BaseVariable, ref<long>
//{
//    constructor()
//    {
//        theValue := 0l
//    }
//    
//    constructor(long n) 
//    {
//        theValue := n
//    }
//    
////    constructor new : n
////        <= new(T<long>(n)).
//        
//    append(n)
//        <= append(cast long(n));
//
//    reduce(n)
//        <= reduce(cast long(n));
//                
//    append(long n)
//    {
//        long v := theValue;
//        v := v + n;
//        
//        theValue := v
//    }
//    
//    reduce(long n)
//    {
//        long v := theValue;
//        v := v - n;
//        
//        theValue := v
//    }
//}
