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
        
    dispatch => system'core_routines'__closuretape.        
}


