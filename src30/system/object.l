// === basic templates ===

#template dispatchable : template_subj
{
    /// Dispatches the verb to the target with a specified subject
    #method cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target object with generic subject
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
}

#template numeric : template_subj
{
    #field(template_subj)$target.
    
    #method(stacksafe,suppress:w3) add &template_subj:val
      = $target + val.
    
    #method(stacksafe,suppress:w3) subtract &template_subj:val
      = $target - val.
    
    #method(stacksafe,suppress:w3) multiply &template_subj:val
      = $target * val.
    
    #method(stacksafe,suppress:w3) divide &template_subj:val
      = $target / val.
      
    #method(bool,stacksafe) equal &template_subj:val
      = $target == val.
        
    #method(bool,stacksafe) less &template_subj:val
      = $target < val.      
}

#template(variable) varof : template_subj
{
    #field(template_subj)$value.
    
    #method set : val
    [
        $value := val template_subj.
    ]
    
    #method append : val
    [
        $value := $value + val template_subj.
    ]
    
    #method reduce : val
    [
        $value := $value - val template_subj.
    ]

    #method multiplyBy : val
    [
        $value := $value * val template_subj.
    ]
    
    /// Divides into the value
    #method divideInto : val
    [
        $value := $value / val template_subj.
    ]

    #method(suppress:w3) get = $value.
}

// --- super object ---

/// A common ancestor
#class(dispatchable:object) Object
{
    /// Creates the object
    #constructor new []

    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    #method(bool) equal : anObject => system'core_routines'__object.
                        
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

