#import system.
#import system'routines.
#import system'text.

// --- outputOp ---

extension outputOp
{
    writeLiteral:anObject1
    [
        self
            write:(anObject1::convertorOp toLiteral).
            
       ^ self.
    ]
    
    writeLiteral:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
            
       ^ self.
    ]
    
    writeLiteral:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral).
            
       ^ self.
    ]
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral).
            
       ^ self.
    ]
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral).
            
       ^ self.
    ]
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
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
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
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
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
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
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
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
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
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
    
    writeLiteral:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
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
    
    writeLiteral &args:Arguments
    [
        int aLength := Arguments length.
        int anIndex := 0.
        while (anIndex < aLength) 
        [
            self write:((Arguments @ anIndex)::convertorOp toLiteral).
            
            anIndex += 1.
        ].
    ]
    
    write : anObject1 : anObject2 &padding:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &padding:aWidth &with:aChar.
            
        ^ self.
    ]
    
    write : anObject1 : anObject2 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]
    
    writeLiteral : anObject1 : anObject2 &paddingLeft:aWidth
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:#32.
            
        ^ self.
    ]
    
    writeLine : anObject1 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar writeLine.
            
        ^ self.
    ]
    
    writeLine : anObject1 &paddingLeft:aWidth
        = self::outputOp writeLine:anObject1 &paddingLeft:aWidth &with:#32.
    
    writeLine : anObject1 : anObject2 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar writeLine.
            
        ^ self.
    ]

    write : anObject1 &paddingRight:aWidth
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:#32.
            
        ^ self.
    ]
    
    write : anObject1 : anObject2 &paddingRight:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    writeLine : anObject1 &paddingRight:aWidth &with:aChar
    [
        self write:(anObject1::convertorOp toLiteral) &paddingRight:aWidth &with:aChar writeLine.
            
        ^ self.
    ]
    
    writeLine : anObject1 &paddingRight:aWidth
        = self::outputOp writeLine:anObject1 &paddingRight:aWidth &with:#32.

    writeLine : anObject1 : anObject2 &paddingRight:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar writeLine.
            
        ^ self.
    ]

    write : anObject1 : anObject2 : anObject3 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    write : anObject1 : anObject2 : anObject3 &paddingRight:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    write : anObject1 : anObject2 : anObject3 : anObject4 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    write : anObject1 : anObject2 : anObject3 : anObject4 &paddingRight:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingLeft:aWidth &with:aChar.
            
        ^ self.
    ]

    write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 &paddingRight:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
        aBuffer
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            write:(anObject5::convertorOp toLiteral).
        
        self write:(aBuffer literal) &paddingRight:aWidth &with:aChar.
            
        ^ self.
    ]
    
    write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6 &paddingLeft:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
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

    write : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6 &paddingRight:aWidth &with:aChar
    [
        textbuffer aBuffer := TextBuilder new.
        
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
            
    writeLine:anObject1
    [
        self
            write:(anObject1::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    writeLine:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    writeLine:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    writeLine:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toLiteral)
            write:(anObject2::convertorOp toLiteral)
            write:(anObject3::convertorOp toLiteral)
            write:(anObject4::convertorOp toLiteral)
            writeLine.
            
       ^ self.
    ]
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5
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
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
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
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
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
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
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
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
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
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
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
    
    writeLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
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
    
    writeLine &args:Arguments
    [
        int aLength := Arguments length.
        
        int anIndex := 0.
        while (anIndex < aLength)
        [
            self write:((Arguments @ anIndex)::convertorOp toLiteral).
            
            anIndex += 1.
        ].
        
        self writeLine.
    ]
    
    writeWideLine:anObject1
    [
        self
            write:(anObject1::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    writeWideLine:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    writeWideLine:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            writeLine.
            
       ^ self.
    ]
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5
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
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
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
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
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
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
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
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
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
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
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
    
    writeWideLine:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
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
    
    writeWideLine &args:Arguments
    [
        int aLength := Arguments length.
        
        int anIndex := 0.
        while (anIndex < aLength)
        [
            self write:((Arguments @ anIndex)::convertorOp toLiteral).
            
            anIndex += 1.
        ].
        
        self writeLine.
    ]
    
    writeWide:anObject1
    [
        self
            write:(anObject1::convertorOp toWide).
            
       ^ self.
    ]
    
    writeWide:anObject1:anObject2
    [
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide).
            
       ^ self.
    ]
    
    writeWide:anObject1:anObject2:anObject3
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide).
            
       ^ self.
    ]
    
    writeWide:anObject1:anObject2:anObject3:anObject4
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide).
            
       ^ self.
    ]
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5
    [ 
        self
            write:(anObject1::convertorOp toWide)
            write:(anObject2::convertorOp toWide)
            write:(anObject3::convertorOp toWide)
            write:(anObject4::convertorOp toWide)
            write:(anObject5::convertorOp toWide).
            
       ^ self.
    ]
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6
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
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7
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
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8
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
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9
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
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10
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
    
    writeWide:anObject1:anObject2:anObject3:anObject4:anObject5:anObject6:anObject7:anObject8:anObject9:anObject10:anObject11
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
    
    writeWide&args:Arguments
    [
        int aLength := Arguments length.
        
        int anIndex := 0.
        while (anIndex < aLength)
        [
            self write:((Arguments @ anIndex)::convertorOp toLiteral).
            
            anIndex += 1.
        ].
    ]
}

// --- formatterOp ---

class $helper =
{
    stacksafe readIndex &literal:s &vint:i &vint:retVal
    [
        char ch := #0.
        int  n := 0.
        
        retVal := 0.
        s read &int:i &vchar:ch.
        while (ch != #125)
        [
            intConvertor convert &char:ch &vint:n.
            ((n >= 30h)and:(n <= 39h))?
            [                
                n := n - 30h.
                
                retVal *= 10.
                retVal += n.
            ]
            ! [ InvalidArgumentException new raise. ].
                
            i += 1.
            s read &int:i &vchar:ch.   
        ].
    ]
}.

extension formatterOp
{
    stacksafe write : pattern &args:args
    [
        literal s := pattern literal.
        textbuffer buffer := TextBuilder new.
        
        int  len := s length.
        int  i := 0.
        char ch := #0.
        int  chlen := 0.
        int  index := 0.        
        
        while (i < len)
        [
            s read &int:i &vchar:ch.
            
            (ch == #123)
                ? [ 
                    i := i + 1.
                    
                    $helper readIndex &literal:s &vint:i &vint:index.
                    buffer write:((args getAt &int:index)::convertorOp toLiteral).
                    
                    chlen := 1.
                ]
                ! [ 
                    buffer write &char:ch. 
                    ch readLength &vint:chlen.
                ].            
            
            i := i + chlen.
        ].
        
        self write:buffer.
    ]
    
    writeLine : pattern &args:args
    [
        self::formatterOp write:pattern &args:args.
        
        self writeLine.
    ]
}

// --- inputOp ---

extension inputOp
{
    readLine : anObject
    [
        (self read &literal)::convertorOp convert &to:anObject.
        
        ^ anObject.
    ]

    readLine : anObject1 : anObject2
    [
        var aLine := self read &literal.
        
        var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convert &to:anObject1.
        (aParams@1)::convertorOp convert &to:anObject2.
            
        ^ self.
    ]
        
    readLine : anObject1 : anObject2 : anObject3
    [ 
        var aLine := self read &literal.
        
        var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convert &to:anObject1.
        (aParams@1)::convertorOp convert &to:anObject2.
        (aParams@2)::convertorOp convert &to:anObject3.
            
        ^ self.
    ]
        
    readLine : anObject1 : anObject2 : anObject3 : anObject4
    [
        var aLine := self read &literal.
        
        var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convert &to:anObject1.
        (aParams@1)::convertorOp convert &to:anObject2.
        (aParams@2)::convertorOp convert &to:anObject3.
        (aParams@3)::convertorOp convert &to:anObject4.
            
        ^ self.
    ]
        
    readLine : anObject1 : anObject2 : anObject3 : anObject4 : anObject5
    [
        var aLine := self read &literal.
        
        var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convert &to:anObject1.
        (aParams@1)::convertorOp convert &to:anObject2.
        (aParams@2)::convertorOp convert &to:anObject3.
        (aParams@3)::convertorOp convert &to:anObject4.
        (aParams@4)::convertorOp convert &to:anObject5.
            
        ^ self.
    ]
        
    readLine : anObject1 : anObject2 : anObject3 : anObject4 : anObject5 : anObject6
    [
        var aLine := self read &literal.
        
        var aParams := aLine split &char:#32.
        
        (aParams@0)::convertorOp convert &to:anObject1.
        (aParams@1)::convertorOp convert &to:anObject2.
        (aParams@2)::convertorOp convert &to:anObject3.
        (aParams@3)::convertorOp convert &to:anObject4.
        (aParams@4)::convertorOp convert &to:anObject5.
        (aParams@5)::convertorOp convert &to:anObject6.
            
        ^ self.
    ]
    
    readLine &args:Arguments
    [
        var aLine := self read &literal.
        
        array aParams := aLine split &char:#32 array.
        
        int aLength := Arguments length.
        
        int anIndex := 0.
        while (anIndex < aLength)
        [
            (aParams getAt &int:anIndex)::convertorOp convert &to:(Arguments @ anIndex).
            
            anIndex += 1.
        ].
            
        ^ self.
    ]
}
