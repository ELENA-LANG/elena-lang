//import system'collections.
import system'dynamic.
//import system'text.
//import extensions.
//
//singleton $message_token.
//singleton $expr_end_token.
//singleton $subexpr_token.
//singleton $expr_var_token.
//singleton $previous_token.
//singleton $assign_token.
//singleton $if_token.
//singleton $code_token.
//
//const int $METHOD_PARAM = 1.
//const int $FORWARD_PARAM = 2.
//
//sealed class $ClassContext
//{
//    stack<object> _stack.
//    list<object>  _tape.
//    
//    constructor new
//    [
//        _stack := stack<object>().
//        _tape := list<object>().
//    ]
//
//    push : object
//    [
//        _stack push:object.
//    ]        
//    
//    pop => _stack.
//    
//    peek => _stack.        
//        
//    int length => _tape.
//    
//    tape = _tape.
//        
//    trim : pos
//        => _tape.        
//    
//    first => _tape.        
//    
//    append : object
//    [
//        _tape append:object.
//    ]    
//    
//    stacksafe getAt(IntNumber index)
//        = _tape[index].        
//        
//    stacksafe setAt(IntNumber index, Object object)
//    [
//        _tape[index] := object.
//    ]
//}
//
//sealed class $ParameterContext
//{
//    type<ArrayList> _buffer.
//    
//    constructor new
//    [
//        _buffer := ArrayList new.
//    ]
//    
//    int level
//    [
//        int len := _buffer length.
//        
//        len := len / 3.
//        
//        ^ len.
//    ]
//    
//    trim(IntNumber level)
//    [
//        int index := level * 3.
//        
//        _buffer trim int(index).
//    ]
//        
//    add(LiteralValue name, IntNumber type, Object value)
//    [
//        _buffer append(name); append(type); append(value)
//    ]
//    
//    resolve literal:name vint:type
//    [
//        int len := _buffer length.
//        int i := len - 3.
//        while (i >= 0)
//        [
//            if (name == _buffer[i])
//            [
//                type set int(_buffer[i+1]).
//
//                ^ _buffer[i+2].
//            ].
//            
//            i := i - 3.
//        ].
//        
//        ^ $nil.
//    ]        
//}
//
//limited class $Strategy
//{
//    type<$Strategy> preloaded_one []
//    type<$Strategy> symbol_one    []
//    type<$Strategy> expression    []
//    type<$Strategy> singleton_one []
//    type<$Strategy> method_one    []
//    type<$Strategy> code_one      []
//    type<$Strategy> returning     []
//    type<$Strategy> variable_one  []
//    type<$Strategy> loop_one      []
//
//    type<$Strategy> if_one($ClassContext context)   []
//    type<$Strategy> next_one($ClassContext context, $ParameterContext parameters) []
//        
//    begin($ClassContext context, $ParameterContext parameters) []
//    end($ClassContext context, $ParameterContext parameters)   []    
//    
//    setReference($ClassContext context, LiteralValue r)       []
//    setConstant($ClassContext context, object o)       []
//    setMessageToken($ClassContext context, LiteralValue m)    []
//    setLiteral($ClassContext context, LiteralValue l)         []
//    setNumericToken($ClassContext context, LiteralValue l)    []
//    setAssignToken($ClassContext context, LiteralValue t)     []
//    setIdentToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
//    setParamToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
//}
//
//singleton $StartOne :: $Strategy
//{
//    type<$Strategy> preloaded_one = $PreloadedOne.
//    
//    type<$Strategy> symbol_one = $SymbolOne.
//    
//    type<$Strategy> singleton_one = $Singleton.
//}
//
//singleton $SymbolOne :: $Strategy
//{
//    type<$Strategy> expression = $SymbolBody.
//    
//    begin($ClassContext context, $ParameterContext parameters)  
//    [
//        context push(context length).    
//    ]
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        int index := context pop.
//        int len := context length - index.
//        var tape := ClosureTape new(context tape, index, len).
//        
//        context trim(index).
//        context append(tape).
//    ]    
//}
//
//singleton $PreloadedOne :: $Strategy
//{
//    begin($ClassContext context, $ParameterContext parameters)  
//    [
//        context push(context length).
//    ]
//    
//    type<$Strategy> expression = $SymbolBody.
//    
//    end($ClassContext context, $ParameterContext parameters)
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
//
//class $BaseCode :: $Strategy
//{
//    type<$Strategy> expression   = $RootExpresson.
//    type<$Strategy> returning    = $RetExpression.
//    type<$Strategy> variable_one = $Variable. 
//    type<$Strategy> loop_one     = $Loop.      
//}
//
//singleton $Code :: $BaseCode.
//
//singleton $SubCode :: $BaseCode
//{
//}
//
//singleton $Method :: $Strategy
//{
//    type<$Strategy> code_one = $Code.
//    
//    type<$Strategy> returning = $RetExpression.
//    
//    setParamToken($ClassContext context, LiteralValue value, $ParameterContext parameters)
//    [
//        int method_level := context peek.        
//        int level := parameters level.
//        
//        level := level - method_level.
//        level := level + 1.
//        
//        parameters add(value, $METHOD_PARAM, level).
//        
//        context append($expr_var_token).
//    ]
//    
//    begin($ClassContext context, $ParameterContext parameters)  
//    [
//        context push(context length).
//        context push(parameters level).
//    ]
//    
//    setMessageToken($ClassContext context, LiteralValue m) 
//    [
//        context append($message_token).
//        context append(m).
//    ]
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        int level := context pop.
//        
//        parameters trim(level).
//        
//        int pos := context pop.
//        var m := $nil.
//        int counter := -1. // self is included as well
//        int index := pos.
//        
//        // define the method name
//        var p := context getAt(index).
//        while (p equal reference:$expr_var_token)
//        [
//            index := index + 1.
//            counter := counter + 1.
//            p := context getAt(index).
//        ].
//
//        if (p equal reference:$message_token)
//        [
//            m := context getAt(index + 1).
//            index := index + 2.
//            p := context getAt(index).
//        ].
//                
//        while (p equal reference:$expr_var_token)
//        [
//            index := index + 1.
//            counter := counter + 1.
//            p := context getAt(index).
//        ].
//        
//        textbuffer name := TextBuffer new.
//        name append(m).
//        name append:"[".
//        name append(counter literal).
//        name append:"]".        
//        
//        int len := context length - index.
//        var tape := ClosureTape new(context tape, index, len).
//        
//        context trim(pos).
//        context append(Message new literal:name).
//        context append(tape).
//    ]    
//}
//
//class $BaseClass :: $Strategy
//{
//    type<$Strategy> method_one = $Method.
//        
//    begin($ClassContext context, $ParameterContext parameters)  
//    [
//        context push(context length).
//    ]
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        var pos := context pop.
//        var len := context length - pos.
//        
//        var class := ClosureClass new(context tape, pos, len).
//        
//        context trim(pos).
//        context append(class).
//    ]
//}
//
//
//singleton $Singleton :: $BaseClass
//{
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        var pos := context pop.
//        var len := context length - pos.
//        
//        var class := ClosureClass new(context tape, pos, len).
//        
//        context trim(pos).
//        context append(class).
//    ]
//}
//
//singleton $Nested :: $BaseClass
//{
//    end($ClassContext context, $ParameterContext parameters)
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
//
//class $BaseExpression :: $Strategy
//{
//    begin($ClassContext context, $ParameterContext parameters)
//    [
//        context push:$expr_end_token.
//    ]    
// 
//    type<$Strategy> singleton_one = $Nested.
//          
//    setReference($ClassContext context, LiteralValue r) 
//    [
//        context push(ConstantClosure new(Symbol new literal:r)).
//    ]
//    
//    setConstant($ClassContext context, object c) 
//    [
//        context push(ConstantClosure new(c)).
//    ]
//    
//    setLiteral($ClassContext context, LiteralValue l) 
//    [
//        context push(ConstantClosure new:l).
//    ]
//
//    setNumericToken($ClassContext context, LiteralValue l)
//    [
//        context push(ConstantClosure new(l toInt)).
//    ]
//    
//    setMessageToken($ClassContext context, LiteralValue m) 
//    [
//        context push:m.
//        context push:$message_token.
//    ]
//    
//    setIdentToken($ClassContext context, LiteralValue m, $ParameterContext parameters) 
//    [
//        int type := 0.
//        object param := parameters resolve literal:m vint:type.
//        if (type == $METHOD_PARAM)
//        [
//            context push:param.
//            context push:$expr_var_token.            
//        ];
//        if (type == $FORWARD_PARAM)
//        [
//            context push(ConstantClosure new(param)).
//        ];
//        [
//            InvalidArgumentException new("Variable cannot be resolved - " + m); raise
//        ].
//    ]
//    
//    setAssignToken($ClassContext context, LiteralValue t)
//    [
//        var token := context pop.
//        if (token equal reference:$expr_var_token)
//        [
//            token := context pop.            
//            context push($AssignClosure new(token)).
//        ];
//        [
//            context push:token.
//            context push:$assign_token.
//        ]
//    ]
//    
//    
//    type<$Strategy> next_one($ClassContext context, $ParameterContext parameters) 
//    [
//        $self end(context, parameters).
//        $self begin(context, parameters).
//        
//        context push:$previous_token.
//    ]
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        object token := context pop.
//        textbuffer buffer := TextBuffer new.
//        int counter := -1.
//        int prealloc := 0.
//        bool messageExpr := false.
//        bool popRequired := false.
//        until(token equal reference:$expr_end_token)
//        [
//            if (token equal reference:$subexpr_token)
//            [
//                counter := counter + 1.
//                context append($StackClosure new(prealloc + counter)).
//                prealloc += 1.
//            ];
//            if (token equal reference:$message_token)
//            [
//                buffer insert int:0 literal:"&".
//                buffer insert int:0 literal(context pop).
//                messageExpr := true.
//            ];
//            [
//                if (token equal reference:$expr_var_token)
//                [
//                    var index := context pop.
//                    
//                    var next_token := context peek.
//                    if(next_token equal reference:$assign_token)
//                    [
//                        token := $AssignClosure new(index).
//                    ];
//                    [
//                        token := $VarClosure new(index).
//                    ]
//                ];
//                if (token equal reference:$previous_token)
//                [
//                    token := $StackClosure new(counter + 1).
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
//            buffer trim int:1.
//            buffer write literal:"[".
//            buffer write int:counter.
//            buffer write literal:"]".
//            
//            context append(MessageClosure new(Message new literal:buffer))
//        ].
//        
//        if (popRequired)
//        [
//            prealloc += 1.
//        ].
//        while (prealloc > 0)
//        [
//            context append:$popPrevClosure.
//            prealloc -= 1.
//        ]
//    ]    
//}
//
//singleton $SymbolBody :: $BaseExpression
//{
//    type<$Strategy> expression = $SubExpression.
//}
//
//singleton $SubExpression :: $BaseExpression
//{
//    type<$Strategy> expression = $SubExpression.
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        $super end(context, parameters).
//        
//        context push:$subexpr_token.
//    ]    
//}
//    
//singleton $RootExpresson :: $BaseExpression
//{
//    type<$Strategy> expression = $SubExpression.
//    
//    type<$Strategy> if_one($ClassContext context)
//    [
//        context push:$if_token.
//        
//        ^ $IfExpression.
//    ]
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        $super end(context, parameters).
//        
//        context append:$popClosure.
//    ]    
//}
//
//singleton $Expresson :: $BaseExpression
//{
//    type<$Strategy> expression = $SubExpression.
//}
//
//singleton $RetExpression :: $BaseExpression
//{
//    type<$Strategy> expression = $Expresson.
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        $super end(context, parameters).
//        
//        context append:$exitClosure.
//    ]
//}
//
//singleton $CondExpression :: $BaseExpression
//{
//    begin($ClassContext context, $ParameterContext parameters)
//    [
//        $super begin(context, parameters).
//    ]
//    
//    type<$Strategy> code_one = $SubCode.
//    
//    type<$Strategy> expression = $SubExpression.
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        $super end(context, parameters).
//        
//        var token := context pop.
//        if (token equal reference:$if_token)
//        [
//            context push(context length).
//            context append:nil.
////            context append:%"tapeOp.tape_goifnot[]".
//            context push($code_token).
//        ];
//        [
//            context push(token).
//        ]
//    ]    
//}
//
//singleton $IfExpression :: $BaseExpression
//{
//    type<$Strategy> expression = $CondExpression.
//    
//    type<$Strategy> code_one = $SubCode.
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        var token := context pop.
//        
//        int pos := context pop.
//        
//        $super end(context, parameters).
//        
//        int offset := context length - pos - 1.
//        
//        context[pos] := $JumpIfNotClosure new(offset).
//    ]        
//}
//
//singleton $Variable :: $Strategy
//{
//    setIdentToken($ClassContext context, LiteralValue value, $ParameterContext parameters) 
//    [
//        context append(ConstantClosure new(Object new)). // !! cannot pass nil
//        
//        int method_level := context peek.        
//        int level := parameters level.
//        
//        level := level - method_level.
//        level := level + 1.
//        
//        parameters add(value, $METHOD_PARAM, level).        
//    ]    
//}
//
//singleton $Loop :: $Strategy
//{
//    type<$Strategy> expression = $LoopExpresson.    
//    
//    begin($ClassContext context, $ParameterContext parameters)
//    [
//        context push(context length).
//    ]
//}
//
//singleton $LoopExpresson :: $BaseExpression
//{
//    type<$Strategy> if_one($ClassContext context)
//    [
//        context push:$if_token.
//        
//        ^ $LoopIfExpression.
//    ]
//}    
//
//singleton $LoopIfExpression :: $BaseExpression
//{
//    type<$Strategy> expression = $CondExpression.
//    
//    end($ClassContext context, $ParameterContext parameters)
//    [
//        var token := context pop.
//        
//        int pos := context pop.
//        
//        $super end(context, parameters).
//        
//        int loop_pos := context pop.
//        
//        context append($GoToClosure new(loop_pos - context length - 1)).
//        
//        int offset := context length - pos - 1.
//        
//        context[pos] := $JumpIfNotClosure new(offset).        
//    ]        
//}

//public class TapeAssembly
//{
//    stack<$Strategy>        _states.
//    T<$Strategy>         _state.
//
//    T<$ClassContext>     _context.
//    T<$ParameterContext> _parameters.
//    
//    constructor new
//    [
//        _state := $StartOne.
//        _states := stack<$Strategy>(32).
//        
//        _context := $ClassContext new.
//        _parameters := $ParameterContext new.
//    ]
//    
//    constructor new(ClosureTape tape)
//        <= new;
//    [
//        tape($self).
//    ]
//    
//    open subject:state
//    [
//        _states push:_state.
//        _state := _state~state get.
//        
//        _state begin(_context, _parameters).
//    ]
//    
//    openPreloadedSymbol
//        <= open(%preloaded_one).
//            
//    openSymbol
//        <= open(%symbol_one).
//    
//    openSingleton
//        <= open(%singleton_one).
//    
//    openMethod
//        <= open(%method_one).
//    
//    openCode
//        <= open(%code_one).
//    
//    openExpression
//        <= open(%expression).
//    
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
//    
//    newMessageToken : m
//    [
//        _state setMessageToken(_context, m literal).
//    ]
//
//    newParamToken : p
//    [
//        _state setParamToken(_context, p literal, _parameters).
//    ]            
//
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
//        _parameters add(name, $FORWARD_PARAM, symbol).
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
//}

public singleton openSymbolClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openSymbol.
}

public singleton openPreloadedSymbolClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openSymbol.
}

public singleton openSingletonClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openSingleton.
}

public singleton openMethodClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openMethod.
}

public singleton openCodeClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openCode.
}

public singleton openExpressionClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openExpression.
}

public singleton openReturningClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openReturning.
}

public singleton openVariableClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openVariable.
}

public singleton openLoopClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t openLoop.
}

public singleton closeClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t close.
}

public singleton newAssignClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t newAssignToken:"=".
}

public assignClosureFactory =
{
    new : x = newAssignClosure.
}.

public singleton ifClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t if.
}

public singleton nextClosure :: DynamicClosure
{
    generic closure eval(object t, V<object> args)
        = t nextOperation.
}

public class includeClosure :: DynamicClosure
{
    literal forward.
    literal reference.
    
    constructor new(literal forward, literal reference)
    [
        @forward:=forward.
        @reference:=reference.
    ]
    
    generic closure eval(object t, V<object> args)
        = t include(forward,reference).
}

public abstract class DynamicClosureWithParam :: DynamicClosure
{
    object param.
    
    constructor new : param
    [
        @param := param
    ]
}

public class newReferenceClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newReference:param.    
}

public class newMessageClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newMessageToken:param.    
}

public class newConstantClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newConstant:param.    
}

public class newLiteralClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newLiteralToken:param.    
}

public class newNumericClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newNumericToken:param.    
}

public class newParamTokenClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newParamToken:param.    
}

public class newIdentifierClosure :: DynamicClosureWithParam
{    
    generic closure eval(object t, V<object> args)
        = t newIdentToken:param.    
}

//singleton $popClosure :: DynamicClosure
//{
//    generic closure(object elementToRemove, object nextElement, object<> args)
//        = nextElement.    
//}
//
//singleton $popPrevClosure :: DynamicClosure
//{
//    generic closure(object prevElement, object elementToRemove, object<> args)
//        = prevElement.    
//}

//singleton $exitClosure :: DynamicClosure
//{
//    generic closure(object<> args)
//        => system'core_routines'__exit_closure.
//}
//
//struct $VarClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object<> args)
//    [
//        int len := args length.
//        int n := len.
//        n := n - index.
//       
//        ^ args[n]
//    ]
//}
//
//struct $StackClosure :: DynamicClosure
//{
//    int index.
//    
//    stacksafe constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object<> args)
//    [
//        ^ args[index]
//    ]
//}
//
//struct $AssignClosure :: DynamicClosure
//{
//    int index.
//    
//    stacksafe constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object target, object<> args)
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
//struct $JumpIfClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object<> args)
//        => system'core_routines'__jumpif_closure.    
//}
//
//struct $JumpIfNotClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object<> args)
//        => system'core_routines'__jumpifnot_closure.    
//}
//        
//struct $GoToClosure :: DynamicClosure
//{
//    int index.
//    
//    constructor new(int index)
//    [
//        @index := index.
//    ]
//    
//    generic closure(object<> args)
//        => system'core_routines'__jump_closure.    
//}