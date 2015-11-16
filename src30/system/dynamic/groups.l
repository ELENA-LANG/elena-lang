#import system.

// --- Wrap ---

#class(group) Wrap :: BaseArray
{
    #method => system'internal'handle_wrap.
}

// --- Extension ---

#class(group) Extension :: BaseArray
{
    #method => system'internal'handle_extension.
}

// --- Group ---

#class(group) Group :: BaseArray
{
    #method => system'internal'handle_group.
}
