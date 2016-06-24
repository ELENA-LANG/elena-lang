#import system.

// --- Wrap ---

#class(dynamic,group) Wrap
{
    #field $item.
    
    #method => system'core_routines'__wrap.
}

// --- Extension ---

#class(dynamic,group) Extension
{
    #field $item.
    
    #method => system'core_routines'__extension.
}

// --- Group ---

#class(dynamic,group) Group
{
    #field $item.
    
    #constructor(stacksafe) $new &int:aLength
        = object:aLength.
        
    #constructor new &args:objects
        <= ($new &int:(objects length))
    [
        $self setFields &args:objects.
    ]
    
    #method => system'core_routines'__group.
}

// --- extensions ---

#class(extension) dynamicOp
{
    #method mix &into:role
        = Extension(role, self).
        
    #method group &with:object
    [
        (Group == self class)
            ? [ 
                #var(int)length := self fieldCount.
            
                #var retVal := Group $new &int:(length + 1).
                                
                #var(int)i := 0.
                
                #loop (i < length)?
                [
                    retVal setField &int:i &object:(self getField &int:i).
                    
                    i += 1.
                ].
                
                retVal setField &int:length &object:object.
                
                ^ retVal.
            ]
            ! [ ^ Group new &args:self:object. ].
    ]        
}
