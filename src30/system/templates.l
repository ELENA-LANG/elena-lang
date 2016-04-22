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
        #var(int)aLength.
        $self target &vint:aLength.
        
        ^ aLength.
   ]
}
