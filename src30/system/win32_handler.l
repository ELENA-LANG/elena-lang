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
}

// --- RTPointer ---

#class(sealed,struct:embeddable) RTPointer
{
    #field(type:int)theIndex.
    
    #constructor new : anObject
        => system'internal'api_alloc_index.

    #method rt_ptr = $self.            
                        
    #method index
    [
        ^ IntNumber new &int:theIndex.
    ]
    
    #method readIndex &vint:aRetVal
    [
        aRetVal << theIndex.
    ]
        
    #method get
        => system'internal'ptr_get.

    #method free
        => system'internal'api_free_index.
}

// --- UnsafePointer ---

#class(sealed,struct:embeddable) UnsafePointer
{
    #field(type:int)theIndex.

    #constructor new : anObject
    [
        $self write:anObject.
    ]
    
    #method dirty_ptr = $self.
    
    #method write : anObject
        => system'internal'dptr_copy.
                
    #method append &int:aValue
    [
        theIndex := theIndex + aValue.
    ]                            
                            
    #method write &handle:anObject
        => system'internal'int_copy.
    
    #method write &literal:aLiteral
        => system'internal'dptr_copy.
    
    #method write &wide:aLiteral
        => system'internal'dptr_copy.
    
    #method write &shortarray:anArray
        => system'internal'dptr_copy.
    
    #method write &bytearray:anArray
        => system'internal'dptr_copy.
}