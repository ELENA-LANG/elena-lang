#import system.
#import system'routines.
#import system'collections.
#import system'dynamic.
#import system'text.

#import extensions.

#class Scope
{
    #field theParent.
    
    #constructor new : aParent
    [
        theParent := aParent.
    ]
    
    #method close = theParent.    
}

#class ExpressionScope :: Scope
{
    #field(stack)      theCallstack.
    #field(textbuffer) theMessage.
    
    #constructor new : aParent
        <= (new : aParent)
    [
        theCallstack := Stack new.
        theMessage := TextBuilder new.
    ]
    
    #method new &symbol : reference
    [
        theCallstack push:(Symbol new &literal:reference).
    ]
        
    #method new &literal : literal
    [
        theCallstack push:literal.
    ]
    
    #method new &message : message
    [
        (theMessage is &empty)
            ! [ theMessage write:"&". ].
        
        theMessage write:message.
    ]
    
    #method save &array_list:retVal
    [
        #var(int)length := theCallstack length.
        length -= 1.
        
        theMessage write &literal:"[".
        theMessage write &int:length.
        theMessage write &literal:"]".
        
        #loop (theCallstack is &empty)!
        [
            retVal += theCallstack pop.
        ].
        
        retVal += Message new &literal:(theMessage literal).
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
    
    #method open &expression
    [
        #var expr := ExpressionScope new:$self.
        
        theStatements += expr.
        
        ^ expr.
    ]
}

#class MethodScope :: Scope
{
    #field theCode.
    #field theSubject.

    #constructor new : aParent
        <= (new : aParent)
    [
        theSubject := TextBuilder new.
    ]
    
    #method new &message : message
    [
        (theSubject is &empty)
            ! [ theSubject write:"&". ].
        
        theSubject write:message.
    ]
        
    #method code
    [
        #var list := ArrayList new.
        
        theCode save &array_list:list.
        
        ^ Tape new &array:list.
    ]    
    
    #method subject
        = Signature new &literal:theSubject.
    
    #method open &code
    [
        theCode := CodeScope new:$self.
        
        ^ theCode.
    ]
}

#class ClassScope :: Scope
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
    
    #method new
    [
        #var list := List new.
        
        theMethods run &each:method
        [
            list += method subject.
            list += method code.
        ].
        
        ^ Struct new &array:(list toArray).
    ]
}

#class Library
{
    #field theClasses.
    
    #constructor new
    [
        theClasses := List new.
    ]
    
    #method open &class
    [
        #var class := ClassScope new:$self.
        
        theClasses += class.
        
        ^ class.
    ]
}

#symbol program =
[
    #var factory := Tape(
        "Hello World", 
        "writeLine", 
        "system'console", 
        "print",
        Library,
        %"new[0]", 
        %"open&class[0]", 
        %"open&method[0]", 
        %"new&message[1]",
        %"open&code[0]",
        %"open&expression[0]",
        %"new&symbol[1]",
        %"new&message[1]",
        %"new&literal[1]",
        %"close[0]",
        %"close[0]",
        %"close[0]",
        %"new[0]").
    
    #var obj := factory eval.
    
//    #var lib := Library new.
//    #var class := lib open &class.
//    #var method := class open &method.
//    method new &message:"print".
//    #var code := method open &code.
//    #var statement1 := code open &expression.
//    statement1 new &symbol:"system'console".
//    statement1 new &message:"writeLine".
//    statement1 new &literal:"Hello World".
//    
//    #var obj := class new.
    
    obj eval&print.
].