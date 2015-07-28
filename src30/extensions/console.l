#define system.
#define system'routines.
//#define system'dynamic.
#define system'text.

// --- outputOp ---

#class(extension) outputOp
{
    #method writeLiteral:anObject1
    [
        self
            write:(anObject1::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral)
            write:(anObject7::convertorOp toLiteral)
            write:(anObject8::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
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
            write:(anObject9::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
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
            write:(anObject10::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
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
            write:(anObject11::convertorOp toLiteral).
            
       ^ self.
    ]
    
    #method writeLiteral &args:Arguments
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
    
    #method write : anObject1 : anObject2 &padding:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &padding:aWidth &with:aChar.
            
        ^ self.
    ]
    
    #method write : anObject1 : anObject2 &paddingLeft:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    #method write : anObject1 &paddingRight:aWidth
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:#32.
            
        ^ self.
    ]
    
    #method write : anObject1 : anObject2 &paddingRight:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    #method write : anObject1 : anObject2 : anObject3 &paddingLeft:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    #method write : anObject1 : anObject2 : anObject3 &paddingRight:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    #method write : anObject1 : anObject2 : anObject3 : anObject4 &paddingLeft:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    #method write : anObject1 : anObject2 : anObject3 : anObject4 &paddingRight:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    #method write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 &paddingLeft:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    #method write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 &paddingRight:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    #method write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6 &paddingLeft:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    #method write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6 &paddingRight:aWidth &with:aChar
    [
        #var(type:textbuffer)aBuffer := TextBuffer new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral)
            write:(anObject6::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
            
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
        
        self writeLine.
    ]
    
    #method writeWideLine:anObject1
    [
        self
            write:(anObject1::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            write:(anObject9::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            write:(anObject9::convertorOp toWide)
            write:(anObject10::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            write:(anObject9::convertorOp toWide)
            write:(anObject10::convertorOp toWide)
            write:(anObject11::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    #method writeWideLine &args:Arguments
    [
        #var(type:int)aLength.
        Arguments readLength &vint:aLength.
        
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            self write:((Arguments @ anIndex)::convertorOp toLiteral).
            
            anIndex := anIndex + 1.
        ].
        
        self writeLine.
    ]
    
    #method writeWide:anObject1
    [
        self
            write:(anObject1::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            write:(anObject9::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            write:(anObject9::convertorOp toWide)
            write:(anObject10::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide)
            write:(anObject6::convertorOp toWide)
            write:(anObject7::convertorOp toWide)
            write:(anObject8::convertorOp toWide)
            write:(anObject9::convertorOp toWide)
            write:(anObject10::convertorOp toWide)
            write:(anObject11::convertorOp toWide).
            
       ^ self.
    ]
    
    #method writeWide&args:Arguments
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

// --- inputOp ---

#class(extension) inputOp
{
    #method readLine : anObject
    [
        (self read &literal)::convertorOp convertTo:anObject.
        
        ^ anObject.
    ]

    #method readLine : anObject1 : anObject2
    [
        #var aLine := self read &literal.
        
        #var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convertTo:anObject1.
        (aParams@1)::convertorOp convertTo:anObject2.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3
    [ 
        #var aLine := self read &literal.
        
        #var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convertTo:anObject1.
        (aParams@1)::convertorOp convertTo:anObject2.
        (aParams@2)::convertorOp convertTo:anObject3.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3 : anObject4
    [
        #var aLine := self read &literal.
        
        #var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convertTo:anObject1.
        (aParams@1)::convertorOp convertTo:anObject2.
        (aParams@2)::convertorOp convertTo:anObject3.
        (aParams@3)::convertorOp convertTo:anObject4.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3 : anObject4 : anObject5
    [
        #var aLine := self read &literal.
        
        #var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convertTo:anObject1.
        (aParams@1)::convertorOp convertTo:anObject2.
        (aParams@2)::convertorOp convertTo:anObject3.
        (aParams@3)::convertorOp convertTo:anObject4.
        (aParams@4)::convertorOp convertTo:anObject5.
    ]
        
    #method readLine : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6
    [
        #var aLine := self read &literal.
        
        #var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convertTo:anObject1.
        (aParams@1)::convertorOp convertTo:anObject2.
        (aParams@2)::convertorOp convertTo:anObject3.
        (aParams@3)::convertorOp convertTo:anObject4.
        (aParams@4)::convertorOp convertTo:anObject5.
        (aParams@5)::convertorOp convertTo:anObject6.
    ]
    
    #method readLine &args:Arguments
    [
        #var aLine := self read &literal.
        
        #var(type:array) aParams := aLine split &char:#32.
        
        #var(type:vint)aLength.        
        Arguments readLength &vint:aLength.
        
        #var(type:int)anIndex := 0.
        #loop (anIndex < aLength) ?
        [
            (aParams getAt &int:anIndex)::convertorOp convertTo:(Arguments @ anIndex).
            
            anIndex := anIndex + 1.
        ].
    ]
}

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
