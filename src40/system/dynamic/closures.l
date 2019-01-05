// --- DynamicClosure ---

public interface class DynamicClosure
{
    abstract closure(params object[] args) {}
}

// --- MessageClosure ---

public struct MessageClosure : DynamicClosure
{
    embeddable __mssg theValue[4];
    
    constructor(mssg message)
        = message;
    
    closure(params object[] args)
        => system'core_routines'__mssg_closure;
}

//// --- ConstantClosure --- 
//
//public class ConstantClosure :: DynamicClosure
//{
//    object constant.
//    
//    constructor new(Object constant)
//    [
//        @constant := constant.
//    ]
//    
//    generic closure(V<object> args)
//        = constant.
//}
