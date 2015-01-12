// --- Handler ---

#class(sealed,integer:4) Handle
{
    #constructor new &handle:aHandle
        => system'internal'int_copy.

    #method equal &handle:anOperand 
        => system'internal'int_equal.

    #method handle = $self.

    #method close
    [
        system'external'KERNEL32 CloseHandle
            &handle:$self.
    ]
}

// --- IntPtr ---

#class(sealed,pointer) Pointer
{
    #constructor new : anObject
        => system'internal'api_alloc_index.

    #method read &out'int:aRetVal
        => system'internal'save_int.

    #method close
        => system'internal'api_free_index.
}