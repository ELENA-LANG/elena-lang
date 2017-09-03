//import system'routines.
import system'collections.
import system'text.
import system'dynamic.
import extensions.

//class Scope
//{
//    object theParent.
//    
//    constructor new : parent
//    [
//        theParent := parent.
//    ]
//
//    save variable:literal array_list:list
//    [
//        theParent save variable:literal array_list:list.
//    ]
//
//    add retPoint:label
//    [
//        theParent add retPoint:label.
//    ]
//
//    assign variable:literal array_list:list
//        => theParent.
//                        
//    close = theParent.        
//}
//
//class IfExpression :: Scope
//{
//    object theCondExpr.
//    object theIfClosure.
//    
//    constructor new : aParent
//        <= new:aParent;
//    [
//    ]
//    
//    open Expression
//    [
//        if ($nil == theCondExpr)
//        [
//            theCondExpr := ExpressionScope new:$self.
//            
//            ^ theCondExpr
//        ];
//        [
//           theIfClosure := ExpressionScope newIfExpr:$self.
//           
//           ^ theIfClosure
//        ]        
//    ]
//    
//    close
//        = theParent close.
//        
//    save array_list:retVal
//    [
//        theCondExpr save array_list:retVal.
//        
//        int jumpLabel := retVal length.
//        
//        retVal append:$nil.
//        retVal append:%"tapeOp.tape_jumpifnot[]".
//        
//        theIfClosure save array_list:retVal.
//        
//        retVal@jumpLabel := retVal length.
//    ]                
//}
//
//class LoopExpression :: IfExpression
//{
//    constructor new : aParent
//        <= new:aParent;
//    [
//    ]
//    
//    eval If []
//    
//    save array_list:retVal
//    [
//        int loopLabel := retVal length.
//        
//        theCondExpr save array_list:retVal.
//        
//        int jumpLabel := retVal length.
//        
//        retVal append:$nil.
//        retVal append:%"tapeOp.tape_jumpifnot[]".
//        
//        theIfClosure save array_list:retVal.
//        retVal append:loopLabel.
//        retVal append:%"tapeOp.tape_jump[]".
//        
//        retVal@jumpLabel := retVal length.
//    ]
//    
//    close = { close = theParent. }.
//}
//
//class ExpressionScope :: Scope
//{
//    stack      theCallstack.
//    textbuffer theMessage.
//    bool       theCallExpr.
//    bool       theAssignExpr.
//    bool       theIfExpr.
//        
//    constructor $clone parent:parent stack:callstack textbuffer:message bool:callExpr bool:assignExpr bool:ifExpr
//        <= new : parent;
//    [
//        theCallstack := callstack.
//        theMessage := message.
//        theCallExpr := callExpr.
//        theAssignExpr := assignExpr.
//        theIfExpr := ifExpr.
//    ]
//        
//    constructor new : aParent
//        <= new : aParent;
//    [
//        theCallstack := Stack new.
//        theMessage := TextBuilder new.
//        theCallExpr := false.
//        theAssignExpr := false.
//        theIfExpr := false.
//    ]
//    
//    constructor newIfExpr : aParent
//        <= new : aParent;
//    [
//        theIfExpr := true.
//    ]        
//        
//    new IdentToken : literal
//    [
//        theCallstack push:
//        { 
//            save array_list:list 
//            [ 
//                theParent save variable:literal array_list:list
//            ]
//            
//            assign array_list:list
//            [
//                theParent assign variable:literal array_list:list
//            ]
//        }.
//    ]        
//        
//    new AssignToken : token
//    [
//        theAssignExpr := true.
//    ]
//        
//    open Expression
//    [
//        var expr := ExpressionScope new:$self.
//        
//        theCallstack push:expr.
//        
//        ^ expr
//    ]
//
//    open Singleton
//    [
//        var expr := SingletonScope new:$self.
//        
//        theCallstack push:expr.
//        
//        ^ expr.
//    ]
//
//    open Code
//    [
//        var expr := $nil.
//        
//        if (theIfExpr)
//        [
//            expr := CodeScope new:$self.
//        ];
//        [
//            expr := ClosureScope new:$self.
//        ].
//            
//        theCallstack push:expr.
//        
//        ^ expr
//    ]
//
//    eval Nest
//    [
//        var nestedExpr := ExpressionScope 
//                            $clone
//                                parent:$self
//                                stack:theCallstack 
//                                textbuffer:theMessage 
//                                bool:theCallExpr 
//                                bool:theAssignExpr 
//                                bool:theIfExpr.
//        
//        theCallstack := Stack new.
//        theMessage := TextBuilder new.
//        theCallExpr := false.
//        theAssignExpr := false.
//        theIfExpr := false.
//        
//        theCallstack push:nestedExpr.
//    ]
//
//    eval If
//    [
//        theIfExpr := true.
//        
//        var expr := IfExpression new:$self.
//        
//        theCallstack push:expr.
//        
//        ^ expr.
//    ]
//
//    save array_list:retVal
//    [
//        if(theCallExpr)
//        [
//            int length := theCallstack length.
//            length := length - 1.
//            
//            theMessage write literal:"[".
//            theMessage write int:length.
//            theMessage write literal:"]".
//            
//            until (theCallstack is empty)
//            [
//                theCallstack pop; save array_list:retVal
//            ].
//            
//            retVal append:(Message new literal:theMessage)
//        ];
//        [
//            theCallstack pop; save array_list:retVal.
//            
//            if (theAssignExpr)
//            [
//                theCallstack pop; assign array_list:retVal
//            ].
//        ].
//    ]    
//}
//
//class CodeScope :: Scope
//{
//    object theStatements.
//    
//    constructor new : aParent
//        <= new : aParent;
//    [
//        theStatements := List new.
//    ]
//
//    save array_list:retVal
//    [
//        theStatements run each: (:statement)
//        [
//            statement save array_list:retVal
//        ].
//    ]
//
//    open Ret_expr
//    [
//        var expr := RetExpressionScope new:$self.
//        
//        theStatements append:expr.
//        
//        ^ expr
//    ]
//        
//    open Expression
//    [
//        var expr := ExpressionScope new:$self.
//        
//        theStatements append:expr.
//        
//        ^ expr
//    ]
//    
//    open Loop
//    [
//        var loop := LoopExpression new:$self.
//        
//        theStatements append:loop.
//        
//        ^ { open Expression = loop. }
//    ]
//    
//    open Variable
//        = 
//        {
//            new IdentToken:token
//            [
//                theParent new VariableToken:token
//            ]
//            
//            close = $owner.
//        }.
//}
//
//class MethodScope :: Scope
//{
//    object theSubject.
//    object theCode.
//    object theParameters.
//    object theRetPoints.
//    vint   theParamCounter.
//    vint   theVarCounter.    
//    
//    constructor new : aParent
//        <= new : aParent;
//    [
//        theSubject := TextBuilder new.
//        theParameters := List new.
//        theRetPoints := ArrayList new.
//        theParamCounter := -1.
//        theVarCounter := 0.
//    ]
//        
//    subject
//        = Signature new literal:theSubject.
//
//    message
//    [
//        theSubject 
//            append:"[";
//            append:(theParamCounter literal);
//            append:"]".
//
//        ^ Message new literal:theSubject         
//    ]
//
//    add retPoint:label
//    [
//        theRetPoints append:label.
//    ]
//
//    code
//    [
//        var list := ArrayList new.
//        
//        int counter := theVarCounter.
//        while (counter > 0)
//        [
//            list append:nil.
//            
//            counter := counter - 1
//        ].
//        
//        theCode save array_list:list.
//        
//        int eopLabel := list length.
//        int retPointLen := theRetPoints length.
//        int i := 0.
//        while (i < retPointLen)
//        [
//            var retPoint := theRetPoints@i.
//            
//            //list@retPoint := eopLabel.
//            list setAt:retPoint:eopLabel.
//            
//            i := i + 1.
//        ].
//        
//        ^ Tape new array:list
//    ]
//        
//    new MessageToken : message
//    [
//        ifnot(theSubject is empty)
//            [ theSubject write:"&" ].
//        
//        theSubject write:message
//    ]    
//    
//    new ParamToken : param
//    [
//        theParameters append:(KeyValue new key:param value:(theParameters length + 1)).
//        
//        theParamCounter append int:1
//    ]
//    
//    new VariableToken : param
//    [
//        theParameters append:(KeyValue new key:param value:(theParameters length + 1)).
//        
//        theVarCounter append int:1
//    ]
//    
//    open Ret_expr
//    [
//        theCode := RetExpressionScope new:$self.
//        
//        ^ theCode
//    ]
//    
//    open Code
//    [
//        theCode := CodeScope new:$self.
//        
//        ^ theCode
//    ]
//    
//    save variable:literal array_list:list
//    [
//        var param := theParameters seek each: (:x)[  x key; equal:literal ].
//        if($nil != param)
//            [
//                list append:(param value).
//                list append:%"tapeOp.tape_var[]"
//            ];
//            [
//                $super save variable:literal array_list:list
//            ].
//    ]
//    
//    assign variable:literal array_list:list
//    [
//        var param := theParameters seek each: (:x)[ ^x key; equal:literal ].
//        if($nil != param)
//            [
//                list append:(param value).
//                list append:%"tapeOp.tape_assign[]"
//            ];
//            [
//                $super save variable:literal array_list:list
//            ]        
//    ]
//}
//
//class BaseClassScope :: Scope
//{
//    object theMethods.
//    
//    constructor new : aParent
//        <= new : aParent;
//    [
//        theMethods := List new.
//    ]
//
//    open Method
//    [
//        var method := MethodScope new:$self.
//        
//        theMethods append:method.
//        
//        ^ method
//    ]        
//}
//
//class SingletonScope :: BaseClassScope
//{
//    constructor new : parent
//        <= new:parent;
//    [
//    ]
//    
//    new IdentToken:name
//    [
//    ]
//    
//    save array_list:retVal
//    [
//        retVal append:($self eval)
//    ]
//    
//    eval
//    [
//        var list := List new.
//        
//        theMethods run each: (:method)
//        [
//            list append:(method message).
//            list append:(method code)
//        ].
//        
//        ^ Class new array:(list toArray)
//    ]
//}
//
//class SymbolBody :: ExpressionScope
//{
//    object theSingleton.
//    
//    constructor new : parent
//        <= new:parent;
//    [
//    ]
//    
//    open Singleton
//    [
//        theSingleton := SingletonScope new:$self.
//        
//        ^ theSingleton
//    ]
//    
//    eval
//    [
//        if ($nil != theSingleton)
//        [
//            ^ theSingleton eval
//        ];
//        [
//            var list := ArrayList new.
//        
//            $self save array_list:list.
//        
//            ^ Tape new array:list
//        ].
//    ]
//}
//
//class ClosureScope :: CodeScope
//{
//    object theParameters.
//    vint   theVarCounter.
//    
//    constructor new : parent
//        <= new:parent;
//    [
//        theVarCounter := 0.
//        theParameters := List new.
//    ]
//    
//    save array_list:retVal
//    [
//        retVal append:($self eval)
//    ]
//    
//    new VariableToken : param
//    [
//        theParameters append:(KeyValue new key:param value:(theParameters length + 1)).
//        
//        theVarCounter append int:1
//    ]
//    
//    open Variable
//        = 
//        {
//            new IdentToken:token
//            [
//                $owner new VariableToken:token
//            ]
//            
//            close = $owner.
//        }.
//
//    eval
//    [
//        var list := ArrayList new.
//        
//        int counter := theVarCounter.
//        while (counter > 0)
//        [
//            list append:nil.
//            
//            counter := counter - 1
//        ].
//        
//        $super save array_list:list.
//        
//        ^ Tape new array:list        
//    ]
//}
//
//class SymbolScope :: Scope
//{
//    object theBody.
//    
//    constructor new : parent
//        <= new:parent;
//    [
//    ]
//    
//    new IdentToken:name
//    [
//    ]
//    
//    open Expression
//    [
//        theBody := SymbolBody new:$self.
//        
//        ^ theBody
//    ]
//    
//    eval
//        => theBody.
//}

singleton $message_token.
singleton $expr_end_token.
singleton $expr_var_token.
singleton $subexpr_token.

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
    
    begin($ClassContext context, $ParameterContext parameters) []
    end($ClassContext context, $ParameterContext parameters)   []
    
    setIdentToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
    setParamToken($ClassContext context, LiteralValue m, $ParameterContext parameters) []
    setReference($ClassContext context, LiteralValue r)       []
    setMessageToken($ClassContext context, LiteralValue m)    []
    setLiteral($ClassContext context, LiteralValue l)         []
    setNumericToken($ClassContext context, LiteralValue l)    []
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

singleton $Code :: $Strategy
{
    expression = $Expresson.
    returning  = $RetExpression.
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
                    token := %"tapeOp.tape_var[]".
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
}

singleton $SymbolBody :: $BaseExpression
{
    expression = $SubExpresson.
}

singleton $SubExpresson :: $BaseExpression
{
    expression = $SubExpresson.
    
    end($ClassContext context, $ParameterContext parameters)
    [
        $super end(context, parameters).
        
        context push:$subexpr_token.
    ]    
}

singleton $Expresson :: $BaseExpression
{
    expression = $SubExpresson.
}

singleton $RetExpression :: $BaseExpression
{
    expression = $Expresson.
    
//    save array_list:retVal
//    [
//        $super save array_list:retVal.
//        
//        int jumpLabel := retVal length.
//        theParent add retPoint:jumpLabel.
//        
//        retVal append:$nil.
//        retVal append:%"tapeOp.tape_jump[]".
//    ]
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
    
//    open Singleton
//    [
//        var class := SingletonScope new:$self.
//        
//        theClasses append:class.   
//        
//        ^ class
//    ]
//
//    init
//    [
//        thePreloaded run each: (:symbol)
//        [
//            symbol eval; eval.
//        ].
//    ]
//
//    save variable:literal array_list:list
//    [
//        var ref := theForwards@literal.
//        if ($nil == ref)
//            [ InvalidArgumentException new:("Variable cannot be resolved - " + literal); raise ];
//            [ list append:(Symbol new literal:ref) ]        
//    ]
        
    first
       => _context.
}
