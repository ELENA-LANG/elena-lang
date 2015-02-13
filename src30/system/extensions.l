
// --- common methods ---

// --- subjectOp ---

#class(extension:subject)subjectOp
{
    #method equal &subject : aSubject
        => system'internal'subject_equal.
}

#class(extension)objectOp
{
    #method equal &reference:anObject
        => system'internal'object_equal.
        
    #method back : anObject = anObject.

    #method(type:bool) isNil
        = nil == self.
        
    #method is &literal
        = self cast:%eval &to:{ literal : anObject = true. char : anObject = true. ! : anObject = false. }.
        
    #method is &integer
        = self cast:%eval &to:{ int : anObject = true. long : anObject = true. ! : anObject = false. }.
        
    #method is &numeric
        = self cast:%eval &to:{ int : anObject = true. long : anObject = true. real : anObject = true. ! : anObject = false. }.
        
    #method safeEqual : anObject
    [
        #var type := anObject cast:%eval &to:{ ! : x = $subject. }.
        
        ^ self::type equal:anObject || back:false.
    ]
}
