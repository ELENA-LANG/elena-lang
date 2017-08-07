// --- type --

type exception = Exception.

//// --- helper ---
//
//extension $op
//{
//    throw
//        => system'core_routines'__object.
//}
//
//// --- CallStack ---
//
///// A call stack
//sealed nonstructural class CallStack
//{
//    intarray theStack.
//    
//    /// Creates a current call stack
//    constructor new
//    [
//        int aStack(64).
//        int aLength := 0.
//        
//        aLength := system'external core_callstack_load
//            intarray:aStack
//            int:64.
//            
//        theStack := IntArray new int:0 int:aLength intarray:aStack
//    ]
//    
//    /// Generates the call stack literal representation
//    literal
//    [
//        int     anIndex := 0.
//        int     aLength := theStack length.
//        int     aBufferLength := 0.
//        int     address := 0.
//
//        byte    aBuffer(512).
//        literal aCallStackText := emptyLiteralValue.
//        while (anIndex < aLength)
//        [
//            theStack read int:anIndex vint:address.
//            
//            aBufferLength := system'external LoadAddressInfo
//                int:address 
//                bytearray:aBuffer
//                int:512.
//            
//            if (aBufferLength > 0)
//                [ aCallStackText := aCallStackText add literal:(LiteralValue $new int:0 int:aBufferLength bytearray:aBuffer); add literal:'newLine ].
//            
//            anIndex := anIndex + 1.
//        ].
//        
//        ^ aCallStackText.
//    ]
//}
//
//// --- exceptionof ---
//
//template exception :: subject
//{
//    literal theMessage.
//    object  theCallStack.
//    
//    /// Returns the message and the call stack
//    literal
//    [
//        literal retVal := theMessage add literal:$10"Call stack:"$10.
//        
//        ^ retVal add literal:(theCallStack literal)
//    ]
//    
//    /// Returns the message
//    message = theMessage.
//    
//    /// Executes the exception handler for the current exception if available.
//    ///
//    /// Otherwise goes to the next exception handler
//    catch : aCatchBlock
//    [
//        aCatchBlock on:$self \ evalSelf((:e)[ e throw ]).
//    ]    
//
//    /// Executes the exception handler for the current exception if available.
//    /// Finally code is called before
//    ///
//    /// Otherwise goes to the next exception handler
//    catch : aFinallyCode : aCatchBlock
//    [
//        aFinallyCode eval.
//        
//        aCatchBlock on:$self \ evalSelf((:e) [ $self throw ]).
//    ]
//
//    raise
//    [
//        $self throw.  
//    ]
//    
//    raise : aFinallyCode
//    [
//        aFinallyCode eval.
//        
//        $self throw.
//    ]
//    
//    subject = $self.
//}

// --- Exception ---

/// A basic exception
class Exception //:: exception<exception>
{
//    /// Creates the exception with the provided message and error type
//    constructor new literal:aMessage
//    [
//        theMessage := aMessage.
//        theCallStack := CallStack new.
//    ]    
}

//// --- FormatException ---
//
//class FormatException :: Exception
//{
//    constructor new
//        <= new literal:"A number is not in the correct format".
//        
//    constructor new : message
//        <= new literal:message.
//}
//
//// --- OutOfRangeException ---
//
//class OutOfRangeException :: Exception
//{
//    /// Creates the exception
//    constructor new
//        <= new literal:"An index is out of range".
//
//    /// Creates the exception
//    constructor new:aMessage
//        <= new literal:aMessage.
//}
//
////// --- IndexOutOfRangeException ---
////
////#class(exceptionof:IndexOutOfRangeError) IndexOutOfRangeException
////{
////    #constructor new &literal:message
////    [
////        theMessage := message.
////        theCallStack := CallStack new.
////    ]
////    
////    /// Creates the exception
////    #constructor new
////        <= new &literal:"An index is out of range".
////
////    /// Creates the exception
////    #constructor new:aMessage
////        <= new &literal:(aMessage literal).
////}
//
//// --- InvalidArgumentException ---
//
//class InvalidArgumentException :: Exception
//{
//    /// Creates the exception
//    constructor new
//        <= new literal:"Invalid argument".
//        
//    /// Creates the exception
//    constructor new:aMessage
//        <= new literal:aMessage.
//}
//
//// --- InvalidOperationException ---
//
//class InvalidOperationException :: Exception
//{
//    /// Creates the exception
//    constructor new
//        <= new literal:"Invalid operation".
//
//    /// Creates the exception
//    constructor new:aMessage
//        <= new literal:aMessage.
//}

// --- MethodNotFound ---

class MethodNotFoundException :: Exception
{
//    /// Creates the exception
//    constructor new
//        <= new literal:"Method not found".
//
//    stacksafe constructor new mssg:missingMessage
//        <= new literal:("Method " add literal:missingMessage; add literal:" not found").
//        
//    stacksafe constructor new object:object mssg:missingMessage
//        <= new literal:(object className; add literal:" : Method "; add literal:missingMessage; add literal:" not found").
}

//// --- MethodNotSupported ---
//
//class NotSupportedException :: Exception
//{
//    /// Creates the exception
//    constructor new
//        <= new literal:"Operation is not supported".
//        
//    /// Creates the exception
//    constructor new :aMessage
//        <= new literal:aMessage.
//}
//
//// --- AbortException ---
//
///// An abort exception 
//class AbortException :: Exception
//{
//    /// Creates the exception
//    constructor new
//        <= new literal:"The process terminated".
//}
