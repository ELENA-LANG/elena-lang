// --- variables ---

public sealed class Integer : BaseVariable, Reference<int>
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

public sealed class Real : BaseVariable, Reference<real>
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
