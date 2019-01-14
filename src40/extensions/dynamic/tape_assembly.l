import system'collections;
import system'dynamic;
//import system'text.
//import extensions.

singleton message_token;
singleton expr_end_token;
//singleton subexpr_token.
singleton expr_var_token;
//singleton previous_token.
//singleton assign_token.
//singleton if_token.
//singleton code_token.

const int METHOD_PARAM = 1;
//const int FORWARD_PARAM = 2.

sealed class ClassContext
{
    Stack<object> _stack;
    List<object>  _tape;
    
    constructor()
    {
        _stack := new Stack<object>();
        _tape := new List<object>()
    }

    push(object)
    {
        _stack.push:object
    }        
    
    pop() => _stack;
    
    peek() => _stack;        
        
    get int Length() => _tape;
    
//    tape = _tape.
//        
//    trim : pos
//        => _tape.        
//    
//    first => _tape.        
    
    append(object)
    {
        _tape.append:object
    }
    
//    getAt(IntNumber index)
//        = _tape[index].        
//        
//    setAt(IntNumber index, Object object)
//    [
//        _tape[index] := object.
//    ]
}

sealed class ParameterContext
{
    ArrayList _buffer;
    
    constructor()
    {
        _buffer := new ArrayList()
    }
    
    get int Level()
    {
        int len := _buffer.Length;
        
        len := len / 3;
        
        ^ len
    }
    
//    trim(IntNumber level)
//    [
//        int index := level * 3.
//        
//        _buffer trim(index).
//    ]
        
    add(String name, IntNumber type, Object value)
    {
        _buffer.append(name).append(type).append(value)
    }
    
//    resolve(literal name, ref<int> retVal)
//    [
//        int len := _buffer length.
//        int i := len - 3.
//        while (i >= 0)
//        [
//            if (name == _buffer[i])
//            [
//                retVal value := _buffer[i+1].
//
//                ^ _buffer[i+2].
//            ].
//            
//            i := i - 3.
//        ].
//        
//        ^ nil.
//    ]        
}

interface class Strategy
{
//    T<Strategy> preloaded_one []
//    T<Strategy> symbol_one    []
    Strategy expression()    {}
    Strategy singleton_one() {}
    Strategy method_one()    {}
    Strategy code_one()      {}
//    T<Strategy> returning     []
//    T<Strategy> variable_one  []
//    T<Strategy> loop_one      []
//
//    T<Strategy> if_one(ClassContext context)   []
//    T<Strategy> next_one(ClassContext context, ParameterContext parameters) []
        
    begin(ClassContext context, ParameterContext parameters) {}
//    end(ClassContext context, ParameterContext parameters)   []    
//    
//    setReference(ClassContext context, LiteralValue r)       []
//    setConstant(ClassContext context, object o)       []
    setMessageToken(ClassContext context, String m)    {}
//    setLiteral(ClassContext context, LiteralValue l)         []
//    setNumericToken(ClassContext context, LiteralValue l)    []
//    setAssignToken(ClassContext context, LiteralValue t)     []
//    setIdentToken(ClassContext context, LiteralValue m, ParameterContext parameters) []
    setParamToken(ClassContext context, String m, ParameterContext parameters) {}
}

singleton StartOne : Strategy
{
//    T<Strategy> preloaded_one = PreloadedOne.
//    
//    T<Strategy> symbol_one = SymbolOne.
    
    Strategy singleton_one() = Singleton;
}

//singleton SymbolOne :: Strategy
//{
//    T<Strategy> expression = SymbolBody.
//    
//    begin(ClassContext context, ParameterContext parameters)  
//    [
//        context push(context length).    
//    ]
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        var t := context tape.
//        var s := t __className.
//        
//        int index := context pop.
//        int len := context length - index.
//        var tape := ClosureTape new(context tape, index, len).
//        
//        context trim(index).
//        context append(tape).
//    ]    
//}
//
//singleton PreloadedOne :: Strategy
//{
//    begin(ClassContext context, ParameterContext parameters)  
//    [
//        context push(context length).
//    ]
//    
//    T<Strategy> expression = SymbolBody.
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        int index := context pop.
//        int len := context length - index.
//        var tape := ClosureTape new(context tape, index, len).
//        
//        context trim(index).
//        context append(tape).
//        
//        tape().
//    ]
//}

class BaseCode : Strategy
{
    Strategy expression()   = RootExpresson;
//    T<Strategy> returning    = RetExpression.
//    T<Strategy> variable_one = Variable. 
//    T<Strategy> loop_one     = Loop.      
}

singleton Code : BaseCode{}

//singleton SubCode :: BaseCode
//{
//}

singleton Method : Strategy
{
    Strategy code_one() = Code;
    
//    T<Strategy> returning = RetExpression.
    
    setParamToken(ClassContext context, String value, ParameterContext parameters)
    {
        int method_level := context.peek();        
        int level := parameters.Level;
        
        level := level - method_level;
        level := level + 1;
        
        parameters.add(value, METHOD_PARAM, level);
        
        context.append(expr_var_token)
    }
    
    begin(ClassContext context, ParameterContext parameters)  
    {
        context.push(context.Length);
        context.push(parameters.Level)
    }
    
    setMessageToken(ClassContext context, String m) 
    {
        context.append(message_token);
        context.append(m)
    }
    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        int level := context pop.
//        
//        parameters trim(level).
//        
//        int pos := context pop.
//        var m := nil.
//        int counter := -1. // self is included as well
//        int index := pos.
//        
//        // define the method name
//        var p := context getAt(index).
//        while (p equalReference:expr_var_token)
//        [
//            index := index + 1.
//            counter := counter + 1.
//            p := context getAt(index).
//        ].
//
//        if (p equalReference:message_token)
//        [
//            m := context getAt(index + 1).
//            index := index + 2.
//            p := context getAt(index).
//        ].
//                
//        while (p equalReference:expr_var_token)
//        [
//            index := index + 1.
//            counter := counter + 1.
//            p := context getAt(index).
//        ].
//        
//        T<TextBuilder> name := TextBuilder new.
//        name append(m).
//        name append:"[".
//        name append(counter literal).
//        name append:"]".        
//        
//        int len := context length - index.
//        var tape := ClosureTape new(context tape, index, len).
//        
//        context trim(pos).
//        context append(Message new(T<literal>(name))).
//        context append(tape).
//    ]    
}

class BaseClass : Strategy
{
    Strategy method_one() = Method;
        
    begin(ClassContext context, ParameterContext parameters)  
    {
        context.push(context.Length)
    }
    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        var pos := context pop.
//        var len := context length - pos.
//        
//        var class := ClosureClass new(context tape, pos, len).
//        
//        context trim(pos).
//        context append(class).
//    ]
}

singleton Singleton : BaseClass
{
//    end(ClassContext context, ParameterContext parameters)
//    [
//        var pos := context pop.
//        var len := context length - pos.
//        
//        var class := ClosureClass new(context tape, pos, len).
//        
//        context trim(pos).
//        context append(class).
//    ]
}

//singleton Nested :: BaseClass
//{
//    end(ClassContext context, ParameterContext parameters)
//    [
//        var pos := context pop.
//        var len := context length - pos.
//        
//        var class := ClosureClass new(context tape, pos, len).
//        
//        context trim(pos).
//        context push(ConstantClosure new(class)).
//    ]
//}

class BaseExpression : Strategy
{
    begin(ClassContext context, ParameterContext parameters)
    {
        context.push:expr_end_token
    }    
 
//    T<Strategy> singleton_one = Nested.
//          
//    setReference(ClassContext context, literal r) 
//    [
//        context push(ConstantClosure new(Symbol new(r))).
//    ]
//    
//    setConstant(ClassContext context, object c) 
//    [
//        context push(ConstantClosure new(c)).
//    ]
//    
//    setLiteral(ClassContext context, LiteralValue l) 
//    [
//        context push(ConstantClosure new:l).
//    ]
//
//    setNumericToken(ClassContext context, LiteralValue l)
//    [
//        context push(ConstantClosure new(l toInt)).
//    ]
//    
//    setMessageToken(ClassContext context, LiteralValue m) 
//    [
//        context push:m.
//        context push:message_token.
//    ]
//    
//    setIdentToken(ClassContext context, LiteralValue m, ParameterContext parameters) 
//    [
//        int type := 0.
//        object param := parameters resolve(m, &type).
//        if (type == METHOD_PARAM)
//        [
//            context push:param.
//            context push:expr_var_token.            
//        ];
//        if (type == FORWARD_PARAM)
//        [
//            context push(ConstantClosure new(param)).
//        ];
//        [
//            InvalidArgumentException new("Variable cannot be resolved - " + m); raise
//        ].
//    ]
//    
//    setAssignToken(ClassContext context, LiteralValue t)
//    [
//        var token := context pop.
//        if (token equalReference:expr_var_token)
//        [
//            token := context pop.            
//            context push(AssignClosure new(token)).
//        ];
//        [
//            context push:token.
//            context push:assign_token.
//        ]
//    ]
//    
//    
//    T<Strategy> next_one(ClassContext context, ParameterContext parameters) 
//    [
//        self end(context, parameters).
//        self begin(context, parameters).
//        
//        context push:previous_token.
//    ]
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        object token := context pop.
//        T<TextBuilder> buffer := TextBuilder new.
//        int counter := -1.
//        int prealloc := 0.
//        bool messageExpr := false.
//        bool popRequired := false.
//        until(token equalReference:expr_end_token)
//        [
//            if (token equalReference:subexpr_token)
//            [
//                counter := counter + 1.
//                context append(StackClosure new(prealloc + counter)).
//                prealloc += 1.
//            ];
//            if (token equalReference:message_token)
//            [
//                buffer insert(0, "&").
//                buffer insert(0, T<literal>(context pop)).
//                messageExpr := true.
//            ];
//            [
//                if (token equalReference:expr_var_token)
//                [
//                    var index := context pop.
//                    
//                    var next_token := context peek.
//                    if(next_token equalReference:assign_token)
//                    [
//                        token := AssignClosure new(index).
//                    ];
//                    [
//                        token := VarClosure new(index).
//                    ]
//                ];
//                if (token equalReference:previous_token)
//                [
//                    token := StackClosure new(counter + 1).
//                    
//                    popRequired := true.
//                ].
//                context append:token.
//                counter := counter + 1.
//            ].
//            
//            token := context pop.
//        ].
//
//        if (messageExpr)
//        [
//            buffer trim(1).
//            buffer write("[").
//            buffer write(counter).
//            buffer write("]").
//            
//            context append(MessageClosure new(Message new(T<literal>(buffer))))
//        ].
//        
//        if (popRequired)
//        [
//            prealloc += 1.
//        ].
//        while (prealloc > 0)
//        [
//            context append:popPrevClosure.
//            prealloc -= 1.
//        ]
//    ]    
}

//singleton SymbolBody :: BaseExpression
//{
//    T<Strategy> expression = SubExpression.
//}
//
//singleton SubExpression :: BaseExpression
//{
//    T<Strategy> expression = SubExpression.
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        super end(context, parameters).
//        
//        context push:subexpr_token.
//    ]    
//}
    
singleton RootExpresson : BaseExpression
{
//    T<Strategy> expression = SubExpression.
//    
//    T<Strategy> if_one(ClassContext context)
//    [
//        context push:if_token.
//        
//        ^ IfExpression.
//    ]
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        super end(context, parameters).
//        
//        context append:popClosure.
//    ]    
}

//singleton Expresson :: BaseExpression
//{
//    T<Strategy> expression = SubExpression.
//}
//
//singleton RetExpression :: BaseExpression
//{
//    T<Strategy> expression = Expresson.
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        super end(context, parameters).
//        
//        context append:exitClosure.
//    ]
//}
//
//singleton CondExpression :: BaseExpression
//{
//    begin(ClassContext context, ParameterContext parameters)
//    [
//        super begin(context, parameters).
//    ]
//    
//    T<Strategy> code_one = SubCode.
//    
//    T<Strategy> expression = SubExpression.
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        super end(context, parameters).
//        
//        var token := context pop.
//        if (token equalReference:if_token)
//        [
//            context push(context length).
//            context append:nilValue.
////            context append:%"tapeOp.tape_goifnot[]".
//            context push(code_token).
//        ];
//        [
//            context push(token).
//        ]
//    ]    
//}
//
//singleton IfExpression :: BaseExpression
//{
//    T<Strategy> expression = CondExpression.
//    
//    T<Strategy> code_one = SubCode.
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        var token := context pop.
//        
//        int pos := context pop.
//        
//        super end(context, parameters).
//        
//        int offset := context length - pos - 1.
//        
//        context[pos] := JumpIfNotClosure new(offset).
//    ]        
//}
//
//singleton Variable :: Strategy
//{
//    setIdentToken(ClassContext context, LiteralValue value, ParameterContext parameters) 
//    [
//        context append(ConstantClosure new(Object new)). // !! cannot pass nil
//        
//        int method_level := context peek.        
//        int level := parameters level.
//        
//        level := level - method_level.
//        level := level + 1.
//        
//        parameters add(value, METHOD_PARAM, level).        
//    ]    
//}
//
//singleton Loop :: Strategy
//{
//    T<Strategy> expression = LoopExpresson.    
//    
//    begin(ClassContext context, ParameterContext parameters)
//    [
//        context push(context length).
//    ]
//}
//
//singleton LoopExpresson :: BaseExpression
//{
//    T<Strategy> if_one(ClassContext context)
//    [
//        context push:if_token.
//        
//        ^ LoopIfExpression.
//    ]
//}    
//
//singleton LoopIfExpression :: BaseExpression
//{
//    T<Strategy> expression = CondExpression.
//    
//    end(ClassContext context, ParameterContext parameters)
//    [
//        var token := context pop.
//        
//        int pos := context pop.
//        
//        super end(context, parameters).
//        
//        int loop_pos := context pop.
//        
//        context append(GoToClosure new(loop_pos - context length - 1)).
//        
//        int offset := context length - pos - 1.
//        
//        context[pos] := JumpIfNotClosure new(offset).        
//    ]        
//}

public class TapeAssembly
{
    Stack<Strategy>  _states;
    Strategy         _state;

    ClassContext     _context;
    ParameterContext _parameters;
    
    constructor()
    {
        _state := StartOne;
        _states := class Stack<Strategy>.allocate(32);

        _context := new ClassContext();
        _parameters := new ParameterContext()
    }
    
    constructor load(ClosureTape tape)
        <= ()
    {
        tape(self)
    }
    
    open(MessageName state)     
    {
        _states.push:_state;
        _state := mixin state(_state).eval();
        
        _state.begin(_context, _parameters)
    }
    
//    openPreloadedSymbol
//        <= open(%preloaded_one).
//            
//    openSymbol
//        <= open(%symbol_one).
    
    openSingleton()
        <= open(subjconst singleton_one);
    
    openMethod()
        <= open(subjconst method_one);
    
    openCode()
        <= open(subjconst code_one);
    
    openExpression()
        <= open(subjconst expression);
    
//    openReturning
//        <= open(%returning).
//    
//    openVariable
//        <= open(%variable_one).
//    
//    openLoop
//        <= open(%loop_one).
//        
//    newReference : r
//    [
//        _state setReference(_context, r literal).
//    ]
//        
//    newConstant : r
//    [
//        _state setConstant(_context, r).
//    ]
//    
//    newLiteralToken : l
//    [
//        _state setLiteral(_context, l literal).
//    ]
//    
//    newNumericToken : l
//    [
//        _state setNumericToken(_context, l literal).
//    ]
    
    newMessageToken(m)
    {
        _state.setMessageToken(_context, m.Printable)
    }

    newParamToken(p)
    {
        _state.setParamToken(_context, p.Printable, _parameters)
    }            

//    newIdentToken : m
//    [
//        _state setIdentToken(_context, m literal, _parameters).
//    ]
//
//    newAssignToken : t
//    [
//        _state setAssignToken(_context, t literal).
//    ]
//    
//    if
//    [
//        _state := _state if_one(_context).
//    ]
//    
//    include(LiteralValue name,LiteralValue symbolName)
//    [
//        var symbol := Symbol new(symbolName).
//        
//        _parameters add(name, FORWARD_PARAM, symbol).
//    ]
//    
//    nextOperation
//    [
//        _state next_one(_context, _parameters).
//    ]
//    
//    close
//    [
//        _state end(_context, _parameters).
//        
//        _state := _states pop.
//    ]
//    
//    eval
//    [
//        var o := _context first.
//        if (o instanceOf:ClosureTape)
//        [
//            ^ o().
//        ];
//        [
//            ^ o
//        ]        
//    ]
//    
//    first => _context.
}

//public singleton openSymbolClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t openSymbol.
//}
//
//public singleton openPreloadedSymbolClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t openSymbol.
//}

public singleton openSingletonClosure : DynamicClosure
{
    closure(object t, params object[] args)
        = t.openSingleton();
}

public singleton openMethodClosure : DynamicClosure
{
    closure(object t, params object[] args)
        = t.openMethod();
}

public singleton openCodeClosure : DynamicClosure
{
    closure(object t, params object[] args)
        = t.openCode();
}

public singleton openExpressionClosure : DynamicClosure
{
    closure(object t, params object[] args)
        = t.openExpression();
}

//public singleton openReturningClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t openReturning.
//}
//
//public singleton openVariableClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t openVariable.
//}
//
//public singleton openLoopClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t openLoop.
//}

public singleton closeClosure : DynamicClosure
{
    closure(object t, params object[] args)
        = t.close();
}

//public singleton newAssignClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t newAssignToken:"=".
//}
//
//public assignClosureFactory =
//{
//    new : x = newAssignClosure.
//}.
//
//public singleton ifClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t if.
//}
//
//public singleton nextClosure :: DynamicClosure
//{
//    generic closure(object t, V<object> args)
//        = t nextOperation.
//}
//
//public class includeClosure :: DynamicClosure
//{
//    literal forward.
//    literal reference.
//    
//    constructor new(literal forward, literal reference)
//    [
//        @forward:=forward.
//        @reference:=reference.
//    ]
//    
//    generic closure(object t, V<object> args)
//        = t include(forward,reference).
//}

public abstract class DynamicClosureWithParam : DynamicClosure
{
    object param;
}

public class newReferenceClosure : DynamicClosureWithParam
{    
    constructor(param)
    {
        this param := param
    }
    
    closure(object t, params object[] args)
        = t.newReference:param;
}

public class newMessageClosure : DynamicClosureWithParam
{    
    constructor(param)
    {
        this param := param
    }
    
    closure(object t, params object[] args)
        = t.newMessageToken:param;    
}

//public class newConstantClosure :: DynamicClosureWithParam
//{    
//    constructor new : param
//    [
//        @param := param
//    ]
//    
//    generic closure(object t, V<object> args)
//        = t newConstant:param.    
//}
//
//public class newLiteralClosure :: DynamicClosureWithParam
//{    
//    constructor new : param
//    [
//        @param := param
//    ]
//    
//    generic closure(object t, V<object> args)
//        = t newLiteralToken:param.    
//}
//
//public class newNumericClosure :: DynamicClosureWithParam
//{    
//    constructor new : param
//    [
//        @param := param
//    ]
//    
//    generic closure(object t, V<object> args)
//        = t newNumericToken:param.    
//}

public class newParamTokenClosure : DynamicClosureWithParam
{    
    constructor(param)
    {
        this param := param
    }
    
    closure(object t, params object[] args)
        = t.newParamToken:param;
}

public class newIdentifierClosure : DynamicClosureWithParam
{    
    constructor(param)
    {
        this param := param
    }
    
    closure(object t, params object[] args)
        = t.newIdentToken:param;    
}

//singleton popClosure :: DynamicClosure
//{
//    generic closure(object elementToRemove, object nextElement, V<object> args)
//        = nextElement.    
//}
//
//singleton popPrevClosure :: DynamicClosure
//{
//    generic closure(object prevElement, object elementToRemove, V<object> args)
//        = prevElement.    
//}
//
//singleton exitClosure :: DynamicClosure
//{
//    generic closure(V<object> args)
//        => system'core_routines'__exit_closure.
//}
//
//struct VarClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(V<object> args)
//    [
//        int len := args length.
//        int n := len.
//        n := n - index.
//       
//        ^ args[n]
//    ]
//}
//
//struct StackClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(V<object> args)
//    [
//        ^ args[index]
//    ]
//}
//
//struct AssignClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object target, V<object> args)
//    [
//        int len := args length.
//        int n := len.
//        n := n - index.
//       
//        args[n] := target.
//       
//        ^ target
//    ]
//}
//
//struct JumpIfClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(V<object> args)
//        => system'core_routines'__jumpif_closure.    
//}
//
//struct JumpIfNotClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(V<object> args)
//        => system'core_routines'__jumpifnot_closure.    
//}
//        
//struct GoToClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(V<object> args)
//        => system'core_routines'__jump_closure.    
//}