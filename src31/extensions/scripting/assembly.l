import system'routines.
import system'collections.
import system'text.
import system'dynamic.
import extensions.

class Scope
{
    object theParent.
    
    constructor new : parent
    [
        theParent := parent.
    ]

    save variable:literal array_list:list
    [
        theParent save variable:literal array_list:list.
    ]

    assign variable:literal array_list:list
        => theParent.
                        
    close = theParent.        
}

class IfExpression :: Scope
{
    object theCondExpr.
    object theIfClosure.
    
    constructor new : aParent
        <= new:aParent;
    [
    ]
    
    open Expression
    [
        if ($nil == theCondExpr)
        [
            theCondExpr := ExpressionScope new:$self.
            
            ^ theCondExpr
        ];
        [
           theIfClosure := ExpressionScope newIfExpr:$self.
           
           ^ theIfClosure
        ]        
    ]
    
    close
        = theParent close.
        
    save array_list:retVal
    [
        theCondExpr save array_list:retVal.
        
        int jumpLabel := retVal length.
        
        retVal append:$nil.
        retVal append:%"tapeOp.tape_jumpifnot[]".
        
        theIfClosure save array_list:retVal.
        
        retVal@jumpLabel := retVal length.
    ]                
}

class LoopExpression :: IfExpression
{
    constructor new : aParent
        <= new:aParent;
    [
    ]
    
    eval If []
    
    save array_list:retVal
    [
        int loopLabel := retVal length.
        
        theCondExpr save array_list:retVal.
        
        int jumpLabel := retVal length.
        
        retVal append:$nil.
        retVal append:%"tapeOp.tape_jumpifnot[]".
        
        theIfClosure save array_list:retVal.
        retVal append:loopLabel.
        retVal append:%"tapeOp.tape_jump[]".
        
        retVal@jumpLabel := retVal length.
    ]
    
    close = { close = theParent. }.
}

class ExpressionScope :: Scope
{
    stack      theCallstack.
    textbuffer theMessage.
    bool       theCallExpr.
    bool       theAssignExpr.
    bool       theIfExpr.
        
    constructor new : aParent
        <= new : aParent;
    [
        theCallstack := Stack new.
        theMessage := TextBuilder new.
        theCallExpr := false.
        theAssignExpr := false.
        theIfExpr := false.
    ]
    
    constructor newIfExpr : aParent
        <= new : aParent;
    [
        theIfExpr := true.
    ]        

    new NumericToken : literal
    [
        theCallstack push:{ save array_list:list [ list append:(literal toInt) ] }.
    ]
        
    new LiteralToken : literal
    [
        theCallstack push:{ save array_list:list [ list append:(literal toLiteral) ] }.
    ]
        
    new IdentToken : literal
    [
        theCallstack push:
        { 
            save array_list:list 
            [ 
                theParent save variable:literal array_list:list
            ]
            
            assign array_list:list
            [
                theParent assign variable:literal array_list:list
            ]
        }.
    ]        
        
    new MessageToken : literal
    [
        theCallExpr := true.
        ifnot(0 == theMessage length)
            [ theMessage append:"&" ].
            
        theMessage append:literal
    ]
        
    new ReferenceToken : symbol
    [
        theCallstack push:{ save array_list:list [ list append:(Symbol new literal:symbol) ] }.
    ]
    
    new AssignToken : token
    [
        theAssignExpr := true.
    ]
        
    open Expression
    [
        var expr := ExpressionScope new:$self.
        
        theCallstack push:expr.
        
        ^ expr
    ]

    open Singleton
    [
        var expr := SingletonScope new:$self.
        
        theCallstack push:expr.
        
        ^ expr.
    ]

    open Code
    [
        var expr := $nil.
        
        if (theIfExpr)
        [
            expr := CodeScope new:$self.
        ];
        [
            expr := ClosureScope new:$self.
        ].
            
        theCallstack push:expr.
        
        ^ expr
    ]

    eval If
    [
        theIfExpr := true.
        
        var expr := IfExpression new:$self.
        
        theCallstack push:expr.
        
        ^ expr.
    ]

    save array_list:retVal
    [
        if(theCallExpr)
        [
            int length := theCallstack length.
            length := length - 1.
            
            theMessage write literal:"[".
            theMessage write int:length.
            theMessage write literal:"]".
            
            until (theCallstack is empty)
            [
                theCallstack pop; save array_list:retVal
            ].
            
            retVal append:(Message new literal:theMessage)
        ];
        [
            theCallstack pop; save array_list:retVal.
            
            if (theAssignExpr)
            [
                theCallstack pop; assign array_list:retVal
            ].
        ].
    ]    
}

class RetExpressionScope :: ExpressionScope
{
    constructor new : aParent
        <= new : aParent;
    [
    ]
}

class CodeScope :: Scope
{
    object theStatements.
    
    constructor new : aParent
        <= new : aParent;
    [
        theStatements := List new.
    ]

    save array_list:retVal
    [
        theStatements run each: (:statement)
        [
            statement save array_list:retVal
        ].
    ]

    open Ret_expr
    [
        var expr := RetExpressionScope new:$self.
        
        theStatements append:expr.
        
        ^ expr
    ]
        
    open Expression
    [
        var expr := ExpressionScope new:$self.
        
        theStatements append:expr.
        
        ^ expr
    ]
    
    open Loop
    [
        var loop := LoopExpression new:$self.
        
        theStatements append:loop.
        
        ^ { open Expression = loop. }
    ]
    
    open Variable
        = 
        {
            new IdentToken:token
            [
                theParent new VariableToken:token
            ]
            
            close = $owner.
        }.
}

class MethodScope :: Scope
{
    object theSubject.
    object theCode.
    object theParameters.
    vint   theParamCounter.
    vint   theVarCounter.
    
    constructor new : aParent
        <= new : aParent;
    [
        theSubject := TextBuilder new.
        theParameters := List new.
        theParamCounter := -1.
        theVarCounter := 0.
    ]
        
    subject
        = Signature new literal:theSubject.

    message
    [
        theSubject 
            append:"[";
            append:(theParamCounter literal);
            append:"]".

        ^ Message new literal:theSubject         
    ]

    code
    [
        var list := ArrayList new.
        
        int counter := theVarCounter.
        while (counter > 0)
        [
            list append:nil.
            
            counter := counter - 1
        ].
        
        theCode save array_list:list.
        
        ^ Tape new array:list
    ]    
        
    new MessageToken : message
    [
        ifnot(theSubject is empty)
            [ theSubject write:"&" ].
        
        theSubject write:message
    ]    
    
    new ParamToken : param
    [
        theParameters append:(KeyValue new key:param value:(theParameters length + 1)).
        
        theParamCounter append int:1
    ]
    
    new VariableToken : param
    [
        theParameters append:(KeyValue new key:param value:(theParameters length + 1)).
        
        theVarCounter append int:1
    ]
    
    open Ret_expr
    [
        theCode := RetExpressionScope new:$self.
        
        ^ theCode
    ]
    
    open Code
    [
        theCode := CodeScope new:$self.
        
        ^ theCode
    ]
    
    save variable:literal array_list:list
    [
        var param := theParameters seek each: (:x)[  x key; equal:literal ].
        if($nil != param)
            [
                list append:(param value).
                list append:%"tapeOp.tape_var[]"
            ];
            [
                $super save variable:literal array_list:list
            ].
    ]
    
    assign variable:literal array_list:list
    [
        var param := theParameters seek each: (:x)[ ^x key; equal:literal ].
        if($nil != param)
            [
                list append:(param value).
                list append:%"tapeOp.tape_assign[]"
            ];
            [
                $super save variable:literal array_list:list
            ]        
    ]
}

class BaseClassScope :: Scope
{
    object theMethods.
    
    constructor new : aParent
        <= new : aParent;
    [
        theMethods := List new.
    ]

    open Method
    [
        var method := MethodScope new:$self.
        
        theMethods append:method.
        
        ^ method
    ]        
}

class SingletonScope :: BaseClassScope
{
    constructor new : parent
        <= new:parent;
    [
    ]
    
    new IdentToken:name
    [
    ]
    
    save array_list:retVal
    [
        retVal append:($self eval)
    ]
    
    eval
    [
        var list := List new.
        
        theMethods run each: (:method)
        [
            list append:(method message).
            list append:(method code)
        ].
        
        ^ Class new array:(list toArray)
    ]
}

class SymbolBody :: ExpressionScope
{
    object theSingleton.
    
    constructor new : parent
        <= new:parent;
    [
    ]
    
    open Singleton
    [
        theSingleton := SingletonScope new:$self.
        
        ^ theSingleton
    ]
    
    eval
    [
        if ($nil != theSingleton)
        [
            ^ theSingleton eval
        ];
        [
            var list := ArrayList new.
        
            $self save array_list:list.
        
            ^ Tape new array:list
        ].
    ]
}

class ClosureScope :: CodeScope
{
    object theParameters.
    vint   theVarCounter.
    
    constructor new : parent
        <= new:parent;
    [
        theVarCounter := 0.
        theParameters := List new.
    ]
    
    save array_list:retVal
    [
        retVal append:($self eval)
    ]
    
    new VariableToken : param
    [
        theParameters append:(KeyValue new key:param value:(theParameters length + 1)).
        
        theVarCounter append int:1
    ]
    
    open Variable
        = 
        {
            new IdentToken:token
            [
                $owner new VariableToken:token
            ]
            
            close = $owner.
        }.

    eval
    [
        var list := ArrayList new.
        
        int counter := theVarCounter.
        while (counter > 0)
        [
            list append:nil.
            
            counter := counter - 1
        ].
        
        $super save array_list:list.
        
        ^ Tape new array:list        
    ]
}

class SymbolScope :: Scope
{
    object theBody.
    
    constructor new : parent
        <= new:parent;
    [
    ]
    
    new IdentToken:name
    [
    ]
    
    open Expression
    [
        theBody := SymbolBody new:$self.
        
        ^ theBody
    ]
    
    eval
        => theBody.
}

class Library :: Scope
{
    object theClasses.
    object theForwards.
    object thePreloaded.
    
    constructor new
    [
        theClasses := List new.
        theForwards := Dictionary new.
        thePreloaded := List new.
    ]
    
    include:ref:symbol
    [
        theForwards@symbol := ref.
    ]
    
    open Symbol
    [
        var symbol := SymbolScope new:$self.
        
        theClasses append:symbol.
        
        ^ symbol
    ]
    
    open Preloaded_symbol
    [
        var symbol := $self open Symbol.

        thePreloaded append:symbol.
                        
        ^ symbol
    ]
    
    open Singleton
    [
        var class := SingletonScope new:$self.
        
        theClasses append:class.   
        
        ^ class
    ]

//    #method open &class
//    [
//        #var class := ClassScope new:$self.
//        
//        theClasses += class.
//        
//        ^ class.
//    ]

    init
    [
        thePreloaded run each: (:symbol)
        [
            symbol eval; eval.
        ].
    ]

    save variable:literal array_list:list
    [
        var ref := theForwards@literal.
        if ($nil == ref)
            [ InvalidArgumentException new:("Variable cannot be resolved - " + literal); raise ];
            [ list append:(Symbol new literal:ref) ]        
    ]
        
    first
        => theClasses.
}
