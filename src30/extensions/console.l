#define system.
#define system'routines.
//#define system'text.
//#define extensions.

// === roles ===

// --- printable ---

#class(role)printable
{
    #method write:anObject
    [
        self write &literal:(convertor toLiteral:anObject).
        
        ^ self.
    ]        
        
    #method write:anObject1:anObject2
    [
        self
            write:anObject1
            write:anObject2.
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3.
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            write:anObject4.
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            write:anObject4
            write:anObject5.
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            write:anObject4
            write:anObject5
            write:anObject6.
            
       ^ self.
    ]
    
    #method writeLine:anObject1
    [
        self
            write:anObject1
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2
    [
        self
            write:anObject1
            writeLine:anObject2.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3
    [ 
        self
            write:anObject1
            write:anObject2
            writeLine:anObject3.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            writeLine:anObject4.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            write:anObject4
            writeLine:anObject5.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            write:anObject4
            write:anObject5
            writeLine:anObject6.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        self
            write:anObject1
            write:anObject2
            write:anObject3
            write:anObject4
            write:anObject5
            write:anObject6
            writeLine:anObject7.
            
       ^ self.
    ]
}

// --- readable ---

#class(role)readable
{
    #method readLine : anObject
    [
        convertor save:(Variant new:(self readLine)) &to:anObject.
        
        ^ anObject.
    ]
        
    #method readLine : anObject1 : anObject2
    [
        #var aLine := self readLine.
        
        #var aParams := aLine ~literalOp split &char:(CharValue new &int:32).
        
        convertor save:(Variant new:(aParams@0)) &to:anObject1.
        convertor save:(Variant new:(aParams@1)) &to:anObject2.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3
    [ 
        #var aBuffer := String new.
        
        self readLine:aBuffer.
        
        #var aParams := aBuffer ~literalOp split &char:(CharValue new &int:32).
        
        convertor save:(Variant new:(aParams@0)) &to:anObject1.
        convertor save:(Variant new:(aParams@1)) &to:anObject2.
        convertor save:(Variant new:(aParams@2)) &to:anObject3.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3 : anObject4
    [
        #var aBuffer := String new.
        
        self readLine:aBuffer.
        
        #var aParams := aBuffer ~literalOp split &char:(CharValue new &int:32).
        
        convertor save:(Variant new:(aParams@0)) &to:anObject1.
        convertor save:(Variant new:(aParams@1)) &to:anObject2.
        convertor save:(Variant new:(aParams@2)) &to:anObject3.
        convertor save:(Variant new:(aParams@3)) &to:anObject4.
    ]
}

// === helper symbols ==

// --- consoleEx ---

#static consoleEx
    = ArrayGroupExtension(Roles(printable, readable), console).

//#symbol consoleEx =
//{
//    readLine
//        = system'console readLine.
//            
//    
//    readChar
//        = system'console readChar.
//}.

// --- printingLn ---

#symbol printingLn = aLine [ 'program'output writeLine:aLine ].
