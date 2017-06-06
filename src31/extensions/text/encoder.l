#import system'io.
#import system'text.

extension(encoder) encoderOpX
{
    save &wide:literal &stream:stream
    [
        byte(255) buffer := 0.
        int       length := literal length.
        int       index := 0.
        int       buf_len := 255.
        int       subs_len := 255.
        while (index < length)
        [
            buf_len := 255.
            
            if (length < 128)
                [ subs_len := length. ];
                [ subs_len := 128. ].
                        
            self convert &int:index &vint:subs_len &wide:literal &bytearray:buffer &vint:buf_len.            
            stream write &bytearray:buffer &int:buf_len.
            
            index := index + subs_len.
        ].
    ]    
    
    save &literal:literal &stream:stream
    [
        byte(255) buffer := 0.
        int       length := literal length.
        int       index := 0.
        int       buf_len := 255.
        int       subs_len := 255.
        while (index < length)
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