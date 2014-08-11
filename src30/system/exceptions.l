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
    
    #method ifFailed : aHandler
        = aHandler eval.
    
    #method ifFailed &function:aHandler
        = aHandler eval:$self.
    
    #method => <exception_redirect>.
}

// --- Exception ---

#class Exception : BaseException
{
    #field theMessage.
    
    #constructor new : aMessage
    [
        theMessage := aMessage.
    ]    
    
    #method literal => theMessage.
    
    #method Message = theMessage.
}

// --- FormatException ---

#class FormatException : Exception
{
    #constructor new
    [
        theMessage := "A number is not in the correct format".
    ]    
    
    #method onFormatError : aHandler    
        = aHandler eval:$self.
    
    #method ifFailed &on:aCatchBlock
        = aCatchBlock onFormatError:$self.
}

// --- OutOfRangeException ---

#class OutOfRangeException : Exception
{
    #constructor new
    [
        theMessage := "An index is out of range".
    ]    
    
    #method onOutOfRangeError : aHandler    
        = aHandler eval:$self.
    
    #method ifFailed &on:aCatchBlock
        = aCatchBlock onOutOfRangeError:$self.
}

// --- InvalidArgumentException ---

#class InvalidArgumentException : Exception
{
    #constructor new
    [
        theMessage := "Invalid argument".
    ]    
    
    #method onInvalidArgumentError : aHandler    
        = aHandler eval:$self.
    
    #method ifFailed &on:aCatchBlock
        = aCatchBlock onInvalidArgumentError:$self.
}

// --- MethodNotFound ---

#class(role) MethodNotFoundException : BaseException
{
    #method onMethodNotFound : aHandler    
    [
        aHandler eval:$self.
    ]
    
    #method ifFailed &on:aCatchBlock
        = aCatchBlock onMethodNotFound:$self.
        
    #method Message = "Method not found".
}

// --- MethodNotSupported ---

#class(role) NotSupportedException : BaseException
{
    #method onNotSupported : aHandler    
    [
        aHandler eval:$self.
    ]
    
    #method ifFailed &on:aCatchBlock
        = aCatchBlock onNotSupported:$self.
        
    #method Message = "Operation is not supported".
}
