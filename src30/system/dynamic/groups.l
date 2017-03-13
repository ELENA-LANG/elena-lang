#import system.

// --- Wrap ---

dynamic group Wrap
{
    object $item.
    
    dispatch => system'core_routines'__wrap.
}

// --- Extension ---

dynamic group Extension
{
    object $item.
    
    dispatch => system'core_routines'__extension.
}

// --- Group ---

dynamic group Group
{
    object $item.
    
    stacksafe constructor $new &int:aLength
        = object(aLength).
        
    constructor new &args:objects
        <= $new &int:(objects length);
    [
        $self setFields &args:objects.
    ]
    
    dispatch => system'core_routines'__group.
}

// --- Cast ---

sealed arrayof(object) group Cast
{
    stacksafe constructor $new &int:aLength
        = object(aLength).
        
    constructor new &args:objects
        <= $new &int:(objects length);
    [
        $self setFields &args:objects.
    ]
    
    dispatch => system'core_routines'__castover.
}

// --- extensions ---

extension dynamicOp
{
    mix &into:role
        = Extension(role, self).
        
    group &with:object
    [
        ($nil == object)
            ? [ ^ self. ].
        
        (Group == self class)
            ? [ 
                int length := self fieldCount.
            
                var retVal := Group $new &int:(length + 1).
                                
                int i := 0.
                
                while (i < length)
                [
                    retVal setField &int:i &object:(self getField &int:i).
                    
                    i += 1.
                ].
                
                retVal setField &int:length &object:object.
                
                ^ retVal.
            ]
            ! [ 
                (Group == object class)
                    ? [
                        int length := object fieldCount.
                    
                        var retVal := Group $new &int:(length + 1).
                                        
                        int i := 0.
                        
                        while (i < length)
                        [
                            retVal setField &int:(i + 1) &object:(self getField &int:i).
                            
                            i += 1.
                        ].
                        
                        retVal setField &int:0 &object:object.
                        
                        ^ retVal.
                    ]
                    ! [ ^ Group new &args:(self,object). ].            
            ].
    ]        
        
    cast &with:object
    [
        ($nil == object)
            ? [ ^ self. ].
        
        (self class == Cast)
            ? [ 
                int length := self fieldCount.
            
                var retVal := Cast $new &int:(length + 1).
                                
                int i := 0.
                
                while (i < length)
                [
                    retVal setField &int:i &object:(self getField &int:i).
                    
                    i += 1.
                ].
                
                retVal setField &int:length &object:object.
                
                ^ retVal.
            ]
            ! [ 
                (object class == Cast)
                    ? [
                        int length := object fieldCount.
                    
                        var retVal := Cast $new &int:(length + 1).
                                        
                        int i := 0.
                        
                        while (i < length)
                        [
                            retVal setField &int:(i + 1) &object:(self getField &int:i).
                            
                            i += 1.
                        ].
                        
                        retVal setField &int:0 &object:object.
                        
                        ^ retVal.
                    ]
                    ! [ ^ Cast new &args:(self,object). ].            
            ].
    ]        
}
