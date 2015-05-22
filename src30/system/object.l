// --- super object ---

#class Object
{
    #constructor new []

    #method(type:bool) equal : anObject => system'internal'object_equal.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &object:$self.
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &object:$self.
        
    #method => system'internal'dispatch.
}

// --- nil constant ---

#symbol nil =
{
    #method equal : anObject = ($nil == anObject).
    
    #method(generic)get = $nil.
}.
