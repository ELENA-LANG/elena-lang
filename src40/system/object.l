// --- super object ---

/// a common ancestor

public class Object
{
    /// Creates the object
    constructor() {}
    
//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    ///
//    /// By default compares the object references.
//    bool equal(o) => system'core_routines'__object.
//    
//    bool notequal(o) = self equal:anObject; inverted.
//    
//    // virtual method declarations : forcing the output type                            
//    predefined bool less : anObject []
//    predefined bool greater : anObject []
//    predefined bool notless : anObject []
//    predefined bool notgreater : anObject []
//                
//    T<literal> literal = self __className.
                
    /// Implements a message dispatching in VMT
    dispatch() => system'core_routines'__object;
}

///// A common boolean value
//public enum class BoolValue
//{
//    /// return the default value
//    constructor
//        = false.
//        
//    abstract bool equal(bool aBool) []
//
//    abstract if : aTrueAction []
//
//    abstract if:aTrueAction else:aFalseAction []
//
//    abstract ifnot : aTrueAction []
//
//    abstract iif(object trueVal, object falseVal) []
//
//    abstract bool or : anExpr []
//
//    abstract bool and : anExpr []
//
//    abstract bool xor : aBool []
//
//    abstract bool inverted []
//
//    abstract clone []
//}
//
//public extension commonOp
//{
//    /// Returns the parameter        
//    back : anObject = anObject.
// 
//    then(Func code)
//        = code().
//        
//    thenClose(Func code)
//    [
//        code().
//        
//        self close
//    ]
//    
//    doFinally(Func c1, Func c2)
//    [
//        (c1()) then:c2
//            | raise:c2.
//    ]
//    
//    doAndClose(Func code)
//    [
//        self thenClose:code
//            | raise:[ self close ].
//    ]        
//}
//
//public singleton __prober
//{
//    dispatch => system'core_routines'__prober.    
//}
