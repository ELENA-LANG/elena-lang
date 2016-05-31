#import system.
#import system'text.
#import system'io.
#import extensions.
#import extensions'dynamic.

#symbol(const,int)INTRODUCTION = 1.
#symbol(const,int)SEND         = 2.
#symbol(const,int)TRANSLATE    = 3.

#class BaseClient
{
    #field(membuffer)theBuffer.
    
    #constructor new
    [
        theBuffer := MemoryBuffer new.
    ]    

    #method(stacksafe)write &bytearray:buffer &int:length
    [
        theBuffer write &int:0 &int:length &bytearray:buffer.
    ]
    
    #method readCommand
    [
        #var(int)len := theBuffer length.
        (len > 0)?
        [
            #var(int)package_len.
            theBuffer read &int:0 &vint:package_len.
            
            (package_len + 4 <= len)?
            [
                #var literal := UTF8Encoding toLiteral &int:4 &int:package_len &bytearray:(theBuffer bytearray).
                
                #var llen := literal length.
                
                theBuffer delete &int:0 &int:(package_len + 4).
                
                ^ literal fromJson.
            ].
        ].
        
        ^ $nil.
    ]        
}