// --- super object ---

#class Object
{
    #constructor new []

    #method equal : anObject => system'internal'object_equal.
        
    #method => system'internal'dispatch.
}

// --- nil constant ---

#symbol nil =
{
    clone = $self.
}.
