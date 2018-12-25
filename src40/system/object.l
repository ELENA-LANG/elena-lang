// --- super object ---

/// a common ancestor

public class Object
{
    /// Creates the object
    constructor() {}
    
    constructor new()
        <= ();
    
    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    bool equal(o) => system'core_routines'__object;
    
    bool notequal(o) = self.equal(o).Inverted;
    
    // virtual method declarations : forcing the output type                            
    predefined bool less(o) {}
    predefined bool greater(o) {}
    predefined bool notless(o) {}
    predefined bool notgreater(o) {}
                
    string Printable = self.__getClassName();
                
    /// Implements a message dispatching in VMT
    dispatch() => system'core_routines'__object;
}

/// A common boolean value
public /*enum */interface class BoolValue
{
    /// return the default value
    static Default
        = false;
        
    abstract bool equal(bool f) {}

    abstract if(trueAction) {}

    abstract if(trueAction,falseAction) {}

    abstract ifnot(action) {}

    abstract iif(object trueVal, object falseVal) {}

    abstract bool or(expr) {}

    abstract bool and(expr) {}

    abstract bool xor(expr) {}

    abstract get bool Inverted() {}

//    abstract clone []
}

public extension commonOp
{
    /// Returns the parameter        
    back(o) = o;
 
    then(Func code)
        = code();
        
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
}

//public singleton __prober
//{
//    dispatch => system'core_routines'__prober.    
//}
