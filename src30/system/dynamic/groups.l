#define system.

// --- Wrap ---

#class(group,dbg:array) Wrap : BaseArray
{
    #method => <handle_wrap>.
}

// --- Extension ---

#class(group,dbg:array) Extension : BaseArray
{
    #method => <handle_extension>.
}

// --- Group ---

#class(group,dbg:array) Group : BaseArray
{
    #method => <handle_group>.
}
