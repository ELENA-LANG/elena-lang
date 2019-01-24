import system'collections;
import system'dynamic;

public interface TapeExpression
{
//    constructor Constant(object o)
//        = TapeConstantExpression new(o).
//        
//    generic constructor MessageCall(TapeExpression dest, literal message, V<TapeExpression> arguments)
//        = TapeMessageCallExpression new(dest,message,arguments).
//        
//    constructor MessageCall(TapeExpression dest, literal message)
//        = TapeMessageCallExpression new(dest,message).
//        
//    constructor Symbol(TapeExpression expression)
//        = TapeSymbolExpression new(expression).
//        
//    generic constructor Singleton(V<TapeExpression> methods)
//        = TapeSingleton new(methods).
//        
//    generic constructor Method(literal message, TapeCode body, V<TapeExpression> parameters)
//        = TapeMethod new(message, body, parameters).
//        
//    constructor Method(literal message, TapeCode body)
//        = TapeMethod new(message, body).
//        
//    generic constructor Code(V<TapeExpression> statements)
//        = TapeCode new(statements).
//        
//    constructor Parameter(literal name)
//        = TapeParameterExpression new(name).
//        
//    constructor Variable(literal name)
//        = TapeVariableExpression new(name).
        
    constructor Declaring(string name)
        = new TapeNewVariableExpression(name);
        
//    constructor Assigning(literal name, TapeExpression value)
//        = TapeAssigningExpression new(name, value).
//        
//    constructor Loop(TapeExpression condition, TapeExpression body)
//        = TapeLoopExpression new(condition, body).
//        
//    constructor Returning(TapeExpression value)
//        = TapeReturningExpression new(value).
//        
//    constructor If(TapeExpression condition, TapeExpression truePart)
//        = TapeIfExpression new(condition, truePart).
        
    //abstract saveTo(List<DynamicClosure> list) {}
    
//    compiled
//    [
//        List<DynamicClosure> list := List<DynamicClosure>(8).
//        
//        self saveTo(list).
//        
//        T<ClosureTape> tape := ClosureTape new:list.
//        
//        ^ [ TapeAssembly new(tape); eval ]
//    ]
}

//public class TapeSymbolExpression :: TapeExpression
//{
//    T<TapeExpression> expr.
//    
//    constructor new(object expr)
//    [
//        @expr := expr.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openSymbolClosure).
//        expr saveTo(list).
//        list append(closeClosure).
//    ]
//}
//
//public class TapeSingleton :: TapeExpression
//{
//    List<TapeExpression> methods.
//    
//    generic constructor new(V<TapeExpression> methods)
//    [
//        @methods := methods.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openSingletonClosure).
//        
//        int len := methods length.
//        int i := 0.
//        while(i < len)
//        [
//            methods[i] saveTo(list).
//            
//            i += 1.            
//        ].
//        
//        list append(closeClosure).
//    ]
//}
//
//public class TapeMethod :: TapeExpression
//{
//    literal               message.
//    T<TapeCode>        body.
//    Array<TapeExpression> parameters.
//    
//    generic constructor new(literal message, TapeCode body, V<TapeExpression> parameters)
//    [
//        @message := message.
//        @body := body.
//        @parameters := parameters.
//    ]
//    
//    constructor new(literal message, TapeCode body)
//    [
//        @message := message.
//        @body := body.
//        @parameters := nil.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openMethodClosure).
//        list append(newMessageClosure new:message).
//        
//        if (nil != parameters)
//        [
//            int len := parameters length.
//            int i := 0.
//            while(i < len)
//            [
//                parameters[i] saveTo(list).
//                
//                i += 1.            
//            ].            
//        ].
//
//        list append(newParamTokenClosure new:"self").
//                
//        body saveTo(list).
//        
//        list append(closeClosure).
//    ]    
//}
//
//public class TapeCode :: TapeExpression
//{
//    List<TapeExpression> statements.
//    
//    generic constructor new(V<TapeExpression> statements)
//    [
//        @statements := statements.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openCodeClosure).
//        
//        int len := statements length.
//        int i := 0.
//        while(i < len)
//        [
//            statements[i] saveTo(list).
//            
//            i += 1.            
//        ].
//        
//        list append(closeClosure).
//    ]
//}
//
//public class TapeConstantExpression :: TapeExpression
//{
//    object object.
//    
//    constructor new(object o)
//    [
//        @object := o.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(newConstantClosure new(object))
//    ]
//}
//
//public class TapeParameterExpression :: TapeExpression
//{
//    literal name.
//    
//    constructor new(literal name)
//    [
//        @name := name.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(newParamTokenClosure new(name))
//    ]
//}

public class TapeVariableExpression : TapeExpression
{
    string name;
    
    constructor(string name)
    {
        this name := name
    }
    
//    saveTo(List<DynamicClosure> list)
//    {
//        list.append(new newIdentifierClosure(name))
//    }
}

public class TapeNewVariableExpression : TapeExpression
{
    string name;
    
    constructor(string name)
    {
        this name := name
    }
    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openVariableClosure).
//        list append(newIdentifierClosure new(name)).
//        list append(closeClosure).
//    ]
}

//public class TapeMessageCallExpression :: TapeExpression
//{
//    literal               message.
//    T<TapeExpression>     dest.
//    Array<TapeExpression> arguments.
//    
//    generic constructor new(TapeExpression dest, literal message, V<TapeExpression> arguments)
//    [
//        @message := message.
//        @dest := dest.
//        @arguments := arguments.
//    ]
//    
//    constructor new(TapeExpression dest, literal message)
//    [
//        @message := message.
//        @dest := dest.
//        @arguments := nil.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openExpressionClosure).
//        
//        dest saveTo(list).
//        
//        list append(newMessageClosure new:message).
//        
//        if (nil != arguments)
//        [
//            int len := arguments length.
//            int i := 0.
//            while(i < len)
//            [
//                arguments[i] saveTo(list).
//                
//                i += 1.            
//            ]
//        ].
//        
//        list append(closeClosure).
//    ]
//}
//
//public class TapeAssigningExpression :: TapeExpression
//{
//    literal            name.
//    T<TapeExpression>  value.
//    
//    constructor new(literal name, TapeExpression value)
//    [
//        @name := name.
//        @value := value.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openExpressionClosure).
//        list append(newIdentifierClosure new(name)).
//        list append(newAssignClosure).
//        
//        value saveTo(list).
//        
//        list append(closeClosure).
//    ]
//}
//
//public class TapeReturningExpression :: TapeExpression
//{
//    T<TapeExpression>  value.
//    
//    constructor new(TapeExpression value)
//    [
//        @value := value.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openReturningClosure).
//        value saveTo(list).        
//        list append(closeClosure).
//    ]
//}
//
//public class TapeLoopExpression :: TapeExpression
//{
//    T<TapeExpression>  condition.
//    T<TapeExpression>  body.
//    
//    constructor new(TapeExpression condition, TapeExpression body)
//    [
//        @condition := condition.
//        @body := body.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openLoopClosure).
//        list append(openExpressionClosure).
//        list append(ifClosure).
//        condition saveTo(list).
//        
//        list append(openExpressionClosure).
//        body saveTo(list).
//        list append(closeClosure).
//        
//        list append(closeClosure).
//        list append(closeClosure).
//    ]
//}
//
//public class TapeIfExpression :: TapeExpression
//{
//    T<TapeExpression>  condition.
//    T<TapeExpression>  truePart.
//    
//    constructor new(TapeExpression condition, TapeExpression truePart)
//    [
//        @condition := condition.
//        @truePart := truePart.
//    ]
//    
//    saveTo(List<DynamicClosure> list)
//    [
//        list append(openExpressionClosure).
//        list append(extensions'dynamic'ifClosure).
//        
//        condition saveTo(list).        
//        truePart saveTo(list).
//        
//        list append(closeClosure).
//    ]
//}
