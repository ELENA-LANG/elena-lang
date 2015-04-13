// --- Handler ---

#class(sealed,integer:4) Handle
{
    #constructor new &handle:aHandle
        => system'internal'int_copy.

    #constructor min
        => system'internal'int_init.

    #method(type:bool) equal &handle:anOperand 
        => system'internal'int_equal.

    #method handle = $self.

    #method close
    [
        system'external'KERNEL32 CloseHandle
            &handle:$self.
    ]
}

// --- RTPointer ---

#class(sealed,pointer) RTPointer
{
    #constructor new : anObject
        => system'internal'api_alloc_index.

    #constructor new &index:anIndex
        => system'internal'int_copy.

    #method read &vint:aRetVal
        => system'internal'save_int.

    #method index
    [
        #var(type:vint)anIndex.
        $self read &vint:anIndex.
        
        ^ IntNumber new &int:anIndex.
    ]

    #method get
        => system'internal'ptr_get.

    #method close
        => system'internal'api_free_index.
}

// --- UnsafePointer ---

#class(sealed,pointer) UnsafePointer
{
    #method write &literal:aLiteral
        => system'internal'dptr_copy.
        
    #method write &wide:aLiteral
        => system'internal'dptr_copy.
    
    #method write &handle:aLiteral
        => system'internal'int_copy.
    
    #method write &shortarray:anArray
        => system'internal'dptr_copy.
    
    #method write &bytearray:anArray
        => system'internal'dptr_copy.
    
    #method append &int:aValue
        => system'internal'dptr_append.
    
    #method read &vint:aRetVal
        => system'internal'save_int.    
}