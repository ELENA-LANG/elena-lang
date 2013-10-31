// --- Symbol ---

#class(role) Symbol
{
    #field(type:int)theReference.
}

// --- Message ---

#class(message,role) Message
{
    #field(type:int)theMessage.
    
    #method => #inline handle_message.
}

// --- Signature ---

#class(signature,role) Signature
{
    #field(type:int)theSignature.
    
    #method => #inline qualify_message.
}

// --- GetProperty ---

#class GetProperty
{
    #field theSubject.
    #field theContent.
    
    #method => #inline get_property.
}