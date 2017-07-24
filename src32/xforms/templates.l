import extensions.

// --- nvntprop ---

method template nxprop :: type,prop_name,get_method,set_method
{
    stacksafe embeddable type get prop_name
    [
        int val := 0.        
        $self get_method vint:val.
        
        ^ val.
    ]
    
    stacksafe set prop_name:val
    [
        $self set_method type:(val toInt)
    ]
}
