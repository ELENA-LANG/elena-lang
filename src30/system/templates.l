//#template(target:field) prop
//{
//    #method set : value
//    [
//        $target := value.
//    ]
//    
//    #method get = $target.
//}

// --- outnprop ---

#template outnprop : template_subj
{
   #subject target.
    
   #method(stacksafe,int,embeddable,suppress:w3) template_subj
   [
        #var(int)val.
        $self target &vint:val.
        
        ^ val.
   ]
}

// --- innprop ---

#template innprop : template_subj
{
   #subject target.
    
   #method(stacksafe,suppress:w2) set &template_subj:val
   [
        $self target &int:val.
        
        ^ val.
   ]
}
 
// --- outlprop ---

#template outlprop : template_subj
{
   #subject target.
    
   #method(stacksafe,long,embeddable,suppress:w3) template_subj
   [
        #var(long)val.
        $self target &vlong:val.
        
        ^ val.
   ]
}
