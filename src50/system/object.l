// --- super object ---

/// a common ancestor

public class Object
{
    constructor new() {} // !! temporal
    
//    /// Creates the object
//    constructor() {}
//    
//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    ///
//    /// By default compares the object references.
//    bool equal(o) => system'core_routines'__object;
//    
//    bool notequal(o) = self.equal(o).Inverted;
//    
//    // virtual method declarations : forcing the output type                            
//    predefined bool less(o) {}
//    predefined bool greater(o) {}
//    predefined bool notless(o) {}
//    predefined bool notgreater(o) {}
//                
//    string Printable = self.__getClassName();
                
    /// Implements a message dispatching in VMT
    dispatch() => intern system'core_routines'__object;
}

///// A common boolean value
//public interface BoolValue
//{
//    /// return the default value
//    static bool Default
//        = false;
//        
//    abstract bool equal(bool f) {}
//
//    abstract if(trueAction) {}
//
//    abstract if(trueAction,falseAction) {}
//
//    abstract ifnot(action) {}
//
//    abstract iif(object trueVal, object falseVal) {}
//
//    abstract bool or(expr) {}
//
//    abstract bool and(expr) {}
//
//    abstract bool xor(expr) {}
//
//    abstract get bool Inverted() {}
//
//    abstract bool clone() {}
//}
//
//public extension commonOp
//{
//    /// Returns the parameter        
//    back(o) = o;
// 
//    then(Func code)
//        = code();
//}
