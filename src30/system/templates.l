// ==== ver 3.4.2 ===

#template(target:field) prop
{
    #method set : value
    [
        $target := value.
    ]
    
    #method get = $target.
}
