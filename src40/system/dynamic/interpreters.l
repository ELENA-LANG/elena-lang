//import system'collections.

// --- Struct ---

public mixin Struct
{
    object[] items;
    
    internal constructor allocate(int n)
        = new object[](n);
    
    constructor(object[] list)
        <= allocate(list.Length)
    {
        int len := list.Length;
        for(int i := 0, i < len, i := i + 1)
        {
            items[i] := list[i]
        }
    }

    internal vprop int Length
    {
        read(ref int retVal)
        {
            int len := 0;
            items $shr len;
            
            retVal := len
        }
    }

    internal at(int index)
        = items[index];

    internal setAt(int index, object o)
    {
        items[index] := o
    }

//    constructor new(ArrayList list, int index, int length)
//        <= new(length);
//    [
//        int i := 0.
//        while(i < length)
//        [
//            self setField(i, list getAt(index + i)).
//            
//            i := i + 1
//        ]
//    ]
    
    dispatch() => system'core_routines'__struct;
}

// --- DynamicStruct ---

public class DynamicStruct
{
    Struct theStruct;
    
    constructor()
    {
        theStruct := Struct.allocate(0)
    }
    
//    constructor new : aStruct
//    [
//        theStruct := T<Struct>(aStruct).
//    ]

    set generic(o)
    {
        bool notFound := true;
        int len := theStruct.Length;
        int i := 0;
        while ((i < len) && notFound)
        {
            var item := theStruct.at(i);
            if (cast MessageName(item) == __received)
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
            theStruct := theStruct.reallocate(len + 2);

            theStruct.setAt(i, __received)
       };

        // set the content for the appropriate field
        theStruct.setAt(i + 1, o);
    }
    
//    containsProperty(Signature subj)
//    [
//        int anIndex := 0.
//        int aLength := 0.
//        
//        theStruct readFieldCountTo(&aLength).
//        while (anIndex < aLength)
//        [
//            var aCurrent := theStruct~classROp getField(anIndex).
//            
//            if(subj equal(T<Signature>(aCurrent)))
//                [ ^ true ].
//                
//            anIndex := anIndex + 2.                
//        ].
//        
//        ^ false
//    ]
    
    dispatch() => theStruct;
}
