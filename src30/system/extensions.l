
// --- common methods ---

/// A generic extender
#class(extension)objectOp
{
    /// Returns true if the specified object instances are equal; otherwise, false.
    #method equal &reference:anObject
        => system'internal'object_equal.
        
    /// Returns the parameter        
    #method back : anObject = anObject.

    /// Returns true if the object is nil
    #method is &nil
        = nil == self.
        
    /// Returns true if the object is literal or character
    #method is &literal
        = self cast:%eval &to:{ literal : anObject = true. wide : anObject = true. char : anObject = true. ! : anObject = false. }.
        
    /// Returns true if the object is integer numereric
    #method is &integer
        = self cast:%eval &to:{ int : x = true. long : x = true. short : x = true. byte : x = true. ! : anObject = false. }.
        
    /// Returns true if the object is numereric
    #method is &numeric
        = self cast:%eval &to:{ int : x = true. long : x = true. real : x = true. short : x = true. byte : x = true. ! : anObject = false. }.
        
    /// Returns true if the parameter is equal to the object or false. It does not raise an exception if the objects are not compatible
    #method safeEqual : anObject
        = anObject cast:%eval &to:{ ! : aTarget : anObject = aTarget::$subject equal:anObject \ back:false. } &with:self.
}
