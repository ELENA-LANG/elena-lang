#type(type:subject) error_type.

#type(type:func1) Error.
#type(type:func1) MethodNotFoundError.
#type(type:func1) FormatError.
#type(type:func1) OutOfRangeError.
#type(type:func1) InvalidArgumentError.
#type(type:func1) NotSupportedError.
#type(type:func1) Abort.

// --- Exception ---

#class Exception
{
    #field theMessage.
    #field(type:subject) theError.
    
    #constructor new &message:aMessage &error_type:anError
    [
        theMessage := aMessage.
        theError := anError.
    ]    
    
    #method if &Error:aHandler
    [
        aHandler eval:$self.
    ]
    
    #method(generic) if : aHandler
    [
        $subject~subjectOp equal &subject:theError ? [ aHandler eval:$self. ] ! [ #throw $self. ]
    ]
    
    #method on : aCatchBlock
    [
        aCatchBlock ~ theError get eval:$self | if &MethodNotFoundError: e [ #throw $self. ].
    ]    
    
//    #method literal => theMessage.
    
    #method message = theMessage.
}

// --- FormatException ---

#class FormatException : Exception
{
    #constructor new
        <= %new &message:"A number is not in the correct format" &error_type: %FormatError.
}

// --- OutOfRangeException ---

#class OutOfRangeException : Exception
{
    #constructor new
        <= %new &message:"An index is out of range" &error_type: %OutOfRangeError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %OutOfRangeError.
}

// --- InvalidArgumentException ---

#class InvalidArgumentException : Exception
{
    #constructor new
        <= %new &message:"Invalid argument" &error_type: %InvalidArgumentError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %InvalidArgumentError.
}

// --- MethodNotFound ---

#class MethodNotFoundException : Exception
{
    #constructor new
        <= %new &message:"Method not found" &error_type: %MethodNotFoundError.
}

// --- MethodNotSupported ---

#class NotSupportedException : Exception
{
    #constructor new
        <= %new &message:"Operation is not supported" &error_type: %NotSupportedError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %NotSupportedError.
}

// --- AbortException ---

#class AbortException : Exception
{
    #constructor new
        <= %new &message:"The process terminated" &error_type: %Abort.
}
