// === basic templates ===

#template dispatchable(template_subj)
{
    template_subj = $self.
    
    /// Dispatches the verb to the target with a specified subject
    cast : verb &to:target = target::verb eval &template_subj:$self.

    /// Dispatches the verb to the target with a specified subject providing a parameter
    cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
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
//    bool equal : anObject => system'core_routines'__object.
    
//    bool notequal : anObject = $self equal:anObject inverted.
                
    /// Implements a message dispatching in VMT
    dispatch => system'core_routines'__object.
}

/// A common boolean value
listable dispatchable(bool) class BaseBoolValue
{
//    if : aTrueAction = $nil.
//
//    if : aTrueAction : aFalseAction = $nil.
//
//    ifnot : aTrueAction = $nil.
//
//    iif:aTrueValue:aFalseValue = $nil.
//
//    bool or : anExpr = $nil.
//
//    bool and : anExpr = $nil.
//
//    bool xor : aBool = $nil.
//
//    bool equal &bool:aBool = $nil.
//
//    bool not = $self inverted.
//
//    bool inverted = $nil.
//
////    #method literal = $nil.
////
////    #method wide = $nil.

    clone = $nil.
}
