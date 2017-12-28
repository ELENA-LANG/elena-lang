import system'collections.
import system'dynamic.
import system'text.

singleton $message_token.
singleton $expr_end_token.
singleton $subexpr_token.
singleton $expr_var_token.

const int $METHOD_PARAM = 1.

sealed class $ClassContext
{
    stack<object> _stack.
    list<object>  _tape.
    
    constructor new
    [
        _stack := stack<object>().
        _tape := list<object>().
    ]

    push : object
    [
        _stack push:object.
    ]        
    
    pop => _stack.
    
    peek => _stack.        
        
    int length => _tape.
    
    tape = _tape.
        
    trim : pos
        => _tape.        
    
    first => _tape.        
    
    append : object
    [
        _tape append:object.
    ]    
    
    stacksafe getAt(IntNumber index)
        = _tape[index].        
}

sealed class $ParameterContext
{
    type<ArrayList> _buffer.
    
    constructor new
    [
        _buffer := ArrayList new.
    ]
    
    int level
    [
        int len := _buffer length.
        
        len := len / 3.
        
        ^ len.
    ]
    
    trim(IntNumber level)
    [
        int index := level * 3.
        
        _buffer trim int(index).
    ]
        
    add(LiteralValue name, IntNumber type, Object value)
    [
        _buffer append(name); append(type); append(value)
    ]
}

limited class $Strategy
{
    type<$Strategy> symbol_one    []
    type<$Strategy> expression    []
    type<$Strategy> singleton_one []
    type<$Strategy> method_one    []
    type<$Strategy> code_one      []
    
    begin($ClassContext context, $ParameterContext parameters) []
    end($ClassContext context, $ParameterContext parameters)   []    
    
    setReference($ClassContext context, LiteralValue r)       []
    setMessageToken($ClassContext context, LiteralValue m)    []
    setLiteral($ClassContext context, LiteralValue l)         []
    setParamToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
}

singleton $StartOne :: $Strategy
{
    type<$Strategy> symbol_one = $SymbolOne.
    
    type<$Strategy> singleton_one = $Singleton.
}

singleton $SymbolOne :: $Strategy
{
    type<$Strategy> expression = $SymbolBody.
    
    begin($ClassContext context, $ParameterContext parameters)  
    [
        context push(context length).    
    ]
    
    end($ClassContext context, $ParameterContext parameters)
    [
        int index := context pop.
        int len := context length - index.
        var tape := ClosureTape new(context tape, index, len).
        
        context trim(index).
        context append(tape).
    ]    
}

class $BaseCode :: $Strategy
{
    type<$Strategy> expression   = $RootExpresson.
}

singleton $Code :: $BaseCode.

singleton $Method :: $Strategy
{
    type<$Strategy> code_one = $Code.
    
    setParamToken($ClassContext context, LiteralValue value, $ParameterContext parameters)
    [
        int method_level := context peek.        
        int level := parameters level.
        
        level := level - method_level.
        level := level + 1.
        
        parameters add(value, $METHOD_PARAM, level).
        
        context append($expr_var_token).
    ]
    
    begin($ClassContext context, $ParameterContext parameters)  
    [
        context push(context length).
        context push(parameters level).
    ]
    
    setMessageToken($ClassContext context, LiteralValue m) 
    [
        context append($message_token).
        context append(m).
    ]
    
    end($ClassContext context, $ParameterContext parameters)
    [
        int level := context pop.
        
        parameters trim(level).
        
        int pos := context pop.
        var m := $nil.
        int counter := -1. // self is included as well
        int index := pos.
        
        // define the method name
        var p := context getAt(index).
        while (p equal reference:$expr_var_token)
        [
            index := index + 1.
            counter := counter + 1.
            p := context getAt(index).
        ].

        if (p equal reference:$message_token)
        [
            m := context getAt(index + 1).
            index := index + 2.
            p := context getAt(index).
        ].
                
        while (p equal reference:$expr_var_token)
        [
            index := index + 1.
            counter := counter + 1.
            p := context getAt(index).
        ].
        
        textbuffer name := TextBuffer new.
        name append(m).
        name append:"[".
        name append(counter literal).
        name append:"]".        
        
        int len := context length - index.
        var tape := ClosureTape new(context tape, index, len).
        
        context trim(pos).
        context append(Message new literal:name).
        context append(tape).
    ]    
}

class $BaseClass :: $Strategy
{
    type<$Strategy> method_one = $Method.
        
    begin($ClassContext context, $ParameterContext parameters)  
    [
        context push(context length).
    ]
    
    end($ClassContext context, $ParameterContext parameters)
    [
        var pos := context pop.
        var len := context length - pos.
        
        var class := ClosureClass new(context tape, pos, len).
        
        context trim(pos).
        context append(class).
    ]
}


singleton $Singleton :: $BaseClass
{
    end($ClassContext context, $ParameterContext parameters)
    [
        var pos := context pop.
        var len := context length - pos.
        
        var class := ClosureClass new(context tape, pos, len).
        
        context trim(pos).
        context append(class).
    ]
}

class $BaseExpression :: $Strategy
{
    begin($ClassContext context, $ParameterContext parameters)
    [
        context push:$expr_end_token.
    ]    
    
    setReference($ClassContext context, LiteralValue r) 
    [
        context push(ConstantClosure new(Symbol new literal:r)).
    ]
    
    setLiteral($ClassContext context, LiteralValue l) 
    [
        context push(ConstantClosure new:l).
    ]
    
    setMessageToken($ClassContext context, LiteralValue m) 
    [
        context push:m.
        context push:$message_token.
    ]
    
    end($ClassContext context, $ParameterContext parameters)
    [
        object token := context pop.
        textbuffer buffer := TextBuffer new.
        int counter := -1.
        bool messageExpr := false.
//        bool popRequired := false.
        until(token equal reference:$expr_end_token)
        [
            if (token equal reference:$subexpr_token)
            [
                counter := counter + 1.
            ];
            if (token equal reference:$message_token)
            [
                buffer insert int:0 literal:"&".
                buffer insert int:0 literal(context pop).
                messageExpr := true.
            ];
            [
//                if (token equal reference:$expr_var_token)
//                [
//                    context append(context pop).
//                    
//                    var next_token := context peek.
//                    if(next_token equal reference:$assign_token)
//                    [
//                        token := %"tapeOp.tape_assign[]".
//                    ];
//                    [
//                        token := %"tapeOp.tape_var[]".
//                    ]
//                ];
//                if (token equal reference:$previous_token)
//                [
//                    context append(counter + 1).
//                    token := %"tapeOp.tape_stack[]".
//                    
//                    popRequired := true.
//                ].
                context append:token.
                counter := counter + 1.
            ].
            
            token := context pop.
        ].

        if (messageExpr)
        [
            buffer trim int:1.
            buffer write literal:"[".
            buffer write int:counter.
            buffer write literal:"]".
            
            context append(MessageClosure new(Message new literal:buffer))
        ].
        
//        if (popRequired)
//        [
//            context append(%"tapeOp.tape_pop_prev[1]").
//        ]
    ]    
}

singleton $SymbolBody :: $BaseExpression
{
    type<$Strategy> expression = $SubExpression.
}

singleton $SubExpression :: $BaseExpression
{
    type<$Strategy> expression = $SubExpression.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        $super end(context, parameters).
        
        context push:$subexpr_token.
    ]    
}
    
singleton $RootExpresson :: $BaseExpression
{
    type<$Strategy> expression = $SubExpression.
    
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
//        context append:%"tapeOp.tape_pop[1]".
//    ]    
}

class TapeAssembly
{
    stack<$Strategy>        _states.
    type<$Strategy>         _state.

    type<$ClassContext>     _context.
    type<$ParameterContext> _parameters.
    
    constructor new
    [
        _state := $StartOne.
        _states := stack<$Strategy>(32).
        
        _context := $ClassContext new.
        _parameters := $ParameterContext new.
    ]
    
    constructor new(ClosureTape tape)
        <= new;
    [
        tape($self).
    ]
    
    open subject:state
    [
        _states push:_state.
        _state := _state~state get.
        
        _state begin(_context, _parameters).
    ]
    
    openSymbol
        <= open(%symbol_one).
    
    openSingleton
        <= open(%singleton_one).
    
    openMethod
        <= open(%method_one).
    
    openCode
        <= open(%code_one).
    
    openExpression
        <= open(%expression).
    
    newReference : r
    [
        _state setReference(_context, r literal).
    ]
    
    newLiteralToken : l
    [
        _state setLiteral(_context, l literal).
    ]
    
    newMessageToken : m
    [
        _state setMessageToken(_context, m literal).
    ]

    newParamToken : p
    [
        _state setParamToken(_context, p literal, _parameters).
    ]            
    
    close
    [
        _state end(_context, _parameters).
        
        _state := _states pop.
    ]
    
    eval
    [
        var o := _context first.
        if (o instanceOf:ClosureTape)
        [
            ^ o().
        ];
        [
            ^ o
        ]        
    ]
    
    first => _context.
}

singleton openSymbolClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target openSymbol.
}

singleton openSingletonClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target openSingleton.
}

singleton openMethodClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target openMethod.
}

singleton openCodeClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target openCode.
}

singleton openExpressionClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target openExpression.
}

singleton closeClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target close.
}

class DynamicClosureWithParam :: DynamicClosure
{
    object param.
    
    constructor new : param
    [
        @param := param
    ]
}

class newReferenceClosure :: DynamicClosureWithParam
{    
    generic closure(object target, object<> args)
        = target newReference:param.    
}

class newMessageClosure :: DynamicClosureWithParam
{    
    generic closure(object target, object<> args)
        = target newMessageToken:param.    
}

class newLiteralClosure :: DynamicClosureWithParam
{    
    generic closure(object target, object<> args)
        = target newLiteralToken:param.    
}

class newParamTokenClosure :: DynamicClosureWithParam
{    
    generic closure(object target, object<> args)
        = target newParamToken:param.    
}
