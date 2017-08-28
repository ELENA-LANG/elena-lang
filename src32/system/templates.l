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

// --- vprop ---

method template vprop :: prop_type,prop_name
{
    method get_method.
    method set_method.
    
    stacksafe embeddable prop_type prop_name
    [
        prop_type val := 0.        
        $self get_method var<prop_type>:val.

        ^ val.
    ]
    
    stacksafe embeddable set prop_name prop_type:val
    [
        $self set_method prop_type:val.
    ]
}

// --- vvprop ---

method template vvprop :: prop_type,prop_name
{
    method get_method.
    method set_method.
    
    stacksafe embeddable prop_type prop_name
    [
        prop_type val := 0.        
        $self get_method var<prop_type>:val.

        ^ val.
    ]
    
    stacksafe embeddable set prop_name prop_type:val
    [
        $self set_method prop_type:val.
    ]
    
    stacksafe embeddable append prop_name prop_type:val
    [
        prop_type newVal := 0.        
        $self get_method var<prop_type>:newVal.
        
        newVal := newVal + val.
        
        $self set_method prop_type:newVal.
    ]
    
    stacksafe embeddable reduce prop_name prop_type:val
    [
        prop_type newVal := 0.        
        $self get_method var<prop_type>:newVal.
        
        newVal := newVal - val.
        
        $self set_method prop_type:newVal.
    ]    
}

// --- rvprop ---

method template rvprop :: type,prop_name
{
    method get_method.
    
    stacksafe embeddable type prop_name
    [
        type val := 0.        
        $self get_method var<type>:val.

        ^ val.
    ]
}

// --- fnprop ---

method template fnprop :: type,template_fn
{
    method target_method.
    
    stacksafe embeddable type template_fn type:n
    [
        type retVal := 0.
        $self target_method type:n var<type>:retVal.
        ^ retVal.            
    ]
}

// --- fn2prop ---

method template fn2prop :: type,template_fn
{
    method target_method.
    
    stacksafe embeddable type template_fn type:n type:m
    [
        type retVal := 0.
        $self target_method type:n type:m var<type>:retVal.
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
        
extension $templateOp =
{
    doFinally func:c1 func:c2
    [
        c1 eval; then:c2
            | raise:c2.
    ]
    
    doAndClose : code
    [
        self thenClose:code
            | raise:[ self close ].
    ]
}.

template finally :: dummy, else =
    true doFinally([],[]).

// --- using statement ---

template using :: expr =
    expr doAndClose:[].
