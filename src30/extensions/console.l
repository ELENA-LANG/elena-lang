#define system.
#define system'routines.
//#define system'text.

// === roles ===

// --- inputOp ---

#class(role)inputOp
{
    #method write:anObject
    [
        self write &literal:(convertor toLiteral:anObject).
        
        ^ self.
    ]        
        
    #method write:anObject1:anObject2
    [
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2).
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3).
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4).
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4)
            write &literal:(convertor toLiteral:anObject5).
            
       ^ self.
    ]
    
    #method write:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4)
            write &literal:(convertor toLiteral:anObject5)
            write &literal:(convertor toLiteral:anObject6).
            
       ^ self.
    ]
    
    #method writeLine:anObject1
    [
        self
            write &literal:(convertor toLiteral:anObject1)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2
    [
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4)
            write &literal:(convertor toLiteral:anObject5)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4)
            write &literal:(convertor toLiteral:anObject5)
            write &literal:(convertor toLiteral:anObject6)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        self
            write &literal:(convertor toLiteral:anObject1)
            write &literal:(convertor toLiteral:anObject2)
            write &literal:(convertor toLiteral:anObject3)
            write &literal:(convertor toLiteral:anObject4)
            write &literal:(convertor toLiteral:anObject5)
            write &literal:(convertor toLiteral:anObject6)
            write &literal:(convertor toLiteral:anObject7)
            writeLine.
            
       ^ self.
    ]
}

// --- outputOp ---

#class(role)outputOp
{
    #method readLine : anObject
    [
        convertor save:(self readLine) &to:anObject.
        
        ^ anObject.
    ]
        
    #method readLine : anObject1 : anObject2
    [
        #var aLine := self readLine.
        
        #var aParams := aLine ~literalOp split &char:(CharValue new &int:32).
        
        convertor save:(aParams@0) &to:anObject1.
        convertor save:(aParams@1) &to:anObject2.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3
    [ 
        #var aBuffer := String new.
        
        self readLine:aBuffer.
        
        #var aParams := aBuffer ~literalOp split &char:(CharValue new &int:32).
        
        convertor save:(aParams@0) &to:anObject1.
        convertor save:(aParams@1) &to:anObject2.
        convertor save:(aParams@2) &to:anObject3.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3 : anObject4
    [
        #var aBuffer := String new.
        
        self readLine:aBuffer.
        
        #var aParams := aBuffer ~literalOp split &char:(CharValue new &int:32).
        
        convertor save:(aParams@0) &to:anObject1.
        convertor save:(aParams@1) &to:anObject2.
        convertor save:(aParams@2) &to:anObject3.
        convertor save:(aParams@3) &to:anObject4.
    ]
}

// === helper symbols ==

// --- consoleEx ---

#static consoleEx
    = ArrayExtension(RoleArray(inputOp, outputOp), console).

// --- printingLn ---

#symbol printingLn = aLine [ 'program'output writeLine:aLine ].
