import extensions.

// --- nvntprop ---

accessor accessor template vxprop :: prop_type,prop_name
{
    stacksafe embeddable prop_type prop_name
    [
        int val := 0.        
        $self $get vint:val.
        
        ^ val.
    ]
    
    stacksafe set prop_name:val
    [
        $self $set prop_type(val toInt)
    ]
}
