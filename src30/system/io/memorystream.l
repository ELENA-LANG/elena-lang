//#import system.
//
//// --- MemoryBuffer ---
//
//#symbol(const,type:int)$TextPageSize = 128.
//
//#subject(class:MemoryBuffer)membuffer.
//
//#class(sealed)MemoryBuffer
//{
//    #field(type:bytearray)theBuffer.
//    #field(type:int)theUsed.
//    #field(type:int)theCapacity.
//    
//    #constructor new &int:length
//    [
//        theBuffer := ByteArray new &int:length.
//        theCapacity := length.
//        theUsed := 0.
//    ]
//    
//    #constructor new
//        <= new &int:$TextPageSize.
//    
//    #method membuffer = $self.
//    
//    #method bytearray = theBuffer.
//    
//    #method(stacksafe) reserve &int:len
//    [
//        theCapacity := theUsed + len.
//        theCapacity := (theCapacity / $TextPageSize + 1) * $TextPageSize.
//        
//        #var(type:bytearray) aNewBuffer := ByteArray new &int:theCapacity.
//            
//        aNewBuffer write &int:0 &int:theUsed &bytearray:theBuffer.
//            
//        theBuffer := aNewBuffer.
//    ]
//    
//    #method $write &int:index &int:length &bytearray:array
//    [        
//        #var(type:bytearray,size:64)temp.
//        #var(type:int)n := index.
//        #var(type:int)len := length.
//        #var(type:int)temp_len := 64.
//        
//        #loop (len > 0)?
//        [
//            (len < 64)
//                ? [ temp_len := len. ]
//                ! [ temp_len := 64. ].
//            
//            array read &int:n &int:temp_len &bytearray:temp.
//            theBuffer write &int:theUsed &int:temp_len &bytearray:temp.
//            
//            n := n + temp_len.
//            len := len - temp_len.
//        ].
//    ]
//    
//    #method write &int:index &int:length &bytearray:array
//    [
//        (index > theUsed)
//            ? [ #throw OutOfRangeException new. ].
//        
//        (theUsed + length > theCapacity)?
//            [ $self reserve:length. ].
//            
//        (index == 0)
//            ? [
//                theBuffer write &int:theUsed &int:length &bytearray:array.
//                theUsed := theUsed + length.
//            ]
//            ! [
//                $self $write &int:index &int:length &bytearray:array
//            ].
//    ]
//    
//    #method(stacksafe)read &int:index &vint:retVal
//    [
//        (index > theUsed)
//            ? [ #throw OutOfRangeException new. ].
//        
//        theBuffer read &int:index &vint:retVal.
//    ]
//    
//    #method(stacksafe) delete &int:anIndex &int:aLength
//    [
//        #var(type:int)n := 0 - aLength.
//        #var(type:int)l := theUsed.
//        l := l - anIndex.
//        l := l - aLength.
//        
//        theBuffer move &int:(anIndex + aLength) &int:l &int:n.
//        
//        theUsed := theUsed - aLength.
//    ]
//    
//    #method(stacksafe)readLength &vint:retVal
//    [
//        retVal << theUsed.
//    ]
//    
//    #method(type:int,stacksafe,embeddable,suppress:w2)length
//    [
//        #var(type:int)retVal.
//        
//        $self readLength &vint:retVal.
//        
//        ^ retVal.
//    ]
//}