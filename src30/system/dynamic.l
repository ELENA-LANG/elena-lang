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
    #constructor(stacksafe) new &verb:aVerb
        => system'internal'int_copy.

    #method verb = $self.

    /// Dispatches incoming messages and redirects to the target
    #method => system'internal'handle_verb.
}

// --- Message ---

/// A dynamic message extender
#class(sealed,message) Message
{
    /// Loads a message
    #constructor(stacksafe) new &mssg:aMessage
        => system'internal'int_copy.

    #method mssg
        = $self.
                        
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
    #constructor(stacksafe) new &mssg:aMessage
        => system'internal'subj_copymssg.
        
    /// Creates a dynamic signature
    #constructor(stacksafe) new &subject:aSubject
        => system'internal'int_copy.

    /// Creates a dynamic signature
    #constructor new &literal:aSubjectName
        => system'internal'subj_load.

    /// Creates a dynamic signature
    #constructor new &wide:aSubjectName
        <= new &literal:(aSubjectName literal).

    #method subject = $self.

    /// Compares the values
    #method(stacksafe) equal &subject : aSubject
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
    #constructor(stacksafe) new &int : aLength
        => system'internal'ns_create.
        
    #constructor new : aLength
        <= new &int:(aLength int).
        
    #method(stacksafe) readLength &vint:aLength
        => system'internal'ns_getlen.
        
    #method(stacksafe) length
    [
        #var(type:int)aLength.
        $self readLength &vint:aLength.
        
        ^ IntNumber new &int:aLength.
    ]
    
    #method mssgarray = $self.

    #method(stacksafe) write &int:anIndex &mssg:aValue
        => system'internal'ns_writedword.

    #method(stacksafe) read &int:anIndex &vmssg:anInt
        => system'internal'ns_readdword.
    
    #method(stacksafe) getAt &int:anIndex
    [
        #var(type:mssg)aMessage.
        $self read &int:anIndex &vmssg:aMessage.
        
        ^ Message new &mssg:aMessage.
    ]
    
    #method getAt : anIndex
        <= getAt &int:(anIndex int).
    
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
        = theArray getAt &int:theIndex.
    
    #method readIndex  &vint:anIndex
    [
        anIndex << theIndex.
    ]
    
    #method writeIndex &int:anIndex 
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
        = theArray getAt &int:theIndex.
}
