
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
    
    constructor()
    {
        value := 0.0r
    }
    
    constructor new()
    {
    }
        
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

public sealed class LongInteger : BaseVariable, ref<long>
{
    constructor()
    {
        value := 0l
    }
    
    constructor(long n) 
    {
        value := n
    }
    
    append(n)
        <= append(cast long(n));

    reduce(n)
        <= reduce(cast long(n));
                
    append(long n)
    {
        long v := value;
        v := v + n;
        
        value := v
    }
    
    reduce(long n)
    {
        long v := value;
        v := v - n;
        
        value := v
    }
}
