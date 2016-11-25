#import system.
#import system'routines.
#import system'collections.
#import system'dynamic.
#import system'text.

#import extensions.
#import extensions'scripting.

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
    
    #method new &symbolToken : reference
    [
        theCallstack push:(Symbol new &literal:reference).
    ]
        
    #method new &literalToken : literal
    [
        theCallstack push:literal.
    ]
    
    #method new &messageToken : message
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
    
    #method new &messageToken : message
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
    
    #method first
        => theClasses.
}

#symbol program =
[
    // class ( method ( message = print code ( expression ( symbol = system'console message = writeLine literal = "Hello World" ) ) ) )
    
    #var factory := scriptEngine 
        load &path:"~\scripts\assembly.es" 
        load &path:"~\scripts\eson.es" 
        eval &path:"~\scripts\test.es".
    
    #var obj := factory eval:(Library new) first new.
    
    obj eval&print.
].