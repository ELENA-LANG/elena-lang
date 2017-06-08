
template eventhandler :: function
{
    arrayof theFunctions(function).
    
    stacksafe implicit function:func
    [
        theFunctions := function(1).
        theFunctions@0 := func.
    ]

    append function:func
    [
        int len := theFunctions length.
        
        arrayof functions(function).
        functions := function(len + 1).
                
        int i := 0.
        while (i < len)
        [
            functions@i := theFunctions@i.
            
            i := i + 1.
        ].
        
        functions@len := func.
        
        theFunctions := functions.
    ]
    
    append : func
        <= append function:func.
    
    dispatch => system'core_routines'__event.
}

field template event :: function, prop_name
{
    eventhandler $target_field(function).
    
    prop_name function:func
    [
        if ($nil == $target_field)
            [ $target_field := func ];
            [ $target_field append function:func ].
    ]
}

