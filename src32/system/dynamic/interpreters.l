type tape_structure = system'dynamic'Struct.
type tape_dynamic_structure = system'dynamic'DynamicStruct.

// --- Tape ---

dynamic __tape_mixin Tape
{
    object $item.
    
    constructor $new int:n
        = object&(n).
    
    constructor new array:list
        <= $new int:(list length);
    [
        int i := 0.
        int n := list length.
        
        while(i < n)
        [
            $self setField int:i object:(list getAt int:i).
            
            i := i + 1
        ].
    ]    
    
    dispatch => system'core_routines'__tape.
}

// --- tapeControl ---

extension tapeOp
{
    tape_var args:params
    [
        int len := params length.
        int n := len.
        n := n - self int.
       
        ^ params[n]
    ]
    
    tape_swap_top args:params
    [
        var top := params[0].
        var before_top := params[1].
        params[0] := before_top.
        params[1] := top.
    ]
    
    tape_assign args:params
        => system'core_routines'__tape.
    
    tape_stack args:params
    [
        int n := self int.
       
        ^ params[n]
    ]
        
    tape_jumpif args:params
        => system'core_routines'__tape.
    
    tape_jumpifnot args:params
        => system'core_routines'__tape.
    
    tape_jump args:params
        => system'core_routines'__tape.
        
    tape_goif args:params
        => system'core_routines'__tape.
    
    tape_go args:params
        => system'core_routines'__tape.
        
    tape_ptr args:params
        => system'core_routines'__tape.
}

// --- Struct ---

dynamic mixin Struct :: dispatchable<tape_structure>
{
    object $item.
    
    constructor $new int:n
        = object&(n).
    
    constructor new array:list
        <= $new int:(list length);
    [
        int i := 0.
        int n := list length.
        
        while(i < n)
        [
            $self setField int:i object:(list getAt int:i).
            
            i := i + 1
        ]
    ]
    
    dispatch => system'core_routines'__struct.
}

// --- DynamicStruct ---

class DynamicStruct
{
    tape_structure theStruct.
    
    constructor new
    [
        theStruct := Struct $new int:0.
    ]
    
    constructor new : aStruct
    [
        theStruct := aStruct tape_structure.
    ]

    tape_structure = theStruct.
    
    generic : anObject
    [
        // looking for the current subject
        int  anIndex := 0.
        int  aLength := 0.
        bool aNotFound := true.
        
        theStruct readFieldCountTo vint:aLength.
        while ((anIndex < aLength)and:aNotFound)
        [
            var aCurrent := theStruct~classROp getField int:anIndex.
            
            if($subject equal subject:(aCurrent subject))
                [ aNotFound := false ];
                [ anIndex := anIndex + 2 ]
        ].
                
        // if not found extend the structure
        if(anIndex == aLength)
        [                                    
            theStruct := theStruct reallocate int:(aLength + 2).

            theStruct~classROp setField int:anIndex object:(Signature new subject:$subject).
        ].

        // set the content for the appropriate field
        theStruct~classROp setField int:(anIndex + 1) object:anObject.
    ]
    
    containsProperty subject:subj
    [
        int anIndex := 0.
        int aLength := 0.
        
        theStruct readFieldCountTo vint:aLength.
        while (anIndex < aLength)
        [
            var aCurrent := theStruct~classROp getField int:anIndex.
            
            if(subj equal subject:(aCurrent subject))
                [ ^ true ].
                
            anIndex := anIndex + 2.                
        ].
        
        ^ false
    ]
    
    dispatch => theStruct.
}

// --- Class ---

dynamic mixin Class
{
    object $item.
    
    constructor $new int:n
        = object&(n).
    
    constructor new array:list
        <= $new int(list length);
    [
        int i := 0.
        int n := list length.
        
        while (i < n)
        [
            $self setField int:i object(list getAt int:i).
            
            i := i + 1
        ]
    ]    
    
    dispatch => system'core_routines'__class.
}
