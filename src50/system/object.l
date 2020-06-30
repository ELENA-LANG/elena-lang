// --- super object ---

[info:"a common ancestor"]
public class Object
{
    /// class constant - a class name 
    const string __className := @classname;
    
    /// class constant - a package info
    const __packageInfo      := @package; 
    
    [info:"Creates the object"]
    constructor() {}
    
    [info:"Returns true if the specified object is equal to the current object; otherwise, false. By default compares the object references."]
    bool equal(o) => intern system'core_routines'__object;
       
    [info:"Returns true if the specified object is not equal to the current object; otherwise, false. By default it inverts the result of equal[2]."]
    bool notequal(o) = self.equal(o).Inverted;
    
    // virtual method declarations : forcing the output type                            
    predefined bool less(o) {}
    predefined bool greater(o) {}
    predefined bool notless(o) {}
    predefined bool notgreater(o) {}
                
    [info:"Returns the string representation. By default it returns the class name."]
    string toPrintable() = /*self.__getClassName()*/__className;
                
    [info:"Implements a message dispatching in VMT"]
    dispatch() => intern system'core_routines'__object;
}

[info:"a common boolean value"]
public interface BoolValue
{
    private constructor() {}
    
    [info:"return the default value"]
    static bool MinValue
        = BoolValue.false;

    [info:"returns true value."]
    const static true = BoolValue
    {
        [info:"executes trueAction."]
        if(trueAction) = trueAction();
    
        [info:"executes trueAction."]
        if(trueAction,falseAction) = trueAction();
    
        [info:"always returns nil."]
        ifnot(trueAction) = nil;
    
        [info:"always returns trueVal."]
        iif(object trueVal, object falseVal) = trueVal;
        
        [info:"executes logical OR operation<br/>Always return true"]
        bool or(expr) = this self;
    
        [info:"executes logical AND operation."]
        bool and(expr) = expr;
    
        [info:"executes logical XOR operation."]
        bool xor(expr)
        {
            cast bool(expr) ? { ^ BoolValue.false };
        }
    
        [info:"determines whether the specified object is equal to the current object boolean value."]
        bool equal(expr) = expr;
    
        [info:"determines whether the specified object is equal to the current object boolean value."]
        bool equal(bool b) = b;
    
        [info:"always returns false."]
        bool Inverted = BoolValue.false;
    
        [info:"returns literal representation."]
        string toPrintable() = "true";
    
        [info:"Returns itself."]
        bool clone() = this self;        
    };
                        
    [info:"returns false value."]
    const static false = BoolValue
    {
        /// Always returns nil
        if(trueAction) = nil;
        
        /// Executes else-action
        if(trueAction,falseAction) = falseAction();
    
        /// Executes the action
        ifnot(action) = action();
    
        iif(object trueVal, object falseVal) = falseVal;
    
        /// Executes logical OR operation
        bool or(expr) = expr;
    
        /// Executes logical AND operation
        ///
        /// Always return false
        bool and(expr) = this self;
    
        /// Executes logical XOR operation
        bool xor(expr)
        {
            cast bool(expr) ? { ^ BoolValue.true };
            
            ^ this self
        }
    
        /// Determines whether the specified object is equal to the current object boolean value
        bool equal(expr)
        {
            cast bool(expr) ? { ^ this self } : { ^ BoolValue.true }
        }
    
        /// Determines whether the specified object is equal to the current object boolean value
        bool equal(bool f)
        {
            ^ f.Inverted
        }
    
        /// Always returns true
        bool Inverted = BoolValue.true;
    
        /// Returns literal representation
        string toPrintable() = "false";
        
        /// Returns itself
        bool clone() = this self;        
    };
                        
    abstract bool equal(bool f) {}

    abstract if(trueAction) {}

    abstract if(trueAction,falseAction) {}

    abstract ifnot(action) {}

    abstract iif(object trueVal, object falseVal) {}

    abstract bool or(expr) {}

    abstract bool and(expr) {}

    abstract bool xor(expr) {}

    get abstract bool Inverted() {}

    abstract bool clone() {}
}

public const true = BoolValue.true;
public const false = BoolValue.false;

public extension commonOp
{
    /// Returns the parameter        
    back(o) = o;
 
    then(Func code)
        = code();
}
