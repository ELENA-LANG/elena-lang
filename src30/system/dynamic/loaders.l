// --- Symbol ---

#class Symbol
{
    #field(type:int)theReference.
}

// --- Message ---

#class(message) Message
{
    #field(type:int)theMessage.
    
    #method => <handle_message>.
}

// --- Signature ---

#class(signature) Signature
{
    #field(type:int)theSignature.
    
    #method => <qualify_message>.
}

// --- GetProperty ---

#class GetProperty
{
    #field theSubject.
    #field theContent.
    
    #method => <get_property>.
}