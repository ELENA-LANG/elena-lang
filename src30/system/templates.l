//#template prop : prop_name
//{
//    #method set &prop_name : value
//    [
//        target := value.
//    ]
//    
//    #method prop_name = target.
//}
//
//#template outprop : prop_name
//{
//    #method prop_name = target.
//}

// --- outnprop ---

#template(-16388) outnprop(template_subj)
{
   stacksafe embeddable int template_subj
   [
        int val := 0.
        $self target &vint:val.
        
        ^ val.
   ]
}

//// --- innprop ---
//
//#template innprop : template_subj
//{
//   #method(stacksafe,w2) set &template_subj:val
//   [
//        $self target &int:val.
//        
//        ^ val.
//   ]
//}
// 
//// --- outlprop ---
//
//#template outlprop : template_subj
//{
//   #method(stacksafe,long,embeddable,w3) template_subj
//   [
//        #var(long)val.
//        $self target &vlong:val.
//        
//        ^ val.
//   ]
//}
//
//// --- outwprop ---
//
//#template outwprop : template_subj
//{
//   #method(stacksafe,short,embeddable,w3) template_subj
//   [
//        #var(short)val.
//        $self target &vshort:val.
//        
//        ^ val.
//   ]
//}
//
//// --- nfn ---
//
//#template nfn : template_fn
//{
//    #method(stacksafe,int,embeddable,w3) template_fn &int:n
//    [
//        #var(int)retVal.
//        $self target &int:n &vint:retVal.
//        ^ retVal.            
//    ]
//}

// --- while statement ---

#template while (expr)
[
    #loop(expr)?
    [
    ].
].

// --- until statement ---

#template until (expr)
[
    #loop(expr)!
    [
    ].
].


