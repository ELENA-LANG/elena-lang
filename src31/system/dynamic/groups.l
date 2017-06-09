// --- Wrap ---

dynamic mixin Wrap
{
    object $item.
    
    dispatch => system'core_routines'__wrap.
}

// --- Extension ---

dynamic mixin Extension
{
    object $item.
    
    dispatch => system'core_routines'__extension.
}

// --- Group ---

dynamic mixin Group
{
    object $item.
    
    stacksafe constructor $new int:aLength
        = object(aLength).
        
    constructor new args:objects
        <= $new int:(objects length);
    [
        $self setFields args:objects.
    ]
    
    dispatch => system'core_routines'__group.
}

// --- Cast ---

sealed mixin Cast :: arrayof(object)
{
    stacksafe constructor $new int:aLength
        = object(aLength).
        
    constructor new args:objects
        <= $new int:(objects length);
    [
        $self setFields args:objects.
    ]
    
    dispatch => system'core_routines'__castover.
}

// --- extensions ---

extension dynamicOp
{
    mix into:role
        = Extension::(role, self).
        
    group with:object
    [
        if($nil == object)
            [ ^ self ].
        
        if(Group == self class_ref)
            [ 
                int length := self fieldCount.
            
                var retVal := Group $new int:(length + 1).
                                
                int i := 0.
                
                while (i < length)
                [
                    retVal setField int:i object:(self getField int:i).
                    
                    i := i + 1.
                ].
                
                retVal setField int:length object:object.
                
                ^ retVal
            ];
            [ 
                if (Group == object class_ref)
                    [
                        int length := object fieldCount.
                    
                        var retVal := Group $new int:(length + 1).
                                        
                        int i := 0.
                        
                        while (i < length)
                        [
                            retVal setField int:(i + 1) object:(self getField int:i).
                            
                            i := i + 1
                        ].
                        
                        retVal setField int:0 object:object.
                        
                        ^ retVal
                    ];
                    [ ^ Group new args:(self,object) ]
            ].
    ]        
        
    cast with:object
    [
        if ($nil == object)
            [ ^ self ].
        
        if(self class_ref == Cast)
            [ 
                int length := self fieldCount.
            
                var retVal := Cast $new int:(length + 1).
                                
                int i := 0.
                
                while (i < length)
                [
                    retVal setField int:i object:(self getField int:i).
                    
                    i := i + 1
                ].
                
                retVal setField int:length object:object.
                
                ^ retVal
            ];
            [ 
                if(object class_ref == Cast)
                    [
                        int length := object fieldCount.
                    
                        var retVal := Cast $new int:(length + 1).
                                        
                        int i := 0.
                        
                        while (i < length)
                        [
                            retVal setField int:(i + 1) object:(self getField int:i).
                            
                            i := i + 1
                        ].
                        
                        retVal setField int:0 object:object.
                        
                        ^ retVal
                    ];
                    [ ^ Cast new args:(self,object) ]
            ].
    ]        
}
