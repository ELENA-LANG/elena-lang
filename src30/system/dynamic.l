//// --- Symbol ---
//
//#class Symbol
//{
//    #field(type:int)theReference.
//    
//    #constructor new &literal:aName
//        <symbol_new>.
//        
//    #method => <symbol_redirect>.
//}

// --- Message ---

#class(message) Message
{
    #method => <handle_message>.
}

// --- Signature ---

#class(sealed,signature,type:subject) Signature
{
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

// --- subjectOp ---

#class(extension:subject)subjectOp
{
    #method equal &subject : aSubject
        <subject_equal>.
}