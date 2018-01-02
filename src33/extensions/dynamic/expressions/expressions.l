import system'collections.
import system'dynamic.
import extensions'dynamic.

limited class TapeExpression
{
    constructor Constant(object o)
        = TapeConstantExpression new(o).
        
    generic constructor MessageCall(TapeExpression target, literal message, TapeExpression<> arguments)
        = TapeMessageCallExpression new(target,message,arguments).
        
    constructor Symbol(TapeExpression expression)
        = TapeSymbolExpression new(expression).
        
    constructor Singleton(TapeExpression<> methods)
        = TapeSingleton new(methods).
        
    constructor Method(literal message, TapeCode body, TapeExpression<> parameters)
        = TapeMethod new(message, body, parameters).
        
    constructor Method(literal message, TapeCode body)
        = TapeMethod new(message, body).
        
    constructor Code(TapeExpression<> statements)
        = TapeCode new(statements).
        
    constructor Parameter(literal name)
        = TapeParameterExpression new(name).
        
    constructor Variable(literal name)
        = TapeVariableExpression new(name).
        
    constructor Declaring(literal name)
        = TapeNewVariableExpression new(name).
        
    constructor Assigning(literal name, TapeExpression value)
        = TapeAssigningExpression new(name, value).
        
    constructor Loop(TapeExpression condition, TapeExpression body)
        = TapeLoopExpression new(condition, body).
        
    constructor Returning(TapeExpression value)
        = TapeReturningExpression new(value).
        
    saveTo(list<DynamicClosure> list)
    [
    ]
    
    compiled
    [
        list<DynamicClosure> list := list<DynamicClosure>(8).
        
        $self saveTo(list).
        
        type<ClosureTape> tape := ClosureTape new:list.
        
        ^ [ TapeAssembly new(tape); eval ]
    ]
}

class TapeSymbolExpression :: TapeExpression
{
    type<TapeExpression> expr.
    
    constructor new(object expr)
    [
        @expr := expr.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openSymbolClosure).
        expr saveTo(list).
        list append(closeClosure).
    ]
}

class TapeSingleton :: TapeExpression
{
    list<TapeExpression> methods.
    
    constructor new(TapeExpression<> methods)
    [
        @methods := methods.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openSingletonClosure).
        
        int len := methods length.
        int i := 0.
        while(i < len)
        [
            methods[i] saveTo(list).
            
            i += 1.            
        ].
        
        list append(closeClosure).
    ]
}

class TapeMethod :: TapeExpression
{
    literal               message.
    type<TapeCode>        body.
    array<TapeExpression> parameters.
    
    constructor new(literal message, TapeCode body, TapeExpression<> parameters)
    [
        @message := message.
        @body := body.
        @parameters := parameters.
    ]
    
    constructor new(literal message, TapeCode body)
    [
        @message := message.
        @body := body.
        @parameters := $nil.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openMethodClosure).
        list append(newMessageClosure new:message).
        
        if ($nil != parameters)
        [
            int len := parameters length.
            int i := 0.
            while(i < len)
            [
                parameters[i] saveTo(list).
                
                i += 1.            
            ].            
        ].

        list append(newParamTokenClosure new:"self").
                
        body saveTo(list).
        
        list append(closeClosure).
    ]    
}

class TapeCode :: TapeExpression
{
    list<TapeExpression> statements.
    
    constructor new(TapeExpression<> statements)
    [
        @statements := statements.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openCodeClosure).
        
        int len := statements length.
        int i := 0.
        while(i < len)
        [
            statements[i] saveTo(list).
            
            i += 1.            
        ].
        
        list append(closeClosure).
    ]
}

class TapeConstantExpression :: TapeExpression
{
    object object.
    
    constructor new(object o)
    [
        @object := o.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(newConstantClosure new(object))
    ]
}

class TapeParameterExpression :: TapeExpression
{
    literal name.
    
    constructor new(literal name)
    [
        @name := name.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(newParamTokenClosure new(name))
    ]
}

class TapeVariableExpression :: TapeExpression
{
    literal name.
    
    constructor new(literal name)
    [
        @name := name.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(newIdentifierClosure new(name))
    ]
}

class TapeNewVariableExpression :: TapeExpression
{
    literal name.
    
    constructor new(literal name)
    [
        @name := name.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openVariableClosure).
        list append(newIdentifierClosure new(name)).
        list append(closeClosure).
    ]
}

class TapeMessageCallExpression :: TapeExpression
{
    literal               message.
    type<TapeExpression>  target.
    array<TapeExpression> arguments.
    
    constructor new(TapeExpression target, literal message, TapeExpression<> arguments)
    [
        @message := message.
        @target := target.
        @arguments := arguments.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openExpressionClosure).
        
        target saveTo(list).
        
        int len := arguments length.
        int i := 0.
        while(i < len)
        [
            arguments[i] saveTo(list).
            
            i += 1.            
        ].
        
        list append(newMessageClosure new:message).
        
        list append(closeClosure).
    ]
}

class TapeAssigningExpression :: TapeExpression
{
    literal               name.
    type<TapeExpression>  value.
    
    constructor new(literal name, TapeExpression value)
    [
        @name := name.
        @value := value.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openExpressionClosure).
        list append(newIdentifierClosure new(name)).
        list append(newAssignClosure).
        
        value saveTo(list).
        
        list append(closeClosure).
    ]
}

class TapeReturningExpression :: TapeExpression
{
    type<TapeExpression>  value.
    
    constructor new(TapeExpression value)
    [
        @value := value.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openReturningClosure).
        value saveTo(list).        
        list append(closeClosure).
    ]
}

class TapeLoopExpression :: TapeExpression
{
    type<TapeExpression>  condition.
    type<TapeExpression>  body.
    
    constructor new(TapeExpression condition, TapeExpression body)
    [
        @condition := condition.
        @body := body.
    ]
    
    saveTo(list<DynamicClosure> list)
    [
        list append(openLoopClosure).
        list append(openExpressionClosure).
        list append(ifClosure).
        condition saveTo(list).
        
        list append(openExpressionClosure).
        body saveTo(list).
        list append(closeClosure).
        
        list append(closeClosure).
        list append(closeClosure).
    ]
}
