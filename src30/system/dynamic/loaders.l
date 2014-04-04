// --- Symbol ---

#class Symbol
{
    #field(type:int)theReference.
    
    #constructor new &literal:aName
        <symbol_new>.
        
    #method => <symbol_redirect>.
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

// --- DispatchSubject ---

#class DispatchSubject : Signature
{
    #constructor => <new_signature>.
}

// --- GetProperty ---

#class GetProperty
{
    #field theSubject.
    #field theContent.
    
    #method => <get_property>.
}
