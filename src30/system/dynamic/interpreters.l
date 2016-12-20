#import system.
//#import system'routines.

#subject struct   = system'dynamic'Struct.

// --- Tape ---

#class(dynamic,group) Tape
{
    #field $item.
    
    #constructor $new &int:n
        = object:n.
    
    #constructor new &array:list
        <= ($new &int:(list length))
    [
        #var(int)i := 0.
        #var(int)n := list length.
        
        #loop(i < n)?
        [
            $self setField &int:i &object:(list getAt &int:i).
            
            i += 1.
        ].
    ]    
    
    #method => system'core_routines'__tape.
}

// --- tapeControl ---

#class(extension)tapeOp
{
    #method var &args:params
    [
        #var(int)len := params length.
        #var(int)n := len.
        n -= self int.
       
        ^ params@n.
    ]
    
    #method stack &args:params
    [
        #var(int)n := self int.
       
        ^ params@n.
    ]
        
    #method jumpif : offset &args:params
        => system'core_routines'__tape.
    
    #method jump &args:params
        => system'core_routines'__tape.
        
    #method goif : offset &args:params
        => system'core_routines'__tape.
    
    #method go &args:params
        => system'core_routines'__tape.
        
    #method ptr &args:params
        => system'core_routines'__tape.
}

// --- Struct ---

#class(dynamic,group,dispatchable:struct) Struct
{
    #field $item.
    
    #constructor new &int:n
        = object:n.
    
    #constructor new &array:list
        <= (new &int:(list length))
    [
        #var(int)i := 0.
        #var(int)n := list length.
        
        #loop(i < n)?
        [
            $self setField &int:i &object:(list getAt &int:i).
            
            i += 1.
        ].
    ]    
    
    #method => system'core_routines'__struct.
}

// --- Dynamic ---

#class Dynamic
{
    #field(struct) theStruct.
    
    #constructor new
    [
        theStruct := Struct new &int:0.
    ]
    
    #constructor new : aStruct
    [
        theStruct := aStruct struct.
    ]

    #method value = theStruct.
            
    /// Dispatches the verb to the target object with generic subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &struct:theStruct.
    
    /// Dispatches the verb to the target object with generic subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &struct:theStruct.
    
    #method(generic,w2)set : anObject
    [
        // looking for the current subject
        #var(int)anIndex := 0.
        #var(int)aLength.
        #var(bool) aNotFound := true.
        
        theStruct readFieldCount &vint:aLength.
        #loop ((anIndex < aLength)and:aNotFound) ?
        [
            #var aCurrent := theStruct::classROp getField &int:anIndex.
            
            ($subject equal &subject:(aCurrent subject))
                ? [ aNotFound := false. ]
                ! [ anIndex := anIndex + 2. ].
        ].
                
        // if not found extend the structure
        (anIndex == aLength) ?
        [                                    
            theStruct := theStruct reallocate &int:(aLength + 2).

            theStruct::classROp setField &int:anIndex &object:(Signature new &subject:$subject).
        ].

        // set the content for the appropriate field
        theStruct::classROp setField &int:(anIndex + 1) &object:anObject.
    ]
    
    #method(generic) if
    [
        #var(int)anIndex := 0.
        #var(int)aLength.
        
        theStruct readFieldCount &vint:aLength.
        #loop (anIndex < aLength) ?
        [
            #var aCurrent := theStruct::classROp getField &int:anIndex.
            
            ($subject equal &subject:(aCurrent subject))
                ? [ ^ true. ].
                
            anIndex := anIndex + 2.                
        ].
        
        ^ false.
    ]
    
    #method => theStruct.
}

// --- obsolete ---

#symbol DynamicStruct = Dynamic.