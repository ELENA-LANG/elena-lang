// --- CallStack ---

/// A call stack
#class(sealed,nonstructural) CallStack
{
    #field(intarray) theStack.
    
    /// Creates a current call stack
    #constructor new
    [
        #var(int:64)aStack.
        #var(int)aLength.
        
        aLength := system'external core_callstack_load
            &intarray:aStack
            &int:64.
            
        theStack := IntArray new &int:0 &int:aLength &intarray:aStack.
    ]
    
    /// Generates the call stack literal representation
    #method literal
    [
        #var(int)anIndex := 0.
        #var(int)aLength := theStack length.
        #var(int)aBufferLength := 0.
        #var(int)address.

        #var(byte:512)aBuffer.
        #var(literal)aCallStackText := emptyLiteralValue.
        #loop (anIndex < aLength) ?
        [
            theStack read &int:anIndex &vint:address.
            
            aBufferLength := system'external LoadAddressInfo
                &int:address 
                &bytearray:aBuffer
                &int:512.
            
            (aBufferLength > 0)
                ? [ aCallStackText := aCallStackText add &literal:(LiteralValue $new &int:0 &int:aBufferLength &bytearray:aBuffer) add &literal:('newLine literal). ].
            
            anIndex += 1.
        ].
        
        ^ aCallStackText.
    ]
}

// --- exceptionof ---

#template exceptionof : subject
{
    #field(literal) theMessage.
    #field theCallStack.
    
    /// Returns the message and the call stack
    #method literal
    [
        #var(literal)retVal := theMessage add &literal:#10"Call stack:"#10.
        
        ^ retVal add &literal:(theCallStack literal).
    ]
    
    /// Returns the message and the call stack
    #method wide = $self literal wide.
    
    /// Returns the message
    #method message = theMessage.
    
    #method if &subject:handler
    [
        handler eval:$self.
    ]
}

// --- Exception ---

/// A basic exception
#class Exception
{
    #field(literal) theMessage.
    #field(subject) theError.
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

#class(exceptionof:FormatError) FormatException
{
    #constructor new
    [
        theMessage := "A number is not in the correct format".
        theCallStack := CallStack new.
    ]
}

// --- OutOfRangeException ---

#class OutOfRangeException //:: Exception
{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"An index is out of range" &subject: %OutOfRangeError.
//        
//    /// Creates the exception
//    #constructor new:aMessage
//        <= %new &literal:(aMessage literal) &subject: %OutOfRangeError.
}

// --- IndexOutOfRangeException ---
//
//#class IndexOutOfRangeException :: Exception
//{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"An index is out of range" &subject: %IndexOutOfRangeError.
//        
//    /// Creates the exception
//    #constructor new:aMessage
//        <= %new &literal:(aMessage literal) &subject: %IndexOutOfRangeError.
//}

// --- InvalidArgumentException ---

#class InvalidArgumentException //:: Exception
{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"Invalid argument" &subject: %InvalidArgumentError.
//        
//    /// Creates the exception
//    #constructor new:aMessage
//        <= %new &literal:(aMessage literal) &subject:%InvalidArgumentError.
//                
//    /// Creates the exception
//    ///
//    /// Obsolete
//    #constructor new &message:aMessage
//        <= %new &literal:(aMessage literal) &subject:%InvalidArgumentError.        
}

// --- InvalidOperationException ---
//
//#class InvalidOperationException :: Exception
//{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"Invalid operation" &subject: %InvalidOperationError.
//        
//    /// Creates the exception
//    #constructor new:aMessage
//        <= %new &literal:(aMessage literal) &subject: %InvalidOperationError.
//}

// --- MethodNotFound ---

#class MethodNotFoundException //:: Exception
{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"Method not found" &subject: %MethodNotFoundError.

    #constructor(stacksafe) new &mssg:missingMessage
//        <= %new &literal:("Method " + missingMessage literal + " not found") &subject: %MethodNotFoundError.
    [
    ]
}

// --- MethodNotSupported ---
//
//#class NotSupportedException :: Exception
//{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"Operation is not supported" &subject: %NotSupportedError.
//        
//    /// Creates the exception
//    #constructor new :aMessage
//        <= %new &literal:(aMessage literal) &subject: %NotSupportedError.
//}
//
// --- AbortException ---
//
//#class AbortException :: Exception
//{
//    /// Creates the exception
//    #constructor new
//        <= %new &literal:"The process terminated" &subject: %Abort.
//}
