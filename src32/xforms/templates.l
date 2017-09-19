import extensions.

// --- nvntprop ---

method template vxprop :: prop_type,prop_name
{
    method get_method.
    method set_method.
    
    stacksafe embeddable prop_type prop_name
    [
        int val := 0.        
        $self get_method vint:val.
        
        ^ val.
    ]
    
    stacksafe set prop_name:val
    [
        $self set_method prop_type(val toInt)
    ]
}
