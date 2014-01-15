// --- Exception ---

#class Exception
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
    [
        aHandler eval:$self.
    ]
}

// --- OutOfRangeException ---

#class OutOfRangeException : Exception
{
    #constructor new
    [
        theMessage := "An index is out of range".
    ]    
    
    #method onOutOfRangeError : aHandler    
    [
        aHandler eval:$self.
    ]
}

// --- InvalidArgumentException ---

#class InvalidArgumentException : Exception
{
    #constructor new
    [
        theMessage := "Invalid argument".
    ]    
    
    #method onInvalidArgumentError : aHandler    
    [
        aHandler eval:$self.
    ]
}
