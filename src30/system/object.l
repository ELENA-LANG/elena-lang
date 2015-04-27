// --- super object ---

#class Object
{
    #constructor new []

    #method(type:bool) equal : anObject => system'internal'object_equal.
        
    #method => system'internal'dispatch.
}

// --- nil constant ---

#symbol nil =
{
    #method equal : anObject = ($nil == anObject).
    
    #method(generic)get = $nil.
}.
