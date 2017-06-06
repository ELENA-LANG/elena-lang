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

//// --- innprop ---
//
//#template(-16388) innprop (template_subj)
//{
//   stacksafe set &template_subj:val
//   [
//        $self target_method &int:val.
//        
//        ^ val.
//   ]
//}
// 
//// --- outlprop ---
//
//#template(-16388) outlprop (template_subj)
//{
//   stacksafe embeddable long template_subj
//   [
//        long val := 0l.
//        $self target_method &vlong:val.
//        
//        ^ val.
//   ]
//}

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

//// --- nfn ---
//
//#template(-16388) nfn (template_fn)
//{
//    stacksafe embeddable int template_fn &int:n
//    [
//        int retVal := 0.
//        $self target_method &int:n &vint:retVal.
//        ^ retVal.            
//    ]
//}
//
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

//// -- try-catch statement ---
//
//#template try (expr)
//[
//    expr | on:
//    {
//    }.
//].
//
//// -- try-finally-catch statement ---
//
//#template try (expr,else)
//[
//    expr then:[] | on:[]:
//    {
//    }.
//].
//
//// -- finally statement ---
//
//#template finally (expr)
//[
//    expr then:
//        [
//        ]         
//        | raise:[].
//].
//
//// --- using statement ---
//
//#template using(expr)
//[
//    finally(expr then:[])
//        [ expr close. ].
//].
