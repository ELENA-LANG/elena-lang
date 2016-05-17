#import system.
#import system'io.
#import system'text.

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
        #var(byte:255)buffer.
        #var(int)length := literal length.
        #var(int)index := 0.
        #var(int)buf_len := 255.
        #var(int)subs_len := 255.
        #loop (index < length)?
        [
            buf_len := 255.
            
            (length < 128)
                ? [ subs_len := length. ]  
                ! [ subs_len := 128. ].
                        
            self convert &int:index &vint:subs_len &wide:literal &bytearray:buffer &vint:buf_len.            
            stream write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]    
    
    #method save &literal:literal &stream:stream
    [
        #var(byte:255)buffer.
        #var(int)length := literal length.
        #var(int)index := 0.
        #var(int)buf_len := 255.
        #var(int)subs_len := 255.
        #loop (index < length)?
        [
            buf_len := 255.
            
            (length < 128)
                ? [ subs_len := length. ]  
                ! [ subs_len := 128. ].
                        
            self convert &int:index &vint:subs_len &literal:literal &bytearray:buffer &vint:buf_len.            
            stream write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]    
}