
field template prop :: type, prop_name
{
    type field.
    
    set prop_name type:val
    [
        field := val
    ]
    
    type prop_name = field.
}

field template rprop :: type, prop_name
{
    type field.
    
    type prop_name = field.
}

// --- outprop ---

method template outprop :: type,prop_name
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

//// --- nnfn ---
//
//method template nnfn :: type,template_fn,target_method
//{
//    stacksafe embeddable type template_fn type:n type:m
//    [
//        type retVal := 0.
//        $self target_method type:n type:m vint:retVal.
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
    expr | catch:
    {
    }.

// -- try-finally-catch statement ---

template try :: expr,else =
    expr then:[] | catch:[]:
    {
    }.

//// -- finally statement ---
//
//template finally :: expr =
//    expr then:
//        [
//        ]         
//        | raise:[].
//
//// --- using statement ---
//
//extension $usingOp
//{
//    doAndClose : code
//    [
//        self close after:code
//            | raise:[ self close ].
//    ]
//}
//
//template using :: expr =
//    expr doAndClose:[].
