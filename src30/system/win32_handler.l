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
