// --- BaseApplication ---

#class BaseApplication
{
}

// --- SDIApplication ---

#class SDIApplication : BaseApplication
{
    #field theMainWindow.
    
    #constructor new
    [
        theMainWindow := 'SDIWindow new.
    ]
    
//    #method start
//    [
//        #var(type:bytearray,size:28)theMsg.
//        
//        #loop system'external'KERNEL32 GetMessage &bytearray:theMsg &handle:0 &int:0 &int:0 ?
//        [
//            system'external'KERNEL32 TranslateMessage &bytearray:theMsg.
//            system'external'KERNEL32 DispatchMessage &bytearray:theMsg.
//        ].
//    ]
}

#static program = 'Application new.