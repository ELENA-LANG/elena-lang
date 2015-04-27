// --- CallStack ---

#class(sealed,nonstructural) CallStack
{
    #field(type:intarray) theStack.
    
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
    
    #method read &index:anIndex &bytearray:aBuffer &vlength:aLength
        => system'internal'callstack_readaddressinfo.
    
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
            $self read &index:anIndex &bytearray:aBuffer &vlength:aBufferLength.
            
            aCallStackText := aCallStackText add &literal:(LiteralValue $new &length:aBufferLength &index:0 &bytearray:aBuffer) add &literal:'newLine.
            
            anIndex := anIndex + 1.
        ].
        
        ^ aCallStackText.
    ]
}

// --- Exception ---

#class Exception
{
    #field(type:literal) theMessage.
    #field theCallStack.
    #field(type:subject) theError.
    
    #constructor new &message:aMessage &error_type:anError
    [
        theMessage := aMessage literal.
        theError := anError.
        theCallStack := CallStack new.
    ]    
    
    #method if &Error:aHandler
    [
        aHandler eval:$self.
    ]
    
    #method(generic) if : aHandler
    [
        ($subject::subjectOp equal &subject:theError) ? [ aHandler eval:$self. ] ! [ #throw $self. ]
    ]
    
    #method on : aCatchBlock
    [
        (aCatchBlock::theError get) eval:$self | if &MethodNotFoundError: e [ #throw $self. ].
    ]
    
    #method literal = theMessage add &literal:#10"Call stack:"#10 add &literal:(theCallStack literal).
    
    #method wide = $self literal wide.
    
    #method message = theMessage.
}

// --- FormatException ---

#class FormatException :: Exception
{
    #constructor new
        <= %new &message:"A number is not in the correct format" &error_type: %FormatError.
}

// --- OutOfRangeException ---

#class OutOfRangeException :: Exception
{
    #constructor new
        <= %new &message:"An index is out of range" &error_type: %OutOfRangeError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %OutOfRangeError.
}

// --- InvalidArgumentException ---

#class InvalidArgumentException :: Exception
{
    #constructor new
        <= %new &message:"Invalid argument" &error_type: %InvalidArgumentError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %InvalidArgumentError.
}

// --- InvalidOperationException ---

#class InvalidOperationException :: Exception
{
    #constructor new
        <= %new &message:"Invalid operation" &error_type: %InvalidOperationError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %InvalidOperationError.
}
// --- MethodNotFound ---

#class MethodNotFoundException :: Exception
{
    #constructor new
        <= %new &message:"Method not found" &error_type: %MethodNotFoundError.
}

// --- MethodNotSupported ---

#class NotSupportedException :: Exception
{
    #constructor new
        <= %new &message:"Operation is not supported" &error_type: %NotSupportedError.
        
    #constructor new &message:aMessage
        <= %new &message:aMessage &error_type: %NotSupportedError.
}

// --- AbortException ---

#class AbortException :: Exception
{
    #constructor new
        <= %new &message:"The process terminated" &error_type: %Abort.
}
