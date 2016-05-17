#import system.

#symbol(const,int)INFINITE = 0FFFFFFFFh.

// --- ThreadHandle ---

#class(struct:embeddable,sealed) ThreadHandle
{
    #field(int) theHandle.
    #field(rt_ptr) theActionPtr.
    
    #constructor new &func:anAction
    [
        theActionPtr := RTPointer new:anAction.

        theHandle := system'external'KERNEL32 CreateThread
                        &int:0
                        &int:0
                        &symbol_ref:system'core_routines'win_thread_start
                        &int:theActionPtr
                        &int:4
                        &int:0.
    ]

    #method start
    [
        system'external'KERNEL32 ResumeThread
            &int:theHandle.
    ]

    #method join
        <= %join &int:INFINITE.
                                                                   
    #method join &int:aTimeOut
    [
        system'external'KERNEL32 WaitForSingleObject
            &int:theHandle
            &int:aTimeOut.
    ]     
                                                                   
    #method close
    [
        
    ]
}