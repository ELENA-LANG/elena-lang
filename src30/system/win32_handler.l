#define system.

// === types ===
#type(class:Handle) handle.

#type(type:int)     int_attributes.

// --- Handler ---

#class(type:handle,sealed,integer:4) Handle
{
    #constructor new &handle:aHandle
        <int_copy>.

    #method equal &handle:anOperand 
        <int_equal>.

    #method handle = $self.

    #method close
    [
        system'external'KERNEL32 CloseHandle
            &handle:$self.
    ]
}
