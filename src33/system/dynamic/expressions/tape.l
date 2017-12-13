// --- ClosureTape ---

mixin ClosureTape
{
    __array<DynamicClosure> theList.
    
    generic constructor new(__array<DynamicClosure> args)
        = args.
        
    dispatch => system'core_routines'__closuretape.        
}


