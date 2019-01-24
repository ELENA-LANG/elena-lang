import system'collections;
import system'dynamic;

public interface TapeExpression
{
    constructor Constant(object o)
        = new TapeConstantExpression(o);
        
    constructor MessageCall(TapeExpression dest, string message, params TapeExpression[] arguments)
        = TapeMessageCallExpression.new(dest,message,params arguments);
        
    constructor MessageCall(TapeExpression dest, string message)
        = new TapeMessageCallExpression(dest,message);
        
//    constructor Symbol(TapeExpression expression)
//        = TapeSymbolExpression new(expression).
        
    constructor Singleton(params TapeExpression[] methods)
        = TapeSingleton.new(params methods);
        
    constructor Method(string message, TapeCode body, params TapeExpression[] parameters)
        = /*new */TapeMethod.new(message, body, params parameters);
        
    constructor Method(string message, TapeCode body)
        = /*new */TapeMethod.new(message, body);
        
    constructor Code(params TapeExpression[] statements)
        = TapeCode.new(params statements);
        
    constructor Parameter(string name)
        = new TapeParameterExpression(name);
        
    constructor Variable(string name)
        = new TapeVariableExpression(name);
        
    constructor Declaring(string name)
        = new TapeNewVariableExpression(name);
        
    constructor Assigning(string name, TapeExpression value)
        = new TapeAssigningExpression(name, value);
        
    constructor Loop(TapeExpression condition, TapeExpression body)
        = new TapeLoopExpression(condition, body);
        
//    constructor Returning(TapeExpression value)
//        = TapeReturningExpression new(value).
//        
//    constructor If(TapeExpression condition, TapeExpression truePart)
//        = TapeIfExpression new(condition, truePart).
        
    abstract saveTo(List<DynamicClosure> list) {}
    
    compiled()
    {
        auto list := new List<DynamicClosure>(8);
        
        self.saveTo(list);
        
        ClosureTape tape := ClosureTape.new(list);
        
        ^ (){ ^ TapeAssembly.load(tape).eval() };
    }
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

public class TapeSingleton : TapeExpression
{
    TapeExpression[] methods;
    
    constructor new(params TapeExpression[] methods)
        <= new(cast TapeExpression[](methods));
    
    constructor new(TapeExpression[] methods)
        = methods;
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openSingletonClosure);
        
        int len := methods.Length;
        for (int i := 0, i < len, i += 1)
        {
            methods[i].saveTo(list)
        };
        
        list.append(closeClosure)
    }
}

public class TapeCode : TapeExpression
{
    TapeExpression[] statements;

    constructor new(TapeExpression[] statements)
        = statements;
            
    constructor new(params TapeExpression[] statements)
        <= new(cast TapeExpression[](statements));
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openCodeClosure);
        
        int len := statements.Length;
        for (int i := 0, i < len, i += 1)
        {
            statements[i].saveTo(list)
        };
        
        list.append(closeClosure)
    }
}

public class TapeMethod : TapeExpression
{
    string           message;
    TapeCode         body;
    TapeExpression[] parameters;
    
    constructor new(string message, TapeCode body, params TapeExpression[] parameters)
    {
        this message := message;
        this body := body;
        this parameters := parameters;
    }
    
    constructor new(string message, TapeCode body)
    {
        this message := message;
        this body := body;
        this parameters := nil;
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openMethodClosure);
        list.append(new newMessageClosure(message));
        
        if (nil != parameters)
        {
            int len := parameters.Length;
            for (int i := 0, i < len, i += 1)
            {
                parameters[i].saveTo(list)
            }
        };

        list.append(new newParamTokenClosure("self"));
                
        body.saveTo(list);
        
        list.append(closeClosure)
    }    
}

public class TapeConstantExpression : TapeExpression
{
    object object;
    
    constructor(object o)
    {
        this object := o
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(new newConstantClosure(object))
    }
}

public class TapeParameterExpression : TapeExpression
{
    string name;
    
    constructor(string name)
    {
        this name := name
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(new newParamTokenClosure(name))
    }
}

public class TapeVariableExpression : TapeExpression
{
    string name;
    
    constructor(string name)
    {
        this name := name
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(new newIdentifierClosure(name))
    }
}

public class TapeNewVariableExpression : TapeExpression
{
    string name;
    
    constructor(string name)
    {
        this name := name
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openVariableClosure);
        list.append(new newIdentifierClosure(name));
        list.append(closeClosure)
    }
}

public class TapeMessageCallExpression : TapeExpression
{
    string           message;
    TapeExpression   dest;
    TapeExpression[] arguments;
    
    constructor new(TapeExpression dest, string message, params TapeExpression[] arguments)
    {
        this message := message;
        this dest := dest;
        this arguments := arguments;
    }
    
    constructor(TapeExpression dest, string message)
    {
        this message := message;
        this dest := dest;
        this arguments := nil
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openExpressionClosure);
        
        dest.saveTo(list);
        
        list.append(new newMessageClosure(message));
        
        if (nil != arguments)
        {
            int len := arguments.Length;
            for (int i := 0, i < len, i += 1)
            {
                arguments[i].saveTo(list)
            }
        };
        
        list.append(closeClosure)
    }
}

public class TapeAssigningExpression : TapeExpression
{
    string          name;
    TapeExpression  value;
    
    constructor(string name, TapeExpression value)
    {
        this name := name;
        this value := value;
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openExpressionClosure);
        list.append(new newIdentifierClosure(name));
        list.append(newAssignClosure);
        
        value.saveTo(list);
        
        list.append(closeClosure)
    }
}

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

public class TapeLoopExpression : TapeExpression
{
    TapeExpression condition;
    TapeExpression body;
    
    constructor(TapeExpression condition, TapeExpression body)
    {
        this condition := condition;
        this body := body;
    }
    
    saveTo(List<DynamicClosure> list)
    {
        list.append(openLoopClosure);
        list.append(openExpressionClosure);
        list.append(ifClosure);
        condition.saveTo(list);
        
        list.append(openExpressionClosure);
        body.saveTo(list);
        list.append(closeClosure);
        
        list.append(closeClosure);
        list.append(closeClosure)
    }
}

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
