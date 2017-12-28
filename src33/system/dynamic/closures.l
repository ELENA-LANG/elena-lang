// --- DynamicClosure ---

limited class DynamicClosure
{
    generic closure(object<> args) []
}

// --- MessageClosure ---

struct MessageClosure :: DynamicClosure
{
    __mssg message.
    
    constructor new mssg:message
        = message.
    
    closure(object<> args)
        => system'core_routines'__mssg_closure.
}

// --- ConstantClosure --- 

class ConstantClosure :: DynamicClosure
{
    object constant.
    
    constructor new(Object constant)
    [
        @constant := constant.
    ]
    
    closure(object<> args)
        = constant.
}
