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
    bool notequal(o) = !self.equal(o);
    
    // virtual method declarations : forcing the output type                            
    predefined bool less(o);
    predefined bool greater(o);
    predefined bool notless(o);
    predefined bool notgreater(o);
                
    [info:"Returns the string representation. By default it returns the class name."]
    string toPrintable() = __className;
                
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
        
        [info:"executes logical OR operation;<br/>Always return true"]
        bool or(expr) = this self;
    
        [info:"executes logical AND operation."]
        bool and(expr) = expr;
    
        [info:"executes logical XOR operation."]
        bool xor(expr)
        {
            cast bool(expr) ? { ^ BoolValue.false };
        }
    
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
        [info:"always returns nil"]
        if(trueAction) = nil;
        
        [info:"executes else-action"]
        if(trueAction,falseAction) = falseAction();
    
        [info:"executes the action"]
        ifnot(action) = action();
    
        [info:"always returns falseVal."]
        iif(object trueVal, object falseVal) = falseVal;
    
        [info:"executes logical OR operation"]
        bool or(expr) = expr;
    
        [info:"executes logical AND operation;<br/>Always return false"]
        bool and(expr) = this self;
    
        [info:"executes logical XOR operation"]
        bool xor(expr)
        {
            cast bool(expr) ? { ^ BoolValue.true };
            
            ^ this self
        }
    
        [info:"determines whether the specified object is equal to the current object boolean value"]
        bool equal(bool f)
        {
            ^ !f
        }
    
        [info:"always returns true"]
        bool Inverted = BoolValue.true;
    
        [info:"returns literal representation"]
        string toPrintable() = "false";
        
        [info:"returns itself"]
        bool clone() = this self;        
    };
                        
    [info:"determines whether the specified object is equal to the current object boolean value;<br/>has to be implemented"]
    abstract bool equal(bool f);

    [info:"executes trueAction if the value is true;<br/>has to be implemented"]
    abstract if(trueAction);

    [info:"executes trueAction if the value is true or falseAction otherwise;<br/>has to be implemented"]
    abstract if(trueAction,falseAction);

    [info:"executes action if the value is false;<br/>has to be implemented"]
    abstract ifnot(action);

    [info:"returns trueVal if the value is true or falseVal otherwise;<br/>has to be implemented"]
    abstract iif(object trueVal, object falseVal);

    [info:"executes logical OR operation<;br/>has to be implemented"]
    abstract bool or(expr);

    [info:"executes logical AND operation;<br/>has to be implemented"]
    abstract bool and(expr);

    [info:"executes logical XOR operation;<br/>has to be implemented"]
    abstract bool xor(expr);

    [info:"inverts the value;<br/>has to be implemented"]
    get abstract bool Inverted();

    [info:"clones the value;<br/>has to be implemented"]
    abstract bool clone();
}

public const true = BoolValue.true;
public const false = BoolValue.false;

public extension commonOp
{
    [info:"returns the parameter"]
    back(o) = o;
 
    [info:"executes the parameter function"]
    then(Func code)
        = code();
}
