
// --- variables ---

public sealed class Integer : BaseVariable, ref<int>
{
    constructor()
    {
        theValue := 0
    }
    
    constructor(int n) 
    {
        theValue := n
    }
    
    constructor new()
        <= ();
    
//    constructor new : n
//        <= new(T<int>(n)).
        
//    append : n
//        <= append(T<int>(n)).
//
//    reduce : n
//        <= reduce(T<int>(n)).
        
    append(int n)
    {
        int v := theValue;
        v := v + n;
        
        theValue := v
    }
    
    reduce(int n)
    {
        int v := theValue;
        v := v - n;
        
        theValue := v
    }
    
    dispatch() => theValue;
}

