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
    const static object& theProperties.

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
    const static object& theProperties.

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

accessor accessor template vprop :: prop_type,prop_name
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

accessor accessor template vvprop :: prop_type,prop_name
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

accessor template vprop :: type,prop_name
{
    stacksafe embeddable type prop_name
    [
        type val := 0.        
        $self $get var<type>:val.

        ^ val.
    ]
}

// --- rvprop ---

accessor template rvprop :: type,prop_name
{
    stacksafe embeddable type prop_name
    [
        type val := 0.        
        $self $get var<type>:val.

        ^ val.
    ]
}

// --- fnprop ---

accessor template fnprop :: type,template_fn
{
    stacksafe embeddable type template_fn type:n
    [
        type retVal := 0.
        $self $get type:n var<type>:retVal.
        ^ retVal.            
    ]
}

// --- fn2prop ---

accessor template fn2prop :: type,template_fn
{
    stacksafe embeddable type template_fn type:n type:m
    [
        type retVal := 0.
        $self $get type:n type:m var<type>:retVal.
        ^ retVal.            
    ]
}

// --- fn3prop ---

accessor template fn3prop :: type,template_fn
{
    stacksafe embeddable type template_fn type:n type:m type:o
    [
        type retVal := 0.
        $self $get type:n type:m type:o var<type>:retVal.
        ^ retVal.            
    ]
}

// --- while statement ---

loop_template while :: expr =
    expr bool ?
    [
    ].

// --- until statement ---

loop_template until :: expr =
    expr bool !
    [
    ].
    
// --- extern statement ---

extern_template extern :: dummy =
    [
    ].

// --- if statement ---

template if :: expr = 
    expr bool ?
    [
    ].

// --- ifnot statement ---

template ifnot :: expr =
    expr bool !
    [
    ].

// --- if-else statement ---

template if :: expr,else =
    expr bool ?
    [
    ]
    ! [
    ].

template ifnot :: expr,else =
    expr bool; inverted ?
    [
    ]
    ! [
    ].

// -- try-catch statement ---

template try :: expr =
    expr | catch:
    {
    }.

// -- try-finally-catch statement ---

template try :: expr,else =
    expr then:[] | catch([],
    {
    }).

// -- finally statement ---

template finally :: expr =
    expr then:
        [
        ]                 
        | raise:[].

template finally :: dummy, else =
    true doFinally([],[]).

// --- using statement ---

template using :: expr =
    expr doAndClose:[].
