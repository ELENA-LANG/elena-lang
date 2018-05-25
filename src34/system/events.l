public sealed template eventhandler<function>
{
    Array<function> functions.
    
    stacksafe cast eval(function func)
    [
        functions := V<function>(1).
        functions[0] := func.
    ]

    append(function func)
    [
        int len := functions length.
        
        Array<function> functions := V<function>(len + 1).

        int i := 0.
        while (i < len)
        [
            functions[i] := @functions[i].
            
            i := i + 1.
        ].
        
        functions[len] := func.
        
        @functions := functions.
    ]
    
    dispatch => system'core_routines'__event.
}

public field template event<prop_type, prop_name>
{
    eventhandler<prop_type> field.
    
    set prop_name(prop_type func)
    [
        if (nil == field)
            [ field := func ];
            [ field append(func) ].
    ]
}
