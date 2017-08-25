
template eventhandler :: function
{
    array<function> theFunctions.
    
    stacksafe explicit function:func
    [
        theFunctions := function&(1).
        theFunctions[0] := func.
    ]

    append function:func
    [
        int len := theFunctions length.
        
        array<function> functions := function&(len + 1).

        int i := 0.
        while (i < len)
        [
            functions[i] := theFunctions[i].
            
            i := i + 1.
        ].
        
        functions[len] := func.
        
        theFunctions := functions.
    ]
    
    dispatch => system'core_routines'__event.
}

field template event :: prop_type, prop_name
{
    eventhandler<prop_type> field.
    
    prop_name prop_type:func
    [
        if ($nil == field)
            [ field := func ];
            [ field append prop_type:func ].
    ]
}
