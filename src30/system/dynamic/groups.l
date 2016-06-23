#import system.

// --- Wrap ---

#class(dynamic,group) Wrap
{
    #field $item.
    
    #method => system'core_routines'__wrap.
}

// --- Extension ---

#class(dynamic,group) Extension
{
    #field $item.
    
    #method => system'core_routines'__extension.
}

// --- Group ---

#class(dynamic,group) Group
{
    #field $item.
    
    #constructor(stacksafe) $new &int:aLength
        = object:aLength.
        
    #constructor new &args:objects
        <= ($new &int:(objects length))
    [
    ]
    
    #method => system'core_routines'__group.
}

// --- extensions ---

#class(extension) dynamicOp
{
    #method mix &into:role
        = Extension(role, self).
}