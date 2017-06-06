#import system'routines.
#import system'collections.
#import system'text.
#import system'dynamic.
#import extensions.

class Scope
{
    object theParent.
    
    constructor new : parent
    [
        theParent := parent.
    ]

    save &variable:literal &array_list:list
    [
        theParent save &variable:literal &array_list:list.
    ]

    assign &variable:literal &array_list:list
        => theParent.
                        
    close = theParent.        
}

class ExpressionScope :: Scope
{
    stack      theCallstack.
    textbuffer theMessage.
    bool       theCallExpr.
    bool       theAssignExpr.
    
    constructor new : aParent
        <= new : aParent;
    [
        theCallstack := Stack new.
        theMessage := TextBuilder new.
        theCallExpr := false.
        theAssignExpr := false.
    ]

    new &numericToken : literal
    [
        theCallstack push:{ save &array_list:list [ list += literal toInt. ] }.
    ]
        
    new &literalToken : literal
    [
        theCallstack push:{ save &array_list:list [ list += literal toLiteral. ] }.
    ]
        
    new &identToken : literal
    [
        theCallstack push:
        { 
            save &array_list:list 
            [ 
                theParent save &variable:literal &array_list:list.
            ]
            
            assign &array_list:list
            [
                theParent assign &variable:literal &array_list:list.
            ]
        }.
    ]        
        
    new &messageToken : literal
    [
        theCallExpr := true.
        (0 == theMessage length)
            ! [ theMessage append:"&". ].
            
        theMessage append:literal.
    ]
        
    new &referenceToken : symbol
    [
        theCallstack push:{ save &array_list:list [ list += Symbol new &literal:symbol. ] }.
    ]
    
    new &AssignToken : token
    [
        theAssignExpr := true.
    ]
        
    open &expression
    [
        var expr := ExpressionScope new:$self.
        
        theCallstack push:expr.
        
        ^ expr.
    ]

    open &singleton
    [
        var expr := SingletonScope new:$self.
        
        theCallstack push:expr.
        
        ^ expr.
    ]
                
    save &array_list:retVal
    [
        theCallExpr
        ? [
            int length := theCallstack length.
            length -= 1.
            
            theMessage write &literal:"[".
            theMessage write &int:length.
            theMessage write &literal:"]".
            
            until (theCallstack is &empty)
            [
                theCallstack pop save &array_list:retVal.
            ].
            
            retVal += Message new &literal:(theMessage literal).
        ]
        ! [
            theCallstack pop save &array_list:retVal.
            
            if (theAssignExpr)
            [
                theCallstack pop assign &array_list:retVal.
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

    save &array_list:retVal
    [
        theStatements run &each:statement
        [
            statement save &array_list:retVal.
        ].
    ]

    open &ret_expr
    [
        var expr := RetExpressionScope new:$self.
        
        theStatements += expr.
        
        ^ expr.
    ]
        
    open &expression
    [
        var expr := ExpressionScope new:$self.
        
        theStatements += expr.
        
        ^ expr.
    ]
    
    open &variable
        = 
        {
            new &identToken:token
            [
                theParent new &variableToken:token.
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
        = Signature new &literal:theSubject.

    message
    [
        theSubject 
            append:"["
            append:(theParamCounter literal)
            append:"]".

        ^ Message new &literal:theSubject.            
    ]

    code
    [
        var list := ArrayList new.
        
        int counter := theVarCounter.
        while (counter > 0)
        [
            list += nil.
            
            counter -= 1.
        ].
        
        theCode save &array_list:list.
        
        ^ Tape new &array:list.
    ]    
        
    new &messageToken : message
    [
        (theSubject is &empty)
            ! [ theSubject write:"&". ].
        
        theSubject write:message.
    ]    
    
    new &paramToken : param
    [
        theParameters += KeyValue new &key:param &value:(theParameters length + 1).
        
        theParamCounter += 1.
    ]
    
    new &variableToken : param
    [
        theParameters += KeyValue new &key:param &value:(theParameters length + 1).
        
        theVarCounter += 1.
    ]
    
    open &ret_expr
    [
        theCode := RetExpressionScope new:$self.
        
        ^ theCode.
    ]
    
    open &code
    [
        theCode := CodeScope new:$self.
        
        ^ theCode.
    ]
    
    save &variable:literal &array_list:list
    [
        var param := theParameters seek &each:x [ x key equal:literal ].
        ($nil != param)
            ? [
                list += param value.
                list += %"tapeOp.var[]".
                
            ]
            ! [
                $super save &variable:literal &array_list:list.
            ].
    ]
    
    assign &variable:literal &array_list:list
    [
        var param := theParameters seek &each:x [ x key equal:literal ].
        ($nil != param)
            ? [
                list += param value.
                list += %"tapeOp.assign[]".                
            ]
            ! [
                $super save &variable:literal &array_list:list.
            ].
        
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

    open &method
    [
        var method := MethodScope new:$self.
        
        theMethods += method.
        
        ^ method.
    ]        
}

class SingletonScope :: BaseClassScope
{
    constructor new : parent
        <= new:parent;
    [
    ]
    
    new &identToken:name
    [
    ]
    
    save &array_list:retVal
    [
        retVal += $self eval.
    ]
    
    eval
    [
        var list := List new.
        
        theMethods run &each:method
        [
            list += method message.
            list += method code.
        ].
        
        ^ Class new &array:(list toArray).        
    ]
}

class SymbolBody :: ExpressionScope
{
    object theSingleton.
    
    constructor new : parent
        <= new:parent;
    [
    ]
    
    open &singleton
    [
        theSingleton := SingletonScope new:$self.
        
        ^ theSingleton.
    ]
    
    eval
    [
        if ($nil != theSingleton)
        [
            ^ theSingleton eval.
        ];
        [
            var list := ArrayList new.
        
            $self save &array_list:list.
        
            ^ Tape new &array:list.            
        ].
    ]
}

class SymbolScope :: Scope
{
    object theBody.
    
    constructor new : parent
        <= new:parent;
    [
    ]
    
    new &identToken:name
    [
    ]
    
    open &expression
    [
        theBody := SymbolBody new:$self.
        
        ^ theBody.
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
    
    open &symbol
    [
        var symbol := SymbolScope new:$self.
        
        theClasses += symbol.
        
        ^ symbol.
    ]
    
    open &preloaded_symbol
    [
        var symbol := $self open &symbol.

        thePreloaded += symbol.
                        
        ^ symbol.
    ]
    
    open &singleton
    [
        var class := SingletonScope new:$self.
        
        theClasses += class.   
        
        ^ class.
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
        thePreloaded run &each:symbol
        [
            symbol eval eval.
        ].
    ]

    save &variable:literal &array_list:list
    [
        var ref := theForwards@literal.
        ($nil == ref)
            ? [ InvalidArgumentException new:("Variable cannot be resolved - " + literal) raise. ]
            ! [ list += Symbol new &literal:ref. ].
        
    ]
        
    first
        => theClasses.
}
