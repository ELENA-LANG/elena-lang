//// --- Symbol ---
//
///// A dynamic symbol
//sealed struct Symbol
//{
//    __symbol theValue.
//    
//    /// Loads a symbol
//    constructor new literal:name
//    [
//        system'core_routines'__symbol load symbol_ref:$self literal:name
//    ]
//        
//    /// Redirects incoming messages to the symbol
//    dispatch => system'core_routines'__symbol.
//}

// --- Message ---

/// A message constant
public sealed struct Message
{
    __mssg theValue.

    constructor new(literal name)
    [
        system'core_routines'__message load(self, name)
    ]
    
//    stacksafe constructor $new(int message)
//    [
//        system'core_routines'__message copy mssg:$self int:message.
//    ]
    
    /// Loads a message
    constructor new(mssg message)
        = message.

    T<signature> signature
        => system'core_routines'__message.

    T<literal> literal
    [
        T<byte,256> aBuffer.
        int         aLength := system'external LoadMessageName(self, T<bytearray>(aBuffer), 255).
        
        ^ LiteralValue new(0, aLength, T<bytearray>(aBuffer))
    ]

    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
    dispatch => system'core_routines'__message.
}

//// --- ExtensionMessage ---
//
///// An extended message constant
//sealed struct ExtensionMessage
//{
//    __extmssg<8> theValue.
//    
//    /// Loads a message
//    stacksafe constructor new ext_mssg:message
//        = message.
//
//    stacksafe subject
//        => system'core_routines'__message.
//
//    stacksafe bool equal ext_mssg : aSubject    
//        => system'core_routines'__extmessage.
//
//    /// Dequalifies incoming messages if they are equal to its value and redirects to the target
//    dispatch => system'core_routines'__extmessage.
//}

// --- Signature ---

/// A dynamic signature extender
public sealed struct Signature
{
    __subj theValue.
    
    /// Creates a dynamic signature
    stacksafe constructor new(mssg message)
    [
        system'core_routines'__signature load(self, message)
    ]
        
    /// Creates a dynamic signature
    constructor new(signature subject)
        = subject.

//    /// Creates a dynamic signature
//    constructor new literal:aSubjectName
//    [
//        system'core_routines'__signature load subject:$self literal:aSubjectName
//    ]
//
//    /// Creates a dynamic signature
//    constructor new wide:aSubjectName
//        <= new literal:aSubjectName.
//
//    /// Compares the values
//    stacksafe bool equal subject : subject
//        = theValue == subject.

    T<literal> literal
    [
        T<byte,256> aBuffer.
        int         aLength := system'external LoadSubjectName(self, T<bytearray>(aBuffer), 255).
        
        ^ LiteralValue new(0, aLength, T<bytearray>(aBuffer))
    ]

    /// Qualifies incoming messages and redirects to the target
    stacksafe dispatch => system'core_routines'__signature.
}
