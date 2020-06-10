// --- Struct ---

public mixin Struct
{
    __string object[] items;
    
    constructor allocate(int n)
        = new object[](n);

    constructor new(params object[] list)
        <= allocate(list.Length)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            items[i] := list[i]
        }
    }
    
    constructor load(object[] list)
        <= allocate(list.Length)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            items[i] := list[i]
        }
    }

    internal int Length
    {
        get()
        {
            int len := 0;
            len $sizeof items;
            
            
            ^ len
        }
    }

    internal Struct expand(int gain)
    {
        int len := self.Length;
        int newLength := len + gain;
        
        auto newArr := Struct.allocate(newLength);
        
        for(int i := 0, i < len, i += 1)
        {
            newArr[i] := items[i];
        };
        
        ^ newArr
    }
                
    internal at(int index)
        = items[index];

    internal setAt(int index, object o)
    {
        items[index] := o
    }
    
    dispatch() => intern system'core_routines'__struct;
}

// --- DynamicStruct ---

public class DynamicStruct
{
    Struct theStruct;
    
    constructor()
    {
        theStruct := Struct.allocate(0)
    }
    
    set generic(o)
    {
        MessageName subj := __received.MessageName;
        
        bool notFound := true;
        int len := theStruct.Length;
        int i := 0;
        while ((i < len) && notFound)
        {
            var item := theStruct.at(i);
            if (cast MessageName(item) == subj)
            {
                notFound := false
            }
            else
            {
                i := i + 2
            }
        };
                
        // if not found extend the structure
        if(i == len)
        {                          
            theStruct := theStruct.expand(2);

            theStruct.setAt(i, subj)
       };

        // set the content for the appropriate field
        theStruct.setAt(i + 1, o);
    }
    
    containsProperty(MessageName subj)
    {
        int index := 0;        
        int length := theStruct.Length;
        
        while (index < length)
        {
            var current := theStruct[index];
            
            if(subj.equal(cast MessageName(current)))
                { ^ true };
                
            index := index + 2
        };
        
        ^ false
    }
    
    dispatch() => theStruct;
}
