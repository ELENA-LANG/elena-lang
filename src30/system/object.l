// === basic templates ===

#template dispatchable(template_subj)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
}

#template dispatchable(template_subj, extra_subj)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
    
    cast : aVerb &and:extra &to:aTarget
        = extra cast:aVerb &template_subj:$self &to:aTarget.
        
    cast : verb &template_subj:param &to:target
        = target::verb eval &template_subj:param &template_subj:$self.
        
    cast : verb &extra_subj:param &to:target
        = target::verb eval &extra_subj:param &template_subj:$self.
}

#template dispatchable(template_subj, extra_subj1, extra_subj2)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
    
    cast : aVerb &and:extra &to:aTarget
        = extra cast:aVerb &template_subj:$self &to:aTarget.
        
    cast : verb &template_subj:param &to:target
        = target::verb eval &template_subj:param &template_subj:$self.
        
    cast : verb &extra_subj1:param &to:target
        = target::verb eval &extra_subj1:param &template_subj:$self.
        
    cast : verb &extra_subj2:param &to:target
        = target::verb eval &extra_subj2:param &template_subj:$self.
}

#template dispatchable(template_subj, extra_subj1, extra_subj2, extra_subj3)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
    
    cast : aVerb &and:extra &to:aTarget
        = extra cast:aVerb &template_subj:$self &to:aTarget.
        
    cast : verb &template_subj:param &to:target
        = target::verb eval &template_subj:param &template_subj:$self.
        
    cast : verb &extra_subj1:param &to:target
        = target::verb eval &extra_subj1:param &template_subj:$self.
        
    cast : verb &extra_subj2:param &to:target
        = target::verb eval &extra_subj2:param &template_subj:$self.
        
    cast : verb &extra_subj3:param &to:target
        = target::verb eval &extra_subj3:param &template_subj:$self.
}

#template dispatchable(template_subj, extra_subj1, extra_subj2, extra_subj3, extra_subj4)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
    
    cast : aVerb &and:extra &to:aTarget
        = extra cast:aVerb &template_subj:$self &to:aTarget.
        
    cast : verb &template_subj:param &to:target
        = target::verb eval &template_subj:param &template_subj:$self.
        
    cast : verb &extra_subj1:param &to:target
        = target::verb eval &extra_subj1:param &template_subj:$self.
        
    cast : verb &extra_subj2:param &to:target
        = target::verb eval &extra_subj2:param &template_subj:$self.
        
    cast : verb &extra_subj3:param &to:target
        = target::verb eval &extra_subj3:param &template_subj:$self.
        
    cast : verb &extra_subj4:param &to:target
        = target::verb eval &extra_subj4:param &template_subj:$self.
}

#template dispatchable(template_subj, extra_subj1, extra_subj2, extra_subj3, extra_subj4, extra_subj5)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
    
    cast : aVerb &and:extra &to:aTarget
        = extra cast:aVerb &template_subj:$self &to:aTarget.
        
    cast : verb &template_subj:param &to:target
        = target::verb eval &template_subj:param &template_subj:$self.
        
    cast : verb &extra_subj1:param &to:target
        = target::verb eval &extra_subj1:param &template_subj:$self.
        
    cast : verb &extra_subj2:param &to:target
        = target::verb eval &extra_subj2:param &template_subj:$self.
        
    cast : verb &extra_subj3:param &to:target
        = target::verb eval &extra_subj3:param &template_subj:$self.
        
    cast : verb &extra_subj4:param &to:target
        = target::verb eval &extra_subj4:param &template_subj:$self.
        
    cast : verb &extra_subj5:param &to:target
        = target::verb eval &extra_subj5:param &template_subj:$self.
}

#template dispatchable(template_subj, extra_subj1, extra_subj2, extra_subj3, extra_subj4, extra_subj5, extra_subj6)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
    
    cast : aVerb &and:extra &to:aTarget
        = extra cast:aVerb &template_subj:$self &to:aTarget.
        
    cast : verb &template_subj:param &to:target
        = target::verb eval &template_subj:param &template_subj:$self.
        
    cast : verb &extra_subj1:param &to:target
        = target::verb eval &extra_subj1:param &template_subj:$self.
        
    cast : verb &extra_subj2:param &to:target
        = target::verb eval &extra_subj2:param &template_subj:$self.
        
    cast : verb &extra_subj3:param &to:target
        = target::verb eval &extra_subj3:param &template_subj:$self.
        
    cast : verb &extra_subj4:param &to:target
        = target::verb eval &extra_subj4:param &template_subj:$self.
        
    cast : verb &extra_subj5:param &to:target
        = target::verb eval &extra_subj5:param &template_subj:$self.
        
    cast : verb &extra_subj6:param &to:target
        = target::verb eval &extra_subj6:param &template_subj:$self.
}

// --- super object ---

/// a common ancestor
dispatchable(object) class Object
{
    /// Creates the object
    constructor new []
    
    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    bool equal : anObject => system'core_routines'__object.
    
    bool notequal : anObject = $self equal:anObject inverted.
                
    /// Implements a message dispatching in VMT
    dispatch => system'core_routines'__object.
}

/// A common boolean value
listable dispatchable(bool) class BaseBoolValue
{
    if : aTrueAction = $nil.

    if : aTrueAction : aFalseAction = $nil.

    ifnot : aTrueAction = $nil.

    iif:aTrueValue:aFalseValue = $nil.

    bool or : anExpr = $nil.

    bool and : anExpr = $nil.

    bool xor : aBool = $nil.

    bool equal &bool:aBool = $nil.

    bool not = $self inverted.

    bool inverted = $nil.

    literal = $nil.

    wide = $nil.

    clone = $nil.
}

extension commonOp
{
    then : code
        = code eval.
}