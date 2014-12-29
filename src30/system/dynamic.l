// --- Symbol ---

#class Symbol
{
    #field(type:int)theReference.
    
    #constructor new &name:aName
        => system'internal'symbol_new.
        
    #method => system'internal'symbol_redirect.
}

// --- Message ---

#class(message) Message
{
}

// --- Verb ---

#class(sealed,signature) Verb
{
    #constructor new &int:aVerb
        => system'internal'int_copy.

    #method => system'internal'handle_message.
}

// --- Signature ---

#class(sealed,signature) Signature
{
    #constructor new &int:aSubject
        => system'internal'int_copy.

    #method => system'internal'qualify_message.
}
