#template(-16388) prop (prop_name)
{
    set &prop_name : value
    [
        $target_field := value.
    ]
    
    prop_name = $target_field.
}

#template(-16388) outprop (prop_name)
{
    prop_name = $target_field.
}

// --- outnprop ---

#template(-16388) outnprop(template_subj)
{
   stacksafe embeddable int template_subj
   [
        int val := 0.
        $self target_method &vint:val.
        
        ^ val.
   ]
}

// --- innprop ---

#template(-16388) innprop (template_subj)
{
   stacksafe set &template_subj:val
   [
        $self target_method &int:val.
        
        ^ val.
   ]
}
 
// --- outlprop ---

#template(-16388) outlprop (template_subj)
{
   stacksafe embeddable long template_subj
   [
        long val := 0l.
        $self target_method &vlong:val.
        
        ^ val.
   ]
}

// --- outwprop ---

#template(-16388) outwprop (template_subj)
{
   stacksafe embeddable short template_subj
   [
        short val := 0.
        $self target_method &vshort:val.
        
        ^ val.
   ]
}

// --- nfn ---

#template(-16388) nfn (template_fn)
{
    stacksafe embeddable int template_fn &int:n
    [
        int retVal := 0.
        $self target_method &int:n &vint:retVal.
        ^ retVal.            
    ]
}

// --- nnfn ---

#template(-16388) nnfn (template_fn)
{
    stacksafe embeddable int template_fn &int:n &int:m
    [
        int retVal := 0.
        $self target_method &int:n &int:m &vint:retVal.
        ^ retVal.            
    ]
}

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

// --- if statement ---

#template if (expr)
[
    (expr) ?
    [
    ].
].

