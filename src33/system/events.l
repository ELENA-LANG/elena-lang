
template eventhandler :: function
{
    array<function> $functions.
    
    stacksafe implicit function:func
    [
        $functions := function<>(1).
        $functions[0] := func.
    ]

    append function:func
    [
        int len := $functions length.
        
        array<function> functions := function<>(len + 1).

        int i := 0.
        while (i < len)
        [
            functions[i] := $functions[i].
            
            i := i + 1.
        ].
        
        functions[len] := func.
        
        $functions := functions.
    ]
    
    dispatch => system'core_routines'__event.
}

field template event :: prop_type, prop_name
{
    eventhandler<prop_type> $field.
    
    prop_name prop_type:func
    [
        if ($nil == $field)
            [ $field := func ];
            [ $field append prop_type:func ].
    ]
}
