#define system.
#define system'text.

// --- ConsoleHelper ---

#class ConsoleHelper
{
    #field theBuffer.
    
    #constructor new
    [
        theBuffer := String new.
    ]
    
    #method readLine:anObject    
        = anObject << system'console readLine:(Variant new).        
        
    #method read &Line:anObject
        = anObject << system'console read &Line:(Variant new).        

    #method read : aValue
    [
        (0 == theBuffer Length) ?
            [ system'console read &Line:theBuffer. ].
            
        stringControl trimStart:20h &in:theBuffer.
               
        #var aLiteral := theBuffer Literal.
        #var anIndex := aLiteral indexOf &type'char:20h &type'index:0.
        
        (-1 == anIndex)
            ? [ aValue << Variant new:theBuffer. theBuffer clear. ]
            ! [ 
                aValue << Variant new:(aLiteral Substring &type'index:0 &type'length:anIndex).
                theBuffer delete &type'index:0 &type'length:anIndex.
            ].
            
        ^ aValue.
    ]

    #method write : anObject
    [
        system'console write:anObject.
    ]
        
    #method write &Line:anObject
    [
        system'console write &Line:anObject.
    ]
        
    #method writeLine:anObject
    [
        system'console writeLine:anObject.
    ]
    
    #method => system'console.
}

#static consoleEx = ConsoleHelper new.