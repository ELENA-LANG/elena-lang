#import system.
#import system'collections.
#import system'dynamic.
#import system'text.

#class Expression
{
    #field(stack)      theCallstack.
    #field(textbuffer) theMessage.
    
    #constructor new
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

#symbol program =
[
    #var expr := Expression new.
    expr new &symbol:"system'console".
    expr new &message:"writeLine".
    expr new &literal:"Hello World".
    
    #var list := ArrayList new.
    expr save &array_list:list.
    
    #var aTape := Tape new &array:list.    

    aTape eval.
].