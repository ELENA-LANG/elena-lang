
#template(sealed) eventhandler (function)
{
    arrayof(function) theFunctions.
    
    sealed stacksafe conversion function:func
    [
        theFunctions := function(1).
        theFunctions@0 := func.
    ]

    append &function:func
    [
        int len := theFunctions length.
        
        arrayof(function) functions := function(len + 1).
        
        int i := 0.
        while (i < len)
        [
            functions@i := theFunctions@i.
            
            i += 1.
        ].
        
        functions@len := func.
        
        theFunctions := functions.
    ]
    
    append : func
        <= append &function:func.
    
    dispatch => system'core_routines'__event.
}

#template(-16388, eventhandler(function)) event (function,prop)
{
    prop &function:func
    [
        ($nil == $target_field)
            ? [ $target_field := func. ]
            ! [ $target_field append &function:func. ].
    ]
}
