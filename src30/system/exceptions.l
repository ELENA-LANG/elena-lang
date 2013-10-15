// --- Exception ---

#class Exception
{
    #field theMessage.
    
    #constructor new : aMessage
    [
        theMessage := aMessage.
    ]    
    
    #method Literal => theMessage.
    
    #method Message = theMessage.
}

// --- FormatException ---

#class FormatException : Exception
{
    #method onFormatError : aHandler    
    [
        aHandler eval:$self.
    ]
}

// --- OutOfRangeException ---

#class OutOfRangeException : Exception
{
    #method onOutOfRangeError : aHandler    
    [
        aHandler eval:$self.
    ]
}

// --- InvalidArgumentException ---

#class InvalidArgumentException : Exception
{
    #method onInvalidArgumentError : aHandler    
    [
        aHandler eval:$self.
    ]
}

// --- BreakException ---

#class BreakException
{
    #field theResult.
    
    #method stop = theResult.
}
