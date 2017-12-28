import system'collections.
import system'dynamic.
import system'text.

singleton $message_token.
singleton $expr_end_token.
singleton $subexpr_token.

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
    
    int length => _tape.
    
    tape = _tape.
        
    trim : pos
        => _tape.        
    
    first => _tape.        
    
    append : object
    [
        _tape append:object.
    ]    
}

sealed class $ParameterContext
{
}

limited class $Strategy
{
    type<$Strategy> symbol_one    []
    type<$Strategy> expression    []
    
    begin($ClassContext context, $ParameterContext parameters) []
    end($ClassContext context, $ParameterContext parameters)   []    
    
    setReference($ClassContext context, LiteralValue r)       []
    setMessageToken($ClassContext context, LiteralValue m)    []
    setLiteral($ClassContext context, LiteralValue l)         []
}

singleton $StartOne :: $Strategy
{
    type<$Strategy> symbol_one = $SymbolOne.
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
    
    open subject:state
    [
        _states push:_state.
        _state := _state~state get.
        
        _state begin(_context, _parameters).
    ]
    
    openSymbol
        <= open(%symbol_one).
    
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
    
    close
    [
        _state end(_context, _parameters).
        
        _state := _states pop.
    ]
    
    eval
    [
        (_context first)().
    ]
}


singleton openSymbolClosure :: DynamicClosure
{
    generic closure(object target, object<> args)
        = target openSymbol.
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
