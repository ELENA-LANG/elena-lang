import system'collections.

// --- ClosureTape ---

mixin ClosureTape
{
    __array<DynamicClosure> theList.
    
    generic constructor new(__array<DynamicClosure> args)
        = args.
        
    constructor new (int length)
        = __array<DynamicClosure>(length).
        
    constructor new (list<DynamicClosure> list)        
        <= new(list length);
    [
        int i := list length.
        while (i > 0)
        [
            i -= 1.
            
            theList[i] := list[i].
        ]
    ]
        
    constructor new (list<DynamicClosure> list, int index, int length)        
        <= new(length);
    [
        int i := index.
        int c := 0.
        while (c < length)
        [
            theList[c] := list[i].

            c += 1.
            i += 1.
        ]
    ]
        
    constructor new (list<object> list, int index, int length)        
        <= new(length);
    [
        int i := index.
        int c := 0.
        while (c < length)
        [
            theList[c] := list[i].

            c += 1.
            i += 1.
        ]
    ]
        
    dispatch => system'core_routines'__closuretape.        
}


