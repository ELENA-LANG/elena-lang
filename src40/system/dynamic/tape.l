import system'collections;

// --- ClosureTape ---

public mixin ClosureTape
{
    DynamicClosure[] theList;
    
    internal constructor allocate(int len)
        = new DynamicClosure[](len);
    
    constructor new(params DynamicClosure[] args)
        <= allocate(args.Length)
    {
        self.setFields(params args)
    }

    /*internal */constructor allocate(int length)
        = new DynamicClosure[](length);
        
    constructor new (List<DynamicClosure> list)        
        <= allocate(list.Length)
    {
        int i := list.Length;
        while (i > 0)
        {
            i -= 1;
            
            theList[i] := list[i]
        }
    }
        
//    constructor new (List<DynamicClosure> list, int index, int length)        
//        <= new(length);
//    [
//        int i := index.
//        int c := 0.
//        while (c < length)
//        [
//            theList[c] := list[i].
//
//            c += 1.
//            i += 1.
//        ]
//    ]
        
    constructor new(List<object> list, int index, int length)        
        <= allocate(length)
    {
        int i := index;
        int c := 0;
        while (c < length)
        {
            theList[c] := list[i];

            c += 1;
            i += 1
        }
    }
        
    dispatch() => system'core_routines'__closuretape;
}

// --- Class ---

public mixin ClosureClass
{
    object[] items;
    
    internal constructor allocate(int n)
        = new object[](n);
    
//    constructor new(Array list)
//        <= newInternal(list length);
//    [
//        int i := 0.
//        int n := list length.
//        
//        while (i < n)
//        [
//            self setField(i, list getAt(i)).
//            
//            i := i + 1
//        ]
//    ]    
//    
//    constructor new(ArrayList list, int index, int length)
//        <= newInternal(length);
//    [
//        int i := 0.
//        while(i < length)
//        [
//            self setField(i,list getAt(index + i)).
//            
//            i := i + 1
//        ]
//    ]
    
    constructor new(List<object> list, int index, int length)
        <= allocate(length)
    {
        for (int i := 0, i < length, i += 1)
        {
            items[i]:= list.at(i)
        }
    }
    
    dispatch() => system'core_routines'__closureclass;
}
