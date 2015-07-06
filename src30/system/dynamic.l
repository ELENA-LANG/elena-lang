// --- Symbol ---

/// A dynamic symbol
#class(sealed,symbol) Symbol
{
    /// Loads a symbol
    #constructor new &literal:aName
        => system'internal'symbol_new.
        
    /// Redirects incoming messages to the symbol
    #method => system'internal'symbol_redirect.
}

// --- Verb ---

/// A dynamic verb extender
#class(sealed,signature) Verb
{
    /// Loads a dynamic verb
    #constructor new &verb:aVerb
        => system'internal'int_copy.

    /// Dispatches incoming messages and redirects to the target
    #method => system'internal'handle_verb.
}

// --- Message ---

/// A dynamic message extender
#class(sealed,message) Message
{
    /// Loads a message
    #constructor new &mssg:aMessage
        => system'internal'int_copy.
        
    #method subject
        => system'internal'mssg_getsubj.

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    #method => system'internal'handle_message.
}

// --- Signature ---

/// A dynamic signature extender
#class(sealed,signature) Signature
{
    /// Creates a dynamic signature
    #constructor new &mssg:aMessage
        => system'internal'subj_copymssg.
        
    /// Creates a dynamic signature
    #constructor new &subject:aSubject
        => system'internal'int_copy.

    /// Creates a dynamic signature
    #constructor new &literal:aSubjectName
        => system'internal'subj_load.

    /// Creates a dynamic signature
    #constructor new &wide:aSubjectName
        <= new &literal:(aSubjectName literal).

    /// Compares the values
    #method equal &subject : aSubject
        => system'internal'subject_equal.
        
    /// Qualifies incoming messages and redirects to the target
    #method => system'internal'qualify_message.
}

// --- MessageVar ---

#class(sealed,struct:embeddable) MessageVar
{
    /// Creates a dynamic signature
    #field(type:mssg)theMessage.
    
    #constructor new &mssg:aMessage
    [
        theMessage := aMessage.
    ]
    
    #method mssg = Message new &mssg:theMessage.
    
    #method => theMessage.
}

// --- MessageArray ---

#class(sealed,stringof:mssg)MessageArray
{
    #constructor new &length : aLength
        => system'internal'ns_create.
        
    #method readLength &vint:aLength
        => system'internal'ns_getlen.
        
    #method length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method mssgarray = $self.

    #method write &index:anIndex &mssg:aValue
        => system'internal'ns_writedword.

    #method read &index:anIndex &vmssg:anInt
        => system'internal'ns_readdword.
    
    #method getAt &index:anIndex
    [
        #var(type:mssg)aMessage.
        $self read &index:anIndex &vmssg:aMessage.
        
        ^ Message new &mssg:aMessage.
    ]
    
    #method getAt : anIndex
        <= getAt &index:(anIndex int).
    
    #method enumerator = MessageArrayEnumerator new &mssgarray:$self.

    #method iterator = MessageArrayIterator new &mssgarray:$self.
    
    #method cast : aVerb &to:aTarget = aTarget::aVerb eval &mssgarray:$self.
    
    #method cast : aVerb &to:aTarget &with:aParam = aTarget::aVerb eval:aParam &mssgarray:$self.
}

// --- MessageArrayIterator ---

#class MessageArrayIterator :: Iterator
{
    #field(type:mssgarray) theArray.
    #field(type:vint)  theIndex.

    #constructor new &mssgarray:anArray
    [
        theArray := anArray.
        theIndex := Integer new.
    ]
    
    #method available
    [
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        ^ theIndex < aLength.
    ]

    #method get
        = theArray getAt &index:theIndex.
    
    #method readIndex  &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method write &index:anIndex 
    [
        theIndex write &int:anIndex.
    ]
}

// --- MessageArrayEnumerator ---

#class MessageArrayEnumerator :: Enumerator
{
    #field(type:mssgarray) theArray.
    #field(type:vint)  theIndex.
    
    #constructor new &mssgarray:anArray
    [
        theArray := anArray.
        theIndex := Integer new &int:(-1).
    ]

    #method reset
    [
        theIndex write &int:-1.
    ]

    #method next
    [
        #var(type:int)aLength.
        theArray readLength &vint:aLength.
        
        theIndex append &int:1.

        ^ theIndex < aLength.
    ]
    
    #method get
        = theArray getAt &index:theIndex.
}
