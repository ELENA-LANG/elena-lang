// ==== ver 3.4.1 ===

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

    #constructor new &int:aSubject
        <int_copy>.
            
    #method => <qualify_message>.
}

//// --- DynamicSignature ---
//
//#class DynamicSignature : Signature
//{
//    #constructor => <new_signature>.
//}

// --- GetProperty ---

#class GetProperty
{
    #field theSubject.
    #field theContent.
    
    #method => <get_property>.
}

//// --- EDispatchTable ---
//
//// Extends pairs of subject, signature
//#symbol EDispatchTable =
//{
//    => <map_subject>.
//}.
