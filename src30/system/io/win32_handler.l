// --- Handler ---

#class Handler
{
    #field(type:int)theData.
    
    #method close
    [
        system'external'KERNEL32 CloseHandle
            &type'handle:$self.
    ]
    
    #method <= &&type'handle:$self.
}