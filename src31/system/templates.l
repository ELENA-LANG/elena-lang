field template prop :: type, prop_name
{
    type field.
    
    set prop_name:val
    [
        field := val
    ]
    
    type prop_name = field.
}

//
//#template(-16388) outprop (prop_name)
//{
//    prop_name = $target_field.
//}

// --- outnprop ---

method template outnprop :: type,prop_name,target_method
{
    stacksafe embeddable type prop_name
    [
        int val := 0.        
        $self target_method vint:val.
        
        ^ val.
    ]
}

// --- nprop ---

method template nprop :: type,prop_name,get_method,set_method
{
    stacksafe embeddable type prop_name
    [
        int val := 0.        
        $self get_method vint:val.
        
        ^ val.
    ]
    
    stacksafe set prop_name:val
    [
        $self set_method type:val
    ]
}

// --- outlprop ---

method template outlprop :: type,prop_name,target_method
{
    stacksafe embeddable type prop_name
    [
        long val := 0.        
        $self target_method vlong:val.
        
        ^ val.
    ]
}


// --- outwprop ---

method template outwprop :: type,prop_name,target_method
{
   stacksafe embeddable type prop_name
   [
        short val := 0.
        $self target_method vshort:val.
        
        ^ val
   ]
}

// --- nfn ---

method template nfn :: type,template_fn,target_method
{
    stacksafe embeddable type template_fn type:n
    [
        type retVal := 0.
        $self target_method type:n vint:retVal.
        ^ retVal.            
    ]
}

//// --- nnfn ---
//
//#template(-16388) nnfn (template_fn)
//{
//    stacksafe embeddable int template_fn &int:n &int:m
//    [
//        int retVal := 0.
//        $self target_method &int:n &int:m &vint:retVal.
//        ^ retVal.            
//    ]
//}

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
    expr | on:
    {
    }.

// -- try-finally-catch statement ---

template try :: expr,else =
    expr then:[] | on:[]:
    {
    }.

// -- finally statement ---

template finally :: expr =
    expr then:
        [
        ]         
        | raise:[].

// --- using statement ---

extension $usingOp
{
    doAndClose : code
    [
        self close after:code
            | raise:[ self close ].
    ]
}

template using :: expr =
    expr doAndClose:[].
