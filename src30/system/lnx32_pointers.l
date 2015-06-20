// --- Handler ---

#class(sealed,integer:4) Handle
{
    #constructor new &handle:aHandle
        => system'internal'int_copy.

    #constructor new &int:aHandle
        => system'internal'int_copy.

    #constructor min
        => system'internal'int_init.

    #method(type:bool) equal &handle:anOperand 
        => system'internal'int_equal.

    #method handle = $self.
    
    /// Dispatches the verb to the target object with handle subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &handle:$self.
    
    /// Dispatches the verb to the target object with bytearray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &handle:$self.
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
    #field(type:int)thePointer.

    #constructor new : anObject
    [
        $self write:anObject.
    ]
    
    #method dirty_ptr = $self.
    
    #method write : anObject
        => system'internal'dptr_copy.
                
    #method append &index:aValue
    [
        thePointer := thePointer + aValue.
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

    #method read &vint:aPointer
        => system'internal'dptr_readint.

    // unsafe code
    #method int
    [
        #var(type:int)aValue.
        $self read &vint:aValue.
        
        ^ IntNumber new &int:aValue.
    ]
/*
    // unsafe code
    #method literal
    [
        #var(type:int)aLen := system'external'KERNEL32 lstrlenA &int:thePointer.
        #var(type:literal)aLiteral := LiteralValue new &length:aLen.
        system'external'kernel32 lstrcpyA &literal:aLiteral &int:thePointer.
        
        ^ aLiteral.
    ]        

    // unsafe code
    #method wide
    [
        #var(type:int)aLen := system'external'KERNEL32 lstrlenW &int:thePointer.
        #var(type:wide)aLiteral := WideLiteralValue new &length:aLen.
        system'external'kernel32 lstrcpyW &wide:aLiteral &int:thePointer.
        
        ^ aLiteral.
    ]
*/    
    /// Dispatches the verb to the target object with handle subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &dirty_ptr:$self.
    
    /// Dispatches the verb to the target object with bytearray subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &dirty_ptr:$self.    
}
                
