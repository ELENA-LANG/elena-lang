// --- DynamicClosure ---

public interface class DynamicClosure
{
    abstract closure(params object[] args) {}
}

// --- MessageClosure ---

public struct MessageClosure : DynamicClosure
{
    embeddable mssg theMessage;
    
    constructor(mssg message)
    {
        theMessage := message
    }
    
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
