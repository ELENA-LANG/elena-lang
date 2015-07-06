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
            
        theStack := IntArray new &length:aLength &index:0 &intarray:aStack.            
    ]
    
    #method $read &index:anIndex &bytearray:aBuffer &vlength:aLength
        => system'internal'callstack_readaddressinfo.
    
    /// Generates the call stack literal representation
    #method literal
    [
        #var(type:int)anIndex := 0.
        #var(type:int)aLength.
        #var(type:int)aBufferLength := 0.
        
        theStack readLength &vint:aLength.

        #var(type:bytearray,size:512)aBuffer.
        #var(type:literal)aCallStackText := emptyLiteralValue.                
        #loop (anIndex < aLength) ?
        [
            aBufferLength := 512.
            $self $read &index:anIndex &bytearray:aBuffer &vlength:aBufferLength.
            (aBufferLength > 0)
                ? [ aCallStackText := aCallStackText add &literal:(LiteralValue $new &length:aBufferLength &index:0 &bytearray:aBuffer) add &literal:('newLine literal). ].
            
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
    #constructor new &message:aMessage &error_type:anError
    [
        theMessage := aMessage literal.
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
        ($subject equal &subject:theError) ? [ aHandler eval:$self. ] ! [ #throw $self. ]
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
        <= %new &message:"A number is not in the correct format" &error_type: %FormatError.
}

// --- OutOfRangeException ---

#class OutOfRangeException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &message:"An index is out of range" &error_type: %OutOfRangeError.
        
    /// Creates the exception
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %OutOfRangeError.
}

// --- InvalidArgumentException ---

#class InvalidArgumentException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &message:"Invalid argument" &error_type: %InvalidArgumentError.
        
    /// Creates the exception
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %InvalidArgumentError.
}

// --- InvalidOperationException ---

#class InvalidOperationException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &message:"Invalid operation" &error_type: %InvalidOperationError.
        
    /// Creates the exception
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %InvalidOperationError.
}
// --- MethodNotFound ---

#class MethodNotFoundException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &message:"Method not found" &error_type: %MethodNotFoundError.
}

// --- MethodNotSupported ---

#class NotSupportedException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &message:"Operation is not supported" &error_type: %NotSupportedError.
        
    /// Creates the exception
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %NotSupportedError.
}

// --- AbortException ---

#class AbortException :: Exception
{
    /// Creates the exception
    #constructor new
        <= %new &message:"The process terminated" &error_type: %Abort.
}
