#define system.
#define system'text.
#define extensions.
#define extensions'io.

// === roles ===

// --- EPrintable ---

#class(role) EPrintable
{
    #method type'widestr
        = literalConvertor convert::self.
}

// === helper symbols ==

// --- consoleEx ---

#symbol consoleEx =
{
    write : anObject
    [
        console write:(literalConvertor convert::anObject).
    ]
    
    write:anObject1:anObject2
    [
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2).
    ]
    
    write:anObject1:anObject2:anObject3
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3).
    ]
    
    write:anObject1:anObject2:anObject3:anObject4
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            write:(literalConvertor convert::anObject4).
    ]
    
    write:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            write:(literalConvertor convert::anObject4)
            write:(literalConvertor convert::anObject5).
    ]
    
    write:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            write:(literalConvertor convert::anObject4)
            write:(literalConvertor convert::anObject5)
            write:(literalConvertor convert::anObject6).
    ]
    
    writeLine
    [
        console writeLine.
    ]
    
    writeLine : anObject
    [
        console writeLine:(literalConvertor convert::anObject).
    ]
    
    writeLine:anObject1:anObject2
    [
        console
            write:(literalConvertor convert::anObject1)
            writeLine:(literalConvertor convert::anObject2).
    ]
    
    writeLine:anObject1:anObject2:anObject3
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            writeLine:(literalConvertor convert::anObject3).
    ]
    
    writeLine:anObject1:anObject2:anObject3:anObject4
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            writeLine:(literalConvertor convert::anObject4).
    ]
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            write:(literalConvertor convert::anObject4)
            writeLine:(literalConvertor convert::anObject5).
    ]
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            write:(literalConvertor convert::anObject4)
            write:(literalConvertor convert::anObject5)
            writeLine:(literalConvertor convert::anObject6).
    ]
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        console
            write:(literalConvertor convert::anObject1)
            write:(literalConvertor convert::anObject2)
            write:(literalConvertor convert::anObject3)
            write:(literalConvertor convert::anObject4)
            write:(literalConvertor convert::anObject5)
            write:(literalConvertor convert::anObject6)
            writeLine:(literalConvertor convert::anObject7).
    ]

    readLine
        = system'console readLine.
            
    readLine : anObject
    [
        convertControl save:(system'console readLine) &to:anObject.
        
        ^ anObject.
    ]
        
    readLine : anObject1 : anObject2
    [
        #var aBuffer := String new.
        
        system'console readLine:aBuffer.
        
        #var aParams := stringControl split:aBuffer &by:(CharValue new &type'int:32).
        
        convertControl save:(aParams@0) &to:anObject1.
        convertControl save:(aParams@1) &to:anObject2.
    ]
        
    readLine : anObject1 : anObject2 : anObject3
    [
        #var aBuffer := String new.
        
        system'console readLine:aBuffer.
        
        #var aParams := stringControl split:aBuffer &by:(CharValue new &type'int:32).
        
        convertControl save:(aParams@0) &to:anObject1.
        convertControl save:(aParams@1) &to:anObject2.
        convertControl save:(aParams@2) &to:anObject3.
    ]
        
    readLine : anObject1 : anObject2 : anObject3 : anObject4
    [
        #var aBuffer := String new.
        
        system'console readLine:aBuffer.
        
        #var aParams := stringControl split:aBuffer &by:(CharValue new &type'int:32).
        
        convertControl save:(aParams@0) &to:anObject1.
        convertControl save:(aParams@1) &to:anObject2.
        convertControl save:(aParams@2) &to:anObject3.
        convertControl save:(aParams@3) &to:anObject4.
    ]
    
    readChar
        = system'console readChar.
}.

// --- printingLn ---

#symbol printingLn = &&:aLine [ 'program'output writeLine:aLine ].
