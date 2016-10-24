//#ftemplate prop : prop_name
//{
//    #method set &prop_name : value
//    [
//        target := value.
//    ]
//    
//    #method prop_name = target.
//}
//
//#ftemplate outprop : prop_name
//{
//    #method prop_name = target.
//}

// --- outnprop ---

#template outnprop : template_subj
{
   #method(stacksafe,int,embeddable,w3) template_subj
   [
        #var(int)val.
        $self target &vint:val.
        
        ^ val.
   ]
}

// --- innprop ---

#template innprop : template_subj
{
   #method(stacksafe,w2) set &template_subj:val
   [
        $self target &int:val.
        
        ^ val.
   ]
}
 
// --- outlprop ---

#template outlprop : template_subj
{
   #method(stacksafe,long,embeddable,w3) template_subj
   [
        #var(long)val.
        $self target &vlong:val.
        
        ^ val.
   ]
}

//// --- outwprop ---
//
//#mtemplate outwprop : template_subj
//{
//   #method(stacksafe,short,embeddable,w3) template_subj
//   [
//        #var(short)val.
//        $self target &vshort:val.
//        
//        ^ val.
//   ]
//}
