public sealed EventHandler<TF>
{
    TF[] functions;
    
    constructor(TF f)
    {
        functions := new TF[](1);
        functions[0] := f
    }

    append(TF func)
    {
        int len := functions.Length;
        
        TF[] functions := new TF[](len + 1);

        for (int i := 0, i < len, i += 1) {
            functions[i] := this functions[i]
        };
        
        functions[len] := func;
        
        this functions := functions
    }
    
    dispatch() => intern system'core_routines'__event;
}

#property event<T, __name>
{
    EventHandler<T> __name;
    
    set(f)
    {
        if (nil == __name)
        {
            __name := f
        }
        else
        {
            __name.append(f);
        }
    }
}

