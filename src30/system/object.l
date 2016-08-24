// === basic templates ===

#template dispatchable : template_subj
{
    /// Dispatches the verb to the target with a specified subject
    #method cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
}

// --- super object ---

/// A common ancestor
#class(dispatchable:object) Object
{
    /// Creates the object
    #constructor new []

    #method(generic)add []

    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    #method(bool) equal : anObject => system'core_routines'__object.
                        
    /// Implements a message dispatching in VMT
    #method => system'core_routines'__object.
}

// --- nil constant ---

/// A nil value
#symbol(dispatchable:void) nil =
{
    /// Returns true if the specified object is nil.
    #method(bool) equal : anObject = ($nil == anObject).
    
    /// Returns nil
    ///
    /// Any typecasting message will be handled and nil value returned
    #method(generic)get = $nil.
}.

