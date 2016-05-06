// --- Symbol ---

/// A dynamic symbol
#class(sealed,struct:embeddable) Symbol
{
    #field(symbol)theValue.
    
    /// Loads a symbol
    #constructor new &literal:name
    [
        system'core_routines'__symbol load &symbol_ref:$self &literal:name.
    ]
        
    /// Redirects incoming messages to the symbol
    #method => system'core_routines'__symbol.
}

// --- Verb ---

/// A dynamic verb extender
#class(sealed,struct:embeddable) Verb
{
    #field(signature)theValue.
    
    /// Loads a dynamic verb
    #constructor(stacksafe,suppress:w3) new &verb:aVerb
        = aVerb.

    /// Dispatches incoming messages and redirects to the target
    #method => system'core_routines'__verb.
}

// --- Message ---

/// A message constant
#class(sealed,struct:embeddable) Message
{
    #field(message)theValue.
    
    /// Loads a message
    #constructor(stacksafe,suppress:w3) new &mssg:message
        = message.

    #method(stacksafe) subject
        => system'core_routines'__message.

    #method(stacksafe)literal
    [
        #var(byte:256)aBuffer.        
        #var(int)aLength := system'external LoadMessageName &mssg:$self &bytearray:aBuffer &int:255.
        
        ^ LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
    ]

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    #method => system'core_routines'__message.
}

// --- ExtensionMessage ---

/// An extended message constant
#class(sealed,struct:embeddable) ExtensionMessage
{
    #field(extension_message)theValue.
    
    /// Loads a message
    #constructor(stacksafe,suppress:w3) new &ext_mssg:message
        = message.

    #method(stacksafe) subject
        => system'core_routines'__message.

    #method(stacksafe) equal &ext_mssg : aSubject    
        => system'core_routines'__extmessage.

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    #method => system'core_routines'__extmessage.
}


// --- Signature ---

/// A dynamic signature extender
#class(sealed,struct:embeddable) Signature
{
    #field(signature)theValue.
    
    /// Creates a dynamic signature
    #constructor(stacksafe) new &mssg:message
    [
        system'core_routines'__signature load &subject:$self &mssg:message.
    ]
        
    /// Creates a dynamic signature
    #constructor(stacksafe,suppress:w3) new &subject:subject
        = subject.

    /// Creates a dynamic signature
    #constructor new &literal:aSubjectName
    [
        system'core_routines'__signature load &subject:$self &literal:aSubjectName.
    ]

    /// Creates a dynamic signature
    #constructor new &wide:aSubjectName
        <= new &literal:(aSubjectName literal).

    /// Compares the values
    #method(stacksafe,bool) equal &subject : subject
        = theValue == subject.

    #method(stacksafe)literal
    [
        #var(byte:256)aBuffer.        
        #var(int)aLength := system'external LoadSubjectName &subject:$self &bytearray:aBuffer &int:255.
        
        ^ LiteralValue $new &int:0 &int:aLength &bytearray:aBuffer.
    ]
        
    /// Qualifies incoming messages and redirects to the target
    #method => system'core_routines'__signature.
}

// --- MessageArray ---

#class(sealed,arrayof:mssg,dispatchable:mssgarray)MessageArray
{
    #constructor(stacksafe) new &int : aLength
        = mssg:aLength.
        
    #constructor new : aLength
        <= new &int:(aLength int).

//    #method(stacksafe) write &int:anIndex &mssg:aValue
//        => system'core_routines'ns_writedword.
//
//    #method(stacksafe) read &int:anIndex &vmssg:anInt
//        => system'core_routines'ns_readdword.
//    
//    #method enumerator = MessageArrayEnumerator new &mssgarray:$self.
//
//    #method iterator = MessageArrayIterator new &mssgarray:$self.
}

//// --- MessageArrayIterator ---
//
//#class MessageArrayIterator :: Iterator
//{
//    #field(type:mssgarray) theArray.
//    #field(type:vint)  theIndex.
//
//    #constructor new &mssgarray:anArray
//    [
//        theArray := anArray.
//        theIndex := Integer new.
//    ]
//    
//    #method available
//    [
//        #var(type:int)aLength.
//        theArray readLength &vint:aLength.
//        
//        ^ theIndex < aLength.
//    ]
//
//    #method get
//        = theArray getAt &int:theIndex.
//    
//    #method readIndex  &vint:anIndex
//    [
//        anIndex << theIndex.
//    ]
//    
//    #method writeIndex &int:anIndex 
//    [
//        theIndex write &int:anIndex.
//    ]
//}
//
//// --- MessageArrayEnumerator ---
//
//#class MessageArrayEnumerator :: Enumerator
//{
//    #field(type:mssgarray) theArray.
//    #field(type:vint)  theIndex.
//    
//    #constructor new &mssgarray:anArray
//    [
//        theArray := anArray.
//        theIndex := Integer new &int:(-1).
//    ]
//
//    #method reset
//    [
//        theIndex write &int:-1.
//    ]
//
//    #method next
//    [
//        #var(type:int)aLength.
//        theArray readLength &vint:aLength.
//        
//        theIndex append &int:1.
//
//        ^ theIndex < aLength.
//    ]
//    
//    #method get
//        = theArray getAt &int:theIndex.
//}
