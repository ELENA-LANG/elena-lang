// ==== ver 3.4.1 ===

// --- super object ---

/// A common ancestor
#class Object
{
    /// Creates the object
    #constructor new []

    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    #method(type:bool) equal : anObject => system'internal'object_equal.
    
    /// Dispatches the verb to the target object with generic subject
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &object:$self.
    
    /// Dispatches the verb to the target object with generic subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &object:$self.
                        
    /// Implements a message dispatching in VMT
    #method => system'internal'dispatch.
}

// --- nil constant ---

/// A nil value
#symbol nil =
{
    /// Returns true if the specified object is nil.
    #method(type:bool) equal : anObject = ($nil == anObject).
    
    /// Returns nil
    ///
    /// Any typecasting message will be handled and nil value returned
    #method(generic)get = $nil.
}.
