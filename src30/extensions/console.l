#define system.
//#define system'routines.
//#define system'dynamic.
//#define system'text.

// --- outputOp ---

#class(extension) outputOp
{
    #method writeLine:anObject1
    [
        self
            write:(anObject1::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral)
            write:(anObject8::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral)
            write:(anObject8::convertorOp toLiteral)
            write:(anObject9::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral)
            write:(anObject8::convertorOp toLiteral)
            write:(anObject9::convertorOp toLiteral)
            write:(anObject10::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral)
            write:(anObject8::convertorOp toLiteral)
            write:(anObject9::convertorOp toLiteral)
            write:(anObject10::convertorOp toLiteral)
            write:(anObject11::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    #method writeLine &args:Arguments
    [
        #var(type:int)aLength.
        Arguments readLength &vint:aLength.
        
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            self write:((Arguments @ anIndex)::convertorOp toLiteral).
            
            anIndex := anIndex + 1.
        ].
    ]
}

//// --- wideOutputOp ---
//
//#symbol wideOutputOp =
//{
//    #method write:anObject
//    [
//        self write &wide:(convertor toWide:anObject).
//        
//        ^ self.
//    ]        
//        
//    #method write:anObject1:anObject2
//    [
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3:anObject4
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3:anObject4:anObject5
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5).
//            
//       ^ self.
//    ]
//    
//    #method write:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6).
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1
//    [
//        self
//            write &wide:(convertor toWide:anObject1)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2
//    [
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            write &wide:(convertor toWide:anObject7)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            write &wide:(convertor toWide:anObject7)
//            write &wide:(convertor toWide:anObject8)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            write &wide:(convertor toWide:anObject7)
//            write &wide:(convertor toWide:anObject8)
//            write &wide:(convertor toWide:anObject9)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            write &wide:(convertor toWide:anObject7)
//            write &wide:(convertor toWide:anObject8)
//            write &wide:(convertor toWide:anObject9)
//            write &wide:(convertor toWide:anObject10)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            write &wide:(convertor toWide:anObject7)
//            write &wide:(convertor toWide:anObject8)
//            write &wide:(convertor toWide:anObject9)
//            write &wide:(convertor toWide:anObject10)
//            write &wide:(convertor toWide:anObject11)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11:anObject12
//    [ 
//        self
//            write &wide:(convertor toWide:anObject1)
//            write &wide:(convertor toWide:anObject2)
//            write &wide:(convertor toWide:anObject3)
//            write &wide:(convertor toWide:anObject4)
//            write &wide:(convertor toWide:anObject5)
//            write &wide:(convertor toWide:anObject6)
//            write &wide:(convertor toWide:anObject7)
//            write &wide:(convertor toWide:anObject8)
//            write &wide:(convertor toWide:anObject9)
//            write &wide:(convertor toWide:anObject10)
//            write &wide:(convertor toWide:anObject11)
//            write &wide:(convertor toWide:anObject12)
//            writeLine.
//            
//       ^ self.
//    ]
//    
//    #method writeLine &args:Arguments
//    [
//        #var(type:int)aLength.
//        Arguments readLength &vint:aLength.
//        #var(type:int)anIndex := 0.
//        #loop (anIndex < aLength) ?
//        [
//            self write &wide:(convertor toWide:(Arguments @ anIndex)).
//            
//            anIndex := anIndex + 1.
//        ].
//    ]
//}.
//
//// --- inputOp ---
//
//#symbol inputOp =
//{
//    #method readLine : anObject
//    [
//        convertor save:(self read &literal) &to:anObject.
//        
//        ^ anObject.
//    ]
//
//    #method readLine : anObject1 : anObject2
//    [
//        #var aLine := self read &literal.
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
//        #var(type:vint)aLength.        
//        Arguments readLength &vint:aLength.
//        
//        #var(type:int)anIndex := 0.
//        #loop (anIndex < aLength) ?
//        [
//            convertor save:(aParams getAt &index:anIndex) &to:(Arguments @ anIndex).
//            
//            anIndex := anIndex + 1.
//        ].
//    ]
//}.
//
//// --- wideInputOp ---
//
//#symbol wideInputOp =
//{
//    #method readLine : anObject
//    [
//        convertor save:(self read &wide) &to:anObject.
//        
//        ^ anObject.
//    ]
//
//    #method readLine : anObject1 : anObject2
//    [
//        #var aLine := self read &wide.
//        
//        #var aParams := aLine ::literalOp split &char:#32.
//        
//        convertor save:(aParams@0) &to:anObject1.
//        convertor save:(aParams@1) &to:anObject2.
//    ]
//        
//    #method readLine : anObject1 : anObject2 : anObject3
//    [ 
//        #var aBuffer := WideString new.
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
//        #var aBuffer := WideString new.
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
//        #var aBuffer := WideString new.
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
//        #var aBuffer := WideString new.
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
//        #var aBuffer := WideString new.
//        
//        self readLine:aBuffer.
//        
//        #var(type:array) aParams := aBuffer ::literalOp split &char:#32.
//        
//        #var(type:int)aLength.        
//        Arguments readLength &vint:aLength.
//        
//        #var(type:int)anIndex := 0.
//        #loop (anIndex < aLength) ?
//        [
//            convertor save:(aParams getAt &index:anIndex) &to:(Arguments @ anIndex).
//            
//            anIndex := anIndex + 1.
//        ].
//    ]
//}.
//
//// === helper symbols ==
//
//// --- consoleEx ---
//
//#static consoleEx
//    = Extension(console, 'outputOp, 'inputOp).
//
//// --- printingLn ---
//
//#symbol printingLn = aLine [ 'program'output writeLine:aLine ].
