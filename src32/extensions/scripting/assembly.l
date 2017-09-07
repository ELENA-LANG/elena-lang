import system'collections.
import system'text.
import system'dynamic.
import extensions.

singleton $message_token.
singleton $expr_end_token.
singleton $expr_var_token.
singleton $subexpr_token.
singleton $assign_token.
singleton $if_token.
singleton $code_token.

sealed class $ClassContext
{
    type<Stack>     _stack.
    type<ArrayList> _tape.
    
    constructor new
    [
        _stack := Stack new.
        _tape := ArrayList new.
    ]
    
    tape = _tape.
    
    length => _tape.
    
    first => _tape.        
        
    push : object
    [
        _stack push:object.
    ]
    
    pop => _stack.
        
    peek => _stack.        
        
    trim : pos
        => _tape.        
        
    append : object
    [
        _tape append:object.
    ]
    
    stacksafe getAt(IntNumber index)
        = _tape[index].
        
    stacksafe setAt(IntNumber index, Object object)
    [
        _tape[index] := object.
    ]
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
    
    resolve literal:name vint:type
    [
        int len := _buffer length.
        int i := len - 3.
        while (i >= 0)
        [
            if (name == _buffer[i])
            [
                type set int(_buffer[i+1]).

                ^ _buffer[i+2].
            ].
            
            i := i - 3.
        ].
        
        ^ $nil.
    ]
}

limited class $Strategy
{
    type<$Strategy> symbol_one    []
    type<$Strategy> preloaded_one []
    type<$Strategy> expression    []
    type<$Strategy> returning     []
    type<$Strategy> singleton_one []
    type<$Strategy> method_one    []
    type<$Strategy> code_one      []
    type<$Strategy> variable_one  []
    type<$Strategy> loop_one      []
    
    type<$Strategy> if_one($ClassContext context) []
    
    begin($ClassContext context, $ParameterContext parameters) []
    end($ClassContext context, $ParameterContext parameters)   []
    
    setIdentToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
    setParamToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
    setReference($ClassContext context, LiteralValue r)       []
    setMessageToken($ClassContext context, LiteralValue m)    []
    setLiteral($ClassContext context, LiteralValue l)         []
    setNumericToken($ClassContext context, LiteralValue l)    []
    setAssignToken($ClassContext context, LiteralValue t)     []
}

singleton $StartOne :: $Strategy
{
    symbol_one = $SymbolOne.
    
    preloaded_one = $PreloadedOne.
}

singleton $SymbolOne :: $Strategy
{
    expression = $SymbolBody.
}

singleton $PreloadedOne :: $Strategy
{
    begin($ClassContext context, $ParameterContext parameters)  
    [
        context push(context length).
    ]
    
    expression = $SymbolBody.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        int index := context pop.
        int len := context length - index.
        var tape := Tape new(context tape, index, len).
        
        context trim(index).
        context append(tape).
        
        tape eval.
    ]
}

class $BaseClass :: $Strategy
{
    method_one = $Method.
        
    begin($ClassContext context, $ParameterContext parameters)  
    [
        context push(context length).
    ]
    
    end($ClassContext context, $ParameterContext parameters)
    [
        var pos := context pop.
        var len := context length - pos.
        
        var class := Class new(context tape, pos, len).
        
        context trim(pos).
        context append(class).
    ]
}

singleton $Singleton :: $BaseClass
{
}

singleton $Nested :: $BaseClass
{
    end($ClassContext context, $ParameterContext parameters)
    [
        var pos := context pop.
        var len := context length - pos.
        
        var class := Class new(context tape, pos, len).
        
        context trim(pos).
        context push(class).
    ]
}

class $BaseCode :: $Strategy
{
    expression   = $RootExpresson.
    returning    = $RetExpression.
    variable_one = $Variable.   
    loop_one     = $Loop.
}

singleton $Code :: $BaseCode.

singleton $SubCode :: $BaseCode
{
}

singleton $Variable :: $Strategy
{
    setIdentToken($ClassContext context, LiteralValue value, $ParameterContext parameters) 
    [
        context append:nil.
        
        int method_level := context peek.        
        int level := parameters level.
        
        level := level - method_level.
        level := level + 1.
        
        parameters add(value, $METHOD_PARAM, level).        
    ]    
}

singleton $Method :: $Strategy
{
    code_one = $Code.
    
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
    
    setParamToken($ClassContext context, LiteralValue value, $ParameterContext parameters)
    [
        int method_level := context peek.        
        int level := parameters level.
        
        level := level - method_level.
        level := level + 1.
        
        parameters add(value, $METHOD_PARAM, level).
        
        context append($expr_var_token).
    ]
    
    returning = $RetExpression.
    
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
        var tape := Tape new(context tape, index, len).
        
        context trim(pos).
        context append(Message new literal:name).
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
        context push(Symbol new literal:r).
    ]

    setMessageToken($ClassContext context, LiteralValue m) 
    [
        context push:m.
        context push:$message_token.
    ]

    setIdentToken($ClassContext context, LiteralValue m, $ParameterContext parameters) 
    [
        int type := 0.
        object param := parameters resolve literal:m vint:type.
        if (type == $METHOD_PARAM)
        [
            context push:param.
            context push:$expr_var_token.            
        ];
        if (type == $FORWARD_PARAM)
        [
            context push:param.
        ];
        [
            InvalidArgumentException new("Variable cannot be resolved - " + m); raise
        ].
    ]

    setNumericToken($ClassContext context, LiteralValue l)
    [
        context push(l toInt).
    ]

    setLiteral($ClassContext context, LiteralValue l) 
    [
        context push:l.
    ]

    singleton_one = $Nested.

    end($ClassContext context, $ParameterContext parameters)
    [
        var token := context pop.
        textbuffer buffer := TextBuffer new.
        int counter := -1.
        bool messageExpr := false.
        until(token equal reference:$expr_end_token)
        [
            if (token equal reference:$subexpr_token)
            [
                counter := counter + 1.
            ];
            if (token equal reference:$message_token)
            [
                buffer append(context pop).
                messageExpr := true.
            ];
            [
                if (token equal reference:$expr_var_token)
                [
                    context append(context pop).
                    
                    var next_token := context peek.
                    if(next_token equal reference:$assign_token)
                    [
                        token := %"tapeOp.tape_assign[]".
                    ];
                    [
                        token := %"tapeOp.tape_var[]".
                    ]
                ].
                context append:token.
                counter := counter + 1.
            ].
            
            token := context pop.
        ].

        if (messageExpr)
        [
            buffer write literal:"[".
            buffer write int:counter.
            buffer write literal:"]".
            
            context append(Message new literal:buffer)
        ]
    ]
    
    setAssignToken($ClassContext context, LiteralValue t)
    [
        var token := context pop.
        if (token equal reference:$expr_var_token)
        [
            token := context pop.
            context push:%"tapeOp.tape_assign[]".
            context push:token.
        ];
        [
            context push:token.
            context push:$assign_token.
        ]
    ]
}

singleton $SymbolBody :: $BaseExpression
{
    expression = $SubExpression.
}

singleton $SubExpression :: $BaseExpression
{
    expression = $SubExpression.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        $super end(context, parameters).
        
        context push:$subexpr_token.
    ]    
}

singleton $Expresson :: $BaseExpression
{
    expression = $SubExpression.
}

singleton $CondExpression :: $BaseExpression
{
    begin($ClassContext context, $ParameterContext parameters)
    [
        $super begin(context, parameters).
    ]
    
    code_one = $SubCode.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        $super end(context, parameters).
        
        var token := context pop.
        if (token equal reference:$if_token)
        [
            context push(context length).
            context append:nil.
            context append:%"tapeOp.tape_goifnot[]".
            context push($code_token).
        ]
    ]    
}

singleton $IfExpression :: $BaseExpression
{
    expression = $CondExpression.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        int pos := context pop.        
        
        $super end(context, parameters).
        
        int offset := context length - pos - 2.
        
        context[pos] := IntNumber new(offset).
    ]        
}

singleton $LoopIfExpression :: $BaseExpression
{
    expression = $CondExpression.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        int pos := context pop.
        
        $super end(context, parameters).
        
        int loop_pos := context pop.
        
        context append(loop_pos - context length - 2).
        context append:%"tapeOp.tape_go[]".
        
        int offset := context length - pos - 2.
        
        context[pos] := IntNumber new(offset).
        
    ]        
}

singleton $Loop :: $Strategy
{
    expression = $LoopExpresson.    
    
    begin($ClassContext context, $ParameterContext parameters)
    [
        context push(context length).
    ]
}

singleton $LoopExpresson :: $BaseExpression
{
    if_one($ClassContext context)
    [
        context push:$if_token.
        
        ^ $LoopIfExpression.
    ]
}    

singleton $RootExpresson :: $BaseExpression
{
    expression = $SubExpression.
    
    if_one($ClassContext context)
    [
        context push:$if_token.
        
        ^ $IfExpression.
    ]
    
    end($ClassContext context, $ParameterContext parameters)
    [
        $super end(context, parameters).
        
        context append:%"tapeOp.tape_pop[1]".
    ]    
}

singleton $RetExpression :: $BaseExpression
{
    expression = $Expresson.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        $super end(context, parameters).
        
        context append:%"tapeOp.tape_exit[]".
    ]
}

const int $METHOD_PARAM = 1.
const int $FORWARD_PARAM = 2.

class Library //:: Scope
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
    
    include(LiteralValue name,LiteralValue symbolName)
    [
        var symbol := Symbol new(symbolName).
        
        _parameters add(name, $FORWARD_PARAM, symbol).
    ]
    
    open subject:state
    [
        _states push:_state.
        _state := _state~state get.
        
        _state begin(_context, _parameters).
    ]

    openPreloadedSymbol
        <= open(%preloaded_one).
            
    openSymbol
        <= open(%symbol_one).
    
    openExpression
        <= open(%expression).

    openReturning
        <= open(%returning).
        
    openSingleton
        <= open(%singleton_one).
    
    openMethod
        <= open(%method_one).
    
    openCode
        <= open(%code_one).
    
    openVariable
        <= open(%variable_one).
    
    openLoop
        <= open(%loop_one).
    
    if
    [
        _state := _state if_one(_context).
    ]
    
    close
    [
        _state end(_context, _parameters).
        
        _state := _states pop.
    ]

    newIdentToken : m
    [
        _state setIdentToken(_context, m literal, _parameters).
    ]

    newParamToken : p
    [
        _state setParamToken(_context, p literal, _parameters).
    ]            
                                    
    newReference : r
    [
        _state setReference(_context, r literal).
    ]
    
    newMessageToken : m
    [
        _state setMessageToken(_context, m literal).
    ]
    
    newLiteralToken : l
    [
        _state setLiteral(_context, l literal).
    ]
    
    newNumericToken : l
    [
        _state setNumericToken(_context, l literal).
    ]

    newAssignToken : t
    [
        _state setAssignToken(_context, t literal).
    ]
        
    first
       => _context.
}
