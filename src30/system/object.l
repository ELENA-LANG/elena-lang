// === basic templates ===

#template dispatchable
{
    /// Dispatches the verb to the target with a specified subject
    #method cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target object with generic subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
}

#template numeric
{
   #method(stacksafe) add &template_subj:val
      = $target + val.
}

// --- super object ---

/// A common ancestor
#class(dispatchable:object) Object
{
    /// Creates the object
    #constructor new []

//    /// Returns true if the specified object is equal to the current object; otherwise, false.
//    ///
//    /// By default compares the object references.
//    #method(type:bool) equal : anObject => system'core_routines'object_equal.
                        
    /// Implements a message dispatching in VMT
    #method => system'core_routines'__object.
}

//// --- nil constant ---
//
///// A nil value
//#symbol nil =
//{
//    /// Returns true if the specified object is nil.
//    #method(type:bool) equal : anObject = ($nil == anObject).
//    
//    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &void:$nil.
//    
//    /// Dispatches the verb to the target object with generic subject
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &void:$nil.
//    
//    /// Returns nil
//    ///
//    /// Any typecasting message will be handled and nil value returned
//    #method(generic)get = $nil.
//}.

