// --- DynamicClosure ---

public interface class DynamicClosure
{
    abstract generic closure(V<object> args) []
}

// --- MessageClosure ---

public struct MessageClosure :: DynamicClosure
{
    __mssg message.
    
    constructor new(mssg message)
        = message.
    
    generic closure(V<object> args)
        => system'core_routines'__mssg_closure.
}

// --- ConstantClosure --- 

public class ConstantClosure :: DynamicClosure
{
    object constant.
    
    constructor new(Object constant)
    [
        @constant := constant.
    ]
    
    generic closure(V<object> args)
        = constant.
}
