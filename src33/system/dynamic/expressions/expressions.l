import system'collections.
import system'dynamic.

limited class Expression
{
    constructor Constant(object o)
        = ConstantExpression new(o).
        
    generic constructor MessageCall(mssg message, Expression target, Expression<> arguments)
        = MessageCallExpression new(message,target,arguments).
        
    saveTo(list<DynamicClosure> list)
    [
    ]
    
    type<ClosureTape> compiled
    [
        list<DynamicClosure> list/* := list<DynamicClosure>(8)*/.
        
        $self saveTo(list).
        
        ^ ClosureTape new:list.
    ]
}

class ConstantExpression :: Expression
{
    object object.
    
    constructor new(object o)
    [
        @object := o.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(ConstantClosure new(object)).
    ]
}

class MessageCallExpression :: Expression
{
    mssg              message.
    type<Expression>  target.
    array<Expression> arguments.
    
    constructor new(mssg message, Expression target, Expression<> arguments)
    [
        @message := message.
        @target := target.
        @arguments := arguments.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        int len := arguments length.
        int i := len.
        while(i > 0)
        [
            i -= 1.
            
            arguments[i] saveTo(list).
        ].
        
        target saveTo(list).
        
        list append(MessageClosure new:message)
    ]
}
