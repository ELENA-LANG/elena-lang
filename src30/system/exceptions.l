// ==== ver 3.4.1 ===

// --- CallStack ---

/// A call stack
#class(sealed,nonstructural) CallStack
{
    #field(type:intarray) theStack.
    
    /// Creates a current call stack
    #constructor new
    [
        #var(type:intarray,size:64)aStack.
        #var(type:int)aLength.
        
        system'internal'callstack_load eval
            &intarray:aStack
            &length:64
            &vint:aLength.
            
        theStack := IntArray new &int:0 &int:aLength &intarray:aStack.            
    ]
    
    #method(stacksafe) $read &int:anIndex &bytearray:aBuffer &vint:aLength
        => system'internal'callstack_readaddressinfo.
    
    /// Generates the call stack literal representation
    #method literal
    [
        #var(type:int)anIndex := 0.
        #var(type:int)aLength := theStack length.
        #var(type:int)aBufferLength := 0.

        #var(type:bytearray,size:512)aBuffer.
        #var(type:literal)aCallStackText := emptyLiteralValue.                
        #loop (anIndex < aLength) ?
        [
            aBufferLength := 512.
            $self $read &int:anIndex &bytearray:aBuffer &vint:aBufferLength.
            (aBufferLength > 0)
                ? [ aCallStackText := aCallStackText add &literal:(LiteralValue $new &int:0 &int:aBufferLength &bytearray:aBuffer) add &literal:('newLine literal). ].
            
            anIndex := anIndex + 1.
        ].
        
        ^ aCallStackText.
    ]
}

// --- Exception ---

/// A basic exception
#class Exception
{
    #field(type:literal) theMessage.
    #field(type:subject) theError.
    #field theCallStack.

    /// Creates the exception with the provided message and error type
    #constructor new &literal:aMessage &subject:anError
    [
        theMessage := aMessage.
        theError := anError.
        theCallStack := CallStack new.
    ]    
    
    /// Executes the generic exception handler
    #method if &Error:aHandler
    [
        aHandler eval:$self.
    ]
    
    /// If incoming subject equals to the exception type, executes the exception handler.
    ///
    /// Otherwise goes to the next exception handler
    #method(generic) if : aHandler
    [
        ($subject == theError) ? [ aHandler eval:$self. ] ! [ #throw $self. ]
    ]
    
    /// Executes the exception handler for the current exception if available.
    ///
    /// Otherwise goes to the next exception handler
    #method on : aCatchBlock
    [
        (aCatchBlock::theError get) eval:$self | if &MethodNotFoundError: e [ #throw $self. ].
    ]
    
    /// Returns the message and the call stack
    #method literal = theMessage add &literal:#10"Call stack:"#10 add &literal:(theCallStack literal).
    
    /// Returns the message and the call stack
    #method wide = $self literal wide.
    
    /// Returns the message
    #method message = theMessage.
}

// --- FormatException ---

#class FormatException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"A number is not in the correct format" &subject: %FormatError.
}

// --- OutOfRangeException ---

#class OutOfRangeException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"An index is out of range" &subject: %OutOfRangeError.
        
    /// Creates the exception
    #constructor new:aMessage
        <= %new &literal:(aMessage literal) &subject: %OutOfRangeError.
}

// --- IndexOutOfRangeException ---

#class IndexOutOfRangeException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"An index is out of range" &subject: %IndexOutOfRangeError.
        
    /// Creates the exception
    #constructor new:aMessage
        <= %new &literal:(aMessage literal) &subject: %IndexOutOfRangeError.
}

// --- InvalidArgumentException ---

#class InvalidArgumentException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"Invalid argument" &subject: %InvalidArgumentError.
        
    /// Creates the exception
    #constructor new:aMessage
        <= %new &literal:(aMessage literal) &subject:%InvalidArgumentError.
                
    /// Creates the exception
    ///
    /// Obsolete
    #constructor new &message:aMessage
        <= %new &literal:(aMessage literal) &subject:%InvalidArgumentError.        
}

// --- InvalidOperationException ---

#class InvalidOperationException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"Invalid operation" &subject: %InvalidOperationError.
        
    /// Creates the exception
    #constructor new:aMessage
        <= %new &literal:(aMessage literal) &subject: %InvalidOperationError.
}

// --- MethodNotFound ---

#class MethodNotFoundException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"Method not found" &subject: %MethodNotFoundError.
}

// --- MethodNotSupported ---

#class NotSupportedException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"Operation is not supported" &subject: %NotSupportedError.
        
    /// Creates the exception
    #constructor new :aMessage
        <= %new &literal:(aMessage literal) &subject: %NotSupportedError.
}

// --- AbortException ---

#class AbortException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &literal:"The process terminated" &subject: %Abort.
}
