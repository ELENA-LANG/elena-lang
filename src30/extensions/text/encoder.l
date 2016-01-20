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
