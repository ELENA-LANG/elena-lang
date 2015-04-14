// --- Symbol ---

#class(symbol) Symbol
{
    #constructor new &name:aName
        => system'internal'symbol_new.
        
    #method => system'internal'symbol_redirect.
}

// --- Message ---

#class(message) Message
{
    #constructor new &mssg:aMessage
        => system'internal'int_copy.

    #method => system'internal'handle_message.
}

// --- Verb ---

#class(sealed,signature) Verb
{
    #constructor new &verb:aVerb
        => system'internal'int_copy.

    #method => system'internal'handle_verb.
}

// --- Signature ---

#class(sealed,signature) Signature
{
    #constructor new &subject:aSubject
        => system'internal'int_copy.

    #method => system'internal'qualify_message.
}
