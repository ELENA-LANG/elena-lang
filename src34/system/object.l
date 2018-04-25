// --- super object ---

/// a common ancestor

public class Object
{
    /// Creates the object
    constructor new []
    
    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    bool equal : anObject => system'core_routines'__object.
    
    bool notequal : anObject = self equal:anObject; inverted.
                
    T<literal> literal = self __className.
                
    /// Implements a message dispatching in VMT
    dispatch => system'core_routines'__object.
}

/// A common boolean value
public enum class BoolValue
{
    abstract bool equal(bool aBool) []

    abstract if : aTrueAction []

    abstract if:aTrueAction else:aFalseAction []

    abstract ifnot : aTrueAction []

    abstract iif(object trueVal, object falseVal) []

    abstract bool or : anExpr []

    abstract bool and : anExpr []

    abstract bool xor : aBool []

//    abstract bool not []

    abstract bool inverted []

    abstract clone []
}

extension commonOp
{
    /// Returns the parameter        
    back : anObject = anObject.
 
//    then : code
//        = code eval.
//        
//    thenClose:code
//    [
//        code eval.
//        
//        self close
//    ]
//    
//    doFinally func:c1 func:c2
//    [
//        c1 eval; then:c2
//            | raise:c2.
//    ]
//    
//    doAndClose : code
//    [
//        self thenClose:code
//            | raise:[ self close ].
//    ]        
}

singleton __prober
{
    dispatch => system'core_routines'__prober.    
}