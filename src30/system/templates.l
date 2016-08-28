#ftemplate prop : prop_name
{
    #method set &prop_name : value
    [
        target := value.
    ]
    
    #method prop_name = target.
}

#ftemplate outprop : prop_name
{
    #method prop_name = target.
}

// --- outnprop ---

#mtemplate outnprop : template_subj
{
   #method(stacksafe,int,embeddable,suppress:w3) template_subj
   [
        #var(int)val.
        $self target &vint:val.
        
        ^ val.
   ]
}

// --- innprop ---

#mtemplate innprop : template_subj
{
   #method(stacksafe,suppress:w2) set &template_subj:val
   [
        $self target &int:val.
        
        ^ val.
   ]
}
 
// --- outlprop ---

#mtemplate outlprop : template_subj
{
   #method(stacksafe,long,embeddable,suppress:w3) template_subj
   [
        #var(long)val.
        $self target &vlong:val.
        
        ^ val.
   ]
}

// --- outwprop ---

#mtemplate outwprop : template_subj
{
   #method(stacksafe,short,embeddable,suppress:w3) template_subj
   [
        #var(short)val.
        $self target &vshort:val.
        
        ^ val.
   ]
}
