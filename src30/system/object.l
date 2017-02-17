//// === basic templates ===
//
//#template dispatchable : template_subj
//{
//    /// Dispatches the verb to the target with a specified subject
//    #method cast : verb &to:target = target::verb eval &template_subj:$self.
//
//    /// Dispatches the verb to the target with a specified subject providing a parameter
//    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &template_subj:$self.
//}

// --- super object ---

/// a common ancestor
Object
{
    /// Creates the object
    constructor new []
    
    /// Returns true if the specified object is equal to the current object; otherwise, false.
    ///
    /// By default compares the object references.
    equal []
        
    /// Implements a message dispatching in VMT
    dispatch => system'core_routines'__object.
}

//#class/*(dispatchable:object)*/ Object
//{
////    /// Creates the object
////    #constructor new []
//
////    /// Returns true if the specified object is equal to the current object; otherwise, false.
////    ///
////    /// By default compares the object references.
////    #method(bool) equal : anObject => system'core_routines'__object.
////    
////    #method(bool)notequal : anObject = $self equal:anObject inverted.
////                        
////    /// Implements a message dispatching in VMT
////    #method => system'core_routines'__object.
//}

///// A common boolean value
//
//#class(enumlist,dispatchable:bool) BaseBoolValue
//{
//    #method if : aTrueAction = $nil.
//
//    #method if : aTrueAction : aFalseAction = $nil.
//
//    #method ifnot : aTrueAction = $nil.
//
//    #method iif:aTrueValue:aFalseValue = $nil.
//
//    #method(bool) or : anExpr = $nil.
//
//    #method(bool) and : anExpr = $nil.
//
//    #method(bool) xor : aBool = $nil.
//
//    #method(bool) equal &bool:aBool = $nil.
//
//    #method(bool) not = $self inverted.
//
//    #method(bool) inverted = $nil.
//
//    #method literal = $nil.
//
//    #method wide = $nil.
//
//    #method clone = $nil.
//}
