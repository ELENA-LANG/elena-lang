// --- Symbol ---

/// A dynamic symbol
sealed embeddable struct Symbol
{
    __symbol theValue.
    
    /// Loads a symbol
    constructor new literal:name
    [
        system'core_routines'__symbol load symbol_ref:$self literal:name
    ]
        
    /// Redirects incoming messages to the symbol
    dispatch => system'core_routines'__symbol.
}

// --- Verb ---

/// A dynamic verb extender
sealed embeddable struct Verb
{
    __verb theValue.

    /// Loads a dynamic verb
    constructor new verb:aVerb
        = aVerb.
    
    /// Dispatches incoming messages and redirects to the target
    dispatch => system'core_routines'__verb.
}

// --- Message ---

/// A message constant
sealed embeddable struct Message
{
    __mssg theValue.

    constructor new literal:name
    [
        system'core_routines'__message load mssg:$self literal:name
    ]
    
    /// Loads a message
    constructor new mssg:message
        = message.

    stacksafe subject
        => system'core_routines'__message.

    stacksafe literal
    [
        mssg val:= $self.
        
        byte aBuffer(256).
        int  aLength := system'external LoadMessageName mssg:$self bytearray:aBuffer int:255.
        
        ^ LiteralValue $new int:0 int:aLength bytearray:aBuffer
    ]

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch => system'core_routines'__message.
}

// --- ExtensionMessage ---

/// An extended message constant
sealed embeddable struct ExtensionMessage
{
    __extmssg theValue.
    
    /// Loads a message
    stacksafe constructor new ext_mssg:message
        = message.

    stacksafe subject
        => system'core_routines'__message.

    stacksafe equal ext_mssg : aSubject    
        => system'core_routines'__extmessage.

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch => system'core_routines'__extmessage.
}

// --- Signature ---

/// A dynamic signature extender
sealed embeddable struct Signature :: dispatchable(subject)
{
    __subj theValue.
    
    /// Creates a dynamic signature
    stacksafe constructor new mssg:message
    [
        system'core_routines'__signature load subject:$self mssg:message
    ]
        
    /// Creates a dynamic signature
    stacksafe constructor new subject:subject
        = subject.

    /// Creates a dynamic signature
    constructor new literal:aSubjectName
    [
        system'core_routines'__signature load subject:$self literal:aSubjectName
    ]

    /// Creates a dynamic signature
    constructor new wide:aSubjectName
        <= new literal:aSubjectName.

    /// Compares the values
    stacksafe bool equal subject : subject
        = theValue == subject.

    stacksafe literal
    [
        byte aBuffer(256).
        int  aLength := system'external LoadSubjectName subject:$self bytearray:aBuffer int:255.
        
        ^ LiteralValue $new int:0 int:aLength bytearray:aBuffer
    ]

    /// Qualifies incoming messages and redirects to the target
    stacksafe dispatch => system'core_routines'__signature.
}
