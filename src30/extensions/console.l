//#define system.
//#define system'routines.
//#define system'dynamic.
//#define system'text.
//
//// === roles ===
//
//// --- outputOp ---
//
//#class(extension:textio)outputOp
//{
//    #method write:anObject
//    [
//        self write &literal:(convertor toLiteral:anObject).
//        
//        ^ self.
//    ]        
//        
//    #method write:anObject1:anObject2
//    [
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3:anObject4
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3:anObject4:anObject5
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6).
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1
//    [
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2
//    [
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            write &literal:(convertor toLiteral:anObject7)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            write &literal:(convertor toLiteral:anObject7)
//            write &literal:(convertor toLiteral:anObject8)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            write &literal:(convertor toLiteral:anObject7)
//            write &literal:(convertor toLiteral:anObject8)
//            write &literal:(convertor toLiteral:anObject9)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            write &literal:(convertor toLiteral:anObject7)
//            write &literal:(convertor toLiteral:anObject8)
//            write &literal:(convertor toLiteral:anObject9)
//            write &literal:(convertor toLiteral:anObject10)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            write &literal:(convertor toLiteral:anObject7)
//            write &literal:(convertor toLiteral:anObject8)
//            write &literal:(convertor toLiteral:anObject9)
//            write &literal:(convertor toLiteral:anObject10)
//            write &literal:(convertor toLiteral:anObject11)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11:anObject12
//    [ 
//        self
//            write &literal:(convertor toLiteral:anObject1)
//            write &literal:(convertor toLiteral:anObject2)
//            write &literal:(convertor toLiteral:anObject3)
//            write &literal:(convertor toLiteral:anObject4)
//            write &literal:(convertor toLiteral:anObject5)
//            write &literal:(convertor toLiteral:anObject6)
//            write &literal:(convertor toLiteral:anObject7)
//            write &literal:(convertor toLiteral:anObject8)
//            write &literal:(convertor toLiteral:anObject9)
//            write &literal:(convertor toLiteral:anObject10)
//            write &literal:(convertor toLiteral:anObject11)
//            write &literal:(convertor toLiteral:anObject12)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine &args:Arguments
//    [
//        #var(type:int)aLength := Arguments length.
//        #var(type:int)anIndex := 0.
//        #loop (anIndex < aLength) ?
//        [
//            self write &literal:(convertor toLiteral:(Arguments @ anIndex)).
//            
//            anIndex := anIndex + 1.
//        ].
//    ]
//}
//
//// --- inputOp ---
//
//#class(extension:textio)inputOp
//{
//    #method readLine : anObject
//    [
//        convertor save:(self readLine) &to:anObject.
//        
//        ^ anObject.
//    ]
//
//    #method readLine : anObject1 : anObject2
//    [
//        #var aLine := self readLine.
//        
//        #var aParams := aLine ::literalOp split &char:#32.
//        
//        convertor save:(aParams@0) &to:anObject1.
//        convertor save:(aParams@1) &to:anObject2.
//    ]
//        
//    #method readLine : anObject1 : anObject2 : anObject3
//    [ 
//        #var aBuffer := String new.
//        
//        self readLine:aBuffer.
//        
//        #var aParams := aBuffer ::literalOp split &char:#32.
//        
//        convertor save:(aParams@0) &to:anObject1.
//        convertor save:(aParams@1) &to:anObject2.
//        convertor save:(aParams@2) &to:anObject3.
//    ]
//        
//    #method readLine : anObject1 : anObject2 : anObject3 : anObject4
//    [
//        #var aBuffer := String new.
//        
//        self readLine:aBuffer.
//        
//        #var aParams := aBuffer ::literalOp split &char:#32.
//        
//        convertor save:(aParams@0) &to:anObject1.
//        convertor save:(aParams@1) &to:anObject2.
//        convertor save:(aParams@2) &to:anObject3.
//        convertor save:(aParams@3) &to:anObject4.
//    ]
//        
//    #method readLine : anObject1 : anObject2 : anObject3 : anObject4 : anObject5
//    [
//        #var aBuffer := String new.
//        
//        self readLine:aBuffer.
//        
//        #var aParams := aBuffer ::literalOp split &char:#32.
//        
//        convertor save:(aParams@0) &to:anObject1.
//        convertor save:(aParams@1) &to:anObject2.
//        convertor save:(aParams@2) &to:anObject3.
//        convertor save:(aParams@3) &to:anObject4.
//        convertor save:(aParams@4) &to:anObject5.
//    ]
//        
//    #method readLine : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6
//    [
//        #var aBuffer := String new.
//        
//        self readLine:aBuffer.
//        
//        #var aParams := aBuffer ::literalOp split &char:#32.
//        
//        convertor save:(aParams@0) &to:anObject1.
//        convertor save:(aParams@1) &to:anObject2.
//        convertor save:(aParams@2) &to:anObject3.
//        convertor save:(aParams@3) &to:anObject4.
//        convertor save:(aParams@4) &to:anObject5.
//        convertor save:(aParams@5) &to:anObject6.
//    ]
//    
//    #method readLine &args:Arguments
//    [
//        #var aBuffer := String new.
//        
//        self readLine:aBuffer.
//        
//        #var(type:array) aParams := aBuffer ::literalOp split &char:#32.
//        
//        #var(type:int)aLength := Arguments length.
//        #var(type:int)anIndex := 0.
//        #loop (anIndex < aLength) ?
//        [
//            convertor save:(aParams getAt &index:anIndex) &to:(Arguments @ anIndex).
//            
//            anIndex := anIndex + 1.
//        ].
//    ]
//}
//
//// === helper symbols ==
//
//// --- consoleEx ---
//
//#static consoleEx
//    = Extension(console, outputOp, inputOp).
//
//// --- printingLn ---
//
//#symbol printingLn = aLine [ 'program'output writeLine:aLine ].
