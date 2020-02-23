
// --- variables ---

public sealed class Integer : BaseVariable, ref<int>
{
    constructor(int n) 
    {
        value := n
    }

    constructor()
    {
        value := 0
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
        
    append(int n)
    {
        int v := value;
        v := v + n;
        
        value := v
    }
    
    reduce(int n)
    {
        int v := value;
        v := v - n;
        
        value := v
    }
    
    dispatch() => value;
}

public sealed class Real : BaseVariable, ref<real>
{
    constructor(real n) 
    {
        value := n
    }
    
    constructor new()
    {
        value := 0.0r
    }
        
//    constructor new : n
//        <= new(T<real>(n)).
        
    append(real n)
    {
        real v := value;
        v := v + n;
        
        value := v
    }
    
    append(n)
        <= append(cast real(n));
    
    reduce(real n)
    {
        real v := value;
        v := v - n;
        
        value := v
    }
    
    reduce(n)
        <= reduce(cast real(n));
        
    dispatch() => value;
}

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
