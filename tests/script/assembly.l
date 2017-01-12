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
        
    #method open &expression
    [
        #var expr := ExpressionScope new:$self.
        
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

#class MethodScope :: Scope
{
    #field theSubject.
    #field theCode.
    
    #constructor new : aParent
        <= (new : aParent)
    [
        theSubject := TextBuilder new.
    ]
        
    #method subject
        = Signature new &literal:theSubject.

    #method message
    [
        theSubject 
            append:"["
            append:"0"
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
    
    #method open &ret_expr
    [
        theCode := RetExpressionScope new:$self.
        
        ^ theCode.
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
