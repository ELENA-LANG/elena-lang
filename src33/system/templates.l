field template prop :: prop_name
{
    object $field.
    
    set prop_name:val
    [
        $field := val
    ]
    
    prop_name = $field.
}

field template prop :: prop_type, prop_name
{
    prop_type $field.
    
    set prop_name prop_type:val
    [
        $field := val
    ]
    
    prop_type prop_name = $field.
}

field template rprop :: type, prop_name
{
    type $field.
    
    type prop_name = $field.
}

field template dto_prop :: prop_type, prop_name
{
    const static object<> theProperties.

    prop_type $field.
    
    @theProperties := theProperties + %prop_name.
    
    set prop_name prop_type:val
    [
        $field := val
    ]
    
    prop_type prop_name = $field.
    
    __properties = theProperties.
}

field template dto_prop :: prop_name
{
    const static object<> theProperties.

    object $field.
    
    @theProperties := theProperties + %prop_name.
    
    set prop_name:val
    [
        $field := val
    ]
    
    prop_name = $field.
    
    __properties = theProperties.
}

// --- vprop ---

__accessor __accessor template vprop :: prop_type,prop_name
{
    stacksafe embeddable prop_type prop_name
    [
        prop_type val := 0.        
        $self $get var<prop_type>:val.

        ^ val.
    ]
    
    stacksafe embeddable set prop_name prop_type:val
    [
        $self $set prop_type:val.
    ]
}

// --- vvprop ---

__accessor __accessor template vvprop :: prop_type,prop_name
{
    stacksafe embeddable prop_type prop_name
    [
        prop_type val := 0.        
        $self $get var<prop_type>:val.

        ^ val.
    ]
    
    stacksafe embeddable set prop_name prop_type:val
    [
        $self $set prop_type:val.
    ]
    
    stacksafe embeddable append prop_name prop_type:val
    [
        prop_type newVal := 0.        
        $self $get var<prop_type>:newVal.
        
        newVal := newVal + val.
        
        $self $set prop_type:newVal.
    ]
    
    stacksafe embeddable reduce prop_name prop_type:val
    [
        prop_type newVal := 0.        
        $self $get var<prop_type>:newVal.
        
        newVal := newVal - val.
        
        $self $set prop_type:newVal.
    ]    
}

//accessor template vprop :: type,prop_name
//{
//    stacksafe embeddable type prop_name
//    [
//        type val := 0.        
//        $self $get var<type>:val.
//
//        ^ val.
//    ]
//}

// --- rvprop ---

__accessor template rvprop :: type,prop_name
{
    stacksafe embeddable type prop_name
    [
        type val := 0.        
        $self $get var<type>:val.

        ^ val.
    ]
}

// --- fnprop ---

__accessor template fnprop :: type,template_fn
{
    stacksafe embeddable type template_fn type:n
    [
        type retVal := 0.
        $self $get type:n var<type>:retVal.
        ^ retVal.            
    ]
}

// --- fn2prop ---

__accessor template fn2prop :: type,template_fn
{
    stacksafe embeddable type template_fn type:n type:m
    [
        type retVal := 0.
        $self $get type:n type:m var<type>:retVal.
        ^ retVal.            
    ]
}

// --- fn3prop ---

__accessor template fn3prop :: type,template_fn
{
    stacksafe embeddable type template_fn type:n type:m type:o
    [
        type retVal := 0.
        $self $get type:n type:m type:o var<type>:retVal.
        ^ retVal.            
    ]
}

// --- while statement ---

__loop template while :: expr
[
    expr bool ?
    [
    ].
]    

// --- until statement ---

__loop template until :: expr
[
    expr bool !
    [
    ].
]
    
// --- extern statement ---

__extern template extern
[
    [
    ].
]

// --- if statement ---

__block template if :: expr
[
    expr bool ?
    [
    ].
]

// --- ifnot statement ---

__block template ifnot :: expr
[
    expr bool !
    [
    ].
]

// --- if-else statement ---

__block __block template if :: expr
[
    expr bool ?
    [
    ]
    ! [
    ].    
]

__block __block template ifnot :: expr
[
    expr bool; inverted ?
    [
    ]
    ! [
    ].
]

// -- try-catch statement ---

__nested template try :: expr
[
    expr | catch:
    {
    }.
]

//// -- try-finally-catch statement ---
//
//__nested __block template try :: expr =
//    expr then:[] | catch([],
//    {
//    }).

// -- finally statement ---

__block template finally :: expr
[
    expr then:
        [
        ]                 
        | raise:[].
]        

__block __block template finally
[
    true doFinally([],[]).
]


// --- using statement ---

__block template using :: expr
[
    expr doAndClose:[].
]    
