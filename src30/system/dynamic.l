// --- Symbol ---

#class(sealed,symbol) Symbol
{
    #constructor new &name:aName
        => system'internal'symbol_new.
        
    #method => system'internal'symbol_redirect.
}

// --- Message ---

#class(sealed,message) Message
{
    #constructor new &mssg:aMessage
        => system'internal'int_copy.

    #method => system'internal'handle_message.
}

// --- MessageVar ---

#class(sealed,struct:embeddable) MessageVar
{
    #field(type:mssg)theMessage.
    
    #constructor new &mssg:aMessage
    [
        theMessage := aMessage.
    ]
    
    #method mssg = Message new &mssg:theMessage.
    
    #method => theMessage.
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
    #constructor new &mssg:aMessage
        => system'internal'subj_copymssg.
        
    #constructor new &subject:aSubject
        => system'internal'int_copy.

    #constructor new &literal:aSubjectName
        => system'internal'subj_load.

    #constructor new &wide:aSubjectName
        <= new &literal:(aSubjectName literal).

    #method => system'internal'qualify_message.
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
    
    #method cast : aVerb &with:aParam &to:aTarget = aTarget::aVerb eval:aParam &mssgarray:$self.    
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
