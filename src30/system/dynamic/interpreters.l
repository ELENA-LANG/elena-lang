#import system.
//#import system'routines.

#subject structure   = system'dynamic'Struct.

// --- Tape ---

dynamic __tape_group Tape
{
    object $item.
    
    constructor $new &int:n
        = object(n).
    
    constructor new &array:list
        <= $new &int:(list length);
    [
        int i := 0.
        int n := list length.
        
        while(i < n)
        [
            $self setField &int:i &object:(list getAt &int:i).
            
            i += 1.
        ].
    ]    
    
    dispatch => system'core_routines'__tape.
}

// --- tapeControl ---

extension tapeOp
{
    var &args:params
    [
        int len := params length.
        int n := len.
        n -= self int.
       
        ^ params@n.
    ]
    
    assign &args:params
        => system'core_routines'__tape.
    
    stack &args:params
    [
        int n := self int.
       
        ^ params@n.
    ]
        
    jumpif &args:params
        => system'core_routines'__tape.
    
    jump &args:params
        => system'core_routines'__tape.
        
    goif &args:params
        => system'core_routines'__tape.
    
    go &args:params
        => system'core_routines'__tape.
        
    ptr &args:params
        => system'core_routines'__tape.
}

// --- Struct ---

dynamic dispatchable(structure) group Struct
{
    object $item.
    
    constructor new &int:n
        = object(n).
    
    constructor new &array:list
        <= new &int:(list length);
    [
        int i := 0.
        int n := list length.
        
        while(i < n)
        [
            $self setField &int:i &object:(list getAt &int:i).
            
            i += 1.
        ].
    ]    
    
    dispatch => system'core_routines'__struct.
}

// --- Dynamic ---

class Dynamic
{
    structure theStruct.
    
    constructor new
    [
        theStruct := Struct new &int:0.
    ]
    
    constructor new : aStruct
    [
        theStruct := aStruct struct.
    ]

    value = theStruct.
            
    /// Dispatches the verb to the target object with generic subject
    cast : aVerb &to:aTarget = aTarget::aVerb eval &struct:theStruct.
    
    /// Dispatches the verb to the target object with generic subject
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &struct:theStruct.
    
    generic set : anObject
    [
        // looking for the current subject
        int  anIndex := 0.
        int  aLength := 0.
        bool aNotFound := true.
        
        theStruct readFieldCount &vint:aLength.
        while ((anIndex < aLength)and:aNotFound)
        [
            var aCurrent := theStruct::classROp getField &int:anIndex.
            
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
    
    generic if
    [
        int anIndex := 0.
        int aLength := 0.
        
        theStruct readFieldCount &vint:aLength.
        while (anIndex < aLength)
        [
            var aCurrent := theStruct::classROp getField &int:anIndex.
            
            ($subject equal &subject:(aCurrent subject))
                ? [ ^ true. ].
                
            anIndex := anIndex + 2.                
        ].
        
        ^ false.
    ]
    
    dispatch => theStruct.
}

// --- Class ---

dynamic group Class
{
    object $item.
    
    constructor $new &int:n
        = object(n).
    
    constructor new &array:list
        <= $new &int:(list length);
    [
        int i := 0.
        int n := list length.
        
        while (i < n)
        [
            $self setField &int:i &object:(list getAt &int:i).
            
            i += 1.
        ].
    ]    
    
    dispatch => system'core_routines'__class.
}


// --- obsolete ---

symbol DynamicStruct = Dynamic.