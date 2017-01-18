#import system.
#import system'routines.
#import system'collections.
#import system'text.
#import system'dynamic.
#import extensions.

#class Scope
{
    #field theParent.
    
    #constructor new : parent
    [
        theParent := parent.
    ]

    #method save &variable:literal &array_list:list
    [
        theParent save &variable:literal &array_list:list.
    ]
            
    #method close = theParent.        
}

#class ExpressionScope :: Scope
{
    #field(stack)      theCallstack.
    #field(textbuffer) theMessage.
    #field(bool)       theCallExpr.
    
    #constructor new : aParent
        <= (new : aParent)
    [
        theCallstack := Stack new.
        theMessage := TextBuilder new.
        theCallExpr := false.
    ]

    #method new &numericToken : literal
    [
        theCallstack push:{ save &array_list:list [ list += literal toInt. ] }.
    ]
        
    #method new &identToken : literal
    [
        theCallstack push:
        { 
            save &array_list:list 
            [ 
                theParent save &variable:literal &array_list:list.
            ] 
        }.
    ]        
        
    #method open &expression
    [
        #var expr := ExpressionScope new:$self.
        
        theCallstack push:expr.
        
        ^ expr.
    ]

    #method open &singleton
    [
        #var expr := SingletonScope new:$self.
        
        theCallstack push:expr.
        
        ^ expr.
    ]
                
    #method save &array_list:retVal
    [
        theCallExpr
        ? [
            #var(int)length := theCallstack length.
            length -= 1.
            
            theMessage write &literal:"[".
            theMessage write &int:length.
            theMessage write &literal:"]".
            
            #loop (theCallstack is &empty)!
            [
                theCallstack pop save &array_list:retVal.
            ].
            
            retVal += Message new &literal:(theMessage literal).
        ]
        ! [
            theCallstack pop save &array_list:retVal.
        ].
    ]    
}

#class RetExpressionScope :: ExpressionScope
{
    #constructor new : aParent
        <= (new : aParent)
    [
    ]
}

#class CodeScope :: Scope
{
    #field theStatements.
    
    #constructor new : aParent
        <= (new : aParent)
    [
        theStatements := List new.
    ]
    
    #method save &array_list:retVal
    [
        theStatements run &each:statement
        [
            statement save &array_list:retVal.
        ].
    ]

    #method open &ret_expr
    [
        #var expr := RetExpressionScope new:$self.
        
        theStatements += expr.
        
        ^ expr.
    ]
        
    #method open &expression
    [
        #var expr := ExpressionScope new:$self.
        
        theStatements += expr.
        
        ^ expr.
    ]
}

#class MethodScope :: Scope
{
    #field theSubject.
    #field theCode.
    #field theParameters.
    
    #constructor new : aParent
        <= (new : aParent)
    [
        theSubject := TextBuilder new.
        theParameters := List new.
    ]
        
    #method subject
        = Signature new &literal:theSubject.

    #method message
    [
        theSubject 
            append:"["
            append:(theParameters length literal)
            append:"]".
            
        ^ Message new &literal:theSubject.            
    ]

    #method code
    [
        #var list := ArrayList new.
        
        theCode save &array_list:list.
        
        ^ Tape new &array:list.
    ]    
        
    #method new &messageToken : message
    [
        (theSubject is &empty)
            ! [ theSubject write:"&". ].
        
        theSubject write:message.
    ]    
    
    #method new &paramToken : param
    [
        theParameters += KeyValue new &key:param &object:(theParameters length + 2).
    ]
    
    #method open &ret_expr
    [
        theCode := RetExpressionScope new:$self.
        
        ^ theCode.
    ]
    
    #method open &code
    [
        theCode := CodeScope new:$self.
        
        ^ theCode.
    ]
    
    #method save &variable:literal &array_list:list
    [
        #var param := theParameters seek &each:x [ x equal:literal ].
        ($nil != param)
            ? [
                list += param object.
                list += %"tapeOp.var[]".
                
            ]
            ! [
                $super save &variable:literal &array_list:list.
            ].
    ]
}

#class BaseClassScope :: Scope
{
    #field theMethods.
    
    #constructor new : aParent
        <= (new : aParent)
    [
        theMethods := List new.
    ]

    #method open &method
    [
        #var method := MethodScope new:$self.
        
        theMethods += method.
        
        ^ method.
    ]        
}

#class SingletonScope :: BaseClassScope
{
    #constructor new : parent
        <= (new:parent)
    [
    ]
    
    #method save &array_list:retVal
    [
        retVal += $self eval.
    ]
    
    #method eval
    [
        #var list := List new.
        
        theMethods run &each:method
        [
            list += method message.
            list += method code.
        ].
        
        ^ Class new &array:(list toArray).        
    ]
}

#class SymbolBody :: Scope
{
    #field theSingleton.
    
    #constructor new : parent
        <= (new:parent)
    [
    ]
    
    #method open &singleton
    [
        theSingleton := SingletonScope new:$self.
        
        ^ theSingleton.
    ]
    
    #method eval
    [
        ^ theSingleton eval.
    ]
}

#class SymbolScope :: Scope
{
    #field theBody.
    
    #constructor new : parent
        <= (new:parent)
    [
    ]
    
    #method new &identToken:name
    [
    ]
    
    #method open &expression
    [
        theBody := SymbolBody new:$self.
        
        ^ theBody.
    ]
    
    #method eval
        => theBody.
}

#class Library
{
    #field theClasses.
    
    #constructor new
    [
        theClasses := List new.
    ]
    
    #method open &symbol
    [
        #var symbol := SymbolScope new:$self.
        
        theClasses += symbol.
        
        ^ symbol.
    ]
    
//    #method open &class
//    [
//        #var class := ClassScope new:$self.
//        
//        theClasses += class.
//        
//        ^ class.
//    ]
    
    #method first
        => theClasses.
}
