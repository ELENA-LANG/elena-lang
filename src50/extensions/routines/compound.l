import system'collections;
import system'routines;

public singleton EmptyEnumerable
{
    enumerator() = new Enumerator
    {
        bool next() = false;
        
        get() = nil;
        
        reset() {}
        
        enumerable() = self;
    };    
}

public class SingleEnumerable
{
    object value;

    constructor new(value)
    {
        this value := value
    }

    enumerator() = new Enumerator
    {
        bool notpassed := nil != value;
        
        get()
        {
            notpassed := false;
            
            ^ value
        }
        
        bool next()
            = notpassed;
            
        reset()
        {
            notpassed := nil != value
        }            
        
        enumerable() = self;
    };
}

public class CompoundEnumerator : Enumerator
{
    Enumerator[] enumerators;
    int          index;
    
    constructor new(params object[] args)
    {
        index := 0;
        
        int len := args.Length;
        int i := 0;
        enumerators := new Enumerator[](len);        
        while (i < len)
        {
            enumerators[i] := args[i].enumerator();
            i += 1
        }
    }
    
    reset()
    {
        enumerators.forEach:(e){e.reset()}
    }
    
    enumerable() = self;
    
    get()
        = enumerators[index].get();
        
    bool next()
    {
        until (enumerators[index].next())
        {
            int len := enumerators.Length;
            
            index += 1;
            ifnot (index < len)
            {
                ^ false
            }
        };
        
        ^ true
    }        
}

