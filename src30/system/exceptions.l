// --- CallStack ---

/// A call stack
#class(sealed,nonstructural) CallStack
{
//    #field(intarray) theStack.
    
    /// Creates a current call stack
    #constructor new
    [
//        #var(int:64)aStack.
//        #var(int)aLength.
//        
//        aLength := system'external core_callstack_load
//            &intarray:aStack
//            &int:64.
//            
//        theStack := IntArray new &int:0 &int:aLength &intarray:aStack.
    ]
    
//    /// Generates the call stack literal representation
//    #method literal
//    [
//        #var(int)anIndex := 0.
//        #var(int)aLength := theStack length.
//        #var(int)aBufferLength := 0.
//        #var(int)address.
//
//        #var(byte:512)aBuffer.
//        #var(literal)aCallStackText := emptyLiteralValue.
//        #loop (anIndex < aLength) ?
//        [
//            theStack read &int:anIndex &vint:address.
//            
//            aBufferLength := system'external LoadAddressInfo
//                &int:address 
//                &bytearray:aBuffer
//                &int:512.
//            
//            (aBufferLength > 0)
//                ? [ aCallStackText := aCallStackText add &literal:(LiteralValue $new &int:0 &int:aBufferLength &bytearray:aBuffer) add &literal:('newLine literal). ].
//            
//            anIndex += 1.
//        ].
//        
//        ^ aCallStackText.
//    ]
}

// --- exceptionof ---

#template exceptionof : subject
{
    #field(literal) theMessage.
    #field theCallStack.
    
//    /// Returns the message and the call stack
//    #method literal
//    [
//        #var(literal)retVal := theMessage add &literal:#10"Call stack:"#10.
//        
//        ^ retVal add &literal:(theCallStack literal).
//    ]
//    
//    /// Returns the message and the call stack
//    #method wide = $self literal wide.
//    
//    /// Returns the message
//    #method message = theMessage.
    
    /// Executes the exception handler
    #method if &subject:handler
    [
        handler eval:$self.
    ]

    /// Executes the generic exception handler
    #method if &Error:aHandler
    [
        aHandler eval:$self.
    ]

    /// default handler
    #method(generic) if : aHandler
    [
        #throw $self.
    ]
        
    /// Executes the exception handler for the current exception if available.
    ///
    /// Otherwise goes to the next exception handler
    #method on : aCatchBlock
    [
        aCatchBlock subject eval:$self | if &MethodNotFoundError: e [ #throw $self. ].
    ]    
}

//// --- Exception ---
//
///// A basic exception
//#class Exception
//{
//    #field(literal) theMessage.
//    #field(subject) theError.
//    #field theCallStack.
//
//    /// Creates the exception with the provided message and error type
//    #constructor new &literal:aMessage &subject:anError
//    [
//        theMessage := aMessage.
//        theError := anError.
//        theCallStack := CallStack new.
//    ]    
//    
//    /// Executes the generic exception handler
//    #method if &Error:aHandler
//    [
//        aHandler eval:$self.
//    ]
//    
//    /// If incoming subject equals to the exception type, executes the exception handler.
//    ///
//    /// Otherwise goes to the next exception handler
//    #method(generic) if : aHandler
//    [
//        ($subject == theError) ? [ aHandler eval:$self. ] ! [ #throw $self. ]
//    ]
//    
//    /// Executes the exception handler for the current exception if available.
//    ///
//    /// Otherwise goes to the next exception handler
//    #method on : aCatchBlock
//    [
//        (aCatchBlock::theError get) eval:$self | if &MethodNotFoundError: e [ #throw $self. ].
//    ]
//    
//    /// Returns the message and the call stack
//    #method literal = theMessage add &literal:#10"Call stack:"#10 add &literal:(theCallStack literal).
//    
//    /// Returns the message and the call stack
//    #method wide = $self literal wide.
//    
//    /// Returns the message
//    #method message = theMessage.
//}
//
//// --- FormatException ---
//
//#class(exceptionof:FormatError) FormatException
//{
//    #constructor new
//    [
//        theMessage := "A number is not in the correct format".
//        theCallStack := CallStack new.
//    ]
//}

// --- OutOfRangeException ---

#class(exceptionof:OutOfRangeError) OutOfRangeException
{
    #constructor new &literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    #constructor new
        <= new &literal:"An index is out of range".

    /// Creates the exception
    #constructor new:aMessage
        <= new &literal:(aMessage literal).
}

//// --- IndexOutOfRangeException ---
//
//#class(exceptionof:IndexOutOfRangeError) IndexOutOfRangeException
//{
//    #constructor new &literal:message
//    [
//        theMessage := message.
//        theCallStack := CallStack new.
//    ]
//    
//    /// Creates the exception
//    #constructor new
//        <= new &literal:"An index is out of range".
//
//    /// Creates the exception
//    #constructor new:aMessage
//        <= new &literal:(aMessage literal).
//}

// --- InvalidArgumentException ---

#class(exceptionof:InvalidArgumentError) InvalidArgumentException
{
    #constructor new &literal:message
    [
        theMessage := message.
        theCallStack := CallStack new.
    ]
    
    /// Creates the exception
    #constructor new
        <= new &literal:"Invalid argument".
        
    /// Creates the exception
    #constructor new:aMessage
        <= new &literal:(aMessage literal).
        
    /// obsolete
    #constructor new &message:aMessage
        <= new &literal:(aMessage literal).
}

//// --- InvalidOperationException ---
//
//#class(exceptionof:InvalidOperationError) InvalidOperationException
//{
//    #constructor new &literal:message
//    [
//        theMessage := message.
//        theCallStack := CallStack new.
//    ]
//    
//    /// Creates the exception
//    #constructor new
//        <= new &literal:"Invalid operation".
//
//    /// Creates the exception
//    #constructor new:aMessage
//        <= new &literal:(aMessage literal).
//        
//    /// obsolete
//    #constructor new &message:aMessage
//        <= new &literal:(aMessage literal).
//}

// --- MethodNotFound ---

#class(exceptionof:MethodNotFoundError) MethodNotFoundException
{
//    #constructor new &literal:message
//    [
//        theMessage := message.
//        theCallStack := CallStack new.
//    ]
//    
//    /// Creates the exception
//    #constructor new
//        <= new &literal:"Method not found".
//
//    #constructor(stacksafe) new &mssg:missingMessage
//        <= new &literal:("Method " add &literal:(missingMessage literal) add &literal:" not found").
}

//// --- MethodNotSupported ---
//
//#class(exceptionof:NotSupportedError) NotSupportedException
//{
//    #constructor new &literal:message
//    [
//        theMessage := message.
//        theCallStack := CallStack new.
//    ]
//    
//    /// Creates the exception
//    #constructor new
//        <= new &literal:"Operation is not supported".
//        
//    /// Creates the exception
//    #constructor new :aMessage
//        <= new &literal:(aMessage literal).
//}
//
//// --- AbortException ---
//
///// An abort exception 
//#class(exceptionof:Abort) AbortException
//{
//    #constructor new &literal:message
//    [
//        theMessage := message.
//        theCallStack := CallStack new.
//    ]
//    
//    /// Creates the exception
//    #constructor new
//        <= new &literal:"The process terminated".
//}
