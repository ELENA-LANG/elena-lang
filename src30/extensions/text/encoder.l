#import system.
#import system'text.

// ==== ver 3.4.1 ===

#class(extension)encoderOp
{
    #method toByteArray        
        = self cast:%eval &to:
        {
            literal : s
                = UTF8Encoder toByteArray &int:0 &int:(s length) &literal:s.
            
            wide : s
                = UTF16Encoder toByteArray &int:0 &int:(s length) &wide:s.
        }.
}

#class(extension:encoder)encoderOpX
{
    #method save &wide:literal &stream:stream
    [
        #var(type:encoder)encoder := self encoder.
        
        #var(type:bytearray,size:255)buffer.
        #var(type:int)length := literal length.
        #var(type:int)index := 0.
        #var(type:int)buf_len := 255.
        #var(type:int)subs_len := 255.
        #loop (index < length)?
        [
            buf_len := 255.
            
            (length < 128)
                ? [ subs_len := length. ]  
                ! [ subs_len := 128. ].
                        
            encoder convert &int:index &vint:subs_len &wide:literal &bytearray:buffer &vint:buf_len.            
            stream write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]    
    
    #method save &literal:literal &stream:stream
    [
        #var(type:encoder)encoder := self encoder.
        
        #var(type:bytearray,size:255)buffer.
        #var(type:int)length := literal length.
        #var(type:int)index := 0.
        #var(type:int)buf_len := 255.
        #var(type:int)subs_len := 255.
        #loop (index < length)?
        [
            buf_len := 255.
            
            (length < 128)
                ? [ subs_len := length. ]  
                ! [ subs_len := 128. ].
                        
            encoder convert &int:index &vint:subs_len &literal:literal &bytearray:buffer &vint:buf_len.            
            stream write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]    
}