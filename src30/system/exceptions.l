// --- BreakException ---

#class BreakException
{
    #field theResult.
    
    #method stop = theResult.
}

// --- BaseException ---

#class BaseException
{
    #method raise : anError
    [
        #throw anError.
    ]

    #method if : aHandler
    [
        aHandler eval:$self.
    ]
    
    #method on : aCatchBlock
    [
        aCatchBlock if::$self | if &MethodNotFoundError: e [ #throw $self. ].
    ]

    #method => <exception_dispatch>.
}

// --- Exception ---

#class Exception : BaseException
{
    #field theMessage.
    
    #constructor new : aMessage
    [
        theMessage := aMessage.
    ]    
    
//    #method literal => theMessage.
    
    #method Message = theMessage.
}

// --- FormatException ---

#class(type:FormatError) FormatException : Exception
{
    #constructor new
    [
        theMessage := "A number is not in the correct format".
    ]    
}

// --- OutOfRangeException ---

#class(type:OutOfRangeError) OutOfRangeException : Exception
{
    #constructor new
    [
        theMessage := "An index is out of range".
    ]    
}

// --- InvalidArgumentException ---

#class(type:InvalidArgumentError) InvalidArgumentException : Exception
{
    #constructor new
    [
        theMessage := "Invalid argument".
    ]    
}

// --- MethodNotFound ---

#class(role,type:MethodNotFoundError) MethodNotFoundException : BaseException
{
    #method Message = "Method not found".
}

// --- MethodNotSupported ---

#class(role,type:NotSupportedError) NotSupportedException : BaseException
{
    #method Message = "Operation is not supported".
}
